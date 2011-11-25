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

// simplfield.C: routines for simplifying a height field
// High level triangulation, fitting, and display are in this file.
// Low level triangulation is in quadedge.C .
// Scan conversion is in scan.C .

// Michael Garland and Paul Heckbert, 1994

#include "scape.H"

int ndecision = 0;	// count of #swap decisions, total
int nshape = 0;		// count of #swap decisions determined by shape
int nchanged = 0;	// count of #swap decisions changed by shape
int scancount;		// count of #pixels scanned during an update

FitPlane::FitPlane(SimplField &ter, Triangle *tri) {
  // initialize plane equations for z, r, g, b in tri
  init(
    ter.original(), 
    tri->point1(), 
    tri->point2(), 
    tri->point3()
  );

  // copy error of this triangle to FitPlane
  err = tri->get_err();
  // new triangle; need to compute err
  if (err == UNSCANNED)
    err = 0;	
  // old tri; get its err & candidate xy
  else {				
    tri->get_selection(&cx, &cy);

    // candidate's heap index
    int index = tri->locate();	
    cerr = index == NOT_IN_HEAP ? 0 : ter.get_heap()[index].val;
    done = 1;
  }
}

void FitPlane::init(HField *H, const Point2d &p1, const Point2d &p2, const Point2d &p3) {
  cerr = 0;
  err = 0;
  done = 0;

  Vector3d
    v1(p1, H->eval(p1)), 
    v2(p2, H->eval(p2)), 
    v3(p3, H->eval(p3));

  Real diam;
  triarea(v1, v2, v3, area, diam);
  quality = area / diam;
  assert(quality >= 0);//??
  if (area == 0)
    return;

  // note: there's redundant computation in these four calls to init,
  // could be optimized??
  z.init(v1, v2, v3);

  if (emphasis != 0) {
    /**/ if (H->has_luma()) {
      Real l1, l2, l3;

      H->luma(p1, l1);
      H->luma(p2, l2);
      H->luma(p3, l3);

      v1.z = l1; v2.z = l2; v3.z = l3; l.init(v1, v2, v3);
    }

    else if (H->has_color()) {
      Real r1, g1, b1, r2, g2, b2, r3, g3, b3;

      H->color(p1, r1, g1, b1);
      H->color(p2, r2, g2, b2);
      H->color(p3, r3, g3, b3);

      v1.z = r1; v2.z = r2; v3.z = r3; r.init(v1, v2, v3);
      v1.z = g1; v2.z = g2; v3.z = g3; g.init(v1, v2, v3);
      v1.z = b1; v2.z = b2; v3.z = b3; b.init(v1, v2, v3);
    }
  }
}

ostream& operator<<(ostream &os, const FitPlane &fit)
{
    if (fit.cerr>0)
	os << "(" << fit.cx << "," << fit.cy << ") ";
    else
	os << "{" << fit.cx << "," << fit.cy << "} ";

    os << "cerr=" << fit.cerr << " err=" << fit.err
	<< " area=" << fit.area << " qual=" << fit.quality
	<< (fit.done ? " - DONE" : "") << endl;

    return os;
}


void SimplField::free() {
  delete heap;

  Subdivision::free();
}


void SimplField::init(HField *Hf) {
  int x, y, w, h;

  H = Hf;
  w = Hf->get_width();
  h = Hf->get_height();

  model_center = H->center();
  bound_volume = H->bounds();
  render_with_color = 0;
  render_with_mesh = 0;
  render_with_texture = 0;
  render_as_surface = 0;
  render_with_dem = 0;
  dem_step = w / 50;

  is_used.init(w, h);

  // mark points with invalid data as "used", but mark others "unused"
  int count = 0;
  for (x = 0; x < w; x++)
  for (y = 0; y < h; y++) {
    if (H->eval(x,y) == DEM_BAD) {
      count++;

      is_used(x,y) = 1;
    }
    else
      is_used(x,y) = 0;
  }

  if (count)
    logrf("%d input points ignored\n", count);

  heap = new Heap(w * h);

  // Select the corner points into the initial mesh
  Point2d a(    0,     0),
          b(    0, h - 1),
	  c(w - 1, h - 1),
	  d(w - 1,     0);

  Subdivision::init(a, b, c, d);

  is_used(    0,     0) = 1;
  is_used(    0, h - 1) = 1;
  is_used(w - 1, h - 1) = 1;
  is_used(w - 1,     0) = 1;

  init_cache();
}

