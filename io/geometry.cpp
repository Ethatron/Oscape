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

#include "geometry.hpp"

vector<string> skiplist;
bool writetin = false;
bool writeobj = false;
bool writenif = false;
bool writedx9 = false;

bool skipTile(int coordx, int coordy, int reso) {
  bool skip = true;

  if (writeobj || writedx9 || writenif) {
    vector<string>::iterator walk = skiplist.begin();
    while (walk != skiplist.end()) {
      char nbase[MAXPATH], name[MAXPATH];

      /* codification is:
       * "worldspace.tilex*32.tiley*32.32"
       * worldspace == 60 == Tamriel
       */
      /**/ if (wchgame == 0)	// Oblivion
        _snprintf(nbase, sizeof(nbase) - 1, walk->data(), wdspace, coordx, coordy, reso);
      else if (wchgame == 1)	// Skyrim
        _snprintf(nbase, sizeof(nbase) - 1, walk->data(), wdsname, reso, coordx, coordy);

      /* lower-case */
      strlwr(nbase);

      if (writenif) {
	/**/ if (wchgame == 0) {	// Oblivion
	  strcpy(name, nbase);
	  strcat(name, ".nif");
	}
	else if (wchgame == 1) {	// Skyrim
	  strcpy(name, nbase);
	  strcat(name, ".btr");
	}

	skip = skip && !IsOlder(name, writechk);
      }

      if (writeobj) {
	strcpy(name, nbase);
	strcat(name, ".obj");

	skip = skip && !IsOlder(name, writechk);
      }

      if (writedx9) {
	strcpy(name, nbase);
	strcat(name, ".x");

	skip = skip && !IsOlder(name, writechk);
      }

      walk++;
    }
  }
  else
    skip = !!writechk;

  return skip;
}

void wrteGeometry(SimplField& ter, const char *pattern) {
  if (writeobj || writedx9 || writenif) {
//  write_mesh(ter);

    if (writenif) {
      /**/ if (wchgame == 0)	// Oblivion
	wrteNIF(ter, pattern);
      else if (wchgame == 1)	// Skyrim
	wrteBTR(ter, pattern);
    }

    if (writeobj)
      wrteWavefront(ter, pattern);
    if (writedx9)
      wrteDXMesh(ter, pattern);
  }
}
