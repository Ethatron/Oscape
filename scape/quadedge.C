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

#include <stdlib.h>
#include <random.h>
#include <iostream>
#include "Basic.H"
#include "quadedge.H"
#include "stuff.H"

class MemQueue<4> PPool(sizeof(Point2d ), 0x7FFFFFFF);
class MemQueue<4> LPool(sizeof(Line    ), 0x7FFFFFFF);

class MemQueue<4> QPool(sizeof(QuadEdge), 0x7FFFFFFF);
class MemQueue<4> TPool(sizeof(Triangle), 0x7FFFFFFF);
class MemQueue<4> EPool(sizeof(    Edge), 0x7FFFFFFF);

////////////////////////////////////////////////////////////////////////
// This code is a modified version of the Delaunay triangulator
// written by Dani Lischinski
// in Graphics Gems IV
////////////////////////////////////////////////////////////////////////


/*********************** Basic Topological Operators ************************/

Edge* MakeEdge() {
  QuadEdge *ql = new(&QPool) QuadEdge;
  return ql->e;
}

// This operator affects the two edge rings around the origins of a and b,
// and, independently, the two edge rings around the left faces of a and b.
// In each case, (i) if the two rings are distinct, Splice will combine
// them into one; (ii) if the two are the same ring, Splice will break it
// into two separate pieces.
// Thus, Splice can be used both to attach the two edges together, and
// to break them apart. See Guibas and Stolfi (1985) p.96 for more details
// and illustrations.
void Splice(Edge *a, Edge *b) {
  Edge* alpha = a->Onext()->Rot();
  Edge* beta  = b->Onext()->Rot();

  Edge* t1 = b->Onext();
  Edge* t2 = a->Onext();
  Edge* t3 = beta->Onext();
  Edge* t4 = alpha->Onext();

  a->next = t1;
  b->next = t2;
  alpha->next = t3;
  beta->next = t4;
}

void DeleteEdge(Edge *e) {
  Splice(e, e->Oprev());
  Splice(e->Sym(), e->Sym()->Oprev());
  QPool.releaseBlock(e->Qedge());
}

/************* Topological Operations for Delaunay Diagrams *****************/

// Initialize a subdivision to the rectangle defined by the points a, b, c, d.
void Subdivision::init(const Point2d& a, const Point2d& b,
		       const Point2d& c, const Point2d& d) {
  Point2d *da, *db, *dc, *dd;

  da = new(&PPool) Point2d(a), db = new(&PPool) Point2d(b);
  dc = new(&PPool) Point2d(c), dd = new(&PPool) Point2d(d);

  Edge* ea = MakeEdge();
  ea->EndPoints(da, db);
  Edge* eb = MakeEdge();
  Splice(ea->Sym(), eb);
  eb->EndPoints(db, dc);
  Edge* ec = MakeEdge();
  Splice(eb->Sym(), ec);
  ec->EndPoints(dc, dd);
  Edge *ed = MakeEdge();
  Splice(ec->Sym(), ed);
  ed->EndPoints(dd, da);
  Splice(ed->Sym(), ea);
  startingEdge = ea;

  Edge *diag = MakeEdge();
  Splice(ed->Sym(), diag);
  Splice(eb->Sym(), diag->Sym());
  diag->EndPoints(da, dc);

  firstFace = NULL;

  Triangle *f1 = makeFace(ea->Sym());
  Triangle *f2 = makeFace(ec->Sym());
}

void Subdivision::free() {
  PPool.releaseMemAllocated();
  LPool.releaseMemAllocated();

  QPool.releaseMemAllocated();
  TPool.releaseMemAllocated();
  EPool.releaseMemAllocated();
}

// Add a new edge e connecting the destination of a to the
// origin of b, in such a way that all three have the same
// left face after the connection is complete.
// Additionally, the data pointers of the new edge are set.
Edge* Connect(Edge* a, Edge* b) {
  Edge* e = MakeEdge();
  Splice(e, a->Lnext());
  Splice(e->Sym(), b);
  e->EndPoints(a->Dest(), b->Org());

  return e;
}

// These two variables track what faces InsertSite would like to recycle.
// And, yes, this is rather loathsome.
//
// Faces are recycled to optimize heap operations, so that we can recycle
// and update heap entries instead of deleting and then inserting.
// It also saves on destruction and construction of Triangles, but that's
// a much smaller cost.
static Triangle *recycle1 = NULL;
static Triangle *recycle2 = NULL;

