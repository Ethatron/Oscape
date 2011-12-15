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

#include <vector>
//#include <map>
#include <set>
//#include <list>
//#include <queue>
//#include <string>
//#include <utility>

#include "geom2d.H"
#include "hfield.H"

/* ---------------------------------------------------- */

int dimx = 1024;
int dimy = 1024;

class objVertex;
class objFace;

#include "libmemory.hpp"

extern class MemQueue<4> VPool;
extern class MemQueue<4> FPool;

typedef	std::vector<class objVertex *, MemAllocator<class objVertex *> >	vvector;
typedef	std::vector<class objFace   *, MemAllocator<class objFace   *> >	fvector;

//class MemAllocator<objVertex *> VAllc(&VPool);
//class MemAllocator<objFace   *> FAllc(&FPool);

class objVertex : public MemQueueable {
public:
  objVertex() : f(&FPool) { idx = 0; }

  /* the set's key, can change to whatever */
  Point2d vtx;
  /* the vertex's local object coordinates */
  Point2d op; Real oz;

  Real nx, ny, nz;
  Real tx, ty;
  Real x, y, z;

  int idx;

  fvector f;

  bool operator < (const class objVertex &other) const
  {
    return memcmp(&this->vtx, &other.vtx, sizeof(Point2d)) < 0;
#if 0
    return (1024.0 * 1024.0 * dimx *       vtx.y +       vtx.x) <
	   (1024.0 * 1024.0 * dimx * other.vtx.y + other.vtx.x);
#elif 0
    return
      (((unsigned __int64)(((unsigned long int *)(&      vtx.y))[1]) <<  0) ^
       ((unsigned __int64)(((unsigned long int *)(&      vtx.x))[1]) << 32) ^
       ((unsigned __int64)(((unsigned long int *)(&      vtx.y))[0]) <<  0) ^
       ((unsigned __int64)(((unsigned long int *)(&      vtx.x))[0]) << 32)) <
      (((unsigned __int64)(((unsigned long int *)(&other.vtx.y))[1]) <<  0) ^
       ((unsigned __int64)(((unsigned long int *)(&other.vtx.x))[1]) << 32) ^
       ((unsigned __int64)(((unsigned long int *)(&other.vtx.y))[0]) <<  0) ^
       ((unsigned __int64)(((unsigned long int *)(&other.vtx.x))[0]) << 32));
#endif
  }

  void fill();
};

class objFace : public MemQueueable {
public:
  objFace() { narea = 0.0; v[0] = v[1] = v[2] = NULL; }

  Real nx, ny, nz;
  Real narea;

  class objVertex *v[3];

  void fill();
};

void objVertex::fill() {
  struct { Real x,y,z; } crosses;
  Real crossmag;

  crosses.x = 0.0;
  crosses.y = 0.0;
  crosses.z = 0.0;
  crossmag  = 0.0;

  assert(f.size() != 0);

  fvector::const_iterator itf;
  for (itf = f.begin(); itf != f.end(); itf++) {
    crosses.x += (*itf)->nx;
    crosses.y += (*itf)->ny;
    crosses.z += (*itf)->nz;
    crossmag  += (*itf)->narea * 2;
    /* faster than normalize(),
     * has no precision problems either
     * parallelizable
     */
  }

  crosses.x /= crossmag;
  crosses.y /= crossmag;
  crosses.z /= crossmag;

  nx = crosses.x;
  ny = crosses.y;
  nz = crosses.z;
};

void objFace::fill() {
  class objVertex *v1 = v[0];
  class objVertex *v2 = v[1];
  class objVertex *v3 = v[2];

  assert(v1 != v2); assert(v1 != NULL);
  assert(v1 != v3); assert(v2 != NULL);
  assert(v2 != v3); assert(v3 != NULL);

  struct { Real x,y,z; } v1v2;
  struct { Real x,y,z; } v1v3;

  v1v2.x = v1->x - v2->x;
  v1v2.y = v1->y - v2->y;
  v1v2.z = v1->z - v2->z;

  v1v3.x = v1->x - v3->x;
  v1v3.y = v1->y - v3->y;
  v1v3.z = v1->z - v3->z;

  struct { Real x,y,z; } cross;
  Real mag;

  cross.x = v1v2.y * v1v3.z - v1v2.z * v1v3.y;
  cross.y = v1v2.z * v1v3.x - v1v2.x * v1v3.z;
  cross.z = v1v2.x * v1v3.y - v1v2.y * v1v3.x;

  mag = sqrt((cross.x * cross.x) + (cross.y * cross.y) + (cross.z * cross.z));

  if (cross.z < 0) {
    cross.x = -cross.x;
    cross.y = -cross.y;
    cross.z = -cross.z;
  }

  nx = cross.x;
  ny = cross.y;
  nz = cross.z;
  narea = 0.5f * fabs(mag);
};

struct V {
  bool operator()(const class objVertex *s1, const class objVertex *s2) const
  {
    return memcmp(&s1->vtx, &s2->vtx, sizeof(Point2d)) < 0;
#if 0
    return (1024.0 * 1024.0 * dimx * s1->vtx.y + s1->vtx.x) <
	   (1024.0 * 1024.0 * dimx * s2->vtx.y + s2->vtx.x);
#elif 0
    return
      (((unsigned __int64)(((unsigned long int *)(&s1->vtx.y))[1]) <<  0) ^
       ((unsigned __int64)(((unsigned long int *)(&s1->vtx.x))[1]) << 32) ^
       ((unsigned __int64)(((unsigned long int *)(&s1->vtx.y))[0]) <<  0) ^
       ((unsigned __int64)(((unsigned long int *)(&s1->vtx.x))[0]) << 32)) <
      (((unsigned __int64)(((unsigned long int *)(&s2->vtx.y))[1]) <<  0) ^
       ((unsigned __int64)(((unsigned long int *)(&s2->vtx.x))[1]) << 32) ^
       ((unsigned __int64)(((unsigned long int *)(&s2->vtx.y))[0]) <<  0) ^
       ((unsigned __int64)(((unsigned long int *)(&s2->vtx.x))[0]) << 32));
#endif
  }
};

class MemQueue<4> VPool(sizeof(objVertex), 0x7FFFFFFF);
class MemQueue<4> FPool(sizeof(objFace  ), 0x7FFFFFFF);

#ifdef	SPLIT_ON_INJECTION
int tri_procd = 0;
int tri_added = 0;
int tri_sectd = 0;
#endif

#if	defined(SPLIT_ON_INJECTION) && !defined(SPLIT_ON_INJECTION_SNAP)
#define zheight	getZInterp
#else
#define zheight	getZ
#endif

/* ---------------------------------------------------- */

std::set<class objVertex *, struct V> Vertices;
std::vector<class objFace *> Faces;

