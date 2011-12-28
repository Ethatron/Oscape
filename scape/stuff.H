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

#ifndef	STUFF_H
#define	STUFF_H

#include "geom2d.H"
#include "Basic.H"

// ************ I think we ought to conglomerate some of this stuff
// ************ into a more coherent vector library
// (ie.  we have these different Vector3d and Point2d things, both of
// which are quite incomplete and have no sharing of code)
class Vector3d {
public:
    Real x,y,z;

    Vector3d() { x = y = z = 0; }
    Vector3d(Real a, Real b, Real c) { x = a; y = b; z = c; }
    Vector3d(const Point2d& p, Real zz) { x = p.x; y = p.y; z = zz; }
    friend ostream& operator<<(ostream&, const Vector3d&);
};

inline ostream& operator<<(ostream& os, const Vector3d& p)
{
    os << '(' << p.x << "," << p.y << ',' << p.z << ')';
    return os;
}

void getArea(const Vector3d& p, const Vector3d& q, const Vector3d& r, Real &area, Real &diam);

// A rectangular region defined by min and max points
class Region {
    int xmin,ymin;
    int xmax,ymax;
public:
    Region() { }

    void init(int x0, int y0, int x1, int y1) {
	xmin = x0; ymin = y0;
	xmax = x1; ymax = y1;
    }

    void add(int x,int y) {
	if (x < xmin) xmin = x;
	if (x > xmax) xmax = x;
	if (y < ymin) ymin = y;
	if (y > ymax) ymax = y;
    }

    void extent(int& x0, int& y0, int& x1, int& y1) {
	x0 = xmin; y0 = ymin;
	x1 = xmax; y1 = ymax;
    }
};

class Plane {
public:
    Real a, b, c;	/* plane is z=ax+by+c */

    Plane() {}
    Plane(const Vector3d& p, const Vector3d& q, const Vector3d& r) { init(p, q, r); }
    void init(const Vector3d&, const Vector3d&, const Vector3d&);

    Real operator()(Real x, Real y) const {
	return (a * x) + (b * y) + c;
    }

    Real operator()(int x, int y) const {
	return (a * x) + (b * y) + c;
    }
};



typedef Real rgb_val;

class Color {
public:
  rgb_val r, g, b;

  Color() { r = g = b = 0; }
  Color(rgb_val x, rgb_val y, rgb_val z) {
    r = x; g = y; b = z;
  }
};


typedef Real luma_val;

class Luma {
public:
  luma_val l;

  Luma() { l = 0; }
  Luma(luma_val x) {
    l = x;
  }
};


class ctex_array : public array2<Color> {

public:
  ctex_array() : array2<Color>() { }
  ctex_array(int w, int h) : array2<Color>(w, h) { }

  const Color& val(int i, int j) const {
    i %= width;
    j %= height;

    return data[j * width + i];
  }

  Color& ref(int i, int j) {
    i %= width;
    j %= height;

    return data[j * width + i];
  }

  Color& operator()(int i, int j) {
    return ref(i, j);
  }
};

class ltex_array : public array2<Luma> {

public:
  ltex_array() : array2<Luma>() { }
  ltex_array(int w, int h) : array2<Luma>(w, h) { }

  const Luma& val(int i, int j) const {
    i %= width;
    j %= height;

    return data[j * width + i];
  }

  Luma& ref(int i, int j) {
    i %= width;
    j %= height;

    return data[j * width + i];
  }

  Luma& operator()(int i, int j) {
    return ref(i, j);
  }
};





class ColorTexture {

protected:
  ctex_array data;

public:
  ColorTexture(ifstream& in);
  ~ColorTexture() { data.free(); }

  rgb_val r(int i, int j) const { return data.val(i, j).r; }
  rgb_val g(int i, int j) const { return data.val(i, j).g; }
  rgb_val b(int i, int j) const { return data.val(i, j).b; }

  void color(int i, int j, rgb_val& r, rgb_val& g, rgb_val& b) {
    r = data.ref(i, j).r;
    g = data.ref(i, j).g;
    b = data.ref(i, j).b;
  }

  const Color &val(int i, int j) const {
    return data.val(i, j);
  }

  Color &ref(int i, int j) {
    return data.ref(i, j);
  }
};

class LumaTexture {

protected:
  ltex_array data;

public:
  LumaTexture(ifstream& in);
  ~LumaTexture() { data.free(); }

  luma_val l(int i, int j) const { return data.val(i, j).l; }

  void luma(int i, int j, rgb_val& l) {
    l = data.ref(i, j).l;
  }

  const Luma &val(int i, int j) const {
    return data.val(i, j);
  }

  Luma &ref(int i, int j) {
    return data.ref(i, j);
  }
};


typedef ColorTexture RealColorTexture;
typedef LumaTexture RealLumaTexture;



class Zdata {

public:
    Real zmax, zmin;
};

class DEMdata : public Zdata {
    array2<unsigned short> z;

public:
    DEMdata(ifstream&);
    ~DEMdata() { z.free(); }

    Real eval(int x, int y) const { return (Real)z.val(x, y); }
    const unsigned short &val(int x, int y) const { return z.val(x, y); }
    unsigned short &ref(int x, int y) { return z.ref(x, y); }
    int width() const { return z.w(); }
    int height() const { return z.h(); }
};

class RGBAdata {
    array2<unsigned long> c;

public:
    RGBAdata(ifstream&);
    ~RGBAdata() { c.free(); }

    unsigned long eval(int x, int y) const { return (unsigned long)c.val(x, y); }
    const unsigned long &val(int x, int y) const { return c.val(x, y); }
    unsigned long &ref(int x, int y) { return c.ref(x, y); }
    int width() const { return c.w(); }
    int height() const { return c.h(); }
};

#endif