int SimplField::is_used_interp(Real x, Real y) {
// for bilinear interpolation
    int ix = (int)x, intx = x==ix;
    int iy = (int)y, inty = y==iy;
    // be conservative -- if any of the neighbors are bad, then I'm bad
    if (intx && inty) return is_used(ix, iy);
    if (intx) return is_used(ix, iy) || is_used(ix, iy+1);
    if (inty) return is_used(ix, iy) || is_used(ix+1, iy);
    return is_used(ix, iy)   || is_used(ix+1, iy)
	|| is_used(ix, iy+1) || is_used(ix+1, iy+1);
}

void check_for_diagonal(Triangle *tri, void *closure) {
    Edge *e = tri->get_anchor();
    if (!e) return;
    do {
	if (!e->CcwPerim() && !e->Sym()->CcwPerim())
	    *(Edge **)closure = e;	// found internal edge, save ptr to it
	e = e->Lnext();
    } while (e!=tri->get_anchor());
}

Edge *find_diagonal(Subdivision &sub) {
  Edge *diag = 0;
  sub.OverFaces(check_for_diagonal, &diag);
  assert(diag);
  return diag;
}

void SimplField::init_cache() {
  // Add choices from the first two triangles to the heap
  Edge *diag = find_diagonal(*this);
  if (datadep) {
    FitPlane fit;

    check_swap(diag, fit);
  }
  else {
    scan_triangle_dataindep(diag->Lface());
    scan_triangle_dataindep(diag->Sym()->Lface());
  }
}

void SimplField::select(Triangle *tri, int x, int y, Real cerr) {
  if ((debug > 1) && !datadep)
    cout << "  select(" << x << "," << y << ") cerr=" << cerr << endl;

  // triangle has valid candidate
  if (cerr > 1e-4) {
    assert(!is_used(x, y));
    tri->set_selection(x, y);

    // new triangle, so insert
    if (tri->locate() == NOT_IN_HEAP)
      heap->insert(tri, cerr);
    // recycled triangle, so update
    else
      heap->update(tri->locate(), cerr);
  }
  // triangle has no candidate
  else {
    // recycled, kill heap entry
    if (tri->locate() != NOT_IN_HEAP) {
      heap->kill(tri->locate());

      tri->set_location(NOT_IN_HEAP);
    }
  }
}

void SimplField::select_datadep(Triangle *tri, FitPlane &fit) {
  if (debug > 1)
    cout << "  select_datadep " << tri << "\n    candidate=" << fit;

  select(tri, fit.cx, fit.cy, fit.cerr);
  tri->set_err(fit.err);
}

void SimplField::update_cache(Edge *e)
{
  UpdateRegion region(e);
  Triangle *t = region.first();

  scancount = 0;
  do {
    scan_triangle_dataindep(t);
  } while((t = region.next()));

  if (debug)
    cout << "  " << scancount << " pixels" << endl;
}

// returns pointer to an outward-pointing spoke
Edge *SimplField::select_new_point() {
  Real error = max_error();
  logrf("%f\r", error);
  if (error < termination)
    return NULL;

  heap_node *n = heap->extract();
  if (!n) {
    logrf("No more candidates\r");
    return NULL;
  }

  // mark point as selected
  int sx, sy;
  n->tri->get_selection(&sx, &sy);
  assert(!is_used(sx, sy));
  is_used(sx, sy) = TRUE;
  Point2d p(sx, sy);

  if (debug)
    cout << endl << "SELECTING: " << p << "  " << n->val << endl;

  Edge *spoke;
  // data-dependent triangulation
  if (datadep)
    spoke = SimplField::InsertSite(p, n->tri);
  // Delaunay triangulation
  else {
    spoke = Subdivision::InsertSite(p, n->tri);

    // pass update_cache an edge whose origin is the point inserted
    update_cache(spoke);
  }

  return spoke;
}

