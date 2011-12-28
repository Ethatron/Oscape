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
#include "../scape/hfield.H"

#include "texture.hpp"
#include "geometry.hpp"
#include "rasterize.hpp"
#include "../io/texture.hpp"

void wrteColors3(bool cmaps, CView& cf, const char *pattern) {
  // 1k == 32, 3k == 96, 512 == 16 */
  int resx = rasterx / 32;
  int resy = rastery / 32;

  // round down, negative side would be smaller than positive side
  int offx = tilesx / 2;
  int offy = tilesy / 2;

#define ADJUSTMENT  0
  /* 1 more to align texels with coordinates (center) */
  int ww = rasterx + ADJUSTMENT, www = rasterx * 4 + ADJUSTMENT;
  int hh = rastery + ADJUSTMENT, hhh = rastery * 4 + ADJUSTMENT;

  int gw = cf.get_width();
  int gh = cf.get_height();

  int tw = gw / width ; assert(tw > 2);
  int th = gh / height; assert(th > 2);

  /* number of pixels to merge */
  int mw = tw >> 2; assert(mw >= 1);
  int mh = th >> 2; assert(mh >= 1);

  /* allocate persistant output-buffer */
  if (cmaps) {
    pD3DDevice->CreateTexture(www, hhh, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &tcol, NULL);
    if (!tcol) throw runtime_error("Failed to allocate texture");
  }

  /* initialize progress */
  InitProgress((numty - minty) * (numtx - mintx), hh);

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
#pragma omp parallel for schedule(static, (PROGRESS + 1) >> 3) shared(mcol, cf)
      for (int lh = 0; lh < hhh; lh++) {
	const int h = th * (ty * rastery) + mh * (lh);

	/* TODO: critical */
	cf.set_row(h);
	if (!(lh & PROGRESS)) {
	  logrf("%02dx%02d [%dx%d] %f%%\r", ty, tx, hhh, www, (100.0f * h) / ((ty * rastery) + hh));

	  /* advance progress */
	  SetProgress(-1, lh);
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

void wrteColors3(bool cmaps, CView& cf) {
  wrteColors3(cmaps, cf, "%02d.%02d.%02d.%02d");
}
