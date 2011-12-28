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

//
// cfield.C
//
// Implements the CField class.

#include "scape.H"

/* linear interpolation */
#define LERP(t, a, b)	((a) + (t) * ((b) - (a)))

// CField::init --
//
void CField::init(ifstream& dataStream) {
  if (!dataStream.good()) {
    cerr << "ERROR: Input surface data does not seem to exist." << endl;
    exit(1);
  }

  data   = new RGBAdata(dataStream);
  width  = data->width();
  height = data->height();
}


// CField::free --
//
// Like the name says, free the storage that we're currently using.
//
void CField::free() {
  delete data;
}

// bilinear interpolation
// Note: this code could access off edge of array, but such bogus samples
// should be weighted by zero (fx=0 and/or fy=0).
unsigned long CField::eval_interp(Real x, Real y) const {
  int ix = (int)x; Real fx = x - ix;
  int iy = (int)y; Real fy = y - iy;

  unsigned long c0 = eval(ix, iy    ), c1 = eval(ix + 1, iy    );
  unsigned long c2 = eval(ix, iy + 1), c3 = eval(ix + 1, iy + 1);

  Real zr0 = LERP(fx, (c0 >> 24) & 0xFF, (c1 >> 24) & 0xFF);
  Real zg0 = LERP(fx, (c0 >> 16) & 0xFF, (c1 >> 16) & 0xFF);
  Real zb0 = LERP(fx, (c0 >>  8) & 0xFF, (c1 >>  8) & 0xFF);
  Real za0 = LERP(fx, (c0 >>  0) & 0xFF, (c1 >>  0) & 0xFF);
  Real zr1 = LERP(fx, (c2 >> 24) & 0xFF, (c3 >> 24) & 0xFF);
  Real zg1 = LERP(fx, (c2 >> 16) & 0xFF, (c3 >> 16) & 0xFF);
  Real zb1 = LERP(fx, (c2 >>  8) & 0xFF, (c3 >>  8) & 0xFF);
  Real za1 = LERP(fx, (c2 >>  0) & 0xFF, (c3 >>  0) & 0xFF);

  return
    ((unsigned long)LERP(fy, zr0, zr1) << 24) |
    ((unsigned long)LERP(fy, zg0, zg1) << 16) |
    ((unsigned long)LERP(fy, zb0, zb1) <<  8) |
    ((unsigned long)LERP(fy, za0, za1) <<  0)
  ;
}

/* besides enabling much less memory-consumption it also allows us to access
 * >2GB files on 32bit operating systems
 */
#define PARTITION_ROWS	(1024UL)
#define PARTITION_SIZE	(1024UL * stride)
#define PARTITION_OFFSh	(DWORD)(((unsigned __int64)begin * stride) >> 32)
#define PARTITION_OFFSl	(DWORD)(((unsigned __int64)begin * stride) >>  0)

// CField::init --
//
void CView::init(const char *dataName, int w, int h) {
  begin  = 0;
  range  = PARTITION_ROWS;
  width  = w;
  height = h;

  /* adjust to real dimensions */
  WIN32_FILE_ATTRIBUTE_DATA atr;
  if (GetFileAttributesEx(dataName, GetFileExInfoStandard, &atr)) {
    LONGLONG
      rlen = atr.nFileSizeHigh;
      rlen <<= 32;
      rlen += atr.nFileSizeLow;
    LONGLONG 
      plen = width;
      plen *= height;
      plen *= 4;

    int ss = 1;
    while (plen < rlen) {
      plen *= 4;
      ss *= 2;
    }

    width  *= ss;
    height *= ss;
  }

  /* open initial window at "0" */
  LONG     str = sizeof(unsigned char) * 4; str *= width;
  LONGLONG len = sizeof(unsigned char) * 4; len *= width; len *= height;
  LONG     leh = (LONG)(len >> 32);

  oh = (HANDLE)OpenFile(dataName, &of, OF_READ);
  if (!oh) throw runtime_error("Failed to open input file");

  mh = CreateFileMapping(oh, NULL, PAGE_READONLY, 0, 0, NULL);
  if (!mh) throw runtime_error("Failed to map input file");

  data = (const unsigned long *)MapViewOfFile(mh, FILE_MAP_READ, PARTITION_OFFSh, PARTITION_OFFSl, PARTITION_SIZE);
  if (!data) throw runtime_error("Failed to allocate input file");

  stride = str;
}


// CField::free --
//
// Like the name says, free the storage that we're currently using.
//
void CView::free() {
  UnmapViewOfFile(data);
  CloseHandle(mh);
  CloseHandle(oh);
}

// CField::set_row --
//
void CView::set_row(int y) {
  if ((y < begin) || (y >= (begin + range))) {
    int brr = 1;

    /* order threads whoever comes first, sets the new location */
#pragma omp ordered
    {
#pragma omp critical
      {
	if ((y < begin) || (y >= (begin + range))) {
	  /* free the view */
	  UnmapViewOfFile(data);

	  /* move window to include "y" */
	  begin = y - (y % range);

	  /* get a new view */
	  data = (const unsigned long *)MapViewOfFile(mh, FILE_MAP_READ, PARTITION_OFFSh, PARTITION_OFFSl, PARTITION_SIZE);
	  if (!data) throw runtime_error("Failed to allocate input file");
	}
      }
    }

    assert((y >= begin) && (y < (begin + range)));
  }
}

// bilinear interpolation
// Note: this code could access off edge of array, but such bogus samples
// should be weighted by zero (fx=0 and/or fy=0).
unsigned long CView::eval_interp(Real x, Real y) const {
  int ix = (int)x; Real fx = x - ix;
  int iy = (int)y; Real fy = y - iy;

  unsigned long c0 = eval(ix, iy    ), c1 = eval(ix + 1, iy    );
  unsigned long c2 = eval(ix, iy + 1), c3 = eval(ix + 1, iy + 1);

  Real zr0 = LERP(fx, (c0 >> 24) & 0xFF, (c1 >> 24) & 0xFF);
  Real zg0 = LERP(fx, (c0 >> 16) & 0xFF, (c1 >> 16) & 0xFF);
  Real zb0 = LERP(fx, (c0 >>  8) & 0xFF, (c1 >>  8) & 0xFF);
  Real za0 = LERP(fx, (c0 >>  0) & 0xFF, (c1 >>  0) & 0xFF);
  Real zr1 = LERP(fx, (c2 >> 24) & 0xFF, (c3 >> 24) & 0xFF);
  Real zg1 = LERP(fx, (c2 >> 16) & 0xFF, (c3 >> 16) & 0xFF);
  Real zb1 = LERP(fx, (c2 >>  8) & 0xFF, (c3 >>  8) & 0xFF);
  Real za1 = LERP(fx, (c2 >>  0) & 0xFF, (c3 >>  0) & 0xFF);

  return
    ((unsigned long)LERP(fy, zr0, zr1) << 24) |
    ((unsigned long)LERP(fy, zg0, zg1) << 16) |
    ((unsigned long)LERP(fy, zb0, zb1) <<  8) |
    ((unsigned long)LERP(fy, za0, za1) <<  0)
  ;
}
