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
#include "../scape/hfield.H"

#include "texture.hpp"
#include "geometry.hpp"
#include "rasterize.hpp"
#include "../io/texture.hpp"

#ifdef	SPLIT_ON_INJECTION
void wrteNormals0(bool fmaps, bool nmaps, bool hmaps, const HField& hf, const char *pattern) {
    // 1k == 32, 3k == 96, 512 == 16 */
    int resx = restx;
    int resy = resty;

    // round down, negative side would be smaller than positive side
    int offx = offtx;
    int offy = offty;

#define ADJUSTMENT  0
    /* 1 more to align texels with coordinates (center) */
    int ww = rasterx + ADJUSTMENT, www = rasterx / 2 + ADJUSTMENT;
    int hh = rastery + ADJUSTMENT, hhh = rastery / 2 + ADJUSTMENT;

    int gw = hf.getWidth();
    int gh = hf.getHeight();

    /* allocate persistant output-buffer */
    if (nmaps) {
      pD3DDevice->CreateTexture(www, hhh, 0, 0, D3DFMT_A16B16G16R16, D3DPOOL_MANAGED, &tnrm, NULL);
      if (!tnrm) throw runtime_error("Failed to allocate texture");
    }

    if (hmaps) {
      pD3DDevice->CreateTexture(www, hhh, 0, 0, D3DFMT_A16B16G16R16, D3DPOOL_MANAGED, &thgt, NULL);
      if (!thgt) throw runtime_error("Failed to allocate texture");
    }

    /* initialize progress */
    InitProgress((numty - minty) * (numtx - mintx), hh);

    omp_init_cancellation();
    for (int ty = minty; ty < numty; ty++) {
    for (int tx = mintx; tx < numtx; tx++) {
      int coordx = (tx - offx) * resx;
      int coordy = (ty - offy) * resy;

      /* VBs starts on index 0 */
      unsigned int i = 0, idx = 0;

      /* Here is how I compute terrain normals. It is like your MWE (but weighted by
       * triangle area, because the cross products aren't normalized), and only using
       * the 4 surrounding vertices (instead of 6). As you mentioned in your article,
       * you can simplify the cross-product a lot, and the result here is an extremely
       * simple algorithm:
       *
       * Given 4 adjacent points in a uniform grid with a spacing of 1: A, B, C, D
       *
       *        B
       *        |
       *     C--0--A
       *        |
       *        D
       *
       * and a vertical (Z) scale factor s, the desired normal is:
       *
       *     N = cross(A,B) + cross(B,C) + cross(C,D) + cross(D,A)
       *     N = normalize( N )
       *
       * Most of the terms in the sum of the cross products cancel out and the result is this:
       *
       *     Nx = 2 / s * (Cz - Az)
       *     Ny = 2 / s * (Dz - Bz)
       *     Nz = 4 / s2
       *     N = normalize( N )
       *
       * Since N is normalized in the end, the computation can be further simplified by
       * scaling the vector by s/2 before normalizing:
       *
       *     Nx = Cz - Az
       *     Ny = Dz - Bz
       *     Nz = 2 / s
       *     N = normalize( N )
       *
       * If the spacing between the vertices in the grid is sxy instead of 1, then s = sz / sxy
       */
      Real zscale = (2.0f * sizescale) / heightscale;

      if (nmaps && !skipTexture(pattern, "_fn", coordx, coordy, min(resx, resy), true)) {
	/* lock persistant output-buffer */
	D3DLOCKED_RECT rnrm;
	tnrm->LockRect(0, &rnrm, NULL, 0);
	USHORT *mnrm = (USHORT *)rnrm.pBits;

	SetTopic("Calculating tile {%d,%d} normals:", coordx, coordy);

	/* extract values out of the tile-window */
#pragma omp parallel for schedule(static, (PROGRESS + 1) >> 3) shared(mnrm, hf)
	for (int lh = 0; lh < hh; lh++) {
	  const int h = (ty * rastery) + lh;

          omp_skip_cancellation();
	  if (!(lh & PROGRESS)) {
	    logrf("%02dx%02d [%dx%d] %f%%\r", ty, tx, hhh, www, (100.0f * h) / ((ty * rastery) + hh));

	    /* advance progress */
	    omp_catch_cancellation(SetProgress(-1, lh));
	  }

	  /* calculate pointer of writable position */
	  USHORT *wnrm = mnrm + ((lh >> 1) * www) * 4;

	  for (int lw = 0; lw < ww; lw++) {
	    const int w = (tx * rasterx) + lw;

	    Real O = hf.getZ(w, h);

	    Real A = (((w + 1) < gw) ? hf.getZ(w + 1, h) : O);
	    Real B = (((h - 1) >  0) ? hf.getZ(w, h - 1) : O);
	    Real C = (((w - 1) >  0) ? hf.getZ(w - 1, h) : O);
	    Real D = (((h + 1) < gh) ? hf.getZ(w, h + 1) : O);

	    Real X = (((w + 1) < gw && (h + 1) < gh) ? hf.getZ(w + 1, h + 1) : O);
	    Real Y = (((w - 1) >  0 && (h - 1) >  0) ? hf.getZ(w - 1, h - 1) : O);
	    Real Z = (((w - 1) >  0 && (h + 1) < gh) ? hf.getZ(w - 1, h + 1) : O);
	    Real W = (((w + 1) < gh && (h - 1) >  0) ? hf.getZ(w + 1, h - 1) : O);

	    Real dimH = 1.0f;
	    Real dimV = 1.0f / sqrt(2.0f);

	    /* horizontal distance == 1.0 */
	    Real n1x = -(A - C);//(C - A);
	    Real n1y =  (B - D);//(D - B);
	    Real n1z = zscale / (heightadjust1 * dimH);

	    /* diagonal distance == sqrt(2.0) */
	    Real n2x = -(W - Z);//(Z - W);
	    Real n2y =  (Y - X);//(X - Y);
	    Real n2z = zscale / (heightadjust1 * dimV);

	    Real mag1 = sqrt((n1x * n1x) + (n1y * n1y) + (n1z * n1z));
	    Real mag2 = sqrt((n2x * n2x) + (n2y * n2y) + (n2z * n2z));

	    n1x /= mag1;
	    n1y /= mag1;
	    n1z /= mag1;

	    n2x /= mag2;
	    n2y /= mag2;
	    n2z /= mag2;

	    /* weight the diagonal vector sqrt(2) less */
	    Real nx = (n1x + n2x / sqrt(2.0f));
	    Real ny = (n1y + n2y / sqrt(2.0f));
	    Real nz = (n1z + n2z / sqrt(2.0f));

	    Real mag = sqrt((nx * nx) + (ny * ny) + (nz * nz));

	    nx /= mag;
	    ny /= mag;
	    nz /= mag;

	    O = max(0x0000, min(0xFFFF, O));

	    /* output only even pixels */
	    if (!((lh | lw) & 1)) {
	      /* serial write to persistant output-buffer */
	      *wnrm++ = (int)((((nx) * 0.5) + 0.5) * 0xFFFF); // R
	      *wnrm++ = (int)((((ny) * 0.5) + 0.5) * 0xFFFF); // G
	      *wnrm++ = (int)((((nz) * 0.5) + 0.5) * 0xFFFF); // B
	      *wnrm++ =                              0xFFFF ; // A
	    }
	  }
	}

	tnrm->UnlockRect(0);
        omp_end_cancellation();

	SetTopic("Writing tile {%d,%d} normals:", coordx, coordy);

	/* flush persistant output-buffer to disk */
	wrteTexture(tnrm, pattern, "_fn", coordx, coordy, min(resx, resy), true);
      } /* nmaps */

      if (hmaps && !skipTexture(pattern, "_fh", coordx, coordy, min(resx, resy), false)) {
	/* lock persistant output-buffer */
	D3DLOCKED_RECT rhgt;
	thgt->LockRect(0, &rhgt, NULL, 0);
	USHORT *mhgt = (USHORT *)rhgt.pBits;

	SetTopic("Rasterizing tile {%d,%d} faces:", coordx, coordy);

	/* allocate temporary rasterization buffer */
        hmap_w = ww;
        hmap_h = hh;
        hmap_o = new float[ww * hh]; memset(hmap_o, 0, sizeof(float) * ww * hh);

	/* rasterize all triangles in this tile */
	fiterator itf; size_t fsz = SectorFaceO[ty][tx].size(), f = 0;
	for (itf = SectorFaceO[ty][tx].begin(); itf != SectorFaceO[ty][tx].end(); itf++, f++) {
	  if (!(f & PROGRESS))
	    logrf("%02dx%02d [%dx%d] %f%% (rasterize %fu²)\r", ty, tx, hhh, www, (100.0f * f) / fsz, (*itf)->narea);

	  class objVertex *v0 = (*itf)->v[0];
	  class objVertex *v1 = (*itf)->v[1];
	  class objVertex *v2 = (*itf)->v[2];

	  Rasterizer::DrawTriangle(1.0, (*itf), v0, v1, v2);
	}

	SetTopic("Calculating tile {%d,%d} deviations:", coordx, coordy);

	float hdev_p = 0.0;
	float hdev_n = 0.0;

#pragma omp parallel for schedule(static, (PROGRESS + 1) >> 3) shared(mhgt, hf)
	for (int lh = 0; lh < hh; lh++) {
	  const int h = (ty * rastery) + lh;

          omp_skip_cancellation();
	  if (!(lh & PROGRESS)) {
	    logrf("%02dx%02d [%dx%d] %f%% (triangle)\r", ty, tx, hhh, www, (100.0f * h) / ((ty * rastery) + hh));

	    /* advance progress */
	    omp_catch_cancellation(SetProgress(-1, lh));
	  }

	  /* calculate pointer of writable position */
	  USHORT *whgt = mhgt + ((lh >> 1) * www) * 4;

	  for (int lw = 0; lw < ww; lw++) {
	    const int w = (tx * rasterx) + lw;

	    /* restore the normals */
	    float O  = hmap_o[(lh * hmap_w) + lw];

	    /* write interpolated TNB height-map ----------------------- */
	    float o = (float)hf.getZ(w, h) - O;

	    if (O > 0.0) {
	      hdev_p = max(o, hdev_p);
	      hdev_n = min(o, hdev_n);
	    }

	    /* difference heightmap (amplified) -----------------------
	    O = O * 0x100 + 0x8000; */
	    O = max(0x0000, min(0xFFFF, O + 0x8000));

	    /* output only even pixels */
	    if (!((lh | lw) & 1)) {
	      /* serial write to persistant output-buffer */
	      *whgt++ = (int)((((O ) * 1.0f) + 0.0f) * 0x0001); // L
	      *whgt++ = (int)((((O ) * 1.0f) + 0.0f) * 0x0001); // L
	      *whgt++ = (int)((((O ) * 1.0f) + 0.0f) * 0x0001); // L
	      *whgt++ = (int)((((O ) * 1.0f) + 0.0f) * 0x0001); // L
	    }
	  }
	}

	delete[] hmap_o; hmap_o = NULL;

	logpf("%02dx%02d: Heightmap deviation is [%f,%f]\n", ty, tx, hdev_n, hdev_p);

	thgt->UnlockRect(0);
        omp_end_cancellation();

	SetTopic("Writing tile {%d,%d} deviations:", coordx, coordy);

	/* flush persistant output-buffer to disk */
	wrteTexture(thgt, pattern, "_fh", coordx, coordy, min(resx, resy), false);
      } /* hmaps */

      /* advance progress */
      SetProgress((numty - minty) * (ty - minty) + (tx - mintx) + 1);
    }
    }

    if (nmaps) tnrm->Release(); tnrm = NULL;
    if (hmaps) thgt->Release(); thgt = NULL;
}