void RegisterFace(const Point2d& _p1,
		  const Point2d& _p2,
		  const Point2d& _p3, void *closure) {
    HField *H = (HField *)closure;

    /* orient CCW */
    if (getArea2x(_p1, _p2, _p3) < 0.0) {
      RegisterFace(_p1, _p3, _p2, closure); return; }

    Point2d p1, p2, p3;

#ifdef SPLIT_ON_INJECTION_SNAP
    /* allow only integer positions for points */
    p1 = floor(_p1 + 0.5);
    p2 = floor(_p2 + 0.5);
    p3 = floor(_p3 + 0.5);
#else
    /* allow any position for points */
    p1 = _p1;
    p2 = _p2;
    p3 = _p3;
#endif

    class objVertex vp1, vp2, vp3;
    class objVertex *v1, *v2, *v3;

    set<class objVertex *, struct V>::iterator i1;
    set<class objVertex *, struct V>::iterator i2;
    set<class objVertex *, struct V>::iterator i3;

    vp1.vtx = p1; v1 = NULL;
    vp2.vtx = p2; v2 = NULL;
    vp3.vtx = p3; v3 = NULL;

    i1 = Vertices.find(&vp1); if (i1 != Vertices.end()) v1 = *i1;
    i2 = Vertices.find(&vp2); if (i2 != Vertices.end()) v2 = *i2;
    i3 = Vertices.find(&vp3); if (i3 != Vertices.end()) v3 = *i3;

#ifdef SPLIT_ON_INJECTION_SNAP
    /* all three vertices exist already, look
     * if they share a common face
     */
    if (v1 && v2 && v3) {
      /* remove degenerate face cases, the surface is a closed
       * mesh, so the adjacent vertices will snap as well and
       * close this "hole"
       */
      if (v1 == v2)
	return;
      if (v1 == v3)
	return;
      if (v2 == v3)
	return;

      fvector::const_iterator f1, f2, f3;

      /* compare each face for vertex 1 with the faces of vertex 2 */
      for (f1 = v1->f.begin(); f1 != v1->f.end(); f1++) {
	for (f2 = v2->f.begin(); f2 != v2->f.end(); f2++) {
	  /* if they have a shared faced, search that one for vertex 3 */
	  if ((*f1) == (*f2)) {
	    for (f3 = v3->f.begin(); f3 != v3->f.end(); f3++) {
	      /* the face is shared by all three vertices, which means we got a double */
	      if ((*f1) == (*f3))
		return;
	    }
	  }
	}
      }
    }
    /* not all of the vertices have been allocated,
     * still this can be a degenerate case
     */
    else {
      /* remove degenerate face cases, the surface is a closed
       * mesh, so the adjacent vertices will snap as well and
       * close this "hole"
       */
      if ((p1.x == p2.x) && (p1.y == p2.y))
	return;
      if ((p1.x == p3.x) && (p1.y == p3.y))
	return;
      if ((p2.x == p3.x) && (p2.y == p3.y))
	return;
    }
#endif

    if (!v1) {
	v1 = new(&VPool) class objVertex(); assert(v1);
	v1->vtx = p1;

    	v1->tx = 0.0f + p1.x / H->getWidth();
    	v1->ty = 0.0f + p1.y / H->getHeight();

    	v1->x = (v1->op.x = p1.x) * sizescale;
    	v1->y = (v1->op.y = p1.y) * sizescale;
    	v1->z = (v1->oz = H->zheight(p1.x, p1.y)) * heightscale - heightshift;

    	Vertices.insert(v1);
    }

    if (!v2) {
	v2 = new(&VPool) class objVertex(); assert(v2);
	v2->vtx = p2;

	v2->tx = 0.0f + p2.x / H->getWidth();
	v2->ty = 0.0f + p2.y / H->getHeight();

    	v2->x = (v2->op.x = p2.x) * sizescale;
    	v2->y = (v2->op.y = p2.y) * sizescale;
    	v2->z = (v2->oz = H->zheight(p2.x, p2.y)) * heightscale - heightshift;

	Vertices.insert(v2);
    }

    if (!v3) {
	v3 = new(&VPool) class objVertex(); assert(v3);
	v3->vtx = p3;

	v3->tx = 0.0f + p3.x / H->getWidth();
	v3->ty = 0.0f + p3.y / H->getHeight();

    	v3->x = (v3->op.x = p3.x) * sizescale;
    	v3->y = (v3->op.y = p3.y) * sizescale;

    	v3->z = (v3->oz = H->zheight(p3.x, p3.y)) * heightscale - heightshift;

	Vertices.insert(v3);
    }

    assert(v1 != v2);
    assert(v1 != v3);
    assert(v2 != v3);

    class objFace *f;

    f = new(&FPool) class objFace(); assert(f);
    f->v[0] = v1; v1->f.push_back(f);
    f->v[1] = v2; v2->f.push_back(f);
    f->v[2] = v3; v3->f.push_back(f);

    Faces.push_back(f);
}

