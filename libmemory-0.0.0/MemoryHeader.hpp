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

#ifndef	MEMORY_MEMORYHEADER_HPP
#define	MEMORY_MEMORYHEADER_HPP

#include "libmemory-0.0.0/Structures.hpp"

#define MEMF_ANY        	0UL
#define MEMF_CLEAR      	(1UL << 16)
#define MEMF_LARGEST    	(1UL << 17)
#define MEMF_REVERSE    	(1UL << 18)
#define MEMF_TOTAL      	(1UL << 19)
#define MEMF_SEM_PROTECTED	(1UL << 20)
#define MEMF_NO_EXPUNGE 	(1UL << 31)

/* This is for the alignment of memchunk structure-sizes. */
#define ALIGNoverCHUNK(worst)	ROUNDUP_TO_POWER_OF_TWO(worst > sizeof(struct MemChunk) ? worst : sizeof(struct MemChunk))

struct MemChunk {
  struct MemChunk  *mc_Next;
  ptrsize           mc_Bytes;
};

/* This allows to take the end of the MemHeader as the first MemChunk. */
#define ALIGNoverHEADER(worst)	((sizeof(struct MemHeader<worst>) + ALIGNoverCHUNK(worst) - 1) & ~(ALIGNoverCHUNK(worst) - 1))

template<ptrsize alignment = AROS_WORSTALIGN>
struct MemHeader : public MinNode {

  static const ptrsize alignTo   = ALIGNoverCHUNK(alignment);
  static const ptrsize alignMask = alignTo - 1;

public:
  static void alignValues(ptrsize &byteSize, void *&memoryBlock) {
    /* Align size to the requirements */
    byteSize +=   (pointer)memoryBlock &  alignMask;
    byteSize =  (byteSize + alignMask) & ~alignMask;

    /* Align the block as well */
    memoryBlock = (void *)((pointer)memoryBlock & ~alignMask);
  }

  static void alignValue(ptrsize &byteSize) {
    /* Align size to the requirements */
    byteSize =  (byteSize + alignMask) & ~alignMask;
  }

public:
  unsigned short int mh_Attributes;
  struct MemChunk   *mh_First;
  void *             mh_Lower;
  void *             mh_Upper;
  ptrsize            mh_Free;

  bool isWithin(void *ptr) {
    return ((ptr >= this->mh_Lower) && (ptr < this->mh_Upper));
  }

  unsigned long int TypeOfMem(void *address) {
    /* Check if this MemHeader fits */
    if (isWithin(address))
      /* Yes. Prepare returncode */
      return this->mh_Attributes;

    return 0;
  }

  void AvailMem(unsigned long int attributes, ptrsize &ret) {
    /* The current memheader is OK if there's no bit in the
     * 'attributes' that isn't set in the 'mh->mh_Attributes'.
     * MEMF_CLEAR, MEMF_REVERSE, MEMF_NO_EXPUNGE, MEMF_TOTAL and
     * MEMF_LARGEST are treated as if they were always set in
     * the memheader.
     */
    if (!(attributes & ~(MEMF_CLEAR | MEMF_REVERSE | MEMF_TOTAL | MEMF_LARGEST | this->mh_Attributes))) {
      /* Find largest chunk? */
      if (attributes & MEMF_LARGEST) {
        /* Yes. Follow the list of MemChunks and set 'ret' to
    	 * each value that is bigger than all previous ones.
         */
        struct MemChunk *mc = this->mh_First;

        while(mc != NULL) {
    	  if (mc->mc_Bytes > ret)
      	    ret = mc->mc_Bytes;
      	  mc = mc->mc_Next;
        }
      }
      else if (attributes & MEMF_TOTAL)
        /* Determine total size. */
        ret += (memloc *)this->mh_Upper - (memloc *)this->mh_Lower;
      else
        /* Sum up free memory. */
        ret += this->mh_Free;
    }
  }

