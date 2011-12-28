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

#ifndef SIMPLFIELD_H_INCLUDED
#define SIMPLFIELD_H_INCLUDED

#include "quadedge.H"
#include "stuff.H"
#include "hfield.H"

//
// simplfield.H
//
// Defines the various classes/structures used in building and
// maintaining approximations to height fields.


struct heap_node {
    Real val;
    Triangle *tri;
};

class Heap;
class SimplField;

struct FitPlane {	// a set of planes for fitting a surface
			// a temp. data struc for data-dep. triangulation
    Plane z, r,g,b, l;	// plane equations for z, r, g, b as functions of (x,y)
    Real area;		// area of the triangle
    Real quality;	// area/diameter of triangle: large is good, 0 is bad
    int cx, cy;		// candidate point
    Real cerr;		// absolute error of candidate
    Real err;		// error over triangle,
			// used for data-dependent triangulation
			// = sum of squared error if criterion=SUM2,
			// = maximum error if criterion=SUMINF or MAXINF
    int done;		// is all of the above information final? (1=yes, 0=no)

    FitPlane() {done = 0;};
    // set all FitPlane info by copying from Triangle
    FitPlane(SimplField &ter, Triangle *tri);
    // initialize planes in FitPlane to pass through p,q,r and
    // initialize error sum and candidate for subsequent accumulation
    FitPlane(HField *H, const Point2d &p1, const Point2d &p2, const Point2d &p3) { init(H, p1, p2, p3); };
    void init(HField *H, const Point2d &p1, const Point2d &p2, const Point2d &p3);

    friend ostream& operator<<(ostream &, const FitPlane &);
};

class SimplField : public Subdivision  {

    HField *H;          // The height field being approximated
    Heap *heap;         // Heap of candidate points

    void init(HField *);
    void free();
    void init_cache();
    void select(Triangle *tri, int x, int y, Real cerr);
    void selectDatadep(Triangle *tri, FitPlane &fit);
    void update_cache(Edge *e);
    Real compute_choice(int x,int y);
    Real compute_choice_interp(Real x,Real y);
    void checkSwap(Edge *e, FitPlane &abd);
    Edge *InsertSite(const Point2d& x, Triangle *tri);

    void scanTriangleDataindep(Triangle *tri);
    void scanTriangleDatadep(const Point2d &p, const Point2d &q, const Point2d &r, FitPlane *u, FitPlane *v);

    Real angle_between_all_normals(const FitPlane&, const FitPlane&);

public:
#ifdef BITFIELD2
    bitarray2<> is_used;

    bool check_used(int i, int j) const { return is_used.check(i, j); }
    void  mark_used(int i, int j)	{        is_used.mark (i, j); }
    void clear_used(int i, int j)	{        is_used.clear(i, j); }
    bool  skip_used(int i, int j)       { return is_used.skip (i, j); }
#else
    array2<char> is_used;

    bool check_used(int i, int j) const { if (is_used(i, j)) return true;                    return false; }
    void  mark_used(int i, int j)	{                                 is_used(i, j) = 1;               }
    void clear_used(int i, int j)	{                                 is_used(i, j) = 0;               }
    bool  skip_used(int i, int j)	{ if (is_used(i, j)) return true; is_used(i, j) = 1; return false; }
#endif

    SimplField(HField *h) { init(h); }
    ~SimplField() { free(); }

    Edge *select_new_point();
    Edge *select_new_point(int x, int y);
    Edge *select_fix_point(int x, int y);
    int select_new_points(Real limit);

    // for bilinear interpolation
    int isUsedInterp(Real x, Real y) const;	

    Real rms_error();
    Real rms_error_supersample(int ss);
    Real rms_error_estimate();
    Real getCurrentError();
    HField *getHField() { return H; }
    Heap &getHeap() { return *heap; }
};



class Heap {
    heap_node *node;
    int extent;

    void swap(int i,int j);

    int parent(int i) { return (i - 1) / 2; }
    int left(int i) { return 2 * i + 1; }
    int right(int i) { return 2 * i + 2; }

    void upheap(int i);
    void downheap(int i);

public:
    Heap(int s) { extent = 0; node = new heap_node[s]; if (!node)
      throw runtime_error("Failed to allocate memory"); }
    ~Heap() { delete[] node; }

    heap_node& operator[](int i) { return node[i]; }
    int size() { return extent; }

    void insert(Triangle *t, Real v);
    heap_node *extract();
    heap_node *top() { return extent < 1 ? 0 : &node[0]; };
    heap_node *get(int i) { return &node[i]; };
    heap_node& kill(int i);
    void update(int, Real);
};

#endif