void SplitFace(const Point2d& tp1, const Point2d& tp2, const Point2d& tp3, void *closure, bool checkx = true, bool checky = true) {
    HField *H = (HField *)closure;

    Point2d p1 = tp1;
    Point2d p2 = tp2;
    Point2d p3 = tp3;

#ifdef	SPLIT_ON_INJECTION
    /* ------------------------------------------------
     * check if the triangle straddles borders
     */
    Point2d s1; s1.x = floor((1.0f / rasterx) * p1.x); s1.y = floor((1.0f / rastery) * p1.y);
    Point2d s2; s2.x = floor((1.0f / rasterx) * p2.x); s2.y = floor((1.0f / rastery) * p2.y);
    Point2d s3; s3.x = floor((1.0f / rasterx) * p3.x); s3.y = floor((1.0f / rastery) * p3.y);

    /* first vertical split */
    if (checkx)
    if ((s1.x != s2.x) ||
	(s1.x != s3.x)) {
      /* sort points left [-x] to right [+x] */
      if (p1.x > p2.x) { Point2d s = p1; p1 = p2; p2 = s; }
      if (p1.x > p3.x) { Point2d s = p1; p1 = p3; p3 = s; }
      if (p2.x > p3.x) { Point2d s = p2; p2 = p3; p3 = s; }

      s1.x = floor((1.0f / rasterx) * p1.x); s1.y = floor((1.0f / rastery) * p1.y);
      s2.x = floor((1.0f / rasterx) * p2.x); s2.y = floor((1.0f / rastery) * p2.y);
      s3.x = floor((1.0f / rasterx) * p3.x); s3.y = floor((1.0f / rastery) * p3.y);

      /* p2 & p3 are both within [0,1024] */
      if ((p1.x <= p2.x) && (p2.x <= p3.x) && (p3.x <= ((s1.x + 1.0f) * rasterx))) {
      }
      /* p2 is on the next border */
      else if (p2.x == ((s1.x + 1.0f) * rasterx)) {
	Point2d p4;

	p4.x = (s1.x + 1.0f) * rasterx;
	p4.y = p1.y - (p1.x - p4.x) * (p1.y - p3.y) / (p1.x - p3.x);

	/* p1->p2->p4 (left of border) still check y */
	SplitFace(p1, p2, p4, closure, false, true);
	/* p2->p3->p4 (recurse) check both */
	SplitFace(p2, p3, p4, closure, true, true);

	tri_added += 1; return;
      }
      /* p2 is before the next border */
      else if (p2.x <  ((s1.x + 1.0f) * rasterx)) {
	Point2d p4;
	Point2d p5;

	p4.x = (s1.x + 1.0f) * rasterx;
	p4.y = p2.y - (p2.x - p4.x) * (p2.y - p3.y) / (p2.x - p3.x);
	p5.x = (s1.x + 1.0f) * rasterx;
	p5.y = p1.y - (p1.x - p5.x) * (p1.y - p3.y) / (p1.x - p3.x);

	/* deviation from perfect half-split */
	Real areaOH = narea(p1, p2, p3) * 0.5f;

	Real areaA1 = areaOH - narea(p1, p2, p4);
	Real areaA2 = areaOH - narea(p1, p4, p5);
	Real areaB1 = areaOH - narea(p1, p2, p5);
	Real areaB2 = areaOH - narea(p2, p4, p5);

	Real devA = sqrt(areaA1 * areaA1 + areaA2 * areaA2);
	Real devB = sqrt(areaB1 * areaB1 + areaB2 * areaB2);

	if (devA <= devB) {
	  /* p1->p2->p4 (left of border) still check y */
	  /* p1->p4->p5 (left of border) still check y */
	  SplitFace(p1, p2, p4, closure, false, true);
	  SplitFace(p1, p4, p5, closure, false, true);
	}
	else {
	  /* p1->p2->p5 (left of border) still check y */
	  /* p2->p4->p5 (left of border) still check y */
	  SplitFace(p1, p2, p5, closure, false, true);
	  SplitFace(p2, p4, p5, closure, false, true);
	}

	/* p4->p3->p5 (recurse) check both */
	SplitFace(p4, p3, p5, closure, true, true);

	tri_added += 2; return;
      }
      /* p2 is after the next border */
      else if (p2.x >  ((s1.x + 1.0f) * rasterx)) {
	Point2d p4;
	Point2d p5;

	p4.x = (s1.x + 1.0f) * rasterx;
	p4.y = p1.y - (p1.x - p4.x) * (p1.y - p2.y) / (p1.x - p2.x);
	p5.x = (s1.x + 1.0f) * rasterx;
	p5.y = p1.y - (p1.x - p5.x) * (p1.y - p3.y) / (p1.x - p3.x);

	/* deviation from perfect half-split */
	Real areaOH = narea(p1, p2, p3) * 0.5f;

	Real areaA1 = areaOH - narea(p4, p2, p3);
	Real areaA2 = areaOH - narea(p4, p3, p5);
	Real areaB1 = areaOH - narea(p4, p2, p5);
	Real areaB2 = areaOH - narea(p2, p3, p5);

	Real devA = sqrt(areaA1 * areaA1 + areaA2 * areaA2);
	Real devB = sqrt(areaB1 * areaB1 + areaB2 * areaB2);

	/* p1->p4->p5 (left of border) still check y */
	SplitFace(p1, p4, p5, closure, false, true);

	if (devA <= devB) {
	  /* p4->p2->p3 (recurse) check both */
	  /* p4->p3->p5 (recurse) check both */
	  SplitFace(p4, p2, p3, closure, true, true);
	  SplitFace(p4, p3, p5, closure, true, true);
	}
	else {
	  /* p4->p3->p5 (recurse) check both */
	  /* p4->p2->p3 (recurse) check both */
	  SplitFace(p4, p2, p5, closure, true, true);
	  SplitFace(p2, p3, p5, closure, true, true);
	}

	tri_added += 2; return;
      }
    }

    /* then horizontal split */
    if (checky)
    if ((s1.y != s2.y) ||
	(s1.y != s3.y)) {
      /* sort points top [-y] to bottom [+y] */
      if (p1.y > p2.y) { Point2d s = p1; p1 = p2; p2 = s; }
      if (p1.y > p3.y) { Point2d s = p1; p1 = p3; p3 = s; }
      if (p2.y > p3.y) { Point2d s = p2; p2 = p3; p3 = s; }

      s1.x = floor((1.0f / rasterx) * p1.x); s1.y = floor((1.0f / rastery) * p1.y);
      s2.x = floor((1.0f / rasterx) * p2.x); s2.y = floor((1.0f / rastery) * p2.y);
      s3.x = floor((1.0f / rasterx) * p3.x); s3.y = floor((1.0f / rastery) * p3.y);

      /* p2 & p3 are both within [0,1024] */
      if ((p1.y <= p2.y) && (p2.y <= p3.y) && (p3.y <= ((s1.y + 1.0f) * rastery))) {
      }
      /* p2 is on the next border */
      else if (p2.y == ((s1.y + 1.0f) * rastery)) {
	Point2d p4;

	p4.y = (s1.y + 1.0f) * rastery;
	p4.x = p1.x - (p1.y - p4.y) * (p1.x - p3.x) / (p1.y - p3.y);

	/* p1->p2->p4 (left of border) no more checks */
	SplitFace(p1, p2, p4, closure, false, false);
	/* p2->p3->p4 (recurse) still check y */
	SplitFace(p2, p3, p4, closure, false, true);

	tri_added += 1; return;
      }
      /* p2 is before the next border */
      else if (p2.y <  ((s1.y + 1.0f) * rastery)) {
	Point2d p4;
	Point2d p5;

	p4.y = (s1.y + 1.0f) * rastery;
	p4.x = p2.x - (p2.y - p4.y) * (p2.x - p3.x) / (p2.y - p3.y);
	p5.y = (s1.y + 1.0f) * rastery;
	p5.x = p1.x - (p1.y - p5.y) * (p1.x - p3.x) / (p1.y - p3.y);

	/* deviation from perfect half-split */
	Real areaOH = narea(p1, p2, p3) * 0.5f;

	Real areaA1 = areaOH - narea(p1, p2, p4);
	Real areaA2 = areaOH - narea(p1, p4, p5);
	Real areaB1 = areaOH - narea(p1, p2, p5);
	Real areaB2 = areaOH - narea(p2, p4, p5);

	Real devA = sqrt(areaA1 * areaA1 + areaA2 * areaA2);
	Real devB = sqrt(areaB1 * areaB1 + areaB2 * areaB2);

	if (devA <= devB) {
	  /* p1->p2->p4 (left of border) no more checks */
	  /* p1->p4->p5 (left of border) no more checks */
	  SplitFace(p1, p2, p4, closure, false, false);
	  SplitFace(p1, p4, p5, closure, false, false);
	}
	else {
	  /* p1->p2->p5 (left of border) no more checks */
	  /* p2->p4->p5 (left of border) no more checks */
	  SplitFace(p1, p2, p5, closure, false, false);
	  SplitFace(p2, p4, p5, closure, false, false);
	}

	/* p4->p3->p5 (recurse) still check y */
	SplitFace(p4, p3, p5, closure, false, true);

	tri_added += 2; return;
      }
      /* p2 is after the next border */
      else if (p2.y >  ((s1.y + 1.0f) * rastery)) {
	Point2d p4;
	Point2d p5;

	p4.y = (s1.y + 1.0f) * rastery;
	p4.x = p1.x - (p1.y - p4.y) * (p1.x - p2.x) / (p1.y - p2.y);
	p5.y = (s1.y + 1.0f) * rastery;
	p5.x = p1.x - (p1.y - p5.y) * (p1.x - p3.x) / (p1.y - p3.y);

	/* deviation from perfect half-split */
	Real areaOH = narea(p1, p2, p3) * 0.5f;

	Real areaA1 = areaOH - narea(p4, p2, p3);
	Real areaA2 = areaOH - narea(p4, p3, p5);
	Real areaB1 = areaOH - narea(p4, p2, p5);
	Real areaB2 = areaOH - narea(p2, p3, p3);

	Real devA = sqrt(areaA1 * areaA1 + areaA2 * areaA2);
	Real devB = sqrt(areaB1 * areaB1 + areaB2 * areaB2);

	/* p1->p4->p5 (left of border) no more checks */
	SplitFace(p1, p4, p5, closure, false, false);

	if (devA <= devB) {
	  /* p4->p2->p3 (recurse) still check y */
	  /* p4->p3->p5 (recurse) still check y */
	  SplitFace(p4, p2, p3, closure, false, true);
	  SplitFace(p4, p3, p5, closure, false, true);
	}
	else {
	  /* p4->p3->p5 (recurse) still check y */
	  /* p4->p2->p3 (recurse) still check y */
	  SplitFace(p4, p2, p5, closure, false, true);
	  SplitFace(p2, p3, p5, closure, false, true);
	}

	tri_added += 2; return;
      }
    }

    /* ------------------------------------------------ */
#endif

    RegisterFace(p1, p2, p3, closure);
}