// returns pointer to an outward-pointing spoke
Edge *SimplField::select_new_point(int x, int y) {
  Real error = max_error();
  logrf("%f\r", error);
  if (error < termination)
    return NULL;

  if (is_used(x, y))
    return NULL;

  /* TODO: it may be possible that we don't need to search for the triangles
    *       InsertSites takes NULL as well
    */
  Point2d P(x, y);
  heap_node *n = NULL;
  Heap *lheap = heap;
  int lhsize = lheap->heap_size();

 #pragma omp parallel for
  for (int h = 0; h < lhsize; h++) {
    /* quitter for openmp */
    if (n) continue;

    heap_node *t = lheap->get(h);
    Triangle *tr = t->tri;

    /* barycentric test */
    // Compute vectors
    Point2d v0 = tr->point2() - tr->point1();
    Point2d v1 = tr->point3() - tr->point1();
    Point2d v2 =        P     - tr->point1();

    // Compute dot products
    Real dot00 = dot(v0, v0);
    Real dot01 = dot(v0, v1);
    Real dot02 = dot(v0, v2);
    Real dot11 = dot(v1, v1);
    Real dot12 = dot(v1, v2);

    // Compute barycentric coordinates
    Real invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
    Real u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    Real v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    // Check if point is in triangle (or on edge)
    if ((u >= 0.0f) && (v >= 0.0f) && ((u + v) <= 1.0f)) {
      n = t; lheap->kill(h); /*break;*/
    }
  }

  if (!n) {
//  cout << "# no more candidates for " << x << "," << y << endl;
    return NULL;
  }

  int sx, sy;
//n->tri->get_selection(&sx, &sy);
  sx = x; sy = y;			// manual point

  is_used(sx, sy) = TRUE;		// mark point as selected
  Point2d p(sx, sy);
  if (debug)
    cout << endl << "SELECTING: " << p << "  " << n->val << endl;

  Edge *spoke;
  // data-dependent triangulation
  if (datadep)
    spoke = SimplField::InsertSite(p, n->tri);
  // Delaunay triangulation
  else {
    spoke = Subdivision::InsertSite(p, n->tri);

    // pass update_cache an edge whose origin is the point inserted
    update_cache(spoke);
  }

  return spoke;
}

// returns pointer to an outward-pointing spoke
Edge *SimplField::select_fix_point(int x, int y) {
  Real error = max_error();
  logrf("%f\r", error);
  if (error < termination)
    return NULL;

  if (is_used(x, y))
    return NULL;

  int sx, sy;
//n->tri->get_selection(&sx, &sy);
  sx = x; sy = y;			// manual point

  is_used(sx, sy) = TRUE;		// mark point as selected
  Point2d p(sx, sy);
  if (debug)
    cout << endl << "SELECTING: " << p << "  " << endl;

  Edge *spoke;
  // data-dependent triangulation
  if (datadep)
    spoke = SimplField::InsertSite(p, NULL);
  // Delaunay triangulation
  else {
    spoke = Subdivision::InsertSite(p, NULL);

    // pass update_cache an edge whose origin is the point inserted
    update_cache(spoke);
  }

  return spoke;
}

int SimplField::select_new_points(Real limit) {
  buffer<int> xs;
  buffer<int> ys;
  int i, taken = 0;
  Real error = max_error();

  while ((error >= limit) && (heap->heap_size() > 0)) {
    int x, y;
    heap_node *n = heap->extract();

    n->tri->get_selection(&x, &y);

    if (!is_used(x, y)) {
      taken++;
      is_used(x, y) = TRUE;

      xs.insert(x);
      ys.insert(y);
    }

    error = max_error();
  }

  if (!taken)
    return 0;

  for (i = 0; i < taken; i++) {
    int x = xs(i);
    int y = ys(i);
    Point2d p(x, y);

    Edge *e;
    // data dependent triangulation
    if (datadep)
      e = SimplField::InsertSite(p, NULL);
    else {
      e = Subdivision::InsertSite(p, NULL);

      update_cache(e);
    }
  }

  return taken;
}

