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

////////////////////////////////////////////////////////////////////////
// This code is a modified version of the Delaunay triangulator
// written by Dani Lischinski
// in Graphics Gems IV
////////////////////////////////////////////////////////////////////////


// in a triangulation, the topology looks like this:
//
//                    Dest
//                     @
//                   * * *
//         Lnext | *   *   * | Rprev
//               *--   *   --*
//             *       *       *
//   Dprev --*        /*\        *-- Dnext
//         * |     Sym *         | *
//       *             *             *
//     @               *               @
//       *             *             *
//         * |         * e       | *
//   Lprev --*        \*/        *-- Rnext
//             *       *       *
//               *--   *   --*
//         Onext | *   *   * | Oprev
//                   * * *
//                     @
//                    Org

#ifndef QUADEDGE_H
#define QUADEDGE_H

#include "geom2d.H"
#include "../libmemory.hpp"

extern class MemQueue<4> QPool;
extern class MemQueue<4> TPool;
extern class MemQueue<4> EPool;

class QuadEdge;
class Triangle;
class Edge;

typedef void (*edge_callback)(Edge *,void *);
typedef void (*face_callback)(Triangle *,void *);

// value for Triangle->heap_index indicating no candidate in heap
#define NOT_IN_HEAP	-47
// value for Triangle->err indicating triangle hasn't been scan converted
#define UNSCANNED	-1

class Edge : public MemQueueable {
    friend class QuadEdge;
    friend void Splice(Edge*, Edge*);
private:
    int num;
    Edge *next;
    Point2d *data;
    Triangle *lface_data;
public:
    Edge()			{ data = 0; lface_data = NULL; }
    Edge* Rot();
    Edge* invRot();
    Edge* Sym();
    Edge* Onext();
    Edge* Oprev();
    Edge* Dnext();
    Edge* Dprev();
    Edge* Lnext();
    Edge* Lprev();
    Edge* Rnext();
    Edge* Rprev();
    Point2d* Org();
    Point2d* Dest();
    const Point2d& Org2d() const;
    const Point2d& Dest2d() const;
    void EndPoints(Point2d*, Point2d*);
    QuadEdge* Qedge()           { return (QuadEdge *)(this - num); }

    // returns 1 if a counterclockwise perimeter edge,
    // 0 if a clockwise perimeter edge or internal edge
    bool isCCWPerim();

    Triangle *Lface() { return lface_data; }
    void set_Lface(Triangle *f) { lface_data = f; }
    void OverEdges(unsigned int,edge_callback,void *closure);

    friend ostream& operator<<(ostream&,Edge *);
};

class QuadEdge : public MemQueueable {
	friend Edge *MakeEdge();
  private:
	Edge e[4];
	unsigned int ts;
  public:
	QuadEdge();
	int TimeStamp(unsigned int);
};

class Subdivision {
private:
    Edge *startingEdge;
    Triangle *firstFace;

    Triangle *makeFace(Edge *);
    void rebuildFace(Edge *);

protected:
    void init(const Point2d&,const Point2d&,const Point2d&,const Point2d&);
    void free();

    Subdivision() { }
    ~Subdivision() { free(); }

public:
    Subdivision(const Point2d& a, const Point2d& b,
		const Point2d& c, const Point2d& d) { init(a,b,c,d); }

    Edge *Locate(const Point2d& x, Edge *hintedge);
    Edge *Spoke(const Point2d& x, Triangle *tri);
    Edge *InsertSite(const Point2d&, Triangle *tri);

    bool isInterior(Edge *);

    void OverEdges(edge_callback,void *closure);
    void OverFaces(face_callback,void *closure);
    void vef(int &nv, int &ne, int &nf);
};

inline QuadEdge::QuadEdge() {
    e[0].num = 0, e[1].num = 1, e[2].num = 2, e[3].num = 3;
    e[0].next = &(e[0]); e[1].next = &(e[3]);
    e[2].next = &(e[2]); e[3].next = &(e[1]);

    assert(!e[0].lface_data);
    assert(!e[1].lface_data);
    assert(!e[2].lface_data);
    assert(!e[3].lface_data);

    ts = 0;
}

inline int QuadEdge::TimeStamp(unsigned int stamp) {
    if (ts != stamp) {
      ts = stamp;
      return TRUE;
    }
    else
      return FALSE;
}

/************************* Edge Algebra *************************************/

// Return the dual of the current edge, directed from its right to its left.
inline Edge* Edge::Rot() {
  return (num < 3) ? this + 1 : this - 3;
}