void CollectFace(Triangle *t, void *closure) {
  HField *H = (HField *)closure;

  const Point2d& p1 = t->point1();
  const Point2d& p2 = t->point2();
  const Point2d& p3 = t->point3();

#ifndef NDEBUG
/*
set<class objVertex *, struct V>::iterator itv;
  for (itv = Vertices.begin(); itv != Vertices.end(); itv++)
    assert(*itv);
 */
#endif

  SplitFace(p1, p2, p3, closure); tri_procd++;

  if ((tri_procd & 0xFF) == 0) {
    logrf("faces processed %d & added %d\r", tri_procd, tri_added);

    /* advance progress */
    SetProgress(tri_procd);
  }
}

void CalculateGeometryNormals() {
  set<class objVertex *, struct V>::const_iterator itv;
  vector<class objFace *>::const_iterator itf;

  for (itf = Faces.begin(); itf != Faces.end(); itf++)
    (*itf)->fill();
  for (itv = Vertices.begin(); itv != Vertices.end(); itv++)
    (*itv)->fill();

/* damit, no parallel sets ...

  int faces = (int)Faces.size();
#pragma omp parallel for schedule(runtime) shared(Faces)
  for (int f = 0; f < faces; f++)
    Faces[f]->fill();

  int verts = (int)Vertices.size();
#pragma omp parallel for schedule(runtime) shared(Vertices)
  for (int v = 0; v < verts; v++)
    Vertices[v]->fill();
*/
}

/* ---------------------------------------------------- */

#ifdef	SPLIT_ON_INJECTION
std::set<class objVertex *, struct V> SectorVertices[128][128];
std::vector<class objFace *> SectorFaces[128][128];

#include <omp.h>
omp_lock_t SectorLocks[128][128];

void TileGeometry() {
    set<class objVertex *, struct V>::const_iterator itv;
    vector<class objFace *>::const_iterator itf;

    // 1k == 32, 3k == 96, 512 == 16 */
    int resx = rasterx / 32;
    int resy = rastery / 32;

    // round down, negative side would be smaller than positive side
    int offx = tilesx / 2;
    int offy = tilesy / 2;

    InitProgress((int)Faces.size());

    for (int ty = minty; ty < numty; ty++)
    for (int tx = mintx; tx < numtx; tx++)
      omp_init_lock(&SectorLocks[ty][tx]);

    int faces = (int)Faces.size();
#pragma omp parallel for schedule(runtime) shared(Faces, Vertices, SectorFaces, SectorVertices)
    for (int f = 0; f < faces; f++) {
//  for (itf = Faces.begin(); itf != Faces.end(); itf++) {
      class objVertex vo1 = *(Faces[f]->v[0]), *v1;
      class objVertex vo2 = *(Faces[f]->v[1]), *v2;
      class objVertex vo3 = *(Faces[f]->v[2]), *v3;

      int s1x = (int)floor((1.0f / rasterx) * vo1.vtx.x);
      int s1y = (int)floor((1.0f / rastery) * vo1.vtx.y);
      int s2x = (int)floor((1.0f / rasterx) * vo2.vtx.x);
      int s2y = (int)floor((1.0f / rastery) * vo2.vtx.y);
      int s3x = (int)floor((1.0f / rasterx) * vo3.vtx.x);
      int s3y = (int)floor((1.0f / rastery) * vo3.vtx.y);

      /* after the split faces can only belong to one sector
       */
      int sx, sy;

      sx = s1x; sx = min(sx, s2x); sx = min(sx, s3x);
      sy = s1y; sy = min(sy, s2y); sy = min(sy, s3y);

      if ((sy >= minty) && (sy < numty)) {
      if ((sx >= mintx) && (sx < numtx)) {
	int coordx = (sx - offx) * resx;
	int coordy = (sy - offy) * resy;

	if (!skipTile(coordx, coordy, min(resx, resy))) {
	  omp_set_lock(&SectorLocks[sy][sx]);

	  /* this set MUST be searchable by local coordinates
	   * (to enable optimization)
	   * this means local coordinates (not heightfield coordinates)
	   * and floating point rounded values!
	   */
	  vo1.vtx.x = (float)(vo1.x - sx * (sizescale * rasterx));
	  vo1.vtx.y = (float)(vo1.y - sy * (sizescale * rastery));
	  vo2.vtx.x = (float)(vo2.x - sx * (sizescale * rasterx));
	  vo2.vtx.y = (float)(vo2.y - sy * (sizescale * rastery));
	  vo3.vtx.x = (float)(vo3.x - sx * (sizescale * rasterx));
	  vo3.vtx.y = (float)(vo3.y - sy * (sizescale * rastery));

	  /* --------------------------------------------- */
	  set<class objVertex *, struct V>::const_iterator i1;
	  set<class objVertex *, struct V>::const_iterator i2;
	  set<class objVertex *, struct V>::const_iterator i3;

	  v1 = NULL; i1 = SectorVertices[sy][sx].find(&vo1); if (i1 != SectorVertices[sy][sx].end()) v1 = *i1;
	  v2 = NULL; i2 = SectorVertices[sy][sx].find(&vo2); if (i2 != SectorVertices[sy][sx].end()) v2 = *i2;
	  v3 = NULL; i3 = SectorVertices[sy][sx].find(&vo3); if (i3 != SectorVertices[sy][sx].end()) v3 = *i3;

	  if (!v1) {
	    v1 = new(&VPool) class objVertex(); assert(v1);
	    v1->vtx = vo1.vtx;

      	    v1->x = vo1.x - (sizescale * rasterx) * sx;
      	    v1->y = vo1.y - (sizescale * rastery) * sy;
      	    v1->z = vo1.z;

	    assert(v1->x >= 0); assert(v1->x <= sizescale * rasterx);
	    assert(v1->y >= 0); assert(v1->y <= sizescale * rastery);

      	    v1->tx = 0.0f + v1->x / (sizescale * rasterx);
      	    v1->ty = 0.0f + v1->y / (sizescale * rastery);

      	    v1->op.x = vo1.op.x - (rasterx * sx);
      	    v1->op.y = vo1.op.y - (rastery * sy);
      	    v1->oz = vo1.oz;

      	    /* preserve normal (don't change it!!!) */
      	    v1->nx = vo1.nx;
      	    v1->ny = vo1.ny;
      	    v1->nz = vo1.nz;

      	    SectorVertices[sy][sx].insert(v1);
	  }

	  if (!v2) {
	    v2 = new(&VPool) class objVertex(); assert(v2);
	    v2->vtx = vo2.vtx;

      	    v2->x = vo2.x - (sizescale * rasterx) * sx;
	    v2->y = vo2.y - (sizescale * rastery) * sy;
      	    v2->z = vo2.z;

	    assert(v2->x >= 0); assert(v2->x <= sizescale * rasterx);
	    assert(v2->y >= 0); assert(v2->y <= sizescale * rastery);

      	    v2->tx = 0.0f + v2->x / (sizescale * rasterx);
      	    v2->ty = 0.0f + v2->y / (sizescale * rastery);

      	    v2->op.x = vo2.op.x - (rasterx * sx);
      	    v2->op.y = vo2.op.y - (rastery * sy);
      	    v2->oz = vo2.oz;

      	    /* preserve normal (don't change it!!!) */
      	    v2->nx = vo2.nx;
      	    v2->ny = vo2.ny;
      	    v2->nz = vo2.nz;

      	    SectorVertices[sy][sx].insert(v2);
	  }

	  if (!v3) {
	    v3 = new(&VPool) class objVertex(); assert(v3);
	    v3->vtx = vo3.vtx;

      	    v3->x = vo3.x - (sizescale * rasterx) * sx;
	    v3->y = vo3.y - (sizescale * rastery) * sy;
	    v3->z = vo3.z;

	    assert(v3->x >= 0); assert(v3->x <= sizescale * rasterx);
	    assert(v3->y >= 0); assert(v3->y <= sizescale * rastery);

      	    v3->tx = 0.0f + v3->x / (sizescale * rasterx);
      	    v3->ty = 0.0f + v3->y / (sizescale * rastery);

      	    v3->op.x = vo3.op.x - (rasterx * sx);
      	    v3->op.y = vo3.op.y - (rastery * sy);
      	    v3->oz = vo3.oz;

      	    /* preserve normal (don't change it!!!) */
      	    v3->nx = vo3.nx;
      	    v3->ny = vo3.ny;
      	    v3->nz = vo3.nz;

      	    SectorVertices[sy][sx].insert(v3);
	  }

	  class objFace *f;

	  f = new(&FPool) class objFace(); assert(f);
	  f->v[0] = v1; v1->f.push_back(f);
	  f->v[1] = v2; v2->f.push_back(f);
	  f->v[2] = v3; v3->f.push_back(f);
	  f->fill();

	  SectorFaces[sy][sx].push_back(f);

	  omp_unset_lock(&SectorLocks[sy][sx]);
	}
      }
      }

      /* advance progress */
      if ((++tri_sectd & 0xFF) == 0)
	SetProgress(tri_sectd);
    }

    for (int ty = minty; ty < numty; ty++)
    for (int tx = mintx; tx < numtx; tx++)
      omp_destroy_lock(&SectorLocks[ty][tx]);
}

