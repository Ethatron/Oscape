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

void readWaterFile(SimplField& ter, const char *name) {
  FILE *pts = fopen(name, "rb");
  if (!pts)
    return;

  // round down, negative side would be smaller than positive side
  int ox = (tilesx / 2) * (int)(rasterx / 32);
  int oy = (tilesy / 2) * (int)(rastery / 32);
  int gw = ter.getHField()->getWidth();
  int gh = ter.getHField()->getHeight();

  /* calculate number of lines (TODO: make it faster) */
  char buf[512]; int lines = 0;
  while (fgets(buf, 512, pts) != NULL)
    lines++;

  /* initialize progress */
  fseek(pts, 0, SEEK_SET);
  int p = (lines + 499) / 500;
  if (lines)
    InitProgress(lines, 0.0);

  /* read in every point in the file and update progress */
  int x, y, bv = 0, cnt = 0; long l; float f;
  while (fgets(buf, 256, pts) != NULL) {
//while (fscanf(pts, "%d %d\n", &x, &y) == 2) {
    /* comments / sections allowed */
    if ((buf[0] != '\0') &&
	(buf[0] != ';') &&
	(buf[0] != '[') &&
	(buf[0] != '#')) {
      /* can we also interprete it? */
      if (sscanf(buf, "%4d %4d 0x%08x %f\n", &x, &y, &l, &f) == 4) {
	/* from world-space to heightfield-space */
	Point2d p(
	  (x + ox) * 32,
	  (y + oy) * 32
	);

	/* real water */
	if ((l != 0x7F7FFFFF) &&
	    (l != 0x4F7FFFC9) &&
	    (l != 0xCF000000))
	  RegisterWater(p, f + 14000, false);
      }
    }

    /* advance progress */
    if ((cnt % p) == 0)
      SetProgress(cnt, 0.0);

    cnt++;
  }

  logpf("%d water-levels added\n", bv);
  fclose(pts);
}
