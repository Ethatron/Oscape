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

int hmap_w;
int hmap_h;
float *hmap_o = NULL;

#define EPSILON	(1.0 / 1024)

/* point in triangle via barycentric coordinates: */
void WriteTexel(float zoom, class objFace *F, class objVertex *A, class objVertex *B, class objVertex *C, Point2d P) {
  if (((int)P.x <= 0) ||
      ((int)P.y <= 0) ||
      ((int)P.x >= hmap_h) ||
      ((int)P.y >= hmap_h))
    return;

  // Compute vectors
  Point2d v0 = zoom * C->op - zoom * A->op;
  Point2d v1 = zoom * B->op - zoom * A->op;
  Point2d v2 =        P     - zoom * A->op;

  /* TODO: this is a bit weak ... */
  Real epsx = 0.5f / max(fabs(v0.x), fabs(v1.x));
  Real epsy = 0.5f / max(fabs(v0.y), fabs(v1.y));
  Real eps  = min(epsx, epsy);

  // Compute dot products
  Real dot00 = dot(v0, v0);
  Real dot01 = dot(v0, v1);
  Real dot02 = dot(v0, v2);
  Real dot11 = dot(v1, v1);
  Real dot12 = dot(v1, v2);

  // Compute barycentric coordinates
  Real invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
  Real u = (dot11 * dot02 - dot01 * dot12) * invDenom;
  Real v = (dot00 * dot12 - dot01 * dot02) * invDenom;

  // Check if point is in triangle (or on edge)
  if (((u     + eps) >= 0) &&
      ((    v + eps) >= 0) &&
      ((u + v - eps) <= 1)) {
    /* write face-normal (for testing) */
//  Real len = sqrt(F->nx * F->nx + F->ny * F->ny + F->nz * F->nz);
    Real len = 2.0f * F->narea;

    /* write face-height (for testing) */
    hmap_o[((int)P.y * hmap_w) + (int)P.x] = (float)(
      A->oz + u * (C->oz - A->oz) +
	      v * (B->oz - A->oz)
    );
  }
}

class RasterEdge
{
public:
  int X1, Y1, X2, Y2;

  RasterEdge(int x1, int y1,
	     int x2, int y2);
};

class RasterSpan
{
public:
  int X1, X2;

  RasterSpan(int x1, int x2);
};

class Rasterizer
{
protected:
  static
  void DrawSpan(float zoom,
		class objFace *F,
		class objVertex *V0,
		class objVertex *V1,
		class objVertex *V2,
		const RasterSpan &span,
		int y);

  static
  void DrawSpansBetweenEdges(float zoom,
			     class objFace *F,
			     class objVertex *V0,
			     class objVertex *V1,
			     class objVertex *V2,
			     const RasterEdge &e1,
			     const RasterEdge &e2);

public:
  static
  void DrawTriangle(float zoom,
		    class objFace *F,
		    class objVertex *V0,
		    class objVertex *V1,
		    class objVertex *V2);
};

RasterEdge::RasterEdge(int x1, int y1,
		       int x2, int y2)
{
  if (y1 < y2) {
    X1 = x1;
    Y1 = y1;
    X2 = x2;
    Y2 = y2;
  }
  else {
    X1 = x2;
    Y1 = y2;
    X2 = x1;
    Y2 = y1;
  }
}

RasterSpan::RasterSpan(int x1, int x2)
{
  if (x1 < x2) {
    X1 = x1;
    X2 = x2;
  }
  else {
    X1 = x2;
    X2 = x1;
  }
}

void
Rasterizer::DrawSpan(float zoom,
		     class objFace *F,
		     class objVertex *V0,
		     class objVertex *V1,
		     class objVertex *V2,
		     const RasterSpan &span,
		     int y)
{
  int xdiff = span.X2 - span.X1;

#if 0
  if (xdiff == 0)
    return;
#endif

  float factor = 0.0f;
  float factorStep = 1.0f / (float)xdiff;

  // draw each pixel in the span (inclusive)
  // draw one more left<->right, excess will be filtered out
  for (int x = span.X1 - 1; x <= span.X2 + 1; x++) {
//  SetPixel(x, y, span.Color1 + (colordiff * factor));
    WriteTexel(zoom, F, V0, V1, V2, Point2d(x, y));

    factor += factorStep;
  }
}

