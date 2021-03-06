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

#ifndef	CFIELD_H
#define	CFIELD_H

#include "stuff.H"

class CField {
    int width, height;

    RGBAdata *data;

    void init(ifstream& mntns);
    void free();

public:
    CField(ifstream& in) { init(in); }
    ~CField() { free(); }

    /* ------------------------------------------------------------------------- */
    const unsigned long &c_val(int x, int y) const { return data->val(x, y); }
    unsigned long &c_ref(int x, int y) { return data->ref(x, y); }

    /* ------------------------------------------------------------------------- */
    unsigned long eval(int x, int y) const {
      if ((x >= 0) && (x < width) &&
	  (y >= 0) && (y < height))
	return data->eval(x, y);
      else
	return 0;
    }

    unsigned long eval(Real x, Real y) const { return eval((int)x, (int)y); }
    unsigned long eval(const Point2d& p) const { return eval((int)p.x, (int)p.y); }
    // bilinear interpolation
    unsigned long eval_interp(Real x, Real y) const;

    /* ------------------------------------------------------------------------- */
    int get_width() const { return width; }
    int get_height() const { return height; }
};

#include <windows.h>

class CView {
  long stride;

  int begin, range;
  int width, height;

  unsigned __int64 position;
  unsigned __int64 length;

  HANDLE mh;
  OFSTRUCT of;
  HANDLE oh;

  const unsigned long *data;

  void init(const char *mntns, int w, int h);
  void free();

public:
  CView(const char *in, int w, int h) { init(in, w, h); }
  ~CView() { free(); }

  /* ------------------------------------------------------------------------- */
  const unsigned long &c_val(int x, int y) const { return data[(y - begin) * width + x]; }
//unsigned long &c_ref(int x, int y) { return data[(y - begin) * width + x]; }

  /* ------------------------------------------------------------------------- */
  unsigned long eval(int x, int y) const {
#if 0
    if ((x >= 0) && (x < width) &&
	(y >= 0) && (y < height))
      return data->eval(x, y);
    else
      return 0;
#else
    assert((x >= 0) && (x < width ));
    assert((y >= 0) && (y < height));

    assert((y >= begin) && (y < (begin + range)));

    return data[(y - begin) * width + x];
#endif
  }

  unsigned long eval(Real x, Real y) const { return eval((int)x, (int)y); }
  unsigned long eval(const Point2d& p) const { return eval((int)p.x, (int)p.y); }
  // bilinear interpolation
  unsigned long eval_interp(Real x, Real y) const;

  /* ------------------------------------------------------------------------- */
  int get_width() const { return width; }
  int get_height() const { return height; }

  /* ------------------------------------------------------------------------- */
  void set_row(int y);
};

class CSinkBase {
protected:
  long stride;

  int begin, range;
  int width, height;

  unsigned __int64 position;
  unsigned __int64 length;

  HANDLE mh;
  OFSTRUCT of;
  HANDLE oh;
  LPVOID mem;

  void init(const char *mntns, int w, int h, int dts);
  void free();

public:
//CSinkBase(const char *in, int w, int h) { init(in, w, h); }
//~CSinkBase() { free(); }

  void set_row(int y);
};

template<typename dtyp = unsigned long>
class CSink : public CSinkBase {
  dtyp *data;

public:
  CSink(const char *in, int w, int h) { CSinkBase::init(in, w, h, sizeof(dtyp)); data = (dtyp *)mem; }
  ~CSink() { free(); }

  /* ------------------------------------------------------------------------- */
  void c_set(int x, int y, const dtyp &val) { data[(y - begin) * width + x] = val; }
  dtyp &c_ref(int x, int y) { return data[(y - begin) * width + x]; }

  /* ------------------------------------------------------------------------- */
  void sink(int x, int y, dtyp val) {
#if 0
    if ((x >= 0) && (x < width) &&
      (y >= 0) && (y < height))
      return data->eval(x, y);
    else
      return 0;
#else
    assert((x >= 0) && (x < width ));
    assert((y >= 0) && (y < height));

    assert((y >= begin) && (y < (begin + range)));

    data[(y - begin) * width + x] = val;
#endif
  }

  void sink(Real x, Real y, dtyp val) { sink((int)x, (int)y, val); }
  void sink(const Point2d& p, dtyp val) { sink((int)p.x, (int)p.y, val); }

  /* ------------------------------------------------------------------------- */
  long get_stride() const { return stride; }
  int get_width() const { return width; }
  int get_height() const { return height; }

  /* ------------------------------------------------------------------------- */
  void set_row(int y) { CSinkBase::set_row(y); data = (dtyp *)mem; }
};

#endif