// Determine if the diagonals (the line segments a-c and b-d) of the
// quadrilateral with ccw vertices a,b,c,d intersect, and if so,
// put the intersection point in "isect".
// Return 1 if abcd is convex, 0 if concave (if the intersection lies outside).
// We already know that points a and c lie on opposite sides of line bd.
int quadrilateral_diagonal_intersect
    (const Point2d &a, const Point2d &b, const Point2d &c, const Point2d &d,
    Point2d &isect) {

  // do points b and d lie on opposite sides of line ac?
  Line p(a, c);
  if (p.eval(b) < 0 || 
      p.eval(d) > 0) {
    if (debug > 1)
      cout << "    concave quadrilateral" << endl;

    return 0;
  }

  intersect(p, Line(b, d), isect);
  if (debug > 1)
    cout << "    convex quad, isect=" << isect << endl;

  return 1;
}

// ABN = angle between normals criterion for data-dependent triangulation
// see Dyn, IMA J Numer Anal 10, p137
Real angle_between_normals(const Plane &p, const Plane &q)
{
    return acos(
		(p.a*q.a + p.b*q.b + 1) /
		sqrt((p.a*p.a + p.b*p.b + 1)*(q.a*q.a + q.b*q.b + 1))
		);
}

Real SimplField::angle_between_all_normals(const FitPlane &tri1,
					   const FitPlane &tri2) {
  Real a = angle_between_normals(tri1.z, tri2.z);

  /**/ if (emphasis == 0)
    return a;
  else if (H->has_luma())
    return (1 - emphasis) * a +
    emphasis * (H->zmax() / 1) *
    (angle_between_normals(tri1.l, tri2.l));
  else if (H->has_color())
    return (1 - emphasis) * a +
    emphasis * (H->zmax() / 3) *
    (angle_between_normals(tri1.r, tri2.r) +
     angle_between_normals(tri1.g, tri2.g) +
     angle_between_normals(tri1.b, tri2.b));

  return a;
}