void
Rasterizer::DrawSpansBetweenEdges(float zoom,
				  class objFace *F,
				  class objVertex *V0,
				  class objVertex *V1,
				  class objVertex *V2,
				  const RasterEdge &e1,
				  const RasterEdge &e2)
{
  // calculate difference between the y coordinates
  // of the first edge and return if 0
  float e1ydiff = (float)(e1.Y2 - e1.Y1);
  /* we do inclusive */

  // calculate difference between the y coordinates
  // of the second edge and return if 0
  float e2ydiff = (float)(e2.Y2 - e2.Y1);

#if 0
  if (e1ydiff == 0.0f)
    return;
  if (e2ydiff == 0.0f)
    return;
#endif

  // calculate differences between the x coordinates
  // and colors of the points of the edges
  float e1xdiff = (float)(e1.X2 - e1.X1);
  float e2xdiff = (float)(e2.X2 - e2.X1);

  // calculate factors to use for interpolation
  // with the edges and the step values to increase
  // them by after drawing each span
  float factor1     = 0.0f;
  float factorStep1 = 0.0f;
  float factor2     = 0.0f;
  float factorStep2 = 0.0f;

  if (e1ydiff > 0.0)
    factor1     = (float)(e2.Y1 - e1.Y1) / e1ydiff,
    factorStep1 =          1.0f          / e1ydiff;

  if (e2ydiff > 0.0)
    factorStep2 =          1.0f          / e2ydiff;

  // loop through the lines between the edges and draw spans (inclusive)
  // draw one more left<->right, excess will be filtered out
  for (int y = e2.Y1; y <= e2.Y2 + 1; y++) {
    // create and draw span
    RasterSpan span(
      e1.X1 + (int)floor(e1xdiff * factor1),
      e2.X1 + (int) ceil(e2xdiff * factor2)
    );

    DrawSpan(zoom, F, V0, V1, V2, span, y);

    // increase factors
    factor1 += factorStep1;
    factor2 += factorStep2;
  }
}

void
Rasterizer::DrawTriangle(float zoom,
			 class objFace *F,
			 class objVertex *V0,
			 class objVertex *V1,
			 class objVertex *V2)
{
  // create edges for the triangle
  RasterEdge edges[3] = {
    RasterEdge((int)(zoom * V0->op.x), (int)(zoom * V0->op.y),
	       (int)(zoom * V1->op.x), (int)(zoom * V1->op.y)),
    RasterEdge((int)(zoom * V1->op.x), (int)(zoom * V1->op.y),
	       (int)(zoom * V2->op.x), (int)(zoom * V2->op.y)),
    RasterEdge((int)(zoom * V2->op.x), (int)(zoom * V2->op.y),
	       (int)(zoom * V0->op.x), (int)(zoom * V0->op.y))
  };

  int maxLength = 0;
  int longEdge = 0;

  // find edge with the greatest length in the y axis
  for (int i = 0; i < 3; i++) {
    int length = edges[i].Y2 - edges[i].Y1;
    if (length > maxLength) {
      maxLength = length;
      longEdge = i;
    }
  }

  int shortEdge1 = (longEdge + 1) % 3;
  int shortEdge2 = (longEdge + 2) % 3;

  // draw spans between edges; the long edge can be drawn
  // with the shorter edges to draw the full triangle
  DrawSpansBetweenEdges(zoom, F, V0, V1, V2, edges[longEdge], edges[shortEdge1]);
  DrawSpansBetweenEdges(zoom, F, V0, V1, V2, edges[longEdge], edges[shortEdge2]);
}
