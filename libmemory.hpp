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

#ifndef	LIBMEMORY_HPP
#define	LIBMEMORY_HPP

/* compiler specific pragmas */
#pragma	warning (disable : 4996)

/* leak-detection */
#ifdef	WIN32
#ifdef	_DEBUG
#define	_CRTDBG_MAPALLOC
#define	_CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif

/* system-definitions */
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <stddef.h>

/* integer for pointer storage */
typedef unsigned char memloc;
typedef uintptr_t pointer;
typedef uintptr_t ptrsize;

/* aligned allocations */
void *__stdcall mallocAligned(ptrsize size);
void *__stdcall callocAligned(ptrsize size);
void  __stdcall   freeAligned(void *ret);

#include "libmemory-0.0.0/Defines.hpp"
#include "libmemory-0.0.0/Memory.hpp"
#include "libmemory-0.0.0/MemoryHeader.hpp"
#include "libmemory-0.0.0/Pools.hpp"
#include "libmemory-0.0.0/Structures.hpp"

template<ptrsize alignment = AROS_WORSTALIGN>
class MemQueue : private MemPool<alignment> {

public:
  MemQueue(const ptrsize blsize, const long int lmt) :
  MemPool<alignment>(MEMF_ANY /*| MEMF_CLEAR*/, 16384 * blsize, blsize) {
    blocksize = blsize;
    blocklimit = lmt;
    head = NULL;
    nums = 0;
    allc = 0;
    sizedsize = 0;
  }
//~MemQueue() {
//  printf("used %d bytes of memory (%d blocks of size %d, %d buffered)\n", getMemUsed(), blocksize, nums, allc);
//}

  ptrsize  blocksize;
  ptrsize  sizedsize;

  long int blocklimit;
  long int nums, allc;

protected:
  struct MemBlock {
    struct MemBlock *next;
  } *head;

public:
  /* receive a piece of memory the dimension given the initializer
   * this type of allocation is maintained in a reusable cache
   */
  inline void *obtainBlock() {
    void *block;

    if (!(block = head))
      block = head = (struct MemBlock *)MemPool<alignment>::AllocPooled(blocksize);
    else
      allc--;
    head = head->next;
    nums++;

    return block;
  }

  /* receive a piece of memory of custom dimension
   * this type of allocation is uncached
   */
  inline void *obtainBlock(ptrsize size) {
    if (size > 0) {
      sizedsize += size;

      return MemPool<alignment>::AllocPooled(size);
    }

    return NULL;
  }

  /* receive a piece of memory of custom dimension
   * this type of allocation is uncached
   */
  inline void *obtainPuddle(ptrsize size) {
    if (size > 0) {
      sizedsize += size;

      return MemPool<alignment>::AllocPuddled(size);
    }

    return NULL;
  }

  /* receive multiple pieces of memory the dimension given the initializer
   * this type of allocation is uncached
   */
  inline void *obtainBlocks(long int num) {
    sizedsize += blocksize * num;

    return MemPool<alignment>::AllocPooled(blocksize * num);
  }

  /* giveaway a piece of memory the dimension given the initializer
   * this type of allocation is maintained in a reusable cache
   */
  inline void releaseBlock(void *block) {
    struct MemBlock *mem = (struct MemBlock *)block;

    mem->next = head;
    head = mem;

    nums--;
    allc++;
  }

  /* giveaway a piece of memory of custom dimension
   * this type of allocation is uncached
   */
  inline void releaseBlock(void *block, ptrsize size) {
    if (size > 0) {
      sizedsize -= size;

      MemPool<alignment>::FreePooled(block, size);
    }
  }

  /* giveaway a piece of memory of custom dimension
   * this type of allocation is uncached
   */
  inline void releasePuddle(void *block, ptrsize size) {
    if (size > 0) {
      sizedsize -= size;

      MemPool<alignment>::FreePuddled(block, size);
    }
  }

  /* giveaway multiple pieces of memory the dimension given the initializer
   * this type of allocation is uncached
   */
  inline void releaseBlocks(void *block, long int num) {
    sizedsize -= blocksize * num;

    MemPool<alignment>::FreePooled(block, blocksize * num);
  }

  inline void releaseMemAllocated() {
    ResetPool();
    head = NULL;
  }

