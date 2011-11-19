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

#ifndef	MEMORY_MEMORYPOOLS_HPP
#define	MEMORY_MEMORYPOOLS_HPP

#include "MemoryHeader.hpp"

#define ALIGNoverBLOCK(worst)	((sizeof(struct MemBlock) + ROUNDUP_TO_POWER_OF_TWO(worst) - 1) & ~(ROUNDUP_TO_POWER_OF_TWO(worst) - 1))

struct MemBlock : public MinNode {
  ptrsize Size;
};

template<class DataType, ptrsize alignment = AROS_WORSTALIGN>
class Allocator {

  static const ptrsize alignTo   = ROUNDUP_TO_POWER_OF_TWO(alignment);
  static const ptrsize alignMask = alignTo - 1;

public:
  static DataType *__stdcall mallocAligned(ptrsize size) {
    void *org, *ret;

    /* allocate */
  //org =     malloc(size * sizeof(DataType) + sizeof(void *) + alignMask); if (!org) return NULL;
    org = new memloc[size * sizeof(DataType) + sizeof(void *) + alignMask]; if (!org) return NULL;
    /* align */
    ret = (void *)(((pointer)org + sizeof(void *) + alignMask) & (~alignMask));
    /* put start */
    ((void **)ret)[-1] = org;

    return (DataType *)ret;
  }

  static DataType *__stdcall callocAligned(ptrsize size) {
    DataType *ret;

    ret = mallocAligned(size);
    memset(ret, 0, size);

    return ret;
  }

  static void __stdcall freeAligned(DataType *ret) {
    void *org;
    memloc **ml = (memloc **)ret;

    /* get start */
    org = ((void **)ret)[-1];
    /* for debug only */
    ((void **)ret)[-1] = NULL;

    /* deallocate */
  //     free(org);
    delete [] org;
  }
};

/* Pool structure */
template<ptrsize alignment = AROS_WORSTALIGN>
class MemPool : private Allocator<unsigned char, alignment> {

public:
  MemPool(unsigned long int requirements, const ptrsize puddle, const ptrsize thresh) {
    /* puddleSize must not be smaller than threshSize */
//  if (puddle < thresh)
//    throw Exception;

    Requirements = requirements;
    PuddleSize   = puddle;
    ThreshSize   = thresh;

    /* Round puddleSize up to be a multiple of MEMCHUNK_TOTAL. */
    MemHeader<alignment>::alignValue(PuddleSize);

    CreatePool();
  } /* CreatePool */

  ~MemPool() {
    DeletePool();
  } /* DeletePool */

private:
  void CreatePool() {
    /* Allocate memory for the Pool structure using the same requirements
     * as for the whole pool (to make it shareable, residentable or
     * whatever is needed).
     */

    /* Clear the lists */
    PuddleList.NewList();
    BlockList.NewList();

//  if (requirements & MEMF_SEM_PROTECTED)
//    InitSemaphore(&pool->sem);
  }

  void DeletePool() {
    struct MemBlock 	*bl;
    void 	    	*p;

    /* Free the list of puddles */
    while ((p = (void *)PuddleList.RemHead()) != NULL)
      freeAligned((unsigned char *)p/*, PuddleSize + ALIGNoverHEADER*/);

    /* Free the list of single Blocks */
    while ((bl = (struct MemBlock *)BlockList.RemHead()) != NULL)
      freeAligned((unsigned char *)bl/*, bl->size*/);
  }

protected:
  struct MinList PuddleList;
  struct MinList BlockList;

  unsigned long int Requirements;

  ptrsize PuddleSize;
  ptrsize ThreshSize;

public:
  void clearSerialized(const unsigned long int pos, const ptrsize blocksize) {
    struct MemHeader<alignment> *mh;

    /* Follow the list of MemHeaders */
    mh = (struct MemHeader<alignment> *)PuddleList.lh_Head;
    for(;;) {
      ptrsize blocks;
      unsigned char *adr;

      /* Are there no more MemHeaders? */
      if (mh->ln_Succ == NULL)
        blocks = (PuddleSize              ) / blocksize;
      else
        blocks = (PuddleSize - mh->mh_Free) / blocksize;

      adr = (memloc *)mh->mh_First;
      while (--blocks >= 0) {
        *((memloc *)(adr + pos)) = 0;
	adr += blocksize;
      }

      /* No. Try next MemHeader */
      mh = (struct MemHeader<alignment> *)mh->ln_Succ;
    }
  }

  void ResetPool() {
    DeletePool();
    CreatePool();
  }

