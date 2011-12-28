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

#include "scape/geom2d.H"

#define SPLIT_ON_INJECTION
#define SPLIT_ON_INJECTION_SNAP

#if	defined(SPLIT_ON_INJECTION) && defined(SPLIT_ON_INJECTION_SNAP)
#define	MATCH_WITH_HIRES
#endif

/* makes the smooth fall back to point-sampling */
#undef	MULTI_HEIGHTFIELD

#define MAXPATH	1024

/* ---------------------------------------------------- */

extern FILE *repfile;
extern FILE *logfile;
extern FILE *dbgfile;
#define logrf(format, ...) { if (repfile) { fprintf(repfile, format, ## __VA_ARGS__); } }
#define logpf(format, ...) { if (logfile) { fprintf(logfile, format, ## __VA_ARGS__); } }
#define logdf(format, ...) { if (dbgfile) { fprintf(dbgfile, format, ## __VA_ARGS__); } }

/* ---------------------------------------------------- */

extern const char *texFile;
extern const char *greyFile;
extern const char *colrFile;
extern const char *dataFile;
extern const char *watrFile;

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

/* ---------------------------------------------------- */

extern __int64 writechk;
extern int wdspace;
extern char wdsname[256];
extern int wchgame;
extern char *wpattern;

extern unsigned short basinshift;
extern Real heightscale;
extern Real heightshift;
extern Real heightadjust1;
extern Real heightadjust2;
extern Real sizescale;

extern int width, height;
extern int rasterx, rastery;
extern int tilesx, tilesy;
extern int nmtilex, nmtiley;
extern int dotilex, dotiley;

extern int rasterx, tilesx, dotilex, rwsizex;
extern int rastery, tilesy, dotiley, rwsizey;

/* ---------------------------------------------------- */

//#define minty (dotiley >= 0 ? min(dotiley +    0   , tilesy) :   0   )
//#define mintx (dotilex >= 0 ? min(dotilex +    0   , tilesx) :   0   )
//#define numty (dotiley >= 0 ? min(dotiley + nmtiley, tilesy) : tilesy)
//#define numtx (dotilex >= 0 ? min(dotilex + nmtilex, tilesx) : tilesx)

#define minty (dotiley +    0   )
#define mintx (dotilex +    0   )
#define numty (dotiley + nmtiley)
#define numtx (dotilex + nmtilex)

/* ---------------------------------------------------- */

#define	PROGRESS	63

void InitProgress(int rng, Real err);
void SetProgress(int dne, Real err);

void InitProgress(int rng);
void SetProgress(int dne);
void SetStatus(const char *status);
void SetTopic(const char *topic);
void SetTopic(const char *topic, int a, int b);
bool SetTopic(const unsigned long p, const unsigned long t, const char *topic);

void InitProgress(int rng, int srng);
void SetProgress(int dne, int sdne);
void PollProgress();
bool RequestFeedback(const char *question);

bool IsOlder(const char *name, __int64 tschk);
