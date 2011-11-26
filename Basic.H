/* Version: MPL 1.1/LGPL 3.0
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
 * The Original Code is scape.
 * The Modified Code is Oscape.
 *
 * The Original Code is in the public domain.
 * No Rights Reserved.
 *
 * The Initial Developer of the Modified Code is
 * Ethatron <niels@paradice-insight.us>. Portions created by The Initial
 * Developer are Copyright (C) 2011 The Initial Developer.
 * All Rights Reserved.
 *
 * Alternatively, the contents of this file may be used under the terms
 * of the GNU Library General Public License Version 3 license (the
 * "LGPL License"), in which case the provisions of LGPL License are
 * applicable instead of those above. If you wish to allow use of your
 * version of this file only under the terms of the LGPL License and not
 * to allow others to use your version of this file under the MPL,
 * indicate your decision by deleting the provisions above and replace
 * them with the notice and other provisions required by the LGPL License.
 * If you do not delete the provisions above, a recipient may use your
 * version of this file under either the MPL or the LGPL License."
 */

#ifndef BASIC_H_INCLUDED
#define BASIC_H_INCLUDED

//
// Basic.H
//
// Just defines some simple little classes and functions to make
// life a little easier.

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <cfloat>

#ifndef MAXFLOAT
#define MAXFLOAT  FLT_MAX
#define MINFLOAT  FLT_MIN
#endif

using namespace std;

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)>(b))?(b):(a))
#endif

#ifndef ABS
#define ABS(x) (((x)<0)?-(x):(x))
#endif

#ifndef True
#define True 1
#define False 0
#endif

//typedef unsigned int bool;

#define Nil NULL



// Basic array classes

template<class T>
class array {
protected:
    T *data;
    int len;
public:
    array() { data=NULL; len=0; }
    array(int l) { init(l); }
    ~array() { if (data) delete[] data; }

    void init(int l) { len = l; data = new T[l]; }
    void free() { delete[] data; }
    void resize(int l) {
	T *old = data;
	data = new T[l];
	data = (T *)memcpy(data,old,MIN(len,l)*sizeof(T));
	len = l;
	delete[] old;
    }

    T& ref(int i) {
#ifdef SAFETY
	assert( data );
	assert( i>=0 && i<len );
#endif
	return data[i];
    }

    T& operator()(int i) { return ref(i); }

    int length() { return len; }
};

template<class T>
class array2 {
protected:
    T *data;
    int width, height;
public:
    array2() { data=NULL; }
    array2(int w, int h) { init(w,h); }
    ~array2() { free(); }

    void init(int w, int h) {
      data = new T[w * h];
      width = w; height = h;
    }

    void clear() {
      memset(data, 0, sizeof(T) * width * height);
    }

    void free() {
      if (data)
	delete[] data;
      data = NULL;
    }

    const T& val(int i, int j) const {
#ifdef SAFETY
      assert( data );
      assert( i>=0 && i<width );
      assert( j>=0 && j<height );
#endif
      return data[j * width + i];
    }

    T& ref(int i, int j) {
#ifdef SAFETY
      assert( data );
      assert( i>=0 && i<width );
      assert( j>=0 && j<height );
#endif
      return data[j * width + i];
    }

    T& operator()(int i, int j) { return ref(i, j); }

    int w() const { return width; }
    int h() const { return height; }

    void bitread(ifstream& in) {
#ifdef SAFETY
      assert( data );
#endif
      char *loc = (char *)data;
      std::streamsize target = width * height * sizeof(T);
      while (target > 0) {
	in.read(loc, target);
	target -= in.gcount();
	loc += in.gcount();
      }
    }

    T *raw() { return data; }
    void set_raw(T *d) { data = d; }
};

#define	BITFIELD2
/* 4x4/8x8 per value, two-dimensional bitfield */
template<
#ifndef	_WIN64
  class bitfield = unsigned short,
  int hshift = 2,
  int wshift = 2,
  unsigned char hmask = (1 << hshift) - 1,
  unsigned char wmask = (1 << wshift) - 1,
  unsigned char cmask = (1 << (hshift + wshift)) - 1