  /* return the amount of memory actively in use, thus not in cache */
  inline ptrsize getMemUsed() {
    return blocksize * (nums) + sizedsize;
  }

  /* return the amount of memory taken from the system, thus also the cached */
  inline ptrsize getMemAllocated() {
    return blocksize * (nums + allc);
  }

  /* answer if the resource-limit has been reached, this is without effect on
   * the allocator, but may be used for implementing memory-reuse algorithms
   */
  inline bool reachedLimit() {
    return (blocklimit < nums ? true : false);
  }
};

class MemQueueable {

public:
  /* **************************************************************************
   *
   * if order > lastbarrier
   *   use lastbarrier-memorypool
   * if order > firstbarrier
   *   use firstbarrier-memorypool
   * if order > basic
   *   use basic-memorypool
   *
   * use basic-memorypool[size];
   */
  static void *operator new[] (size_t size, class MemQueue<4> *queue) {
    return queue->obtainBlock(size);
  }
  static void operator delete[] (void *adr, size_t size, class MemQueue<4> *queue) {
    queue->releaseBlock(adr, size);
  }
  static void operator delete[] (void *adr, class MemQueue<4> *queue) {
    abort();
  }

  static void *operator new[] (size_t size) {
    abort(); return malloc(size);
  }
  static void operator delete[] (void *adr, size_t size) {
    abort(); free(adr);
  }
  static void operator delete[] (void *adr) {
    abort(); free(adr);
  }

  static void *operator new(size_t size, class MemQueue<4> *queue) {
    return queue->obtainBlock();
  }
  static void operator delete(void *adr, size_t size, class MemQueue<4> *queue) {
    queue->releaseBlock(adr);
  }
  static void operator delete(void *adr, class MemQueue<4> *queue) {
    queue->releaseBlock(adr);
  }

  static void *operator new(size_t size) {
    return malloc(size);
  }
  static void operator delete(void *adr, size_t size) {
    free(adr);
  }
  static void operator delete(void *adr) {
    free(adr);
  }
};

class MemAligned {

public:
  static void *operator new[] (size_t size) {
    return mallocAligned(size);
  }
  static void operator delete[] (void *adr, size_t size) {
    freeAligned(adr);
  }
  static void operator delete[] (void *adr) {
    freeAligned(adr);
  }

  static void *operator new(size_t size) {
    return mallocAligned(size);
  }
  static void operator delete(void *adr, size_t size) {
    freeAligned(adr);
  }
  static void operator delete(void *adr) {
    freeAligned(adr);
  }
};

#include <map>
#include <vector>

template<class T>
class MemAllocator {

public:
  typedef T          value_type;
  typedef size_t     size_type;
  typedef ptrdiff_t  difference_type;

  typedef T*         pointer;
  typedef const T*   const_pointer;

  typedef T&         reference;
  typedef const T&   const_reference;

  MemAllocator(class MemQueue<4> *q)  {
    queue = q;
  }

  // copy constructor
  MemAllocator(const MemAllocator &obj) {
    queue = obj.queue;
  }

private:
  void operator = (const MemAllocator &);

public:
  class MemQueue<4> *queue;

  template<class _Other>
  MemAllocator(const MemAllocator<_Other> &other) {
    queue = other.queue;
  }

  ~MemAllocator() {
  }

  template <class U>
  struct rebind {
    typedef MemAllocator<U> other;
  };

  pointer address(reference r) const {
    return &r;
  }

  const_pointer address(const_reference r) const {
    return &r;
  }

  pointer allocate(size_type n, const void * /*hint*/=0 ) {
    return (pointer)queue->obtainPuddle(n * sizeof(T));
  }

  void deallocate(pointer p, size_type n) {
    queue->releasePuddle(p, n * sizeof(T));
  }

  void construct(pointer p, const T& val) {
    new (p) T(val);
  }

  void destroy(pointer p) {
    p->~T();
  }

  size_type max_size() const {
    return ULONG_MAX / sizeof(T);
  }
};

template<class T>
bool operator == (const MemAllocator<T>& left, const MemAllocator<T>& right) {
  return (left.queue == right.queue);
}

template<class T>
bool operator != (const MemAllocator<T>& left, const MemAllocator<T>& right) {
  return (left.queue != right.queue);
}

#endif	//LIBMEMORY_HPP
