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
// hfield.C
//
// Implements the HField class.
// HFields are used to store simple grid-sampled height fields.
// These height fields are what the program is going to simplify.
// The idea is to separate the height field and its approximation into
// two separate entities.

#include "scape.H"

/* linear interpolation */
#define LERP(t, a, b)	((a) + (t) * ((b) - (a)))

// HField::init --
//
// Takes a stream to read in a height field from and the filename of a
// texture file.  It then does the obvious -- it reads in the height field,
// reads in the texture, and initializes the various internal data arrays.
//
void HField::init(ifstream& dataStream, const char *texFile, const char *greyFile) {
  if (!dataStream.good()) {
    cerr << "ERROR: Input terrain data does not seem to exist." << endl;
    exit(1);
  }

  data   = new DEMdata(dataStream);
  width  = data->width();
  height = data->height();

  //?? optimization: if emphasis==0 || texfile==0 then don't read texture
  if (texFile) {
    ifstream texStream(texFile, ios::binary);
    cout << "# Opening texture file: " << texFile << endl;
    tex = new RealColorTexture(texStream);
    map = NULL;
  }
  else if (greyFile) {
    ifstream greyStream(greyFile, ios::binary);
    cout << "# Opening texture file: " << greyFile << endl;
    tex = NULL;
    map = new RealLumaTexture(greyStream);
  }
  else {
    emphasis = 0.0;
    tex = NULL;
    map = NULL;
  }
}


// HField::free --
//
// Like the name says, free the storage that we're currently using.
//
void HField::free() {
  delete data;
  delete tex;
  delete map;
}

// bilinear interpolation
// Note: this code could access off edge of array, but such bogus samples
// should be weighted by zero (fx=0 and/or fy=0).
Real HField::eval_interp(Real x, Real y) const {
  int ix = (int)x; Real fx = x - ix;
  int iy = (int)y; Real fy = y - iy;

  Real zx0 = LERP(fx, eval(ix, iy    ), eval(ix + 1, iy    ));
  Real zx1 = LERP(fx, eval(ix, iy + 1), eval(ix + 1, iy + 1));

  return LERP(fy, zx0, zx1);
}

// bilinear interpolation
// Note: this code could access off edge of array, but such bogus samples
// should be weighted by zero (fx=0 and/or fy=0).
void HField::luma_interp(Real x, Real y, Real &l) const {
  int ix = (int)x; Real fx = x - ix;
  int iy = (int)y; Real fy = y - iy;
  const Luma *luma;
  Luma lx0, lx1;

  luma = &luma_val(ix, iy + 0);
  lx0.l = LERP(fx, luma[0].l, luma[1].l);

  luma = &luma_val(ix, iy + 1);
  lx1.l = LERP(fx, luma[0].l, luma[1].l);

  l = LERP(fy, lx0.l, lx1.l);
}

// bilinear interpolation
// Note: this code could access off edge of array, but such bogus samples
// should be weighted by zero (fx=0 and/or fy=0).
void HField::color_interp(Real x, Real y, Real &r, Real &g, Real &b) const {
  int ix = (int)x; Real fx = x - ix;
  int iy = (int)y; Real fy = y - iy;
  const Color *color;
  Color cx0, cx1;

  color = &color_val(ix, iy + 0);
  cx0.r = LERP(fx, color[0].r, color[1].r);
  cx0.g = LERP(fx, color[0].g, color[1].g);
  cx0.b = LERP(fx, color[0].b, color[1].b);

  color = &color_val(ix, iy + 1);
  cx1.r = LERP(fx, color[0].r, color[1].r);
  cx1.g = LERP(fx, color[0].g, color[1].g);
  cx1.b = LERP(fx, color[0].b, color[1].b);

  r = LERP(fy, cx0.r, cx1.r);
  g = LERP(fy, cx0.g, cx1.g);
  b = LERP(fy, cx0.b, cx1.b);
}
