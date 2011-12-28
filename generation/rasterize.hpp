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

extern int hmap_w;
extern int hmap_h;
extern float *hmap_o;

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