void wrteNormals0(bool fmaps, bool nmaps, bool hmaps, const HField& hf) {
    wrteNormals0(fmaps, nmaps, hmaps, hf, "%02d.%02d.%02d.%02d");
}
#endif

void wrteColors0(bool cmaps, CView& cf, const char *pattern) {
    // 1k == 32, 3k == 96, 512 == 16 */
    int resx = restx;
    int resy = resty;

    // round down, negative side would be smaller than positive side
    int offx = offtx;
    int offy = offty;

#define ADJUSTMENT  0
    /* 1 more to align texels with coordinates (center) */
    int ww = rasterx + ADJUSTMENT, www = rasterx / 2 + ADJUSTMENT;
    int hh = rastery + ADJUSTMENT, hhh = rastery / 2 + ADJUSTMENT;

    int gw = cf.get_width();
    int gh = cf.get_height();

    int tw = gw / width;
    int th = gh / height;

    /* number of pixels to merge */
    int mw = tw << 1;
    int mh = th << 1;

    /* allocate persistant output-buffer */
    if (cmaps) {
      pD3DDevice->CreateTexture(www, hhh, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &tcol, NULL);
      if (!tcol) throw runtime_error("Failed to allocate texture");
    }

    /* initialize progress */
    InitProgress((numty - minty) * (numtx - mintx), hh);

    omp_init_cancellation();
    for (int ty = minty; ty < numty; ty++) {
    for (int tx = mintx; tx < numtx; tx++) {
      int coordx = (tx - offx) * resx;
      int coordy = (ty - offy) * resy;

      /* VBs starts on index 0 */
      unsigned int i = 0, idx = 0;

      if (cmaps && !skipTexture(pattern, "", coordx, coordy, min(resx, resy), false)) {
	/* lock persistant output-buffer */
	D3DLOCKED_RECT rcol;
	tcol->LockRect(0, &rcol, NULL, 0);
	UCHAR *mcol = (UCHAR *)rcol.pBits;

	SetTopic("Calculating tile {%d,%d} colors:", coordx, coordy);

	/* extract values out of the tile-window */
#pragma omp parallel for schedule(static, (PROGRESS + 1) >> 3) shared(mcol, cf) ordered
	for (int lh = 0; lh < hhh; lh++) {
	  const int h = th * (ty * rastery) + mh * (lh);

	  /* TODO: critical */
          omp_skip_cancellation();
	  cf.set_row(h);
	  if (!(lh & PROGRESS)) {
	    logrf("%02dx%02d [%dx%d] %f%%\r", ty, tx, hhh, www, (100.0f * h) / ((ty * rastery) + hh));

	    /* advance progress */
	    omp_catch_cancellation(SetProgress(-1, lh));
	  }

	  /* calculate pointer of writable position */
	  UCHAR *wcol = mcol + (lh * www) * 4;

	  for (int lw = 0; lw < www; lw++) {
	    const int w = tw * (tx * rasterx) + mw * (lw);

	    unsigned long R = 0;
	    unsigned long G = 0;
	    unsigned long B = 0;
	    unsigned long A = 0;

	    for (int kh = 0; kh < mh; kh++)
	    for (int kw = 0; kw < mw; kw++) {
	      unsigned long c = cf.eval(w + kw, h + kh);
	      unsigned char r = (c >> 24) & 0xFF;
	      unsigned char g = (c >> 16) & 0xFF;
	      unsigned char b = (c >>  8) & 0xFF;
	      unsigned char a =             0xFF;

	      R += r;
	      G += g;
	      B += b;
	      A += a;
	    }

	    R /= mw * mh;
	    G /= mw * mh;
	    B /= mw * mh;
	    A /= mw * mh;

	    /* serial write to persistant output-buffer */
	    *wcol++ = (UCHAR)B; // R
	    *wcol++ = (UCHAR)G; // G
	    *wcol++ = (UCHAR)R; // B
	    *wcol++ = (UCHAR)A; // A
	  }
	}

	tcol->UnlockRect(0);
        omp_end_cancellation();

	SetTopic("Writing tile {%d,%d} colors:", coordx, coordy);

	/* flush persistant output-buffer to disk */
	wrteTexture(tcol, pattern, "", coordx, coordy, min(resx, resy), false);
      } /* cmaps */

      /* advance progress */
      SetProgress((numty - minty) * (ty - minty) + (tx - mintx) + 1);
    }
    }

    if (cmaps) tcol->Release(); tcol = NULL;
}

void wrteColors0(bool cmaps, CView& cf) {
    wrteColors0(cmaps, cf, "%02d.%02d.%02d.%02d");
}