// Swap edge e if that yields a triangulation with lower total squared error,
// and update triangles accordingly.
// Error info for the triangle to the left of edge e is passed in
// in the structure abd, if available.
// Iff this info is uninitialized, then abd.done==0
//
// In diagram below, edge e goes from b to d, abcd is quadrilateral around it.
// Current edge is bd, we're checking to see if we should swap to ac.
//
//         d
//         o
//        /|\
//       / | \
//      /  |  \
//     /   |   \
//  a o -- o -- o c
//     \   |p  /
//      \  |  /
//       \ | /
//        \|/
//       b o
void SimplField::check_swap(Edge *e, FitPlane &abd) {
  if (debug > 1)
    cout << endl << "check_swap" << e << endl;

  // tricheck(e);
  const Point2d &a = e->Onext()->Dest2d();
  const Point2d &b = e->Org2d();
  const Point2d &c = e->Oprev()->Dest2d();
  const Point2d &d = e->Dest2d();

  if (debug > 1)
    cout << "  a=" << a << " b=" << b << " c=" << c << " d=" << d << endl;

  if (!abd.done)
    abd.init(H, a, b, d);

  Point2d p;				// intersection of diagonals ac and bd
  if (e->CcwPerim() ||
    (
    /* tricheck(e->Sym()), */
    !quadrilateral_diagonal_intersect(a, b, c, d, p)
    )
    ) {
      // either e is on perimeter or quadrilateral is concave
      // in either case we can't swap diagonals
      // but we still need to set selection
      if (debug > 1)
	cout << (e->CcwPerim() ? "  on perimeter" : "  concave")
	<< ", abd: " << abd;

      if (!abd.done) {
	scan_triangle_datadep(a, b, d, /*null fitplane*/ 0, &abd);

	if (debug > 1)
	  cout << "  abd; " << abd;
      }

      select_datadep(e->Lface(), abd);

      if (debug>1)
	cout << "end1 check_swap" << endl;

      return;
  }

  FitPlane cdb(*this, e->Sym()->Lface()), dac(H, d, a, c), bca(H, b, c, a);
  if (debug > 1) {
    if (abd.area == 0 || cdb.area == 0 || dac.area == 0 || bca.area == 0)
      cout << "---- abd.area=" << abd.area <<
      " cdb.area=" << cdb.area <<
      " dac.area=" << dac.area <<
      " bca.area=" << bca.area << endl;

    cout << "  abd: " << abd;
    cout << "  cdb: " << cdb;
    cout << "  dac: " << dac;
    cout << "  bca: " << bca;
  }

  // scan convert the four sub-triangles of quadrilateral abcd,
  // collecting info about fit errors and candidates in abd, cdb, dac, bca
  scan_triangle_datadep(p, d, a, &abd, &dac);
  scan_triangle_datadep(p, a, b, &abd, &bca);
  scan_triangle_datadep(p, b, c, &cdb, &bca);
  scan_triangle_datadep(p, c, d, &cdb, &dac);
  if (debug > 1) {
    cout << "  abd; " << abd;
    cout << "  cdb; " << cdb;
    cout << "  dac; " << dac;
    cout << "  bca; " << bca;
  }

  // now all four FitPlanes are done (even though their "done" bits may not
  // say so); see which diagonal of quadrilateral is best
  Real err_bd, err_ac;
  switch (criterion) {
    case SUMINF:	// in this case we're summing maximum errors
    case SUM2:	// in this case we're summing sums of squared errors
      err_bd = abd.err + cdb.err;
      err_ac = dac.err + bca.err;
      break;
    case MAXINF:
      err_bd = MAX(abd.err, cdb.err);
      err_ac = MAX(dac.err, bca.err);
      break;
    case ABN:
      err_bd = angle_between_all_normals(abd, cdb);
      err_ac = angle_between_all_normals(dac, bca);
      break;
  }

  // check the quality of the two triangulations
  Real qual_bd = abd.quality * cdb.quality;
  Real qual_ac = dac.quality * bca.quality;
  Real qual_ratio = fabs(qual_bd) < fabs(qual_ac) ?
    qual_bd / qual_ac : qual_ac / qual_bd;

  assert(qual_ratio >= 0 && qual_ratio <= 1);//??

  if (debug > 1)
    cout << "  ebd=" << err_bd << " eac=" << err_ac
    //<< " BD-AC=" << err_bd-err_ac
    << " qbd=" << qual_bd << " qac=" << qual_ac
    << " rat=" << qual_ratio << endl;

  if ((err_bd <= err_ac || bca.area == 0 || dac.area == 0) !=
    (qual_ratio > qual_thresh
    ? err_bd  <= err_ac		// pick diagonal with lowest error
    : qual_bd >= qual_ac	// pick diagonal with best shaped triangles
    )) {
    nchanged++;
    if (debug > 1)
      cout << "  DECISION CHANGED BY QUALITY MEASURE\n";//??
  }

  if (qual_ratio <= qual_thresh)
    nshape++;
  ndecision++;

  if (qual_ratio > qual_thresh
    ? err_bd  <= err_ac		// pick diagonal with lowest error
    : qual_bd >= qual_ac	// pick diagonal with best shaped triangles
    ) {
    // current diagonal (bd) is best, either because it has lower
    // error, or because triangulating the other way would lead to
    // badly shaped triangles
    if (debug > 1)
      cout << "  not swapping " << e << endl;

    // set candidate for triangle abd
    select_datadep(e->Lface(), abd);
    if (!cdb.done)			// first call to check_swap
      select_datadep(e->Sym()->Lface(), cdb);
  }
  else {				// other diagonal (ac) is best
    if (debug > 1)
      cout << "  SWAPPING " << e << endl;

    // swap diagonals
    Swap(e);

    dac.done = 1;
    bca.done = 1;

    // recurse on the new triangles
    check_swap(e->Oprev(), dac);
    check_swap(e->Lprev(), bca);
  }

  if (debug > 1)
    cout << "end2 check_swap" << endl;
}

