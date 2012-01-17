/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 * This is derived from the Berkeley source:
 *	@(#)random.c	5.5 (Berkeley) 7/6/88
 * It was reworked for the GNU C Library by Roland McGrath.
 * Rewritten to be reentrant by Ulrich Drepper, 1995
 */

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#define	__USE_MISC
//#include <random.h>

#if defined __USE_SVID || defined __USE_XOPEN_EXTENDED

/* Linear congruential.  */
#define	TYPE_0		0
#define	BREAK_0		8
#define	DEG_0		0
#define	SEP_0		0

/* x**7 + x**3 + 1.  */
#define	TYPE_1		1
#define	BREAK_1		32
#define	DEG_1		7
#define	SEP_1		3

/* x**15 + x + 1.  */
#define	TYPE_2		2
#define	BREAK_2		64
#define	DEG_2		15
#define	SEP_2		1

/* x**31 + x**3 + 1.  */
#define	TYPE_3		3
#define	BREAK_3		128
#define	DEG_3		31
#define	SEP_3		3

/* x**63 + x + 1.  */
#define	TYPE_4		4
#define	BREAK_4		256
#define	DEG_4		63
#define	SEP_4		1

#define	MAX_TYPES	5	/* Max number of types above.  */

__forceinline
int _random_r(struct random_data *buf);

__forceinline
int _srandom_r(unsigned int seed, struct random_data *buf) {
  int type;
  int *state;
  long int i;
  long int word;
  int *dst;
  int kc;

  if (buf == NULL)
    goto fail;
  type = TYPE_3;
  if ((unsigned int) type >= MAX_TYPES)
    goto fail;

  state = buf->state;
  /* We must make sure the seed is not 0.  Take arbitrarily 1 in this case.  */
  if (seed == 0)
    seed = 1;
  state[0] = seed;
  if (type == TYPE_0)
    goto done;

  dst = state;
  word = seed;
  kc = DEG_3;
  for (i = 1; i < kc; ++i)
    {
      /* This does:
	   state[i] = (16807 * state[i - 1]) % 2147483647;
	 but avoids overflowing 31 bits.  */
      long int hi = word / 127773;
      long int lo = word % 127773;
      word = 16807 * lo - 2836 * hi;
      if (word < 0)
	word += 2147483647;
      *++dst = word;
    }

  buf->fptr = &state[SEP_3];
  buf->rptr = &state[0];
  kc *= 10;
  while (--kc >= 0) {
    int discard = _random_r(buf);
  }

 done:
  return 0;

 fail:
  return -1;
}

__forceinline
int _random_r(struct random_data *buf) {
  int *state;
  int result;

  assert(buf != NULL);

  state = buf->state;

  {
    int *fptr = buf->fptr;
    int *rptr = buf->rptr;
    int *end_ptr = buf->end_ptr;
    int val;

    val = *fptr += *rptr;
    /* Chucking least random bit.  */
    result = (val >> 1) & 0x7fffffff;

    ++fptr;
    if (fptr >= end_ptr) {
      fptr = state;
      ++rptr;
    }
    else {
      ++rptr;
      if (rptr >= end_ptr)
	rptr = state;
    }

    buf->fptr = fptr;
    buf->rptr = rptr;
  }

  return result;
}

#endif