void ExtrudeBorders() {
    set<class objVertex *, struct V>::const_iterator itv;
    vector<class objFace *>::const_iterator itf;

    // 1k == 32, 3k == 96, 512 == 16 */
    int resx = rasterx / 32;
    int resy = rastery / 32;

    // round down, negative side would be smaller than positive side
    int offx = tilesx / 2;
    int offy = tilesy / 2;

    for (int ty = minty; ty < numty; ty++) {
    for (int tx = mintx; tx < numtx; tx++) {
      int coordx = (tx - offx) * resx;
      int coordy = (ty - offy) * resy;

      if (!skipTile(coordx, coordy, min(resx, resy))) {
	for (itv = SectorVertices[ty][tx].begin(); itv != SectorVertices[ty][tx].end(); itv++) {
	  /* border-vertex */
	  if (((*itv)->x == 0) ||
	      ((*itv)->y == 0) ||
	      ((*itv)->x == sizescale * rasterx) ||
	      ((*itv)->y == sizescale * rastery)) {
	  }
	}
      }
    }
    }
}

#if 0
#define	SectorVerticeO	SectorVertices
#define	SectorFaceO	SectorFaces

typedef std::set<class objVertex *, struct V>::iterator viterator;
typedef std::vector<class objFace *>::iterator fiterator;

void roptimize_faces() {
}
#else
std::vector<class objVertex *> SectorVerticeO[128][128];
std::vector<class objFace *> SectorFaceO[128][128];
std::vector<unsigned int> SectorRemapO;

typedef std::vector<class objVertex *>::iterator viterator;
typedef std::vector<class objFace *>::iterator fiterator;

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <list>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include "../AMDTootle/include/tootlelib.h"

#if   _MSC_VER == 1500
/* static lib */
#define LIBFILE	"Static_2k8_"
#ifndef	NDEBUG
#define LIBSUFX	"MT_d"
#else
#define LIBSUFX	"MT"
#endif
#elif _MSC_VER == 1600
/* dynamic lib */
#define LIBFILE	"DLL"
#define LIBSUFX	""
#endif

#ifndef	NDEBUG
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#ifdef	_WIN64
#pragma comment(lib,"../../../AMDTootle/lib/Tootle" LIBFILE LIBSUFX "64")
#else
#pragma comment(lib,"../../../AMDTootle/lib/Tootle" LIBFILE LIBSUFX)
#endif
#else
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#ifdef	_WIN64
#pragma comment(lib,"../../../AMDTootle/lib/Tootle" LIBFILE LIBSUFX "64")
#else
#pragma comment(lib,"../../../AMDTootle/lib/Tootle" LIBFILE LIBSUFX)
#endif
#endif

#include "../AMDTootle/samples/Timer.cpp"
#include "../AMDTootle/samples/Timer.h"

bool optimizemesh = true;

//=================================================================================================================================
/// Enumeration for the choice of test cases for tootle.
//=================================================================================================================================
enum TootleAlgorithm
{
   NA_TOOTLE_ALGORITHM,                // Default invalid choice.
   TOOTLE_VCACHE_ONLY,                 // Only perform vertex cache optimization.
   TOOTLE_CLUSTER_VCACHE_OVERDRAW,     // Call the clustering, optimize vertex cache and overdraw individually.
   TOOTLE_FAST_VCACHECLUSTER_OVERDRAW, // Call the functions to optimize vertex cache and overdraw individually.  This is using
                                       //  the algorithm from SIGGRAPH 2007.
   TOOTLE_OPTIMIZE,                    // Call a single function to optimize vertex cache, cluster and overdraw.
   TOOTLE_FAST_OPTIMIZE                // Call a single function to optimize vertex cache, cluster and overdraw using
                                       //  a fast algorithm from SIGGRAPH 2007.
};

//=================================================================================================================================
/// A simple structure to store the settings for this sample app
//=================================================================================================================================
struct TootleSettings
{
   const char*           pMeshName ;
   const char*           pViewpointName ;
   unsigned int          nClustering ;
   unsigned int          nCacheSize;
   TootleFaceWinding     eWinding;
   TootleAlgorithm       algorithmChoice;         // five different types of algorithm to test Tootle
   TootleOverdrawOptimizer eODrawOptimizer;
   TootleVCacheOptimizer eVCacheOptimizer;        // the choice for vertex cache optimization algorithm, it can be either
                                                  //  TOOTLE_VCACHE_AUTO, TOOTLE_VCACHE_LSTRIPS, TOOTLE_VCACHE_DIRECT3D or
                                                  //  TOOTLE_VCACHE_TIPSY.
   bool                  bOptimizeVertexMemory;   // true if you want to optimize vertex memory location, false to skip
   bool                  bMeasureOverdraw;        // true if you want to measure overdraw, false to skip
};

//=================================================================================================================================
/// A simple structure to hold Tootle statistics
//=================================================================================================================================
struct TootleStats
{
   unsigned int nClusters;
   float        fVCacheIn;
   float        fVCacheOut;
   float        fOverdrawIn;
   float        fOverdrawOut;
   float        fMaxOverdrawIn;
   float        fMaxOverdrawOut;
   double       fOptimizeVCacheTime;
   double       fClusterMeshTime;
   double       fOptimizeOverdrawTime;
   double       fVCacheClustersTime;
   double       fOptimizeVCacheAndClusterMeshTime;
   double       fTootleOptimizeTime;
   double       fTootleFastOptimizeTime;
   double       fMeasureOverdrawTime;
   double       fOptimizeVertexMemoryTime;
};

const float INVALID_TIME = -1;

