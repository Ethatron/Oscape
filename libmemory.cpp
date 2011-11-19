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

#include "libmemory.hpp"

void *__stdcall mallocAligned(ptrsize size) {
  void *org, *ret;

  /* allocate */
//org =     malloc(size + sizeof(void *) + 15); if (!org) return NULL;
  org = new memloc[size + sizeof(void *) + 15]; if (!org) return NULL;
  /* align */
  ret = (void *)(((pointer)org + sizeof(void *) + 15) & (~15));
  /* put start */
  ((void **)ret)[-1] = org;

  return ret;
}

void *__stdcall callocAligned(ptrsize size) {
  void *ret;

  ret = mallocAligned(size);
  memset(ret, 0, size);

  return ret;
}

void __stdcall freeAligned(void *ret) {
  void *org;
//memloc **ml = (memloc **)ret;

  /* get start */
  org = ((void **)ret)[-1];
  /* for debug only */
  ((void **)ret)[-1] = NULL;

  /* deallocate */
//     free(org);
  delete [] org;
}