void Subdivision::rebuildFace(Edge *e) {
  Triangle *f;

  if (recycle1) {
    f = recycle1;
    f->setAnchor(e);
    f->attachFace();
    recycle1 = NULL;
  }
  else if (recycle2) {
    f = recycle2;
    f->setAnchor(e);
    f->attachFace();
    recycle2 = NULL;
  }
  else
    f = makeFace(e);

  // this call creates a new Triangle with null heap index,
  // among other things
}


// Essentially turns edge e counterclockwise inside its enclosing
// quadrilateral. The data pointers are modified accordingly.
void Swap(Edge* e) {
  Triangle *f1 = e->Lface();
  Triangle *f2 = e->Sym()->Lface();

  Edge* a = e->Oprev();
  Edge* b = e->Sym()->Oprev();
  Splice(e, a);
  Splice(e->Sym(), b);
  Splice(e, a->Lnext());
  Splice(e->Sym(), b->Lnext());
  e->EndPoints(a->Dest(), b->Dest());

  f1->setAnchor(e);
  f2->setAnchor(e->Sym());
  f1->attachFace();
  f2->attachFace();
}

/*************** Geometric Predicates for Delaunay Diagrams *****************/

// Returns TRUE if the point d is inside the circle defined by the
// points a, b, c. See Guibas and Stolfi (1985) p.107.
static inline bool isInCircle(const Point2d& a, const Point2d& b,
			      const Point2d& c, const Point2d& d) {
  return (a.x * a.x + a.y * a.y) * getArea2x(b, c, d) -
	 (b.x * b.x + b.y * b.y) * getArea2x(a, c, d) +
	 (c.x * c.x + c.y * c.y) * getArea2x(a, b, d) -
	 (d.x * d.x + d.y * d.y) * getArea2x(a, b, c) > 0;
}

// Returns TRUE if the points a, b, c are in a counterclockwise order
static inline bool isCCW(const Point2d& a, const Point2d& b, const Point2d& c) {
  return (getArea2x(a, b, c) > 0);
}

static inline bool isRightOf(const Point2d& x, const Edge* e) {
  return isCCW(x, e->Dest2d(), e->Org2d());
}

static inline bool isLeftOf(const Point2d& x, const Edge* e) {
  return isCCW(x, e->Org2d(), e->Dest2d());
}

bool Edge::isCCWPerim() {
  return !isRightOf(Oprev()->Dest2d(), this);
}

// A predicate that determines if the point x is on the edge e.
// The point is considered on if it is in the EPS-neighborhood
// of the edge.
static inline bool isOnEdge(const Point2d& x, const Edge* e) {
  Real t1, t2, t3;

  t1 = (x - e->Org2d ()).norm();
  t2 = (x - e->Dest2d()).norm();

  if (t1 < EPS || t2 < EPS)
    return true;

  t3 = (e->Org2d() - e->Dest2d()).norm();
  if (t1 > t3 || t2 > t3)
    return false;

  Line line(e->Org2d(), e->Dest2d());
  return (fabs(line.eval(x)) < EPS);
}

/************* An Incremental Algorithm for the Construction of *************/
/************************ Delaunay Diagrams *********************************/