//=================================================================================================================================
/// A helper function to print formatted TOOTLE statistics
/// \param f      A file to print the statistics to
/// \param pStats The statistics to be printed
//=================================================================================================================================
void PrintStats( FILE *fp, TootleStats* pStats )
{
   if (!fp)
      return;

   assert( fp );
   assert( pStats );

   fprintf( fp, " Clusters         : %u\n"
                " CacheIn/Out      : %.3fx (%.3f/%.3f)\n",
              pStats->nClusters,
              pStats->fVCacheIn/pStats->fVCacheOut,
              pStats->fVCacheIn,
              pStats->fVCacheOut );

   if ( pStats->fMeasureOverdrawTime >= 0 )
   {
      fprintf( fp, " OverdrawIn/Out   : %.3fx (%.3f/%.3f)\n"
                   " OverdrawMaxIn/Out: %.3fx (%.3f/%.3f)\n",
               pStats->fOverdrawIn/pStats->fOverdrawOut,
               pStats->fOverdrawIn,
               pStats->fOverdrawOut,
               pStats->fMaxOverdrawIn/pStats->fMaxOverdrawOut,
               pStats->fMaxOverdrawIn,
               pStats->fMaxOverdrawOut );
   }

   // print out the timing result if appropriate.
   if ( pStats->fOptimizeVCacheTime >= 0 )
   {
      fprintf( fp, " OptimizeVCache               = %.4lf seconds\n", pStats->fOptimizeVCacheTime );
   }

   if ( pStats->fClusterMeshTime >= 0 )
   {
      fprintf( fp, " ClusterMesh                  = %.4lf seconds\n", pStats->fClusterMeshTime );
   }

   if ( pStats->fVCacheClustersTime >= 0 )
   {
      fprintf( fp, " VCacheClusters               = %.4lf seconds\n", pStats->fVCacheClustersTime );
   }

   if ( pStats->fOptimizeVCacheAndClusterMeshTime >= 0 )
   {
      fprintf( fp, " OptimizeVCacheAndClusterMesh = %.4lf seconds\n", pStats->fOptimizeVCacheAndClusterMeshTime );
   }

   if ( pStats->fOptimizeOverdrawTime >= 0 )
   {
      fprintf( fp, " OptimizeOverdraw             = %.4lf seconds\n", pStats->fOptimizeOverdrawTime );
   }

   if ( pStats->fTootleOptimizeTime >= 0 )
   {
      fprintf( fp, " TootleOptimize               = %.4lf seconds\n", pStats->fTootleOptimizeTime );
   }

   if ( pStats->fTootleFastOptimizeTime >= 0 )
   {
      fprintf( fp, " TootleFastOptimize           = %.4lf seconds\n", pStats->fTootleFastOptimizeTime );
   }

   if ( pStats->fMeasureOverdrawTime >= 0 )
   {
      fprintf( fp, " MeasureOverdraw              = %.4lf seconds\n", pStats->fMeasureOverdrawTime );
   }

   if ( pStats->fOptimizeVertexMemoryTime >= 0 )
   {
      fprintf( fp, " OptimizeVertexMemory         = %.4lf seconds\n", pStats->fOptimizeVertexMemoryTime );
   }
}

// 3D Vector ( for posistion and normals )
struct Vertex3D {
   float x;
   float y;
   float z;
}; // End of ObjVertex3D

