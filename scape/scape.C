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
// scape.C
//
// The basic toplevel program to drive the terrain simplification.
//

#include "../globals.h"
#include "scape.H"
#include <string.h>
#include <time.h>
#include <math.h>
#include <limits.h>
//#include <sys/resource.h>

extern int update_cost;

/* ---------------------------------------------------- */

void greedy_insert(SimplField& ter) {
  int p = (limit + 499) / 500;
  for (int i = 5; i <= limit && ter.select_new_point(); i++) {
    if ((i % p) == 0)
      SetProgress(i, ter.getCurrentError());
  }
}

void greedy_insert_error(SimplField& ter) {
  int p = (limit + 499) / 500;
  for (int i = 5; i <= limit && ter.select_new_points(5.0); i++) {
    if ((i % p) == 0)
      SetProgress(i, ter.getCurrentError());
  }
}

/* ---------------------------------------------------- */

#ifdef CMDLINE
#include "../generation/geometry.hpp"
#include "../generation/texture.hpp"

#include "../io/geometry.hpp"
#include "../io/texture.hpp"
#include "../io/data.hpp"

int main(int argc,char **argv) {
  repfile = stderr;
  logfile = stdout;

  srandom(0);
  parseCommandline(argc, argv);

  if (datadep)
    cout << "# doing data-dependent triangulation" << endl
	<< "#  with "
	<< (criterion==SUMINF ? "sum" : criterion==MAXINF ?
	    "max" : criterion==SUM2 ? "sqerr" : "abn")
	<< " criterion, threshold "
	<< qual_thresh << ", and fraction " << area_thresh
	<< endl;
  else
    cout << "# doing Delaunay triangulation" << endl;

  cout << "# emphasis=" << emphasis << " npoint=" << limit << endl;

  if( parallelInsert ) {
    cout << "# Using constant threshold parallel insert:  thresh=";
    cout << thresh << endl;
  }

  if( multinsert ) {
    cout << "# Using fractional threshold insert:  thresha="<<alpha;
    cout << endl;
  }

  // initialize Tootle
  if (TootleInit() != TOOTLE_OK)
    return 0;

  ifstream mntns(dataFile, ios::binary);
  HField H(mntns, texFile, greyFile);
  SimplField ter(&H);

  width  = H.getWidth();
  height = H.getHeight();

  if (rasterx > width ) rasterx = width;
  if (rastery > height) rastery = height;
  tilesx = (int)floor((Real)width  / rasterx),
  tilesy = (int)floor((Real)height / rastery);

  while ((Real)width  != (tilesx * rasterx))
    tilesx = (int)floor((Real)width  / --rasterx);
  while ((Real)height != (tilesy * rastery))
    tilesy = (int)floor((Real)height / --rastery);

#ifdef	SPLIT_ON_INJECTION
  /* damit, the heightfield is [0,width), not inclusive */
  for (int h = 0; h <= height; h += rastery)
  for (int w = 0; w <=  width; w += rasterx)
    ter.select_new_point(min(w, width - 1), min(h, height - 1));
#endif

  greedy_insert(ter);
//greedy_insert_error(ter);

#ifdef	MATCH_WITH_HIRES
  readPointsFile(ter);
#endif

  cout << "Writing meshes and textures ..." << endl;

  TransferGeometry(ter);
  TransferGeometry();
  RevisitGeometry();

  if (!nomodel) {
#ifdef	MATCH_WITH_HIRES
    wrtePointsFile(ter);
#endif

    wrteTIN(ter);
    wrteWavefront(ter);
    wrteNIF(ter);
    wrteDXMesh(ter);
  }

#ifdef	SPLIT_ON_INJECTION
  if (writeobjmaps || writefcemaps) {
    if ( writeloresmaps)
      wrteNormals0(writefcemaps, true, true, H);
    if (!writenoresmaps)
      wrteNormals1(writefcemaps, true, true, H);
    if ( writehiresmaps)
      wrteNormals2(writefcemaps, true, true, H);
  }
#else
  if (writeobjmaps || writefcemaps) {
    write_nrm1024(ter);
    if (writehiresmaps)
      write_nrm2047(ter);
  }
#endif

  // clean up tootle
  TootleCleanup();

  // You can output a PostScript version of the mesh by uncommenting the
  // following line.
  //output_ps(ter);

  return 0;
}
#endif
