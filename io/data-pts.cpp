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

#include "../globals.h"
#include "../scape/simplfield.H"
#include "../generation/geometry.hpp"

#ifdef	MATCH_WITH_HIRES
void readPointsFile(SimplField& ter, const char *name) {
  FILE *pts = fopen(name, "rb");
  if (!pts)
    return;

  // round down, negative side would be smaller than positive side
  int ox = (tilesx / 2) * (int)(sizescale * rasterx);
  int oy = (tilesy / 2) * (int)(sizescale * rastery);
  int gw = ter.getHField()->getWidth();
  int gh = ter.getHField()->getHeight();

  /* calculate number of lines (TODO: make it faster) */
  char buf[256]; int lines = 0;
  while (fgets(buf, 256, pts) != NULL)
    lines++;

  /* initialize progress */
  fseek(pts, 0, SEEK_SET);
  int p = (lines + 499) / 500;
  if (lines)
    InitProgress(lines, ter.getCurrentError());

  /* read in every point in the file and update progress */
  int x, y, bv = 0, cnt = 0; Real fx, fy;
  while (fgets(buf, 256, pts) != NULL) {
//while (fscanf(pts, "%d %d\n", &x, &y) == 2) {
    /* comments / sections allowed */
    if ((buf[0] != '\0') &&
        (buf[0] != ';') &&
        (buf[0] != '[') &&
        (buf[0] != '#')) {
      /* can we also interprete it? */
      if (sscanf(buf, "%d %d\n", &x, &y) == 2) {
	/* from world-space to heightfield-space */
	x = (int)floor((Real)(x + ox) / sizescale);
	y = (int)floor((Real)(y + oy) / sizescale);

	/* heightfield-space coordinates */
	int sx = (int)floor((1.0f / rasterx) * x) * rasterx;
	int sy = (int)floor((1.0f / rastery) * y) * rastery;

	/* collect tile-border vertices only */
	if ((x == sx) ||
	    (y == sy) ||
	    (x == gw) ||
	    (y == gh)) {
	  x = min(x, gw - 1);
	  y = min(y, gh - 1);

	  logrf("%d/%d: ", bv, cnt);
//	  bv += ter.select_new_point(x, y) ? 1 : 0;
	  bv += ter.select_fix_point(x, y) ? 1 : 0;
	}

	/* TODO: add fractional support (requires identification if it's a custom point file) */
      }
    }

    /* advance progress */
    if ((cnt % p) == 0)
      SetProgress(cnt, ter.getCurrentError());

    cnt++;
  }

  logpf("%d border-vertices added\n", bv);
  fclose(pts);
}

void wrtePointsFile(SimplField& ter, const char *name) {
  FILE *pts = fopen(name, "wb");
  if (!pts)
    return;

  // round down, negative side would be smaller than positive side
  int ox = (tilesx / 2) * (int)(sizescale * rasterx);
  int oy = (tilesy / 2) * (int)(sizescale * rastery);
  int gw = ter.getHField()->getWidth();
  int gh = ter.getHField()->getHeight();
  int bv = 0;

  set<class objVertex *, struct V>::const_iterator itv;
  for (itv = Vertices.begin(); itv != Vertices.end(); itv++) {
    class objVertex *vo = (*itv);

    /* heightfield-space coordinates */
    Real sx = floor((1.0f / rasterx) * vo->vtx.x) * rasterx;
    Real sy = floor((1.0f / rastery) * vo->vtx.y) * rastery;

    /* collect tile-border vertices */
    if ((vo->vtx.x == sx) ||
	(vo->vtx.y == sy) ||
	(vo->vtx.x == gw) ||
	(vo->vtx.y == gh)) {
      bv++;

      /* world-space coordinates */
      fprintf(pts, "%d %d\n", (int)(vo->x - ox), (int)(vo->y - oy));
    }
  }

  logpf("%d border-vertices preserved\n", bv);
  fclose(pts);
}

void readPointsFile(SimplField& ter) {
  readPointsFile(ter, "out.pts");
}

void wrtePointsFile(SimplField& ter) {
  wrtePointsFile(ter, "out.pts");
}
#endif