  void *AllocAbs(ptrsize byteSize, void *location) {
    /* Test if the memory belongs to this MemHeader. */
    if (isWithin(location)) {
      struct MemChunk *p1, *p2, *p3, *p4;

      /* Start and end(+1) of the block */
      p3 = (struct MemChunk *)location;
      p4 = (struct MemChunk *)((memloc *)p3 + byteSize);

      /* The free memory list is only single linked, i.e. to remove
       * elements from the list I need the node's predessor. For the
       * first element I can use freeList->mh_First instead of a real
       * predessor.
       */
      p1 = (struct MemChunk *)&this->mh_First;
      p2 = p1->mc_Next;

      /* Follow the list to find a chunk with our memory. */
      while (p2 != NULL) {
        /* Found a chunk that fits? */
        if ((memloc *)p2 + p2->mc_Bytes >= (memloc *)p4 && p2 <= p3) {
          /* Check if there's memory left at the end. */
          if ((memloc *)p2 + p2->mc_Bytes != (memloc *)p4) {
            /* Yes. Add it to the list */
            p4->mc_Next = p2->mc_Next;
            p4->mc_Bytes = (memloc *)p2 + p2->mc_Bytes - (memloc *)p4;
            p2->mc_Next = p4;
          }

          /* Check if there's memory left at the start. */
          if (p2 != p3)
            /* Yes. Adjust the size */
            p2->mc_Bytes = (memloc *)p3 - (memloc *)p2;
          else
            /* No. Skip the old chunk */
            p1->mc_Next = p2->mc_Next;

          /* Adjust free memory count */
          this->mh_Free -= byteSize;

          /* Return the memory */
//        Permit();
          return p3;
        }

        /* goto next chunk */
        p1 = p2;
        p2 = p2->mc_Next;
      }
    }

    /* The MemHeader didn't have the memory */
    return NULL;
  }

  void *Allocate(ptrsize byteSize) {
    struct MemChunk *p1, *p2;

    /* Zero bytes requested? May return everything ;-). */
    if (!byteSize)
      return NULL;

    /* First round byteSize to a multiple of MEMCHUNK_TOTAL. */
    byteSize = AROS_ROUNDUP2(byteSize, ALIGNoverCHUNK(alignment));

    /* Is there enough free memory in the list? */
    if (this->mh_Free < byteSize)
      return NULL;

    /* The free memory list is only single linked, i.e. to remove
     * elements from the list I need the node as well as it's
     * predessor. For the first element I can use this->mh_First
     * instead of a real predecessor.
     */
    p1 = (struct MemChunk *)&this->mh_First;
    if ((pointer)p1->mc_Next == 0xFDFDFDFD)
      p1->mc_Next = NULL;
    p2 = p1->mc_Next;

    /* Is the list enpty? */
    if (p2 == NULL)
      return NULL;

    /* Follow the list */
    for (;;) {
      /* Check if current block is large enough */
      if (p2->mc_Bytes >= byteSize) {
        /* It is. Remove it from the list and return it. */
        if (p2->mc_Bytes == byteSize)
      	  /* Fits exactly. Just relink the list. */
      	  p1->mc_Next = p2->mc_Next;
        else {
      	  /* Split the current chunk and return the first bytes. */
      	  p1->mc_Next = (struct MemChunk *)((memloc *)p2 + byteSize);
      	  p1 = p1->mc_Next;
      	  p1->mc_Next = p2->mc_Next;
      	  p1->mc_Bytes = p2->mc_Bytes - byteSize;
        }

        /* Adjust free memory count and return */
        this->mh_Free -= byteSize;

        /* Return allocated block to caller */
        return p2;
      }

      /* Go to next block */
      p1 = p2;
      p2 = p1->mc_Next;

      /* Check if this was the end */
      if (p2 == NULL)
        return NULL;
    }

    return NULL;
  } /* Allocate */

  void Deallocate(void *memoryBlock, ptrsize byteSize) {
    struct MemChunk *p1, *p2, *p3;
    unsigned char *p4;

    /* If there is no memory free nothing */
    if (!byteSize)
      return;

    /* The free memory list is only single linked, i.e. to insert
     * elements into the list I need the node as well as it's
     * predessor. For the first element I can use this->mh_First
     * instead of a real predessor.
     */
    p1 = (struct MemChunk *)&this->mh_First;
    p2 = this->mh_First;

    /* Start and end(+1) of the block */
    p3 = (struct MemChunk *)memoryBlock;
    p4 = (memloc *)p3 + byteSize;

    /* No chunk in list? Just insert the current one and return. */
    if (p2 == NULL) {
      p3->mc_Bytes = byteSize;
      p3->mc_Next  = NULL;
      p1->mc_Next  = p3;

      this->mh_Free += byteSize;
      return;
    }

    /* Follow the list to find a place where to insert our memory. */
    do {
      /* Found a block with a higher address? */
      if (p2 >= p3)
        /* End the loop with p2 non-zero */
	break;

      /* goto next block */
      p1 = p2;
      p2 = p2->mc_Next;

    /* If the loop ends with p2 zero add it at the end. */
    } while(p2 != NULL);

    /* If there was a previous block merge with it. */
    if (p1 != (struct MemChunk *)&this->mh_First) {
      /* Merge if possible */
      if ((memloc *)p1 + p1->mc_Bytes == (memloc *)p3)
	p3 = p1;
      else
	/* Not possible to merge */
	p1->mc_Next = p3;
    }
    else
      /* There was no previous block. Just insert the memory at
       * the start of the list.
       */
      p1->mc_Next = p3;

    /* Try to merge with next block (if there is one ;-) ). */
    if (p4 == (memloc *)p2 && p2 != NULL) {
      /* Overlap checking already done. Doing it here after
       *  the list potentially changed would be a bad idea.
       */
      p4 += p2->mc_Bytes;
      p2  = p2->mc_Next;
    }

    /* relink the list and return. */
    p3->mc_Next  = p2;
    p3->mc_Bytes = p4 - (memloc *)p3;

    this->mh_Free += byteSize;
  } /* Deallocate */

