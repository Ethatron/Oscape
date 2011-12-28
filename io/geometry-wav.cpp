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

ostream *obj_out = NULL;

void wrteWavefront(const char *name) {
  set<class objVertex *, struct V>::iterator itv;
  vector<class objFace *>::iterator itf;
  int idx = 1;

  ofstream obj(name, ios::binary);
  obj_out = &obj;

  obj.setf(ios::fixed, ios::floatfield);

  obj.precision(4); //
  for (itv = Vertices.begin(); itv != Vertices.end(); itv++) {
    /* assign index the moment of writing it out */
    (*itv)->idx = idx++;

    obj << "v "  << (*itv)->x  << " " << (*itv)->y  << " " << (*itv)->z  << endl;
  }

  /* these are sort of meaning-less as we have
    * [0,1] texture coordinated _per tile_ later on
    */
  if (emituvs) {
    obj.precision(8); // 1/2^23
    for (itv = Vertices.begin(); itv != Vertices.end(); itv++) {
      obj << "vt " << (*itv)->tx << " " << (*itv)->ty << endl;
    }
  }

  if (emitnrm) {
    obj.precision(5); // 1/2^16
    for (itv = Vertices.begin(); itv != Vertices.end(); itv++) {
      obj << "vn " << (*itv)->nx << " " << (*itv)->ny << " " << (*itv)->nz << endl;
    }
  }

  for (itf = Faces.begin(); itf != Faces.end(); itf++) {
    obj << "f ";
    obj << (*itf)->v[0]->idx << "/" << (*itf)->v[0]->idx << "/" << (*itf)->v[0]->idx << " ";
    obj << (*itf)->v[1]->idx << "/" << (*itf)->v[1]->idx << "/" << (*itf)->v[1]->idx << " ";
    obj << (*itf)->v[2]->idx << "/" << (*itf)->v[2]->idx << "/" << (*itf)->v[2]->idx << endl;
  }
}

#ifdef	SPLIT_ON_INJECTION
void wrteWavefront(SimplField& ter, const char *pattern) {
  viterator itv;
  fiterator itf;

  // 1k == 32, 3k == 96, 512 == 16 */
  int resx = rasterx / 32;
  int resy = rastery / 32;

  // round down, negative side would be smaller than positive side
  int offx = tilesx / 2;
  int offy = tilesy / 2;

  /* initialize progress */
  InitProgress((numty - minty) * (numtx - mintx));

  for (int ty = minty; ty < numty; ty++) {
  for (int tx = mintx; tx < numtx; tx++) {
    int coordx = (tx - offx) * resx;
    int coordy = (ty - offy) * resy;

    SetTopic(emituvs ? "Saving UV tile {%d,%d}:" : "Saving non-UV tile {%d,%d}:", coordx, coordy);

    char name[MAXPATH];

    /**/ if (wchgame == 0)	// Oblivion
      _snprintf(name, sizeof(name) - 1, pattern, wdspace, coordx, coordy, min(resx, resy));
    else if (wchgame == 1)	// Skyrim
      _snprintf(name, sizeof(name) - 1, pattern, wdsname, min(resx, resy), coordx, coordy);

    /* lower-case */
    strlwr(name);
    strcat(name, ".obj");

    /* check if to write */
    if (IsOlder(name, writechk)) {
      logrf("writing \"%s\"\r", name);

      /* OBJ starts on index 1 */
      int idx = 1;

      ofstream obj(name,ios::binary);
      obj_out = &obj;

      obj.setf(ios::fixed, ios::floatfield);

      obj.precision(4); //
      for (itv = SectorVerticeO[ty][tx].begin(); itv != SectorVerticeO[ty][tx].end(); itv++) {
      	/* assign index the moment of writing it out */
	(*itv)->idx = idx++;

	obj << "v "  << (*itv)->x  << " " << (*itv)->y  << " " << (*itv)->z  << endl;
      }

      /* these are sort of meaning-less as we have
	* [0,1] texture coordinated _per tile_ later on
	*/
      if (emituvs) {
	obj.precision(8); // 1/2^23
	for (itv = SectorVerticeO[ty][tx].begin(); itv != SectorVerticeO[ty][tx].end(); itv++) {
	  obj << "vt " << (*itv)->tx << " " << (*itv)->ty << endl;
	}
      }

      if (emitnrm) {
	obj.precision(5); // 1/2^16
	for (itv = SectorVerticeO[ty][tx].begin(); itv != SectorVerticeO[ty][tx].end(); itv++) {
	  obj << "vn " << (*itv)->nx << " " << (*itv)->ny << " " << (*itv)->nz << endl;
	}
      }

      for (itf = SectorFaceO[ty][tx].begin(); itf != SectorFaceO[ty][tx].end(); itf++) {
	obj << "f ";
	obj << (*itf)->v[0]->idx << "/" << (*itf)->v[0]->idx << "/" << (*itf)->v[0]->idx << " ";
	obj << (*itf)->v[1]->idx << "/" << (*itf)->v[1]->idx << "/" << (*itf)->v[1]->idx << " ";
	obj << (*itf)->v[2]->idx << "/" << (*itf)->v[2]->idx << "/" << (*itf)->v[2]->idx << endl;
      }
    }

    /* advance progress */
    SetProgress((numty - minty) * (ty - minty) + (tx - mintx) + 1);
  }
  }
}
#endif

void wrteWavefront(SimplField& ter) {
  wrteWavefront("out.obj");

#ifdef	SPLIT_ON_INJECTION
  wrteWavefront(ter, "%02d.%02d.%02d.%02d");
#endif
}
