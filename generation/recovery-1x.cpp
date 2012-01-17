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
#include "../openmp.h"
#include "../scape/cfield.H"
#include "../io/texture.hpp"

#include "geometry.hpp"
#include "rasterize.hpp"
#include "texture.hpp"

#include <windows.h>

void wrteRecovery1(string weouth, int sizex, int sizey) {
    // 1k == 32, 3k == 96, 512 == 16 */
    int resx = restx;
    int resy = resty;

    // round down, negative side would be smaller than positive side
    int offx = offtx;
    int offy = offty;

    /* get the heightfield sink */
    CSink<unsigned short> cs(weouth.data(), sizex, sizey);

#define ADJUSTMENT  0
    /* 1 more to align texels with coordinates (center) */
    int ww = rasterx + ADJUSTMENT;
    int hh = rastery + ADJUSTMENT;

    /* initialize progress */
    InitProgress((numty - minty) * (numtx - mintx), hh);

    omp_init_cancellation();
    for (int ty = minty; ty < numty; ty++) {
    for (int tx = mintx; tx < numtx; tx++) {
      int coordx = (tx - offx) * resx;
      int coordy = (ty - offy) * resy;

      /* VBs starts on index 0 */
      unsigned int i = 0, idx = 0;

      cs.set_row(ty * rastery);
      unsigned short *mem = &cs.c_ref(0, ty * rastery);
      long str = cs.get_stride();
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

#pragma omp parallel for schedule(static, (PROGRESS + 1) >> 3) shared(mem) ordered
	for (int lh = 0; lh < hh; lh++) {
	  const int h = /*(ty * rastery) +*/ lh;

          omp_skip_cancellation();
	  if (!(lh & PROGRESS)) {
	    logrf("%02dx%02d [%dx%d] %f%% (triangle)\r", ty, tx, hh, ww, (100.0f * h) / ((ty * rastery) + hh));

	    /* advance progress */
	    omp_catch_cancellation(SetProgress(-1, lh));
	  }

	  /* calculate pointer of writable position */
//	  USHORT *whgt = (USHORT *)(mem + (h * str)) + (tx * rasterx);
	  unsigned short *whgt = mem + (h * width) + (tx * rasterx);

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
	omp_end_cancellation();
      } /* fmaps */

      /* advance progress */
      SetProgress((numty - minty) * (ty - minty) + (tx - mintx) + 1);
    }
    }
}

void wrteRecovery1(string weouth, int sizex, int sizey, const char *pattern) {
  // 1k == 32, 3k == 96, 512 == 16 */
  int resx = restx;
  int resy = resty;

  // round down, negative side would be smaller than positive side
  int offx = offtx;
  int offy = offty;

  /* data-mine for the required resolution */
  int realrasterx = 0;
  int realrastery = 0;

  for (int ty = minty; ty < numty; ty++) {
  for (int tx = mintx; tx < numtx; tx++) {
    int coordx = (tx - offx) * resx;
    int coordy = (ty - offy) * resy;

    if (1) {
      /* integrate all maps in this tile */
      D3DXIMAGE_INFO *icol = chckTexture(pattern, "", coordx, coordy, min(resx, resy), false);

      if (icol) {
	realrasterx = max(realrasterx, (int)icol->Width);
	realrastery = max(realrastery, (int)icol->Height);
      }
    }
  }
  }

  /* revisit the dimensions */
  rasterx = realrasterx; width  = sizex = rasterx * numtx;
  rastery = realrastery; height = sizey = rastery * numty;

  /* get the color-map sink */
  CSink<unsigned long> cs(weouth.data(), sizex, sizey);

#define ADJUSTMENT  0
  /* 1 more to align texels with coordinates (center) */
  int ww = rasterx + ADJUSTMENT;
  int hh = rastery + ADJUSTMENT;

  /* initialize progress */
  InitProgress((numty - minty) * (numtx - mintx), hh);

  omp_init_cancellation();
  for (int ty = minty; ty < numty; ty++) {
  for (int tx = mintx; tx < numtx; tx++) {
    int coordx = (tx - offx) * resx;
    int coordy = (ty - offy) * resy;

    /* VBs starts on index 0 */
    unsigned int i = 0, idx = 0;

    cs.set_row(ty * rastery);
    unsigned long *mem = &cs.c_ref(0, ty * rastery);
    long str = cs.get_stride();
    if (1) {
      SetTopic("Integrating tile {%d,%d} maps:", coordx, coordy);

      /* integrate all maps in this tile */
      tcol = readTexture(pattern, "", coordx, coordy, min(resx, resy), false, rasterx, rastery);

      /* lock persistant output-buffer */
      D3DLOCKED_RECT rcol;
      tcol->LockRect(0, &rcol, NULL, 0);
      ULONG *mcol = (ULONG *)rcol.pBits;

      SetTopic("Saving tile {%d,%d}:", coordx, coordy);

#pragma omp parallel for schedule(static, (PROGRESS + 1) >> 3) shared(mem) ordered
      for (int lh = 0; lh < hh; lh++) {
	const int h = /*(ty * rastery) +*/ lh;

        omp_skip_cancellation();
	if (!(lh & PROGRESS)) {
	  logrf("%02dx%02d [%dx%d] %f%% (triangle)\r", ty, tx, hh, ww, (100.0f * h) / ((ty * rastery) + hh));

	  /* advance progress */
	  omp_catch_cancellation(SetProgress(-1, lh));
	}

	/* calculate pointer of writable position */
//	ULONG *wcol = (ULONG *)(mem + (h * str)) + (tx * rasterx);
	unsigned long *wcol = mem + (h * width) + (tx * rasterx);

	for (int lw = 0; lw < ww; lw++) {
	  const int w = /*(tx * rasterx) +*/ lw;

	  /* restore the height */
	  unsigned long C  = mcol[(lh * rastery) + lw];

	  ULONG a = (C >> 24) & 0xFF; /*a*/
	  ULONG r = (C >> 16) & 0xFF; /*a*/
	  ULONG g = (C >>  8) & 0xFF; /*a*/
	  ULONG b = (C >>  0) & 0xFF; /*a*/

	  unsigned long
	  color  = 0;
	  color |= (r << 24);
	  color |= (g << 16);
	  color |= (b <<  8);
	  color |= (a <<  0);

	  /* serial write to persistant output-buffer */
	  *wcol++ = (unsigned long)(color);
	  // O ^= oz, which is Z in image-space
	}
      }

      tcol->UnlockRect(0);
      tcol->Release();
      tcol = NULL;

      omp_end_cancellation();
    } /* fmaps */

    /* advance progress */
    SetProgress((numty - minty) * (ty - minty) + (tx - mintx) + 1);
  }
  }
}