void OptimizeGeometry() {
    set<class objVertex *, struct V>::const_iterator itv;
    vector<class objFace *>::const_iterator itf;

    // initialize settings to defaults
    TootleSettings settings;

    settings.pMeshName             = NULL;
    settings.pViewpointName        = NULL;
    settings.nClustering           = 0;
    settings.nCacheSize            = TOOTLE_DEFAULT_VCACHE_SIZE;
    settings.eWinding              = TOOTLE_CCW;
#ifndef	NDEBUG
    settings.algorithmChoice       = (1 ? TOOTLE_FAST_OPTIMIZE : TOOTLE_OPTIMIZE);
    settings.eODrawOptimizer	   = (1 ? TOOTLE_OVERDRAW_FAST : TOOTLE_OVERDRAW_RAYTRACE);
#else
    settings.algorithmChoice       = (0 ? TOOTLE_FAST_OPTIMIZE : TOOTLE_OPTIMIZE);
    settings.eODrawOptimizer	   = (0 ? TOOTLE_OVERDRAW_FAST : TOOTLE_OVERDRAW_RAYTRACE);
#endif
    settings.eVCacheOptimizer      = TOOTLE_VCACHE_TIPSY;            // the auto selection as the default to optimize vertex cache
    settings.bOptimizeVertexMemory = true;                           // default value is to optimize the vertex memory
    settings.bMeasureOverdraw      = true;                           // default is to measure overdraw

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

      SetTopic("Optimizing tile {%d,%d}:", coordx, coordy);

      if (!skipTile(coordx, coordy, min(resx, resy))) {
      char buf[256];

      /* VBs starts on index 0 */
      unsigned int i = 0, idx = 0;

      sprintf(buf, "%02d.%02d.%02d.%02d", wdspace, coordx, coordy, min(resx, resy));

      logpf("optimizing \"%s\"\n", buf);
      logpf(" Vertices         : %d\n", SectorVertices[ty][tx].size());
      logpf(" Faces            : %d\n", SectorFaces   [ty][tx].size());

      // build buffers containing only the vertex positions and indices, since this is what Tootle requires
      std::vector<Vertex3D> vertices;
      vertices.resize(SectorVertices[ty][tx].size());

      SectorVerticeO[ty][tx].resize(SectorVertices[ty][tx].size());
      i = 0; idx = 0; for (itv = SectorVertices[ty][tx].begin(); itv != SectorVertices[ty][tx].end(); itv++) {
        SectorVerticeO[ty][tx][idx] = (*itv);

      	/* assign index the moment of writing it out */
	(*itv)->idx = idx++;

        vertices[i].x = (float)((*itv)->x);
        vertices[i].y = (float)((*itv)->y);
        vertices[i].z = (float)((*itv)->z); i++;
      }

      std::vector<unsigned int> indices;
      indices.resize(SectorFaces[ty][tx].size() * 3);

      SectorFaceO[ty][tx].resize(SectorFaces[ty][tx].size());
      i = 0; idx = 0; for (itf = SectorFaces[ty][tx].begin(); itf != SectorFaces[ty][tx].end(); itf++) {
        SectorFaceO[ty][tx][idx] = (*itf);

	/* assign index the moment of writing it out */
	idx++;

        indices[i] = (*itf)->v[0]->idx; i++;
        indices[i] = (*itf)->v[1]->idx; i++;
        indices[i] = (*itf)->v[2]->idx; i++;
      }

      if (optimizemesh) {
	// ******************************************
	//    Load viewpoints if necessary
	// ******************************************

	// read viewpoints if needed
	std::vector<Vertex3D> viewpoints;
  #if 1
	viewpoints.resize(4 * 16);

	Real radius = (sizescale * rasterx);
	Real center = radius * 0.5f;
	Real diamtr = radius * 2.0f;
	/* restrict to pretty steep angles (highest mountains
	 * towards the plains, no below sea-level up views)
	 */
	Real height = radius * 0.25f; // 45°, 22.5°, 11.25°
  //    Real height = 8192.0;

	/* cameras on a cylinder around the terrain-patch (unit-sphere though) */
	for (int h = 0; h <  4; h++)
	for (int a = 0; a < 16; a++) {
	  Real x = radius * cos((M_PI * a * 2) / 16);
	  Real y = radius * sin((M_PI * a * 2) / 16);
	  Real z = (height * h) / 4;
	  Real n = sqrt(x * x + y * y + z * z);
	  struct Vertex3D v = {(float)(x / n), (float)(y / n), (float)(z / n)};

	  viewpoints.push_back(v);
	}
  #endif

	// if we didn't get any viewpoints, then use a NULL array
	const float* pViewpoints = NULL;
	unsigned int nViewpoints = (unsigned int)viewpoints.size();
	if (viewpoints.size() > 0)
	  pViewpoints = (const float *)&viewpoints[0];

	// *****************************************************************
	//   Prepare the mesh and initialize stats variables
	// *****************************************************************

	unsigned int  nFaces    = (unsigned int)  indices.size() / 3;
	unsigned int  nVertices = (unsigned int)  vertices.size();
	float*        pfVB      = (float *)       &vertices[0];
	unsigned int* pnIB      = (unsigned int *)&indices[0];
	unsigned int  nStride   = 3 * sizeof(float);

	TootleStats stats;

	// initialize the timing variables
	stats.fOptimizeVCacheTime               = INVALID_TIME;
	stats.fClusterMeshTime                  = INVALID_TIME;
	stats.fVCacheClustersTime               = INVALID_TIME;
	stats.fOptimizeVCacheAndClusterMeshTime = INVALID_TIME;
	stats.fOptimizeOverdrawTime             = INVALID_TIME;
	stats.fTootleOptimizeTime               = INVALID_TIME;
	stats.fTootleFastOptimizeTime           = INVALID_TIME;
	stats.fMeasureOverdrawTime              = INVALID_TIME;
	stats.fOptimizeVertexMemoryTime         = INVALID_TIME;

	TootleResult result;

	// measure input VCache efficiency
	if (TootleMeasureCacheEfficiency(pnIB, nFaces, settings.nCacheSize, &stats.fVCacheIn) != TOOTLE_OK)
      	  continue;

	if (settings.bMeasureOverdraw) {
	  // measure input overdraw.  Note that we assume counter-clockwise vertex winding.
	  TootleMeasureOverdraw(pfVB, pnIB, nVertices, nFaces, nStride, pViewpoints, nViewpoints, settings.eWinding,
					&stats.fOverdrawIn, &stats.fMaxOverdrawIn);
	}

	// allocate an array to hold the cluster ID for each face
	std::vector<unsigned int> faceClusters;
	faceClusters.resize(nFaces + 1);
	unsigned int nNumClusters;

	Timer timer;
	timer.Reset();

	// **********************************************************************************************************************
	//   Optimize the mesh:
	//
	// The following cases show five examples for developers on how to use the library functions in Tootle.
	// 1. If you are interested in optimizing vertex cache only, see the TOOTLE_VCACHE_ONLY case.
	// 2. If you are interested to optimize vertex cache and overdraw, see either TOOTLE_CLUSTER_VCACHE_OVERDRAW
	//     or TOOTLE_OPTIMIZE cases.  The former uses three separate function calls while the latter uses a single
	//     utility function.
	// 3. To use the algorithm from SIGGRAPH 2007 (v2.0), see TOOTLE_FAST_VCACHECLUSTER_OVERDRAW or TOOTLE_FAST_OPTIMIZE
	//     cases.  The former uses two separate function calls while the latter uses a single utility function.
	//
	// Note the algorithm from SIGGRAPH 2007 (v2.0) is very fast but produces less quality results especially for the
	//  overdraw optimization.  During our experiments with some medium size models, we saw an improvement of 1000x in
	//  running time (from 20+ minutes to less than 1 second) for using v2.0 calls vs v1.2 calls.  The resulting vertex
	//  cache optimization is very similar while the overdraw optimization drops from 3.8x better to 2.5x improvement over
	//  the input mesh.
	//  Developers should always run the overdraw optimization using the fast algorithm from SIGGRAPH initially.
	//  If they require a better result, then re-run the overdraw optimization using the old v1.2 path (TOOTLE_OVERDRAW_AUTO).
	//  Passing TOOTLE_OVERDRAW_AUTO to the algorithm will let the algorithm choose between Direct3D or raytracing path
	//  depending on the total number of clusters (less than 200 clusters, it will use Direct3D path otherwise it will
	//  use raytracing path since the raytracing path will be faster than the Direct3D path at that point).
	//
	// Tips: use v2.0 for fast optimization, and v1.2 to further improve the result by mix-matching the calls.
	// **********************************************************************************************************************

	result = TOOTLE_OK;
	switch (settings.algorithmChoice) {
	  case TOOTLE_VCACHE_ONLY:
	    // *******************************************************************************************************************
	    // Perform Vertex Cache Optimization ONLY
	    // *******************************************************************************************************************
	    stats.nClusters = 1;

	    // Optimize vertex cache
	    if ((result = TootleOptimizeVCache(pnIB, nFaces, nVertices, settings.nCacheSize,
				     pnIB, NULL, settings.eVCacheOptimizer)) != TOOTLE_OK)
	      break;

	    stats.fOptimizeVCacheTime = timer.GetElapsed();
	    break;

	  case TOOTLE_CLUSTER_VCACHE_OVERDRAW:
	    // *******************************************************************************************************************
	    // An example of calling clustermesh, vcacheclusters and optimize overdraw individually.
	    // This case demonstrate mix-matching v1.2 clustering with v2.0 overdraw optimization.
	    // *******************************************************************************************************************

	    // Cluster the mesh, and sort faces by cluster.
	    if ((result = TootleClusterMesh(pfVB, pnIB, nVertices, nFaces, nStride,
          	   settings.nClustering, pnIB, &faceClusters[0], NULL)) != TOOTLE_OK)
	       break;

	    stats.fClusterMeshTime = timer.GetElapsed();
	    timer.Reset();

	    // The last entry of of faceClusters store the total number of clusters.
	    stats.nClusters = faceClusters[nFaces];

	    // Perform vertex cache optimization on the clustered mesh.
	    if ((result = TootleVCacheClusters(pnIB, nFaces, nVertices,
          	  settings.nCacheSize, &faceClusters[0],
		   pnIB, NULL, settings.eVCacheOptimizer)) != TOOTLE_OK)
	       break;

	    stats.fVCacheClustersTime = timer.GetElapsed();
	    timer.Reset();

	    // Optimize the draw order (using v1.2 path: TOOTLE_OVERDRAW_AUTO, the default path is from v2.0--SIGGRAPH version).
	    if ((result = TootleOptimizeOverdraw(pfVB, pnIB, nVertices, nFaces, nStride, pViewpoints, nViewpoints,
          	  settings.eWinding, &faceClusters[0], pnIB, NULL, settings.eODrawOptimizer)) != TOOTLE_OK)
	       break;

	    stats.fOptimizeOverdrawTime = timer.GetElapsed();
	    break;

	  case TOOTLE_FAST_VCACHECLUSTER_OVERDRAW:
	    // *******************************************************************************************************************
	    // An example of calling v2.0 optimize vertex cache and clustering mesh with v1.2 overdraw optimization.
	    // *******************************************************************************************************************

	    // Optimize vertex cache and create cluster
	    // The algorithm from SIGGRAPH combine the vertex cache optimization and clustering mesh into a single step
	    if ((result = TootleFastOptimizeVCacheAndClusterMesh(pnIB, nFaces, nVertices,
          	  settings.nCacheSize, pnIB, &faceClusters[0], &nNumClusters, TOOTLE_DEFAULT_ALPHA)) != TOOTLE_OK)
	       break;

	    stats.fOptimizeVCacheAndClusterMeshTime = timer.GetElapsed();
	    timer.Reset();

	    stats.nClusters = nNumClusters;

	    // In this example, we use TOOTLE_OVERDRAW_AUTO to show that we can mix-match the clustering and
	    //  vcache computation from the new library with the overdraw optimization from the old library.
	    //  TOOTLE_OVERDRAW_AUTO will choose between using Direct3D or CPU raytracing path.  This path is
	    //  much slower than TOOTLE_OVERDRAW_FAST but usually produce 2x better results.
	    if ((result = TootleOptimizeOverdraw(pfVB, pnIB, nVertices, nFaces, nStride, NULL, 0,
          	  settings.eWinding, &faceClusters[0], pnIB, NULL, settings.eODrawOptimizer)) != TOOTLE_OK)
	       break;

	    stats.fOptimizeOverdrawTime = timer.GetElapsed();
	    break;

	  case TOOTLE_OPTIMIZE:
	    // *******************************************************************************************************************
	    // An example of using a single utility function to perform v1.2 optimizations.
	    // *******************************************************************************************************************

	    // This function will compute the entire optimization (cluster mesh, vcache per cluster, and optimize overdraw).
	    // It will use TOOTLE_OVERDRAW_FAST as the default overdraw optimization
	    if ((result = TootleOptimize(pfVB, pnIB, nVertices, nFaces, nStride,
          	  settings.nCacheSize, pViewpoints, nViewpoints, settings.eWinding,
          	  pnIB, &nNumClusters, settings.eVCacheOptimizer)) != TOOTLE_OK)
	       break;

	    stats.fTootleOptimizeTime = timer.GetElapsed();
	    stats.nClusters = nNumClusters;
	    break;

	  case TOOTLE_FAST_OPTIMIZE:
	    // *******************************************************************************************************************
	    // An example of using a single utility function to perform v2.0 optimizations.
	    // *******************************************************************************************************************

	    // This function will compute the entire optimization (optimize vertex cache, cluster mesh, and optimize overdraw).
	    // It will use TOOTLE_OVERDRAW_FAST as the default overdraw optimization
	    if ((result = TootleFastOptimize(pfVB, pnIB, nVertices, nFaces, nStride,
          	  settings.nCacheSize, settings.eWinding, pnIB, &nNumClusters, TOOTLE_DEFAULT_ALPHA)) != TOOTLE_OK)
	      break;

	    stats.fTootleFastOptimizeTime = timer.GetElapsed();
	    stats.nClusters = nNumClusters;
	    break;

	  default:
	    // wrong algorithm choice
	    break;
	}

	if (result != TOOTLE_OK)
	  continue;

	// measure output VCache efficiency
	if (TootleMeasureCacheEfficiency(pnIB, nFaces, settings.nCacheSize, &stats.fVCacheOut) != TOOTLE_OK)
	  continue;

	if (settings.bMeasureOverdraw) {
	  // measure output overdraw
	  timer.Reset();

	  TootleMeasureOverdraw(pfVB, pnIB, nVertices, nFaces, nStride, pViewpoints, nViewpoints, settings.eWinding,
					  &stats.fOverdrawOut, &stats.fMaxOverdrawOut);

	  stats.fMeasureOverdrawTime = timer.GetElapsed();
	}

	//-----------------------------------------------------------------------------------------------------
	// recreate the faces based on the reordered index-buffer

	for (unsigned int ii = 0, iif = 0; ii < indices.size(); ii += 3, iif += 1) {
	  class objFace *f = SectorFaceO[ty][tx][iif];

	  f->v[0]->idx = indices[ii + 0];
	  f->v[1]->idx = indices[ii + 1];
	  f->v[2]->idx = indices[ii + 2];

	  /* recreate normal (doesn't change by reordering) */
	  f->fill();
	}

	//-----------------------------------------------------------------------------------------------------
	// PERFORM VERTEX MEMORY OPTIMIZATION (rearrange memory layout for vertices based on the final indices
	//  to exploit vertex cache prefetch).
	//  We want to optimize the vertex memory locations based on the final optimized index buffer that will
	//  be in the output file.
	//  Thus, in this sample code, we recompute a copy of the indices that point to the original vertices
	//  (pnIBTmp) to be passed into the function TootleOptimizeVertexMemory.  If we use the array pnIB, we
	//  will optimize for the wrong result since the array pnIB is based on the rehashed vertex location created
	//  by the function ObjLoader.
	//-----------------------------------------------------------------------------------------------------
	timer.Reset();

	// The actual total number of vertices referenced by the indices
	if (settings.bOptimizeVertexMemory) {
	  SectorRemapO.resize(nVertices);
	  unsigned int* pfRM = (unsigned int *)&SectorRemapO[0];

	  // For this sample code, we are just going to use vertexRemapping array result.  This is to support general obj
	  //  file input and output.
	  //  In fact, we are sending the wrong vertex buffer here (it should be based on the original file instead of the
	  //  rehashed vertices).  But, it is ok because we do not request the reordered vertex buffer as an output.
	  if (TootleOptimizeVertexMemory(pfVB, pnIB, nVertices, nFaces, nStride,
					 pfVB, pnIB, pfRM) != TOOTLE_OK)
	    continue;

	  //TootleResult TOOTLE_DLL TootleOptimizeVertexMemory( const void*         pVB,
	  //                                                    const unsigned int* pnIB,
	  //                                                    unsigned int        nVertices,
	  //                                                    unsigned int        nFaces,
	  //                                                    unsigned int        nVBStride,
	  //                                                    void*               pVBOut,
	  //                                                    unsigned int*       pnIBOut,
	  //                                                    unsigned int*       pnVertexRemapOut );

	  //-----------------------------------------------------------------------------------------------------
	  // recreate the vertices/faces based on the reordered vertex/index-buffer

	  i = 0; idx = 0; for (itv = SectorVertices[ty][tx].begin(); itv != SectorVertices[ty][tx].end(); itv++) {
	    /* move from position i to idx */
	    idx = SectorRemapO[i++];

	    SectorVerticeO[ty][tx][idx] = (*itv);

	    /* normal is still valid this way */
	    (*itv)->idx = idx;
	  }

	  //-----------------------------------------------------------------------------------------------------
	  // recreate the faces based on the reordered index-buffer

	  for (unsigned int ii = 0, iif = 0; ii < indices.size(); ii += 3, iif += 1) {
	    class objFace *f = SectorFaceO[ty][tx][iif];

	    f->v[0]->idx = indices[ii + 0];
	    f->v[1]->idx = indices[ii + 1];
	    f->v[2]->idx = indices[ii + 2];

	    /* recreate normal (doesn't change by reordering) */
	    f->fill();
	  }

	  stats.fOptimizeVertexMemoryTime = timer.GetElapsed();
	}

	PrintStats(logfile, &stats);
      }
      }

      /* advance progress */
      SetProgress((numty - minty) * (ty - minty) + (tx - mintx) + 1);
    }
    }
}
#endif
#endif

