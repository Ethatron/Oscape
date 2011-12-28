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

#include <math.h>
#include <random.h>
#include <sstream>
#include <vector>

using namespace std;

#include "../globals.h"
#include "Basic.H"
#include "quadedge.H"
#include "stuff.H"

extern void parseCommandline(int argc, char *argv[]);

extern Real emphasis;

extern int datadep;	// triangulation method: 1=data-dependent, 0=Delaunay

extern Real termination;
extern Real qual_thresh;	// quality threshold, 0<=thresh<=1
				// thresh=0 means pure data-dependent
				// thresh=.5 is a good value
				// thresh=1 means pure shape-dependent
					// (similar to Delaunay)

enum Criterion {SUMINF, MAXINF, SUM2, ABN};
// criteria for triangulating a quadrilateral, if doing data-dep. triangulation
//	SUMINF means minimize the sum of the Linf (maximum absolute) errors
//	MAXINF means minimize the maximum of the Linf (maximum absolute) errors
//	SUM2 means minimize the sum of the L2 (squared) errors

extern Criterion criterion;

extern Real area_thresh;// Maximum fraction of triangle area that is permitted
			// to be partially covered by samples.
			// Controls supersampling resolution.
			// 0 => infinite supersampling
			// .8 => moderate supersampling
			// 1e30 => no supersampling

extern int scancount;		// count of #pixels scanned during an update

extern int nscan, nsuper; // count of triangles scan converted & supersampled

#define debug 0
//extern int debug;	// debugging level: 0=none, 1=some, 2=more

extern Real thresh;
extern int parallelInsert;
extern int multinsert;
extern int limit;
extern Real alpha;
extern bool optimizemesh;

#define DEM_BAD 65535   // height value in DEM file of points to be ignored

#include "hfield.H"
#include "cfield.H"
#include "simplfield.H"

/* ---------------------------------------------------- */

void greedy_insert(SimplField& ter);
void greedy_insert_error(SimplField& ter);