// Inserts a new point x into triangle tri of a data-dependent triangulation,
// applying the local optimization procedure to find the best local
// triangulation, updating the Triangle structures and heap to contain
// the best candidate for each triangle.
// Returns a pointer to an outward-pointing spoke.
//
// a variant of Subdivision::InsertSite
// --- Tri can be NULL
Edge *SimplField::InsertSite(const Point2d& x, Triangle *tri) {
  // Examine suspect quadrilaterals, swapping diagonals if necessary
  Edge *startspoke = Spoke(x, tri), *e = startspoke, *diag;
  FitPlane fit;

  scancount = 0;
  do {
    diag = e->Lprev();
    e = e->Dprev();		// advance to next spoke
    if (!e->CcwPerim())
      check_swap(diag, fit);

    // check quadrilateral with diagonal "diag"
    // and swap if that yields lower error
    // note: it's essential that we advance e before calling check_swap,
    // since the latter might change the topology of the spoke vertex
  } while (e != startspoke);

  if (debug)
    cout << scancount << " pixels scanned total" << endl;

  return e->Sym();
}


Real SimplField::rms_error() {
  int x,y;
  Real diff, err = 0;
  int width = H->get_width(),
     height = H->get_height();

  for (x = 0; x < width ; x++)
  for (y = 0; y < height; y++) {
    diff = compute_choice(x, y);
    err += diff * diff;
  }

  return sqrt(err / (width * height));
}

// compute RMS error using supersampling (more accurate, but slower)
Real SimplField::rms_error_supersample(int ss) {
  int x, y;
  Real diff, err = 0;
  int width = H->get_width(),
     height = H->get_height();

  for (x = 0; x <= (width  - 1) * ss; x++)
  for (y = 0; y <= (height - 1) * ss; y++) {
    diff = compute_choice_interp((Real)x / ss, (Real)y / ss);
    err += diff * diff;
  }

  return sqrt(err / (width * height * ss * ss));
}

static Real sqerr;

static void sum_err(Triangle *tri, void *) {
  sqerr += tri->get_err();
}

Real SimplField::rms_error_estimate()
// approximate RMS error; probably a bit off because points on triangulation
// edges might be counted 0, 1, or 2 times depending on roundoff error and
// discretization details of scan converter
{
  // we're not keeping track of squared error in these cases
  if (!datadep || criterion != SUM2)
    return -1;

  sqerr = 0;
  OverFaces(sum_err, 0);

  return sqrt(sqerr / (H->get_width() * H->get_height()));
}

Real SimplField::max_error() {
  return heap->top() ? heap->top()->val : 0.0f;
}