// Returns an edge e, s.t. the triangle to the left of e is interior to the
// subdivision and either x is on e (inclusive of endpoints) or x lies in the
// interior of the triangle to the left of e.
// The search starts from either hintedge, if it is not NULL, else
// startingEdge, and proceeds in the general direction of x.
//
// Algorithm is a variant of Green and Sibson's walking method for
// point location, as described by Guibas and Stolfi (ACM Trans. on Graphics,
// Apr. 1985, p.121), but modified in three ways:
//	* Randomness added to avoid infinite loops.
//	* Supports queries on perimeter of subdivision,
//	  provided perimeter is convex.
//	* Uses two area computations per step, not three.
Edge *Subdivision::Locate(const Point2d& x, Edge *hintedge) {
    Edge* e = hintedge ? hintedge : startingEdge, *eo, *ed;
    Real t, to, td;

    t = getArea2x(x, e->Dest2d(), e->Org2d());
    // x is to the right of edge e
    if (t > 0) {
      t = -t;
      e = e->Sym();
    }

    // x is on e or to the left of e

    // edges e, eo, ed point upward in the diagram below:
    //
    //         /|
    //     ed / |
    //       /  |
    //      /   |
    //     /    |
    //     \    | e
    //      \   |
    //       \  |
    //     eo \ |
    //         \|

    while (TRUE) {
	eo = e->Onext();
	to = getArea2x(x, eo->Dest2d(), eo->Org2d());
	ed = e->Dprev();
	td = getArea2x(x, ed->Dest2d(), ed->Org2d());
	if (td>0)			// x is below ed
	    if (to>0 || to==0 && t==0) {// x is interior, or origin endpoint
		startingEdge = e;
		return e;
	    }
	    else {			// x is below ed, below eo
		t = to;
		e = eo;
	    }
	else				// x is on or above ed
	    if (to>0)			// x is above eo
		if (td==0 && t==0) {	// x is destination endpoint
		    startingEdge = e;
		    return e;
		}
		else {			// x is on or above ed and above eo
		    t = td;
		    e = ed;
		}
	    else			// x is on or below eo
		if (t==0 && !isLeftOf(eo->Dest2d(), e))
					// x on e but subdiv. is to right
		    e = e->Sym();
		else if (random()&1) {	// x is on or above ed and
		    t = to;		// on or below eo; step randomly
		    e = eo;
		}
		else {
		    t = td;
		    e = ed;
		}
    }
}

// Inserts a new point x into triangle tri of a subdivision and
// adds "spokes" connecting the point to the vertices of the surrounding
// polygon.  Returns a pointer to one of the inward-pointing spokes.
//
// --- Tri can be NULL
Edge *Subdivision::Spoke(const Point2d& x, Triangle *tri) {
  // Point x is inside the triangle tri or on its boundary.
  // To make sure boundary cases are handled properly, we call Locate.
  Edge* e = Locate(x, tri ? tri->getAnchor() : NULL);

  if ((x == e->Org2d ()) ||
      (x == e->Dest2d())) {
    // point is already in the mesh
    cout << "already in mesh: (" << x.x << "," << x.y << ")" << endl;
    assert(0);
  }

  Edge *pedge = 0;
  if (isOnEdge(x, e)) {
    if (e->isCCWPerim()) {
      // if point x lies on a perimeter edge then add spokes
      // before deleting it
      recycle1 = e->Lface();
      recycle1->dontAnchor(e);

      // save pointer to old perimeter edge
      pedge = e;		
    }
    else {
      // point is on an edge -- delete 2 faces
      // unless the edge is a border edge and has no outer face
      recycle1 = e->Lface();
      recycle1->dontAnchor(e);
      recycle2 = e->Sym()->Lface();
      recycle2->dontAnchor(e->Sym());

      e = e->Oprev();
      DeleteEdge(e->Onext());
    }
  } 
  else {
    // point is in triangle, delete that face only
    recycle1 = e->Lface();
    recycle1->dontAnchor(e);
  }

  // Add spokes: connect the new point to the vertices of the containing
  // triangle (or quadrilateral, if the new point fell on an
  // existing edge.)
  Edge* base = MakeEdge();
  base->EndPoints(e->Org(), new(&PPool) Point2d(x));
  Splice(base, e);
  startingEdge = base;
  do {
    base = Connect(e, base->Sym());
    e = base->Oprev();
  } while (e->Lnext() != startingEdge);

  // delete old perimeter edge and mark new ones
  if (pedge)		
    DeleteEdge(pedge);

  // Update all the faces in our new spoked polygon.
  // If point x on perimeter, then don't add an exterior face
  base = pedge ? startingEdge->Rprev() : startingEdge->Sym();
  do {
    rebuildFace(base);
    base = base->Onext();
  } while(base != startingEdge->Sym());

  return startingEdge;
}

//
// Tests whether e is an interior edge.
//
// WARNING: This topological test will not work if the boundary is
//          a triangle.  This is not a problem in scape; the boundary is
//          always a rectangle.  But if you try to adapt this code, please
//          keep this in mind.
inline bool Subdivision::isInterior(Edge *e) {
  return ((e->Lnext()->Lnext()->Lnext() == e) &&
          (e->Rnext()->Rnext()->Rnext() == e));
}


