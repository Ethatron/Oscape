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

#ifndef	HFIELD_H
#define	HFIELD_H

//
// hfield.H
//
// Defines the HField class.
// HFields are used to store simple grid-sampled height fields.
// These height fields are what the program is going to simplify.
// The idea is to separate the height field and its approximation into
// two separate entities.


class HField {
    int width, height;

    DEMdata *data;
    RealColorTexture *tex;
    RealLumaTexture *map;

    void init(ifstream& mntns, const char *colfile, const char *lmafile);
    void free();

public:
    HField(ifstream& in, const char *colfile, const char *lmafile) { init(in, colfile, lmafile); }
    ~HField() { free(); }

    /* ------------------------------------------------------------------------- */
    const unsigned short &z_val(int x, int y) const { return data->val(x, y); }
    const Color &color_val(int i, int j) const { return tex->val(i, j); }
    const Luma &luma_val(int i, int j) const { return map->val(i, j); }

    unsigned short &z_ref(int x, int y) { return data->ref(x, y); }
    Color &color_ref(int i, int j) { return tex->ref(i, j); }
    Luma &luma_val(int i, int j) { return map->ref(i, j); }

    /* ------------------------------------------------------------------------- */
    Real eval(int x,int y) const {
      if ((x >= 0) && (x < width) &&
	  (y >= 0) && (y < height))
	return data->eval(x, y);
      else
	return data->zmin;
    }

    Real eval(Real x, Real y) const { return eval((int)x, (int)y); }
    Real eval(const Point2d& p) const { return eval((int)p.x, (int)p.y); }
    // bilinear interpolation
    Real eval_interp(Real x, Real y) const;

    /* ------------------------------------------------------------------------- */
    void color(int x, int y, Real& r, Real& g, Real& b) const {
      if ((x >= 0) && (x < width) &&
	  (y >= 0) && (y < height)) {
	r = tex->r(x, y);
	g = tex->g(x, y);
	b = tex->b(x, y);
      }
      else {
	r = g = b = 0;
      }
    }
    void color(const Point2d& p, Real& r, Real& g, Real& b) const {
	color((int)p.x, (int)p.y, r, g, b);
    }
    // bilinear interpolation
    void color_interp(Real x, Real y, Real &r, Real &g, Real &b) const;

    /* ------------------------------------------------------------------------- */
   void luma(int x, int y, Real& l) const {
      if ((x >= 0) && (x < width) &&
	  (y >= 0) && (y < height)) {
	l = map->l(x, y);
      }
      else {
	l = 0;
      }
    }
    void luma(const Point2d& p, Real& l) const {
      luma((int)p.x, (int)p.y, l);
    }
    // bilinear interpolation
    void luma_interp(Real x, Real y, Real &l) const;

    /* ------------------------------------------------------------------------- */
    Real zmax() const { return data->zmax; }
    Real zmin() const { return data->zmin; }
    int get_width() const { return width; }
    int get_height() const { return height; }

    bool has_color() const { return (tex != NULL); }
    bool has_luma() const { return (map != NULL); }
};

#endif
