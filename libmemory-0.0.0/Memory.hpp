/* Version: MPL 1.1
 *
 * "The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is The Amiga Research OS, short AROS.
 *
 * Copyright (C) 1998-2001 AROS - The Amiga Research OS
 * Copyright (C) 2004-2011 Ethatron <niels@paradice-insight.us>.
 * All Rights Reserved.
 */

#ifndef	MEMORY_HPP
#define	MEMORY_HPP

#include "libmemory-0.0.0/Structures.hpp"
#include "libmemory-0.0.0/MemoryHeader.hpp"

struct MemEntry {
  union {
    unsigned long int meu_Reqs;
    void *meu_Addr;
  } me_Un;

  ptrsize me_Length;
};

#define me_Reqs me_Un.meu_Reqs
#define me_Addr me_Un.meu_Addr

struct MemList {
  struct SysNode     ml_Node;
  unsigned short int ml_NumEntries;
  struct MemEntry    ml_ME[1];
};

struct MemHandlerData {
  ptrsize           memh_RequestSize;
  unsigned long int memh_RequestFlags;
  unsigned long int memh_Flags;
};

struct Interrupt : public SysNode {
  void *is_Data;
  long int (* is_Code)(struct MemHandlerData *mhd, void *data); /* server code entry */
};

#define MEMHF_RECYCLE	(1L << 0)

#define MEM_ALL_DONE	(-1)
#define MEM_DID_NOTHING	0
#define MEM_TRY_AGAIN	1

/* Some defines for the memory handling functions. */

template<ptrsize alignment = AROS_WORSTALIGN>
struct MemoryManager {

protected:
  struct SysList    MemList;

  struct MinList ex_MemHandlers;
  void          *ex_MemHandler;      /* PRIVATE */

public:
  ptrsize AvailMem(unsigned long int attributes) {
    ptrsize ret = 0;
    struct MemHeader<alignment> *mh;

    /* Nobody else should access the memory lists now. */
//  Forbid();

    /* Get pointer to first memory header... */
    mh = (struct MemHeader<alignment> *)this->MemList.lh_Head;

    /* And follow the list. */
    while(mh->ln_Succ != NULL) {
      mh->AvailMem(attributes, ret);

      mh = (struct MemHeader<alignment> *)mh->ln_Succ;
    }

    /* All done. Permit dispatches and return. */
//  Permit();
    return ret;
  } /* AvailMem */

  unsigned long int TypeOfMem(void *address) {
    unsigned long int ret = 0;
    struct MemHeader<alignment> *mh;

    /* Nobody should change the memory list now. */
//  Forbid();

    /* Follow the list of MemHeaders */
    mh = (struct MemHeader<alignment> *)this->MemList.lh_Head;
    while(mh->ln_Succ != NULL) {
      if ((ret = mh->TypeOfMem(address)))
        break;

      /* Go to next MemHeader */
      mh = (struct MemHeader<alignment> *)mh->ln_Succ;
    }

    /* Allow Taskswitches and return */
//  Permit();
    return ret;
  } /* TypeOfMem */

  void *AllocAbs(ptrsize byteSize, void *location) {
    void *ret = NULL;
    struct MemHeader<alignment> *mh;

    /* Zero bytes requested? May return everything ;-). */
    if (!byteSize)
      return ret;

    /* Align location and size to the required alignment */
    MemHeader<alignment>::alignValues(byteSize, location);

    /* Protect the memory list from access by other tasks. */
//  Forbid();

    /* Loop over MemHeader structures */
    mh = (struct MemHeader<alignment> *)this->MemList.lh_Head;
    while (mh->ln_Succ) {
      if ((ret = mh->AllocAbs(byteSize, location)))
        break;

      /* Test next MemHeader */
      mh = (struct MemHeader<alignment> *)mh->ln_Succ;
    }

    /* There's nothing we could do */
//  Permit();
    return ret;
  } /* AllocAbs */

  void *AllocMem(ptrsize byteSize, unsigned long int requirements) {
    struct Interrupt *lmh;

    /* Zero bytes requested? May return everything ;-). */
    if (!byteSize)
      return NULL;;

    /* First round byteSize to a multiple of the required alignment */
    MemHeader<alignment>::alignValue(byteSize);

    /* Protect memory list against other tasks */
//  Forbid();

    /* Loop over low memory handlers */
    lmh = (struct Interrupt *)this->ex_MemHandlers.lh_Head;
    for (;;) {
      struct MemHeader<alignment> *mh;

      /* Loop over MemHeader structures */
      mh = (struct MemHeader<alignment> *)this->MemList.lh_Head;
      while (mh->ln_Succ != NULL) {
        void *res;

        if ((res = mh->AllocMem(byteSize, requirements)))
          return res;

        /* Go to next memory header */
        mh = (struct MemHeader<alignment> *)mh->ln_Succ;
      }

      /* Is it forbidden to call low-memory handlers? */
      if (requirements & MEMF_NO_EXPUNGE) {
//	Permit();
        return NULL;
      }

      /* All memory headers done. Check low memory handlers. */
      {
        unsigned long int lmhr;
        struct MemHandlerData lmhd = {byteSize, requirements, 0};

        do {
          /* Is there another one? */
      	  if (lmh->ln_Succ == NULL) {
      	    /* No. return 'Not enough memory'. */
//     	    Permit();
      	    return NULL;
      	  }

      	  /* Yes. Execute it. */
      	  lmhr = lmh->is_Code(&lmhd, lmh->is_Data);

      	  /* Check returncode. */
      	  if (lmhr == MEM_TRY_AGAIN) {
      	    /* MemHandler said he did something. Try again. */
      	    /* Is there any program that depends on this flag??? */
      	    lmhd.memh_Flags |= MEMHF_RECYCLE;
      	    break;
      	  }

      	  /* Nothing more to expect from this handler. */
      	  lmh = (struct Interrupt *)lmh->ln_Succ;
      	  lmhd.memh_Flags &= ~MEMHF_RECYCLE;

          /* If this handler did nothing at all there's no need
           * to try the allocation. Try the next handler immediately.
           */
        } while (lmhr == MEM_DID_NOTHING);
      }
    }

    return NULL;
  } /* AllocMem */

  void FreeMem(void *memoryBlock, ptrsize byteSize) {
    struct MemHeader<alignment> *mh;

    /* If there is no memory free nothing */
    if (!byteSize)
     return;

    /* Align location and size to the required alignment */
    MemHeader<alignment>::alignValues(byteSize, memoryBlock);

    /* Protect the memory list from access by other tasks. */
//  Forbid();

    /* Loop over MemHeader structures */
    mh = (struct MemHeader<alignment> *)this->MemList.lh_Head;
    while(mh->ln_Succ) {
      if (mh->FreeMem(memoryBlock, byteSize))
        return;

      mh = (struct MemHeader<alignment> *)mh->ln_Succ;
    }

//  Permit();
  } /* FreeMem */
};

#endif	//MEMORY_HPP