void freeGeometry() {
  set<class objVertex *, struct V>::iterator itv;
  vector<class objFace *>::iterator itf;

#if 0
  for (itf = Faces.begin(); itf != Faces.end(); itf++)
    delete(&FPool) (*itf);
  for (itv = Vertices.begin(); itv != Vertices.end(); itv++)
    delete(&VPool) (*itv);

  for (int ty = minty; ty < numty; ty++) {
  for (int tx = mintx; tx < numtx; tx++) {
    for (itf = SectorFaces[ty][tx].begin(); itf != SectorFaces[ty][tx].end(); itf++)
      delete (*itf);
    for (itv = SectorVertices[ty][tx].begin(); itv != SectorVertices[ty][tx].end(); itv++)
      delete (*itv);
  }
  }
#endif

  /* raw containers */
  Faces.clear();
  Vertices.clear();

  for (int ty = minty; ty < numty; ty++) {
  for (int tx = mintx; tx < numtx; tx++) {
    /* unoptimized containers */
    SectorFaces[ty][tx].clear();
    SectorVertices[ty][tx].clear();

    /* optimized containers */
    SectorFaceO[ty][tx].clear();
    SectorVerticeO[ty][tx].clear();
  }
  }

  SectorRemapO.clear();

  /* get rid of the pools */
  FPool.releaseMemAllocated();
  VPool.releaseMemAllocated();
}