Real SimplField::compute_choice(int x, int y) {
  static Edge *cur_edge = NULL;	// edge of triangle whose planes we've saved
  static Plane z_plane, r_plane, g_plane, b_plane, l_plane;  // saved plane equations

  Point2d ref(x, y);
  Edge *e = Locate(ref, 0);

  if (e != cur_edge) {
    // if current point not contained in the same triangle as previous
    // point, then compute (and save) plane equations of the new triangle
    Triangle *tri = e->Lface();
    assert(tri);

//  if (!tri) tri = e->Sym()->Lface();	// needed for perimeter points
    const Point2d& p1 = tri->point1();
    const Point2d& p2 = tri->point2();
    const Point2d& p3 = tri->point3();

    Vector3d 
      v1(p1, H->eval(p1)),
      v2(p2, H->eval(p2)),
      v3(p3, H->eval(p3));

    z_plane.init(v1, v2, v3);

    if (emphasis != 0) {
      /**/ if (H->has_luma()) {
	Real l1, l2, l3;

	H->luma(p1, l1);
	H->luma(p2, l2);
	H->luma(p3, l3);

	v1.z = l1; v2.z = l2; v3.z = l3; l_plane.init(v1, v2, v3);
      }

      else if (H->has_color()) {
	Real r1, g1, b1, r2, g2, b2, r3, g3, b3;

	H->color(p1, r1, g1, b1);
	H->color(p2, r2, g2, b2);
	H->color(p3, r3, g3, b3);

	v1.z = r1; v2.z = r2; v3.z = r3; r_plane.init(v1, v2, v3);
	v1.z = g1; v2.z = g2; v3.z = g3; g_plane.init(v1, v2, v3);
	v1.z = b1; v2.z = b2; v3.z = b3; b_plane.init(v1, v2, v3);
      }
    }
  }

  // evaluate the plane equations
  Real diff = fabs(z_plane(x, y) - H->eval(x, y));

  if (emphasis != 0) {
    /**/ if (H->has_luma()) {
      Real l0;

      H->luma(x ,y, l0);

      diff = (1 - emphasis) * diff +
	emphasis * (H->zmax() / 1) * (
	fabs(l_plane(x, y) - l0));
    }

    else if (H->has_color()) {
      Real r0, g0, b0;

      H->color(x ,y, r0, g0, b0);

      diff = (1 - emphasis) * diff +
	emphasis * (H->zmax() / 3) * (
	fabs(r_plane(x, y) - r0) +
	fabs(g_plane(x, y) - g0) +
	fabs(b_plane(x, y) - b0));
    }
  }

  return diff;
}

// just like compute_choice except it takes real arguments
// (used by rms_error_supersample)
Real SimplField::compute_choice_interp(Real x, Real y) {
  // edge of triangle whose planes we've saved
  static Edge *cur_edge = NULL;	
  // saved plane equations
  static Plane z_plane, r_plane, g_plane, b_plane, l_plane;  

  Point2d ref(x,y);
  Edge *e = Locate(ref, 0);

  if (e != cur_edge) {
    // if current point not contained in the same triangle as previous
    // point, then compute (and save) plane equations of the new triangle
    Triangle *tri = e->Lface();
    assert(tri);

    //if (!tri) tri = e->Sym()->Lface();	// needed for perimeter points
    const Point2d& p1 = tri->point1();
    const Point2d& p2 = tri->point2();
    const Point2d& p3 = tri->point3();

    Vector3d 
      v1(p1, H->eval(p1)),
      v2(p2, H->eval(p2)),
      v3(p3, H->eval(p3));

    z_plane.init(v1, v2, v3);

    if (emphasis != 0) {
      /**/ if (H->has_luma()) {
	Real l1, l2, l3;

	H->luma(p1, l1);
	H->luma(p2, l2);
	H->luma(p3, l3);

	v1.z = l1; v2.z = l2; v3.z = l3; l_plane.init(v1, v2, v3);
      }

      else if (H->has_color()) {
	Real r1, g1, b1, r2, g2, b2, r3, g3, b3;

	H->color(p1, r1, g1, b1);
	H->color(p2, r2, g2, b2);
	H->color(p3, r3, g3, b3);

	v1.z = r1; v2.z = r2; v3.z = r3; r_plane.init(v1, v2, v3);
	v1.z = g1; v2.z = g2; v3.z = g3; g_plane.init(v1, v2, v3);
	v1.z = b1; v2.z = b2; v3.z = b3; b_plane.init(v1, v2, v3);
      }
    }
  }

  // evaluate the plane equations
  Real diff = fabs(z_plane(x,y) - H->eval_interp(x,y));

  if (emphasis != 0) {
    /**/ if (H->has_luma()) {
      Real l0;

      H->luma_interp(x, y, l0);

      diff = (1 - emphasis) * diff +
	emphasis * (H->zmax() / 1) * (
	fabs(l_plane(x, y) - l0));
    }

    else if (H->has_color()) {
      Real r0, g0, b0;

      H->color_interp(x, y, r0, g0, b0);

      diff = (1 - emphasis) * diff +
	emphasis * (H->zmax() / 3) * (
	fabs(r_plane(x, y) - r0) +
	fabs(g_plane(x, y) - g0) +
	fabs(b_plane(x, y) - b0));
    }
  }

  return diff;
}