// Return the dual of the current edge, directed from its left to its right.
inline Edge* Edge::invRot() {
  return (num > 0) ? this - 1 : this + 3;
}

// Return the edge from the destination to the origin of the current edge.
inline Edge* Edge::Sym() {
  return (num < 2) ? this + 2 : this - 2;
}

// Return the next ccw edge around (from) the origin of the current edge.
inline Edge* Edge::Onext() {
  return next;
}

// Return the next cw edge around (from) the origin of the current edge.
inline Edge* Edge::Oprev() {
  return Rot()->Onext()->Rot();
}

// Return the next ccw edge around (into) the destination of the current edge.
inline Edge* Edge::Dnext() {
  return Sym()->Onext()->Sym();
}

// Return the next cw edge around (into) the destination of the current edge.
inline Edge* Edge::Dprev() {
  return invRot()->Onext()->invRot();
}

// Return the ccw edge around the left face following the current edge.
inline Edge* Edge::Lnext() {
  return invRot()->Onext()->Rot();
}

// Return the ccw edge around the left face before the current edge.
inline Edge* Edge::Lprev() {
  return Onext()->Sym();
}

// Return the edge around the right face ccw following the current edge.
inline Edge* Edge::Rnext() {
  return Rot()->Onext()->invRot();
}

// Return the edge around the right face ccw before the current edge.
inline Edge* Edge::Rprev() {
  return Sym()->Onext();
}

/************** Access to data pointers *************************************/

inline Point2d* Edge::Org() {
  return data;
}

inline Point2d* Edge::Dest() {
  return Sym()->data;
}

inline const Point2d& Edge::Org2d() const {
  return *data;
}

inline const Point2d& Edge::Dest2d() const {
  return (num < 2) ? *((this + 2)->data) : *((this - 2)->data);
}

inline void Edge::EndPoints(Point2d* _or, Point2d* _de) {
  data = _or;
  Sym()->data = _de;
}


inline ostream& operator<<(ostream& os, Edge *e) {
  os << "[" << e->Org2d() << "-->" << e->Dest2d() << "]";
  return os;
}



class Triangle : public MemQueueable {
    Edge *anchor;	// An edge of this triangle
    int sx, sy;		// The proposed selection point
    int heap_index;	// index into heap
	// == NOT_IN_HEAP if this triangle has no candidate in the heap
    Real err;		// error over triangle,
			// used for data-dependent triangulation
			// = sum of squared error if criterion=SUM2,
			// = maximum error if criterion=SUMINF or MAXINF
public:
    Triangle *next;

    Triangle(Edge *e);
    int locate() { return heap_index; }
    void setLocation(int h) { heap_index = h; }
    void setSelection(int  x, int  y) { sx =  x; sy =  y; }
    void getSelection(int *x, int *y) { *x = sx; *y = sy; }
    void setError(Real error) { err = error; }
    Real getError() { return err; }

    const Point2d& point1() const { return anchor->Org2d(); }
    const Point2d& point2() const { return anchor->Dest2d(); }
    const Point2d& point3() const { return anchor->Lnext()->Dest2d(); }

    Edge *getAnchor() { return anchor; }
    void setAnchor(Edge *e) { anchor = e; }
    void dontAnchor(Edge *e);

    void attachFace();

    friend ostream& operator<<(ostream&,Triangle *);
};

inline Triangle::Triangle(Edge *e) {
  heap_index = NOT_IN_HEAP;
  anchor = e;
  err = UNSCANNED;
  sx = sy = 0;

  attachFace();
}

inline Triangle *Subdivision::makeFace(Edge *e) {
  Triangle *f = new(&TPool) Triangle(e);

  f->next = firstFace;
  firstFace = f;

  return f;
}

inline ostream& operator << (ostream& os, Triangle *t) {
  os << "Triangle" << t->point1() << t->point2() << t->point3();

  return os;
}


class UpdateRegion {
  Edge *start;
  Edge *current;

public:
  UpdateRegion(Edge *);

  Triangle *first();
  Triangle *next();
};

void Swap(Edge* e);

// Returns twice the area of the oriented triangle (a, b, c), i.e., the
// area is positive if the triangle is oriented counterclockwise.
inline Real getArea2x(const Point2d& a, const Point2d& b, const Point2d& c) {
  return (b.x - a.x) * (c.y - a.y) -
	 (b.y - a.y) * (c.x - a.x);
}

#endif /* QUADEDGE_H */
