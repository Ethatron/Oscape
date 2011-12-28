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
 * The Original Code is Oscape.
 *
 * The Initial Developer of the Original Code is
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

#include "../globals.h"
#include "../scape/simplfield.H"

#include "geometry.hpp"
#include "rasterize.hpp"

#include <windows.h>

/* besides enabling much less memory-consumption it also allows us to access
 * >2GB files on 32bit operating systems
 */
#define PARTITION_ROWS	(rastery)
#define PARTITION_SIZE	(rastery * str)
#define PARTITION_OFFSh	(DWORD)(((unsigned __int64)(ty * rastery) * str) >> 32)
#define PARTITION_OFFSl	(DWORD)(((unsigned __int64)(ty * rastery) * str) >>  0)

void wrteRecovery1(string weouth, int sizex, int sizey) {
  bool rethrow = false;
  char rethrowing[256];

  /* create output file */
//OFSTRUCT of; HANDLE ohx = (HANDLE)OpenFile(weoutx.data(), &of, OF_READWRITE);
  HANDLE oh = CreateFile(weouth.data(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (oh == (HANDLE)HFILE_ERROR) throw runtime_error("Failed to open output file");

  /* mark them sparse (cool for zeroes) */
  DWORD ss; BOOL
  s = DeviceIoControl(oh, FSCTL_SET_SPARSE, NULL, 0, NULL, 0, &ss, NULL);

//DWORD lenh = GetFileSize(oh, NULL);
  DWORD str =         sizex * sizeof(unsigned short) * 1;
  DWORD len = sizey * sizex * sizeof(unsigned short) * 1;

  FILE_ZERO_DATA_INFORMATION z;
  z.FileOffset.QuadPart = 0;
  z.BeyondFinalZero.QuadPart = len;

  /* mark all zeros (cool for zeroes) */
  s = DeviceIoControl(oh, FSCTL_SET_ZERO_DATA, &z, sizeof(z), NULL, 0, &ss, NULL);

  SetFilePointer(oh, len, NULL, FILE_BEGIN); BOOL sfs = SetEndOfFile(oh);
  if (!sfs) throw runtime_error("Failed to resize output file");

  HANDLE mh = CreateFileMapping(oh, NULL, PAGE_READWRITE, 0, 0, NULL);
  if (!mh) throw runtime_error("Failed to map output file");

    // 1k == 32, 3k == 96, 512 == 16 */
    int resx = rasterx / 32;
    int resy = rastery / 32;

    // round down, negative side would be smaller than positive side
    int offx = tilesx / 2;
    int offy = tilesy / 2;

#define ADJUSTMENT  0
    /* 1 more to align texels with coordinates (center) */
    int ww = rasterx + ADJUSTMENT;
    int hh = rastery + ADJUSTMENT;

    /* initialize progress */
    InitProgress((numty - minty) * (numtx - mintx), hh);

    for (int ty = minty; ty < numty; ty++) {
      char *mem = (char *)MapViewOfFile(mh, FILE_MAP_ALL_ACCESS, PARTITION_OFFSh, PARTITION_OFFSl, PARTITION_SIZE);
      if (!mem) throw runtime_error("Failed to allocate output file");

    for (int tx = mintx; tx < numtx; tx++) {
      int coordx = (tx - offx) * resx;
      int coordy = (ty - offy) * resy;

      /* VBs starts on index 0 */
      unsigned int i = 0, idx = 0;

      if (1) {
	SetTopic("Rasterizing tile {%d,%d} faces:", coordx, coordy);

	/* allocate temporary rasterization buffer */
        hmap_w = ww;
        hmap_h = hh;
        hmap_o = new float[ww * hh]; memset(hmap_o, 0, sizeof(float) * ww * hh);

	/* rasterize all triangles in this tile */
	fiterator itf; size_t fsz = SectorFaces[ty][tx].size(), f = 0;
	for (itf = SectorFaces[ty][tx].begin(); itf != SectorFaces[ty][tx].end(); itf++, f++) {
	  if (!(f & PROGRESS))
	    logrf("%02dx%02d [%dx%d] %f%% (rasterize %fu²)\r", ty, tx, hh, ww, (100.0f * f) / fsz, (*itf)->narea);

	  class objVertex *v0 = (*itf)->v[0];
	  class objVertex *v1 = (*itf)->v[1];
	  class objVertex *v2 = (*itf)->v[2];

	  Rasterizer::DrawTriangle(1.0, (*itf), v0, v1, v2);
	}

	SetTopic("Saving tile {%d,%d}:", coordx, coordy);

#pragma omp parallel for schedule(static, (PROGRESS + 1) >> 3) shared(mem)
	for (int lh = 0; lh < hh; lh++) {
	  const int h = /*(ty * rastery) +*/ lh;

	  if (!(lh & PROGRESS)) {
	    logrf("%02dx%02d [%dx%d] %f%% (triangle)\r", ty, tx, hh, ww, (100.0f * h) / ((ty * rastery) + hh));

	    /* advance progress */
	    SetProgress(-1, lh);
	  }

	  /* calculate pointer of writable position */
	  USHORT *whgt = (USHORT *)(mem + (h * str)) + (tx * rasterx);

	  for (int lw = 0; lw < ww; lw++) {
	    const int w = /*(tx * rasterx) +*/ lw;

	    /* restore the height */
	    float O  = hmap_o[(lh * hmap_w) + lw];

	    /* serial write to persistant output-buffer */
	    *whgt++ = (int)max(0, O);
	    // O ^= oz, which is Z in image-space
	  }
	}

	delete[] hmap_o; hmap_o = NULL;
      } /* fmaps */

      /* advance progress */
      SetProgress((numty - minty) * (ty - minty) + (tx - mintx) + 1);
    }

      UnmapViewOfFile(mem);
    }

    CloseHandle(mh);
    CloseHandle(oh);
}