  void *AllocPuddled(ptrsize memSize) {
    struct MemBlock *bl;
    ptrsize size;

    /* Get enough memory for the memory block including the header. */
    size = memSize + ALIGNoverBLOCK(alignment);

    /* malloc does not clear the memory! */
    if (Requirements & MEMF_CLEAR)
      bl = (struct MemBlock *)callocAligned(size/*, Requirements & ~MEMF_SEM_PROTECTED*/);
    else
      bl = (struct MemBlock *)mallocAligned(size/*, Requirements & ~MEMF_SEM_PROTECTED*/);

    /* No memory left */
    if (bl == NULL)
      return NULL;

    /* Initialize the header */
    bl->Size = size;

    /* Add the block to the BlockList */
    BlockList.AddHead(bl);

    /* Set pointer to allocated memory */
    return (memloc *)bl + ALIGNoverBLOCK(alignment);
  }

  void *AllocPooled(ptrsize memSize) {
    void *ret = NULL;

//  if (Requirements & MEMF_SEM_PROTECTED)
//    ObtainSemaphore(&pool->sem);

    /* If the memSize is bigger than the ThreshSize allocate seperately. */
    if (memSize > ThreshSize) {
      AllocPuddled(memSize);
    }
    else {
      struct MemHeader<alignment> *mh;

      /* Follow the list of MemHeaders */
      mh = (struct MemHeader<alignment> *)PuddleList.lh_Head;
      for(;;) {
        /* Are there no more MemHeaders? */
	if (mh->ln_Succ == NULL) {
	  /* Get a new one */
	  mh = (struct MemHeader<alignment> *)mallocAligned(PuddleSize + ALIGNoverHEADER(alignment)/*, Requirements & ~MEMF_SEM_PROTECTED*/);

	  /* No memory left? */
	  if (mh == NULL)
	    return NULL;

	  /* Initialize new MemHeader */
	  mh->mh_First	    	 = (struct MemChunk *)((memloc *)mh + ALIGNoverHEADER(alignment));
	  mh->mh_First->mc_Next  = NULL;
	  mh->mh_First->mc_Bytes = PuddleSize;
	  mh->mh_Lower 	    	 = mh->mh_First;
	  mh->mh_Upper 	    	 = (memloc *)mh->mh_First + PuddleSize;
	  mh->mh_Free  	    	 = PuddleSize;

	  /* And add it to the list */
	  PuddleList.AddHead(mh);

	  /* Fall through to get the memory */
	}

	/* Try to get the memory */
	ret = mh->Allocate(memSize);

	/* Got it? */
	if (ret != NULL)
	  break;

	/* No. Try next MemHeader */
	mh = (struct MemHeader<alignment> *)mh->ln_Succ;
      }

      /* Allocate does not clear the memory! */
      if (Requirements & MEMF_CLEAR)
        memset(ret, 0, memSize);
    }

//  if (Requirements & MEMF_SEM_PROTECTED)
//    ReleaseSemaphore(&pool->sem);

    /* Everything fine */
    return ret;
  } /* AllocPooled */

  void FreePuddled(void *memory, ptrsize memSize) {
    struct MemBlock *bl;

    /* Get pointer to header */
    bl = (struct MemBlock *)((memloc *)memory - ALIGNoverBLOCK(alignment));

    /* Remove it from the list */
    bl->Remove();

    //    if (bl->Size != memSize + ALIGNoverBLOCK(alignment)) {
    //      kprintf("\nFreePooled: free size does not match alloc size: allocsize = %d freesize = %d!!!\n\n", bl->Size - ALIGNoverBLOCK(alignment), memSize);
    //      throw new Exception();
    //    }

    /* And Free the memory */
    freeAligned((unsigned char *)bl/*, bl->Size*/);
  }

  void FreePooled(void *memory, ptrsize memSize) {
//  if (Requirements & MEMF_SEM_PROTECTED)
//    ObtainSemaphore(&pool->sem);

    /* If memSize is bigger than the ThreshSize it's allocated seperately. */
    if (memSize > ThreshSize) {
      FreePuddled(memory, memSize);
    }
    else {
      /* Look for the right MemHeader */
      struct MemHeader<alignment> *mh = (struct MemHeader<alignment> *)PuddleList.lh_Head;

      for(;;) {
        /* Found the MemHeader. Free the memory. */
        if (mh->FreeMem(memory, memSize)) {
      	  /* Is this MemHeader completely free now? */
      	  if (mh->mh_Free == PuddleSize) {
      	    /* Yes. Remove it from the list. */
      	    mh->Remove();

      	    /* And free it. */
      	    freeAligned((unsigned char *)mh/*, PuddleSize + ALIGNoverHEADER*/);
      	  }

      	  /* All done. */
      	  break;
        }

        /* Try next MemHeader */
        mh = (struct MemHeader<alignment> *)mh->ln_Succ;
      }
    }

//  if (Requirements & MEMF_SEM_PROTECTED)
//    ReleaseSemaphore(&pool->sem);
   } /* FreePooled */
};

#endif