  void *AllocMem(ptrsize byteSize, unsigned long int requirements) {
    struct MemChunk *p1,*p2;

    /* Check for the right requirements and enough free memory.
     * The requirements are OK if there's no bit in the
     * 'attributes' that isn't set in the 'this->mh_Attributes'.
     * MEMF_CLEAR, MEMF_REVERSE and MEMF_NO_EXPUNGE are treated
     * as if they were always set in the memheader.
     */
    if (!(requirements & ~(MEMF_CLEAR | MEMF_REVERSE | MEMF_NO_EXPUNGE | this->mh_Attributes)) && this->mh_Free >= byteSize) {
      struct MemChunk *mc = NULL;

      /* The free memory list is only single linked, i.e. to remove
       * elements from the list I need node's predessor. For the
       * first element I can use this->mh_First instead of a real predessor.
       */
      p1 = (struct MemChunk *)&this->mh_First;
      p2 = p1->mc_Next;

      /* Is there anything in the list? */
      if (p2 != NULL) {
        /* Then follow it */
        for (;;) {
      	  /* Check if the current block is large enough */
      	  if (p2->mc_Bytes >= byteSize) {
      	    /* It is. */
      	    mc = p1;
      	    /* Use this one if MEMF_REVERSE is not set.*/
      	    if (!(requirements & MEMF_REVERSE))
      	      break;
      	    /* Else continue - there may be more to come. */
      	  }

      	  /* Go to next block */
      	  p1 = p2;
      	  p2 = p1->mc_Next;

      	  /* Check if this was the end */
      	  if (p2 == NULL)
      	    break;
        }

        /* Something found? */
        if (mc != NULL) {
      	  /* Remember: if MEMF_REVERSE is set
      	   * p1 and p2 are now invalid.
      	   */
      	  p1 = mc;
      	  p2 = p1->mc_Next;

      	  /* Remove the block from the list and return it. */
      	  if (p2->mc_Bytes == byteSize) {
      	    /* Fits exactly. Just relink the list. */
      	    p1->mc_Next = p2->mc_Next;
      	    mc = p2;
      	  }
      	  else {
      	    if (requirements & MEMF_REVERSE) {
      	      /* Return the last bytes. */
      	      p1->mc_Next = p2;
      	      mc = (struct MemChunk *)((memloc *)p2 + p2->mc_Bytes - byteSize);
      	    }
      	    else {
      	      /* Return the first bytes. */
      	      p1->mc_Next = (struct MemChunk *)((memloc *)p2 + byteSize);
      	      mc = p2;
      	    }

      	    p1 = p1->mc_Next;
      	    p1->mc_Next  = p2->mc_Next;
      	    p1->mc_Bytes = p2->mc_Bytes - byteSize;
      	  }

      	  this->mh_Free -= byteSize;

      	  /* No need to forbid dispatching any longer. */
//     	  Permit();

      	  if (requirements & MEMF_CLEAR)
      	    /* Clear memory. */
      	    memset(mc, 0, byteSize);

      	  return mc;
        }
      }
    }

    return NULL;
  }

  bool FreeMem(void *memoryBlock, ptrsize byteSize) {
    /* Test if the memory belongs to this MemHeader. */
    if (isWithin(memoryBlock)) {
      Deallocate(memoryBlock, byteSize);

      return true;
    }

    return false;
  }
};

#endif	//MEMORY_MEMORYHEADER_HPP