// Inserts a new point x into triangle tri of a subdivision representing
// a Delaunay triangulation, and fixes the affected edges so that the result
// is still a Delaunay triangulation. This is based on the
// pseudocode from Guibas and Stolfi (1985) p.120, with slight
// modifications and a bug fix.
// Returns a pointer to an outward-pointing spoke.
//
// --- Tri can be NULL
Edge *Subdivision::InsertSite(const Point2d& x, Triangle *tri) {
  Edge *startspoke = Spoke(x, tri);

  //
  // Reorient the spoke so that it points away from the insertion site.
  // This is what our caller expects to get back, and I prefer to think
  // of the spokes this way.
  startspoke = startspoke->Sym();
  Edge *s = startspoke;

  do {
    Edge *e = s->Lnext();
    Edge *t = e->Oprev();

    if (isInterior(e) && isInCircle(e->Org2d(), t->Dest2d(), e->Dest2d(), x))
      Swap(e);
    else {
      s = s->Onext();
      if (s == startspoke)
	break;
    }
  } while(TRUE);

  return startspoke;
}


/*****************************************************************************/

static unsigned int timestamp = 0;

void Subdivision::OverEdges(edge_callback f, void *closure) {
  if (++timestamp == 0)
    timestamp = 1;

  startingEdge->OverEdges(timestamp, f, closure);
}

void Edge::OverEdges(unsigned int stamp, edge_callback f, void *closure) {
  if (Qedge()->TimeStamp(stamp)) {
    (*f)(this, closure);

    // Recurse to neighbors
    Onext()->OverEdges(stamp, f, closure);
    Oprev()->OverEdges(stamp, f, closure);
    Dnext()->OverEdges(stamp, f, closure);
    Dprev()->OverEdges(stamp, f, closure);
  }
}

void Subdivision::OverFaces(face_callback f, void *closure) {
  Triangle *t = firstFace;
  while (t) {
    (*f)(t, closure);

    t = t->next;
  }
}


UpdateRegion::UpdateRegion(Edge *e) {
  start = e;
}

Triangle *UpdateRegion::first() {
  current = start;
  if (current->Lface())
    return current->Lface();
  // in case p is on perimeter and one of faces is null
  current = current->Onext();
  return current->Lface();
  // no need to check, can't have two null faces
}

Triangle *UpdateRegion::next() {
  current = current->Onext();
  if (current==start)
    return NULL;
  if (current->Lface())
    return current->Lface();
  // in case p is on perimeter and one of faces is null
  current = current->Onext();
  if (current==start)
    return NULL;
  return current->Lface();
  // no need to check, can't have two null faces
}

void Triangle::attachFace() {
  anchor->set_Lface(this);
  anchor->Lnext()->set_Lface(this);
  anchor->Lprev()->set_Lface(this);
}

void Triangle::dontAnchor(Edge *e) {
  if (anchor == e) {
    if (e->Lnext()->Lface() == this)
      anchor = e->Lnext();
    else if (e->Lprev()->Lface() == this)
      anchor = e->Lprev();
    else
      assert(NULL);
  }
}

// intersect: point of intersection p of two lines k, l
void intersect(const Line &k, const Line &l, Point2d &p) {
  Real den = k.a * l.b - k.b * l.a;
  assert(den != 0);

  p.x = (k.b * l.c - k.c * l.b) / den;
  p.y = (k.c * l.a - k.a * l.c) / den;
}


// routines to compute the number of edges, vertices, and faces

// returns number of triangles adjacent to vertex at e->Org
// (counts only interior faces, not exterior faces)
static inline int vert_degree(Edge *e) {
  Edge *e0 = e;
  int deg = 0;
  do {
    deg += e->Lface() != 0;
    e = e->Onext();
  } while (e != e0);

  return deg;
}

// returns number of triangles adjacent to edge e
static inline int edge_degree(Edge *e) {
  return (e->       Lface() != 0) +
         (e->Sym()->Lface() != 0);
}

static double dv, de;
static int df;

static inline void count_vef(Triangle *tri, void *) {
  Edge *e1 = tri->getAnchor();
  Edge *e2 = e1->Lnext();
  Edge *e3 = e2->Lnext();

  dv += 1. / vert_degree(e1) + 1. / vert_degree(e2) + 1. / vert_degree(e3);
  de += 1. / edge_degree(e1) + 1. / edge_degree(e2) + 1. / edge_degree(e3);

  df++;
}

// returns number of vertices, edges, and faces in subdivision
void Subdivision::vef(int &nv, int &ne, int &nf) {
  dv = de = 0;
  df = 0;

  OverFaces(count_vef, 0);

  // round, in case of roundoff error
  nv = (int)(dv + .5);		
  ne = (int)(de + .5);
  nf = df;
}
