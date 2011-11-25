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

#include "Basic.H"
#include <math.h>
#include <random.h>
#include <sstream>
#include <vector>

using namespace std;

#include "quadedge.H"
#include "stuff.H"

#define SPLIT_ON_INJECTION
#define SPLIT_ON_INJECTION_SNAP

#if	defined(SPLIT_ON_INJECTION) && defined(SPLIT_ON_INJECTION_SNAP)
#define	MATCH_WITH_HIRES
#endif

extern void parse_cmdline(int argc, char *argv[]);

extern const char *texFile;
extern const char *greyFile;
extern const char *colrFile;
extern const char *dataFile;

extern bool writeobjmaps;
extern bool writefcemaps;
extern bool writenoresmaps;
extern bool writehiresmaps;
extern bool writeloresmaps;
extern bool nomodel;
extern bool nobasin;
extern bool noextrd;
extern bool emituvs;
extern bool emitnrm;
extern int rasterx, tilesx, dotilex, rwsizex;
extern int rastery, tilesy, dotiley, rwsizey;
extern int wdspace;

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

extern __int64 writechk;
extern Real thresh;
extern int parallelInsert;
extern int multinsert;
extern int limit;
extern Real alpha;
extern unsigned short basinshift;
extern bool optimizemesh;

extern int width, height;
extern int rasterx, rastery;
extern int tilesx, tilesy;
extern int nmtilex, nmtiley;
extern int dotilex, dotiley;

#define DEM_BAD 65535   // height value in DEM file of points to be ignored

#include "views.H"

#include "hfield.H"
#include "cfield.H"
#include "simplfield.H"

#ifdef	MATCH_WITH_HIRES
void read_pts(SimplField& ter, const char *name);
void write_pts(SimplField& ter, const char *name);
void read_pts(SimplField& ter);
void write_pts(SimplField& ter);
#endif

/* ---------------------------------------------------- */

void greedy_insert(SimplField& ter);
void greedy_insert_error(SimplField& ter);

void free_faces();

void write_prolog(SimplField &ter);
void write_prolog();
void write_optimize();

/* ---------------------------------------------------- */

extern vector<string> skiplist;
extern bool writetin;
extern bool writeobj;
extern bool writedx9;
extern bool writenif;

bool skiptile(int coordx, int coordy, int reso);

void write_mesh(SimplField& ter);

void write_obj(const char *name);
void write_objs(SimplField& ter, const char *pattern);
void write_obj(SimplField& ter);

void write_nif(const char *name);
void write_nifs(SimplField& ter, const char *pattern);
void write_nif(SimplField& ter);

void write_d9x(const char *name);
void write_d9xs(SimplField& ter, const char *pattern);
void write_d9x(SimplField& ter);

bool skipmsh(const char *pattern, const char *pfx, int coordx, int coordy, int reso, bool uvs);
void write_meshes(SimplField& ter, const char *pattern);

void free_textures();

#include "scape-tex.h"

/* ---------------------------------------------------- */

extern bool writeppm;
extern bool writepng;
extern bool writedds;

void write_nrmhgt0(bool fmaps, bool nmaps, bool hmaps, const HField& hf, const char *pattern);
void write_nrmhgt0(bool fmaps, bool nmaps, bool hmaps, const HField& hf);
void write_nrmhgt1(bool fmaps, bool nmaps, bool hmaps, const HField& hf, const char *pattern);
void write_nrmhgt1(bool fmaps, bool nmaps, bool hmaps, const HField& hf);
void write_nrmhgt2(bool fmaps, bool nmaps, bool hmaps, const HField& hf, const char *pattern);
void write_nrmhgt2(bool fmaps, bool nmaps, bool hmaps, const HField& hf);

void write_col0(bool cmaps, const CField& cf, const char *pattern);
void write_col0(bool cmaps, const CField& cf);
void write_col1(bool cmaps, const CField& cf, const char *pattern);
void write_col1(bool cmaps, const CField& cf);
void write_col2(bool cmaps, const CField& cf, const char *pattern);
void write_col2(bool cmaps, const CField& cf);
void write_col3(bool cmaps, const CField& cf, const char *pattern);
void write_col3(bool cmaps, const CField& cf);

bool skiptex(const char *pattern, const char *pfx, int coordx, int coordy, int reso, bool xyz);
void writetex(LPDIRECT3DTEXTURE9 tex, const char *pattern, const char *pfx, int coordx, int coordy, int reso, bool xyz);

/* ---------------------------------------------------- */

extern FILE *repfile;
extern FILE *logfile;
extern FILE *dbgfile;
#define logrf(format, ...) { if (repfile) { fprintf(repfile, format, ## __VA_ARGS__); } }
#define logpf(format, ...) { if (logfile) { fprintf(logfile, format, ## __VA_ARGS__); } }
#define logdf(format, ...) { if (dbgfile) { fprintf(dbgfile, format, ## __VA_ARGS__); } }

/* ---------------------------------------------------- */

void InitProgress(int rng, Real err);
void SetProgress(int dne, Real err);

void InitProgress(int rng);
void SetProgress(int dne);
void SetTopic(const char *topic);
void SetTopic(const char *topic, int a, int b);
bool SetTopic(const unsigned long p, const unsigned long t, const char *topic);

void InitProgress(int rng, int srng);
void SetProgress(int dne, int sdne);
void PollProgress();
bool RequestFeedback(const char *question);

bool IsOlder(const char *name, __int64 tschk);