#else
  class bitfield = unsigned __int64,
  int hshift = 3,
  int wshift = 3,
  unsigned char hmask = (1ULL << hshift) - 1,
  unsigned char wmask = (1ULL << wshift) - 1,
  unsigned char cmask = (1ULL << (hshift + wshift)) - 1
#endif
>
class bitarray2 : private array2<bitfield> {

protected:
  int width, height;

public:
  bitarray2() : array2() { }
  bitarray2(int w, int h) : array2() { init(w, h); }
  ~bitarray2() { free(); }

  void init(int w, int h) {
    width  = w;
    height = h;
    array2::init(
      (w + wmask) >> wshift, 
      (h + hmask) >> hshift
    );
  }

  void clear() {
    array2::clear();
  }

  void free() {
    array2::free();
  }

public:
  inline bool check(int i, int j) const {
    unsigned long q = (j >> hshift) * array2::width + (i >> wshift);
    bitfield p = 1; p <<= (cmask & ((j << wshift) | (i & wmask)));

    return !!(data[q] & p);
  }

  inline void mark(int i, int j) {
    unsigned long q = (j >> hshift) * array2::width + (i >> wshift);
    bitfield p = 1; p <<= (cmask & ((j << wshift) | (i & wmask)));

    data[q] = (data[q] | p);
  }

  inline void clear(int i, int j) {
    unsigned long q = (j >> hshift) * array2::width + (i >> wshift);
    bitfield p = 1; p <<= (cmask & ((j << wshift) | (i & wmask)));

    data[q] = (data[q] | p) ^ p;
  }

  inline void set(int i, int j, int b) {
    unsigned long q = (j >> hshift) * array2::width + (i >> wshift);
    bitfield p = 1; p <<= (cmask & ((j << wshift) | (i & wmask)));
    bitfield s = b; b <<= (cmask & ((j << wshift) | (i & wmask)));

    data[q] = (data[q] & ~p) ^ s;
  }

  inline bool skip(int i, int j) {
    unsigned long q = (j >> hshift) * array2::width + (i >> wshift);
    bitfield p = 1; p <<= (cmask & ((j << wshift) | (i & wmask)));
    bool res = !!(data[q] & p);
    data[q] = (data[q] | p);

    return res;
  }

public:
  /* ------------------------------------------------------ */
  inline bool operator ()(int i, int j) const { return check(i, j); }
//inline void operator =(int i, int j) const { return check(i, j); }

  /* ------------------------------------------------------ */
  class bit2 {
    friend class bitarray2;

    bitfield *data, smp;
    unsigned char hp, lp;

    bit2(bitfield *d, unsigned char pos) {
      data = d;

      hp = pos & (hmask << wshift);
      lp = pos & (wmask);

      smp = *data++ >> hp;
    }

public:
    inline bool operator *() {
      return !!((smp >> lp) & 1);
    }

    inline void operator ++() {
      /* wrap around, advance */
      if ((lp = (lp + 1) & wmask) == 0)
	smp = *data++ >> hp;
    }
  };

  bit2 ref(int i, int j) {
    unsigned long q = (j >> hshift) * array2::width + (i >> wshift);
    unsigned char o = (cmask & ((j << wshift) | (i & wmask)));

    return bit2(data + q, o);
  }

  int w() const { return width; }
  int h() const { return height; }
};

template<class T>
class buffer {
    array<T> data;
    int fill;
public:
    buffer(int len=20) { data.init(len); fill=0; }
    ~buffer() { data.free(); }

    void reset() { fill=0; }
    void freeze() { data.resize(fill); }
    void insert(T x) {
	if( fill >= data.length() )
	    data.resize(data.length()*2);
	data(fill++) = x;
    }
    int length() { return fill; }

    const T& val(int i) const { return data.val(i); }
    T& ref(int i) { return data.ref(i); }
    T& operator()(int i) { return data.ref(i); }
};


#endif   // BASIC_H_INCLUDED
