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

//
// scape.C
//
// The basic toplevel program to drive the terrain simplification.
//

#include "scape.H"
#include <string.h>
#include <time.h>
#include <math.h>
#include <limits.h>
//#include <sys/resource.h>

/* makes the smooth fall back to point-sampling */
#undef	MULTI_HEIGHTFIELD
#define MAXPATH	1024

int width, height;
int tilesx, tilesy;

__int64 writechk = 0;
int wdspace = 60;
char wdsname[256] = "tamriel";
int wchgame = 0;
char *wpattern = "%02d.%02d.%02d.%02d";

unsigned short basinshift  = (8192 - 512) / (2 * 4);
Real heightscale =    2.0f * 4.0f; /*Annwyn*/
Real heightshift = 8192.0f - 0.0f; /*Annwyn*/
Real heightadjust1 = 4.0f / (4.0f); /*Oblivion*/
Real heightadjust2 = 4.0f / (2.0f * sqrt(2.0f))/*Oblivion (conciously more steep!)*/;
Real sizescale = 131072.0f / 1024.0f;

extern int update_cost;

FILE *repfile = NULL;
FILE *logfile = NULL;

ostream *tin_out = NULL;
ostream *obj_out = NULL;

/* ---------------------------------------------------- */

vector<string> skiplist;
bool writetin = false;
bool writeobj = false;
bool writenif = false;
bool writedx9 = false;

bool skipTile(int coordx, int coordy, int reso) {
  bool skip = true;

  if (writeobj || writedx9 || writenif) {
    vector<string>::iterator walk = skiplist.begin();
    while (walk != skiplist.end()) {
      char nbase[MAXPATH], name[MAXPATH];

      /* codification is:
       * "worldspace.tilex*32.tiley*32.32"
       * worldspace == 60 == Tamriel
       */
      /**/ if (wchgame == 0)	// Oblivion
        _snprintf(nbase, sizeof(nbase) - 1, walk->data(), wdspace, coordx, coordy, reso);
      else if (wchgame == 1)	// Skyrim
        _snprintf(nbase, sizeof(nbase) - 1, walk->data(), wdsname, reso, coordx, coordy);

      /* lower-case */
      strlwr(nbase);

      if (writenif) {
	/**/ if (wchgame == 0) {	// Oblivion
	  strcpy(name, nbase);
	  strcat(name, ".nif");
	}
	else if (wchgame == 1) {	// Skyrim
	  strcpy(name, nbase);
	  strcat(name, ".btr");
	}

	skip = skip && !IsOlder(name, writechk);
      }

      if (writeobj) {
	strcpy(name, nbase);
	strcat(name, ".obj");

	skip = skip && !IsOlder(name, writechk);
      }

      if (writedx9) {
	strcpy(name, nbase);
	strcat(name, ".x");

	skip = skip && !IsOlder(name, writechk);
      }

      walk++;
    }
  }

  return skip;
}

/* ---------------------------------------------------- */

void ps_edge(Edge *e,void *closure)
{
    ostream& out = *(ostream *)closure;

    const Point2d& a = e->Org2d();
    const Point2d& b = e->Dest2d();

    out << a.x << " " << a.y << " ";
    out << b.x << " " << b.y << " L" << endl;
}

void ps_face(Triangle *t,void *closure)
{
    ostream& out = *(ostream *)closure;
    int x, y;

    t->getSelection(&x, &y);

    out << x << " " << y << " C" << endl;
}

void output_ps(SimplField& ter)
{
    ofstream out("tin.eps",ios::binary);
    out << "%!PS-Adobe-2.0 EPSF-2.0" << endl;
    out << "%%Creator: Scape" << endl;
    out << "%%BoundingBox: 0 0 " << ter.getHField()->getWidth()-1 << " ";
    out << ter.getHField()->getHeight()-1 << endl;
    out << "%%EndComments" << endl;
    out << "/L {moveto lineto stroke} bind def" << endl;
    out << "/C {newpath 2.5 0 360 arc closepath fill} bind def" << endl;
    out << ".2 setlinewidth" << endl;

    ter.OverEdges(ps_edge,(ostream *)&out);
    out << ".3 setgray" << endl;
    ter.OverFaces(ps_face,(ostream *)&out);

    out << "showpage" << endl;
}


void output_face(Triangle *t,void *closure)
{
    SimplField *ter = (SimplField *)closure;
    ostream& tin = *tin_out;

    const Point2d& p1 = t->point1();
    const Point2d& p2 = t->point2();
    const Point2d& p3 = t->point3();

    tin << "t ";

    tin << (p1.x * sizescale) << " " << (p1.y * sizescale) << " ";
    tin << (ter->getHField()->getZ(p1) * heightscale - heightshift) << "   ";

    tin << (p2.x * sizescale) << " " << (p2.y * sizescale) << " ";
    tin << (ter->getHField()->getZ(p2) * heightscale - heightshift) << "   ";

    tin << (p3.x * sizescale) << " " << (p3.y * sizescale) << " ";
    tin << (ter->getHField()->getZ(p3) * heightscale - heightshift) << endl;
}


void wrteTIN(SimplField& ter)
{
    ofstream tin("out.tin",ios::binary);
    tin_out = &tin;

    ter.OverFaces(output_face,&ter);
}

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

Real narea(const Point2d& v1, const Point2d& v2, const Point2d& v3) {
  struct { Real x,y,z; } v1v2;
  struct { Real x,y,z; } v1v3;

  v1v2.x = v1.x - v2.x;
  v1v2.y = v1.y - v2.y;
  v1v2.z =   0  -   0 ;

  v1v3.x = v1.x - v3.x;
  v1v3.y = v1.y - v3.y;
  v1v3.z =   0  -   0 ;

  struct { Real x,y,z; } cross;
  Real mag;

  cross.x = v1v2.y * v1v3.z - v1v2.z * v1v3.y;
  cross.y = v1v2.z * v1v3.x - v1v2.x * v1v3.z;
  cross.z = v1v2.x * v1v3.y - v1v2.y * v1v3.x;

  mag = sqrt(
    (cross.x * cross.x) +
    (cross.y * cross.y) +
    (cross.z * cross.z)
  );

  return 0.5f * fabs(mag);
}

#include "scape-geometry.C"

/* ---------------------------------------------------- */

void TransferGeometry(SimplField &ter) {
  tri_procd = 0;
  tri_added = 0;
  tri_sectd = 0;

  dimx = ter.getHField()->getWidth();
  dimy = ter.getHField()->getHeight();

  if (limit) InitProgress(limit);
  ter.OverFaces(CollectFace, ter.getHField());

  logpf("faces processed %d & added %d\n", tri_procd, tri_added);

  CalculateGeometryNormals();
//write_???();
}

void TransferGeometry() {
#ifdef	SPLIT_ON_INJECTION
  TileGeometry();
//write_???s();
#endif
}

void RevisitGeometry() {
#ifdef	SPLIT_ON_INJECTION
  OptimizeGeometry();
//write_???s();
#endif
}

/* ---------------------------------------------------- */

void readWaterFile(SimplField& ter, const char *name) {
  FILE *pts = fopen(name, "rb");
  if (!pts)
    return;

  // round down, negative side would be smaller than positive side
  int ox = (tilesx / 2) * (int)(rasterx / 32);
  int oy = (tilesy / 2) * (int)(rastery / 32);
  int gw = ter.getHField()->getWidth();
  int gh = ter.getHField()->getHeight();

  /* calculate number of lines (TODO: make it faster) */
  char buf[512]; int lines = 0;
  while (fgets(buf, 512, pts) != NULL)
    lines++;

  /* initialize progress */
  fseek(pts, 0, SEEK_SET);
  int p = (lines + 499) / 500;
  if (lines)
    InitProgress(lines, 0.0);

  /* read in every point in the file and update progress */
  int x, y, bv = 0, cnt = 0; long l; float f;
  while (fgets(buf, 256, pts) != NULL) {
//while (fscanf(pts, "%d %d\n", &x, &y) == 2) {
    /* comments / sections allowed */
    if ((buf[0] != '\0') &&
	(buf[0] != ';') &&
	(buf[0] != '[') &&
	(buf[0] != '#')) {
      /* can we also interprete it? */
      if (sscanf(buf, "%4d %4d 0x%08x %f\n", &x, &y, &l, &f) == 4) {
	/* from world-space to heightfield-space */
	Point2d p(
	  (x + ox) * 32,
	  (y + oy) * 32
	);

	/* real water */
	if ((l != 0x7F7FFFFF) && (l != 0x4F7FFFC9))
	  RegisterWater(p, f + 14000, false);
      }
    }

    /* advance progress */
    if ((cnt % p) == 0)
      SetProgress(cnt, 0.0);

    cnt++;
  }

  logpf("%d water-levels added\n", bv);
  fclose(pts);
}

/* ---------------------------------------------------- */

#ifdef	MATCH_WITH_HIRES
void readPointsFile(SimplField& ter, const char *name) {
  FILE *pts = fopen(name, "rb");
  if (!pts)
    return;

  // round down, negative side would be smaller than positive side
  int ox = (tilesx / 2) * (int)(sizescale * rasterx);
  int oy = (tilesy / 2) * (int)(sizescale * rastery);
  int gw = ter.getHField()->getWidth();
  int gh = ter.getHField()->getHeight();

  /* calculate number of lines (TODO: make it faster) */
  char buf[256]; int lines = 0;
  while (fgets(buf, 256, pts) != NULL)
    lines++;

  /* initialize progress */
  fseek(pts, 0, SEEK_SET);
  int p = (lines + 499) / 500;
  if (lines)
    InitProgress(lines, ter.getCurrentError());

  /* read in every point in the file and update progress */
  int x, y, bv = 0, cnt = 0; Real fx, fy;
  while (fgets(buf, 256, pts) != NULL) {
//while (fscanf(pts, "%d %d\n", &x, &y) == 2) {
    /* comments / sections allowed */
    if ((buf[0] != '\0') &&
        (buf[0] != ';') &&
        (buf[0] != '[') &&
        (buf[0] != '#')) {
      /* can we also interprete it? */
      if (sscanf(buf, "%d %d\n", &x, &y) == 2) {
	/* from world-space to heightfield-space */
	x = (int)floor((Real)(x + ox) / sizescale);
	y = (int)floor((Real)(y + oy) / sizescale);

	/* heightfield-space coordinates */
	int sx = (int)floor((1.0f / rasterx) * x) * rasterx;
	int sy = (int)floor((1.0f / rastery) * y) * rastery;

	/* collect tile-border vertices only */
	if ((x == sx) ||
	    (y == sy) ||
	    (x == gw) ||
	    (y == gh)) {
	  x = min(x, gw - 1);
	  y = min(y, gh - 1);

	  logrf("%d/%d: ", bv, cnt);
//	  bv += ter.select_new_point(x, y) ? 1 : 0;
	  bv += ter.select_fix_point(x, y) ? 1 : 0;
	}

	/* TODO: add fractional support (requires identification if it's a custom point file) */
      }
    }

    /* advance progress */
    if ((cnt % p) == 0)
      SetProgress(cnt, ter.getCurrentError());

    cnt++;
  }

  logpf("%d border-vertices added\n", bv);
  fclose(pts);
}

void wrtePointsFile(SimplField& ter, const char *name) {
  FILE *pts = fopen(name, "wb");
  if (!pts)
    return;

  // round down, negative side would be smaller than positive side
  int ox = (tilesx / 2) * (int)(sizescale * rasterx);
  int oy = (tilesy / 2) * (int)(sizescale * rastery);
  int gw = ter.getHField()->getWidth();
  int gh = ter.getHField()->getHeight();
  int bv = 0;

  set<class objVertex *, struct V>::const_iterator itv;
  for (itv = Vertices.begin(); itv != Vertices.end(); itv++) {
    class objVertex *vo = (*itv);

    /* heightfield-space coordinates */
    Real sx = floor((1.0f / rasterx) * vo->vtx.x) * rasterx;
    Real sy = floor((1.0f / rastery) * vo->vtx.y) * rastery;

    /* collect tile-border vertices */
    if ((vo->vtx.x == sx) ||
	(vo->vtx.y == sy) ||
	(vo->vtx.x == gw) ||
	(vo->vtx.y == gh)) {
      bv++;

      /* world-space coordinates */
      fprintf(pts, "%d %d\n", (int)(vo->x - ox), (int)(vo->y - oy));
    }
  }

  logpf("%d border-vertices preserved\n", bv);
  fclose(pts);
}

void readPointsFile(SimplField& ter) {
  readPointsFile(ter, "out.pts");
}

void wrtePointsFile(SimplField& ter) {
  wrtePointsFile(ter, "out.pts");
}
#endif

/* ---------------------------------------------------- */

void wrteWavefront(const char *name) {
  set<class objVertex *, struct V>::iterator itv;
  vector<class objFace *>::iterator itf;
  int idx = 1;

  ofstream obj(name, ios::binary);
  obj_out = &obj;

  obj.setf(ios::fixed, ios::floatfield);

  obj.precision(4); //
  for (itv = Vertices.begin(); itv != Vertices.end(); itv++) {
    /* assign index the moment of writing it out */
    (*itv)->idx = idx++;

    obj << "v "  << (*itv)->x  << " " << (*itv)->y  << " " << (*itv)->z  << endl;
  }

  /* these are sort of meaning-less as we have
    * [0,1] texture coordinated _per tile_ later on
    */
  if (emituvs) {
    obj.precision(8); // 1/2^23
    for (itv = Vertices.begin(); itv != Vertices.end(); itv++) {
      obj << "vt " << (*itv)->tx << " " << (*itv)->ty << endl;
    }
  }

  if (emitnrm) {
    obj.precision(5); // 1/2^16
    for (itv = Vertices.begin(); itv != Vertices.end(); itv++) {
      obj << "vn " << (*itv)->nx << " " << (*itv)->ny << " " << (*itv)->nz << endl;
    }
  }

  for (itf = Faces.begin(); itf != Faces.end(); itf++) {
    obj << "f ";
    obj << (*itf)->v[0]->idx << "/" << (*itf)->v[0]->idx << "/" << (*itf)->v[0]->idx << " ";
    obj << (*itf)->v[1]->idx << "/" << (*itf)->v[1]->idx << "/" << (*itf)->v[1]->idx << " ";
    obj << (*itf)->v[2]->idx << "/" << (*itf)->v[2]->idx << "/" << (*itf)->v[2]->idx << endl;
  }
}

#ifdef	SPLIT_ON_INJECTION
void wrteWavefront(SimplField& ter, const char *pattern) {
  viterator itv;
  fiterator itf;

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

    SetTopic(emituvs ? "Saving UV tile {%d,%d}:" : "Saving non-UV tile {%d,%d}:", coordx, coordy);

    char name[MAXPATH];

    /**/ if (wchgame == 0)	// Oblivion
      _snprintf(name, sizeof(name) - 1, pattern, wdspace, coordx, coordy, min(resx, resy));
    else if (wchgame == 1)	// Skyrim
      _snprintf(name, sizeof(name) - 1, pattern, wdsname, min(resx, resy), coordx, coordy);

    /* lower-case */
    strlwr(name);
    strcat(name, ".obj");

    /* check if to write */
    if (IsOlder(name, writechk)) {
      logrf("writing \"%s\"\r", name);

      /* OBJ starts on index 1 */
      int idx = 1;

      ofstream obj(name,ios::binary);
      obj_out = &obj;

      obj.setf(ios::fixed, ios::floatfield);

      obj.precision(4); //
      for (itv = SectorVerticeO[ty][tx].begin(); itv != SectorVerticeO[ty][tx].end(); itv++) {
      	/* assign index the moment of writing it out */
	(*itv)->idx = idx++;

	obj << "v "  << (*itv)->x  << " " << (*itv)->y  << " " << (*itv)->z  << endl;
      }

      /* these are sort of meaning-less as we have
	* [0,1] texture coordinated _per tile_ later on
	*/
      if (emituvs) {
	obj.precision(8); // 1/2^23
	for (itv = SectorVerticeO[ty][tx].begin(); itv != SectorVerticeO[ty][tx].end(); itv++) {
	  obj << "vt " << (*itv)->tx << " " << (*itv)->ty << endl;
	}
      }

      if (emitnrm) {
	obj.precision(5); // 1/2^16
	for (itv = SectorVerticeO[ty][tx].begin(); itv != SectorVerticeO[ty][tx].end(); itv++) {
	  obj << "vn " << (*itv)->nx << " " << (*itv)->ny << " " << (*itv)->nz << endl;
	}
      }

      for (itf = SectorFaceO[ty][tx].begin(); itf != SectorFaceO[ty][tx].end(); itf++) {
	obj << "f ";
	obj << (*itf)->v[0]->idx << "/" << (*itf)->v[0]->idx << "/" << (*itf)->v[0]->idx << " ";
	obj << (*itf)->v[1]->idx << "/" << (*itf)->v[1]->idx << "/" << (*itf)->v[1]->idx << " ";
	obj << (*itf)->v[2]->idx << "/" << (*itf)->v[2]->idx << "/" << (*itf)->v[2]->idx << endl;
      }
    }

    /* advance progress */
    SetProgress((numty - minty) * (ty - minty) + (tx - mintx) + 1);
  }
  }
}
#endif

void wrteWavefront(SimplField& ter) {
  wrteWavefront("out.obj");

#ifdef	SPLIT_ON_INJECTION
  wrteWavefront(ter, "%02d.%02d.%02d.%02d");
#endif
}

/* ---------------------------------------------------- */

#if 1
#define	NIFLIB_STATIC_LINK
#define NIFLIB_QUOTEDIR(__D)  #__D
//#define NIFLIB_BASEDIR(__D)	NIFLIB_QUOTEDIR(../NIFtools/trunk/niflib/)
#define NIFLIB_BASEDIR(__L)	NIFLIB_QUOTEDIR(../NIFtools (Ethatron)/niflib/__L)

#include NIFLIB_BASEDIR(include/niflib.h)
#include NIFLIB_BASEDIR(include/obj/NiNode.h)
#include NIFLIB_BASEDIR(include/obj/NiTriShape.h)
#include NIFLIB_BASEDIR(include/obj/NiTriShapeData.h)
#include NIFLIB_BASEDIR(include/obj/BSMultiBoundNode.h)
#include NIFLIB_BASEDIR(include/obj/BSMultiBound.h)
#include NIFLIB_BASEDIR(include/obj/BSMultiBoundAABB.h)
#include NIFLIB_BASEDIR(include/obj/BSLightingShaderProperty.h)
#include NIFLIB_BASEDIR(include/obj/BSShaderTextureSet.h)

using namespace Niflib;

NiObjectRef FindRoot( vector<NiObjectRef> const & objects ) {
  //--Look for a NiNode that has no parents--//

  //Find the first NiObjectNET derived object
  NiAVObjectRef root;
  for (unsigned int i = 0; i < objects.size(); ++i) {
    root = DynamicCast<NiAVObject>(objects[i]);
    if ( root != NULL ) {
      break;
    }
  }

  //Make sure a node was found, if not return first node
  if ( root == NULL )
    return objects[0];

  //Move up the chain to the root node
  while ( root->GetParent() != NULL ) {
    root = StaticCast<NiAVObject>(root->GetParent());
  }

  return StaticCast<NiObject>(root);
}

//#define NIFLIB_LIBDIR	"../../../NIFtools/trunk/niflib/"
#define NIFLIB_LIBDIR	"../../../NIFtools (Ethatron)/niflib/"

#if   _MSC_VER == 1500
#define LIBDIR	"vc9"
#elif _MSC_VER == 1600
#define LIBDIR	"vc10"
#endif

#ifndef	NDEBUG
#ifdef	_WIN64
#pragma comment(lib,NIFLIB_LIBDIR LIBDIR "/lib/niflib_static_debug_x64")
#else
#pragma comment(lib,NIFLIB_LIBDIR LIBDIR "/lib/niflib_static_debug")
#endif
#else
#ifdef	_WIN64
#pragma comment(lib,NIFLIB_LIBDIR LIBDIR "/lib/niflib_static_x64")
#else
#pragma comment(lib,NIFLIB_LIBDIR LIBDIR "/lib/niflib_static")
#endif
#endif

void wrteNIF(const char *name) {
}

void wrteNIF(SimplField& ter, const char *pattern) {
  viterator itv;
  fiterator itf;

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

    SetTopic(emituvs ? "Saving UV tile {%d,%d}:" : "Saving non-UV tile {%d,%d}:", coordx, coordy);

    char name[MAXPATH];

    /**/ if (wchgame == 0)	// Oblivion
      _snprintf(name, sizeof(name) - 1, pattern, wdspace, coordx, coordy, min(resx, resy));
    else if (wchgame == 1)	// Skyrim
      _snprintf(name, sizeof(name) - 1, pattern, wdsname, min(resx, resy), coordx, coordy);

    /* lower-case */
    strlwr(name);
    strcat(name, ".nif");

    /* check if to write */
    if (IsOlder(name, writechk)) {
      logrf("writing \"%s\"\r", name);

      /* NIF starts on index 0 */
      int idx = 0, ifx = 0;

//    NiNodeRef root = new NiNode;
//    root->SetName("");

      /* normal treatment --------------------------------------------------------------- */
      if ((SectorVerticeO[ty][tx].size() < 0xFFFF) &&
	  (SectorFaceO   [ty][tx].size() < 0xFFFF)) {
	NiTriShapeRef root = new NiTriShape;
	NiTriShapeDataRef data = new NiTriShapeData;
	NifInfo info;

	info.version = VER_20_0_0_5;
	info.endian = ENDIAN_LITTLE;
	info.userVersion = 11;
	info.userVersion2 = 11;

	root->SetData(data);
	root->SetFlags(14);
	root->SetLocalScale(0.0f);
	root->SetLocalTranslation(Niflib::Vector3(
	  (float)((sizescale * rasterx) * (coordx / resx)),
	  (float)((sizescale * rastery) * (coordy / resy)),
	  0.0f
	));

	/* NIF starts on index 0 */
	idx = 0;

	{
	  vector<Niflib::Vector3> passv;

	  for (itv = SectorVerticeO[ty][tx].begin(); itv != SectorVerticeO[ty][tx].end(); itv++) {
      	    /* assign index the moment of writing it out */
	    (*itv)->idx = idx++;

	    passv.push_back(Niflib::Vector3(
	      (float)(*itv)->x,
	      (float)(*itv)->y,
	      (float)(*itv)->z)
	    );
	  }

	  data->SetVertices(passv);
	}

	if (emituvs) {
	  data->SetUVSetCount(1);

	  vector<Niflib::TexCoord> passt;

	  /* these are sort of meaning-less as we have
	    * [0,1] texture coordinated _per tile_ later on
	    */
	  for (itv = SectorVerticeO[ty][tx].begin(); itv != SectorVerticeO[ty][tx].end(); itv++) {
	    passt.push_back(Niflib::TexCoord((float)(*itv)->tx, (float)(*itv)->ty));
	  }

	  data->SetUVSet(0, passt);
	}

	if (emitnrm) {
	  vector<Niflib::Vector3> passn;

	  for (itv = SectorVerticeO[ty][tx].begin(); itv != SectorVerticeO[ty][tx].end(); itv++) {
	    passn.push_back(Niflib::Vector3((float)(*itv)->nx, (float)(*itv)->ny, (float)(*itv)->nz));
	  }

	  data->SetNormals(passn);
	}

	{
	  vector<Niflib::Triangle> passi;

	  for (itf = SectorFaceO[ty][tx].begin(); itf != SectorFaceO[ty][tx].end(); itf++) {
	    passi.push_back(Niflib::Triangle(
	      (*itf)->v[0]->idx,
	      (*itf)->v[1]->idx,
	      (*itf)->v[2]->idx
	    ));
	  }

	  data->SetTriangles(passi);
	}

	WriteNifTree(name, root, info);
      }
      /* special treatment --------------------------------------------------------------- */
      else {
	if (!RequestFeedback("The current tile contains too much triangles.\n"
			      "Attempt to split it to reduce it's size?\n"
			      "These files won't be compatible with Oblivion."))
	  throw runtime_error("ExitThread");

	NiNodeRef root = new NiNode;
	NifInfo info;

	info.version = VER_20_0_0_5;
	info.endian = ENDIAN_LITTLE;
	info.userVersion = 11;
	info.userVersion2 = 11;
	/* mark the file to have been optimized (change version when adding more features) */
	info.creator = "OS01";

//	Real sidex = 0.5 * sizescale * rasterx;
	Real sidex = 0.0;
	Real sidey = 0.0;

	{
	  /* calculate the median of either direction */
	  for (itv = SectorVerticeO[ty][tx].begin(); itv != SectorVerticeO[ty][tx].end(); itv++) {
	    sidex += (*itv)->x;
	    sidey += (*itv)->y;
	  }

	  sidex /= SectorVerticeO[ty][tx].size();
	  sidey /= SectorVerticeO[ty][tx].size();
	}

	/* TODO: don't emit double faces (same face in multiple quadrants) */
	for (int sy = -1; sy <= 1; sy += 2)
	for (int sx = -1; sx <= 1; sx += 2) {
	  NiTriShapeRef node = new NiTriShape;
	  NiTriShapeDataRef data = new NiTriShapeData;

	  node->SetData(data);
	  node->SetFlags(14);
	  node->SetLocalScale(0.0f);
	  node->SetLocalTranslation(Niflib::Vector3(
	    (float)((sizescale * rasterx) * (coordx / resx)),
	    (float)((sizescale * rastery) * (coordy / resy)),
	    0.0f
	  ));

	  vector<Niflib::Vector3> passv;
	  vector<Niflib::Triangle> passi;
	  vector<Niflib::Vector3> passn;
	  vector<Niflib::TexCoord> passt;

	  /* counter starts on index 0 */
	  idx = 0, ifx = 0;

	  {
	    /* unmark them all */
	    for (itv = SectorVerticeO[ty][tx].begin(); itv != SectorVerticeO[ty][tx].end(); itv++)
	      (*itv)->idx = -1;
	  }

	  {
	    /* mark per quadrant */
	    for (itf = SectorFaceO[ty][tx].begin(); itf != SectorFaceO[ty][tx].end(); itf++) {
	      Real tendencyy =
		(sidey - (*itf)->v[0]->y) +
		(sidey - (*itf)->v[1]->y) +
		(sidey - (*itf)->v[2]->y);
	      Real tendencyx =
		(sidex - (*itf)->v[0]->x) +
		(sidex - (*itf)->v[1]->x) +
		(sidex - (*itf)->v[2]->x);

	      if (((sy <= 0) && (tendencyy <= 0)) ||
		  ((sy >  0) && (tendencyy >  0)))
	      if (((sx <= 0) && (tendencyx <= 0)) ||
		  ((sx >  0) && (tendencyx >  0))) {
      		/* mark all used vertices in this slice */
      		if ((*itf)->v[0]->idx < 0)
		  (*itf)->v[0]->idx = idx++;
      		if ((*itf)->v[1]->idx < 0)
		  (*itf)->v[1]->idx = idx++;
      		if ((*itf)->v[2]->idx < 0)
		  (*itf)->v[2]->idx = idx++;

		ifx++;
	      }
	    }
	  }

	  /* blaaaa */
	  if ((idx > 65535) || (ifx > 65535)) {
	    throw runtime_error("Tile couldn't be split sufficiently to fit into a NIF. Reduce the number of target-points!");
	    continue;
	  }

	  /* NIF starts on index 0 */
	  idx = 0;

	  {
	    /* preserve order of optimization, reindex all marked vertices */
	    for (itv = SectorVerticeO[ty][tx].begin(); itv != SectorVerticeO[ty][tx].end(); itv++) {
	      if ((*itv)->idx >= 0)
		(*itv)->idx = idx++;
	    }
	  }

	  assert(idx >= 0);
	  assert(idx <= 65535);

	  {
	    passv.resize(idx);
	    for (itv = SectorVerticeO[ty][tx].begin(); itv != SectorVerticeO[ty][tx].end(); itv++) {
	      assert((*itv)->idx <= 65535);

      	      if ((*itv)->idx >= 0)
		passv[(*itv)->idx] = (Niflib::Vector3(
		  (float)(*itv)->x,
		  (float)(*itv)->y,
		  (float)(*itv)->z)
		);
	    }

	    data->SetVertices(passv);
	  }

	  if (emituvs) {
	    data->SetUVSetCount(1);

	    /* these are sort of meaning-less as we have
	      * [0,1] texture coordinated _per tile_ later on
	      */
	    passt.resize(idx);
	    for (itv = SectorVerticeO[ty][tx].begin(); itv != SectorVerticeO[ty][tx].end(); itv++) {
	      assert((*itv)->idx <= 65535);

      	      if ((*itv)->idx >= 0)
		passt[(*itv)->idx] = (Niflib::TexCoord(
		  (float)(*itv)->tx,
		  (float)(*itv)->ty)
		);
	    }

	    data->SetUVSet(0, passt);
	  }

	  if (emitnrm) {
	    passn.resize(idx);
	    for (itv = SectorVerticeO[ty][tx].begin(); itv != SectorVerticeO[ty][tx].end(); itv++) {
	      assert((*itv)->idx <= 65535);

      	      if ((*itv)->idx >= 0)
		passn[(*itv)->idx] = (Niflib::Vector3(
		  (float)(*itv)->nx,
		  (float)(*itv)->ny,
		  (float)(*itv)->nz
		));
	    }

	    data->SetNormals(passn);
	  }

	  {
	    for (itf = SectorFaceO[ty][tx].begin(); itf != SectorFaceO[ty][tx].end(); itf++) {
	      assert((*itf)->v[0]->idx <= 65535);

	      /* triangles touching the active area shouldn't make it */
	      if (((*itf)->v[0]->idx >= 0) &&
		  ((*itf)->v[1]->idx >= 0) &&
		  ((*itf)->v[2]->idx >= 0))
		passi.push_back(Niflib::Triangle(
		  (*itf)->v[0]->idx,
		  (*itf)->v[1]->idx,
		  (*itf)->v[2]->idx
		));
	    }

	    data->SetTriangles(passi);
	  }

	  root->AddChild((NiAVObjectRef)node);
	}

	WriteNifTree(name, root, info);
      }
    }

    /* advance progress */
    SetProgress((numty - minty) * (ty - minty) + (tx - mintx) + 1);
  }
  }
}

void wrteNIF(SimplField& ter) {
  wrteNIF("");

#ifdef	SPLIT_ON_INJECTION
  wrteNIF(ter, "%02d.%02d.%02d.%02d");
#endif
}

/* ---------------------------------------------------- */

void readBTR(const char *name) {
}

void readBTR(SimplField& ter, const char *pattern) {
  viterator itv;
  fiterator itf;

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

    SetTopic("Loading tile {%d,%d}:", coordx, coordy);

    char name[MAXPATH];
    char nbase[MAXPATH];

    /**/ if (wchgame == 0)	// Oblivion
      _snprintf(nbase, sizeof(nbase) - 1, pattern, wdspace, coordx, coordy, min(resx, resy));
    else if (wchgame == 1)	// Skyrim
      _snprintf(nbase, sizeof(nbase) - 1, pattern, wdsname, min(resx, resy), coordx, coordy);

    /* lower-case */
    strcpy(name, nbase);
    strlwr(name);
    strcat(name, ".btr");

    if (1) {
      vector<NiObjectRef> list;
      vector<NiAVObjectRef> chld;
      BSMultiBoundNodeRef root;
      NiTriShapeRef land;
      NiTriShapeDataRef ldta;
      NifInfo info;

      /* clear first */
      memset(&info, 0, sizeof(info));

      /* read the NIF */
      list = ReadNifList(name, &info);
      if (list.size()) {
	/* find the root */
	root = DynamicCast<BSMultiBoundNode>(FindRoot(list));
	if (root) {
	  chld = root->GetChildren();
	  if (chld.size()) {
	    land = DynamicCast<NiTriShape>(chld[0]);
	    if (land) {
	      ldta = DynamicCast<NiTriShapeData>(land->GetData());
	      if (ldta) {
		vector<Niflib::Vector3> v = ldta->GetVertices();
		vector<Niflib::Triangle> f = ldta->GetTriangles();

		vector<Niflib::Triangle>::iterator itf;
		for (itf = f.begin(); itf != f.end(); itf++) {
		  int i1 = (*itf).v1; Point2d p1(v[i1].x, v[i1].y); Real z1 = v[i1].z;
		  int i2 = (*itf).v2; Point2d p2(v[i2].x, v[i2].y); Real z2 = v[i2].z;
		  int i3 = (*itf).v3; Point2d p3(v[i3].x, v[i3].y); Real z3 = v[i3].z;

		  p1.x = (p1.x * land->scale) + tx * (sizescale * rasterx);
		  p1.y = (p1.y * land->scale) + ty * (sizescale * rasterx);
		  z1   = (z1   * land->scale) + 14000;

		  p2.x = (p2.x * land->scale) + tx * (sizescale * rasterx);
		  p2.y = (p2.y * land->scale) + ty * (sizescale * rasterx);
		  z2   = (z2   * land->scale) + 14000;

		  p3.x = (p3.x * land->scale) + tx * (sizescale * rasterx);
		  p3.y = (p3.y * land->scale) + ty * (sizescale * rasterx);
		  z3   = (z3   * land->scale) + 14000;

		  p1.x = (p1.x / sizescale);
		  p1.y = (p1.y / sizescale);
		  z1   = (z1   + heightshift) / heightscale;

		  p2.x = (p2.x / sizescale);
		  p2.y = (p2.y / sizescale);
		  z2   = (z2   + heightshift) / heightscale;

		  p3.x = (p3.x / sizescale);
		  p3.y = (p3.y / sizescale);
		  z3   = (z3   + heightshift) / heightscale;

		  RegisterFace(p1, z1, p2, z2, p3, z3);
		}
	      }
	    }
	  }
	}
      }
    }

    /* advance progress */
    SetProgress((numty - minty) * (ty - minty) + (tx - mintx) + 1);
  }
  }
}

void readBTR(SimplField& ter) {
  readBTR("");

#ifdef	SPLIT_ON_INJECTION
  readBTR(ter, "%s.%02d.%02d.%02d");
#endif
}

/* ---------------------------------------------------- */

void wrteBTR(const char *name) {
}

void wrteBTR(SimplField& ter, const char *pattern) {
  viterator itv;
  fiterator itf;

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

    SetTopic(emituvs ? "Saving UV tile {%d,%d}:" : "Saving non-UV tile {%d,%d}:", coordx, coordy);

    char name[MAXPATH], textc[MAXPATH], textn[MAXPATH];
    char nbase[MAXPATH], tbase[MAXPATH], *sbase;

    /**/ if (wchgame == 0)	// Oblivion
      _snprintf(nbase, sizeof(nbase) - 1, pattern, wdspace, coordx, coordy, min(resx, resy));
    else if (wchgame == 1)	// Skyrim
      _snprintf(nbase, sizeof(nbase) - 1, pattern, wdsname, min(resx, resy), coordx, coordy);

    /* lower-case */
    strcpy(name, nbase);
    strlwr(name);
    strcat(name, ".btr");

    /**/ if (wchgame == 0)	// Oblivion
      _snprintf(tbase, sizeof(tbase) - 1, "Data\\Textures\\LandscapeLOD\\Generated\\", wdsname);
    else if (wchgame == 1)	// Skyrim
      _snprintf(tbase, sizeof(tbase) - 1, "Data\\Textures\\Terrain\\%s\\", wdsname);

    /* file-name */
    if (!(sbase = strrchr(nbase, '\\')))
      sbase = nbase;
    else
      sbase++;

    strcpy(textc, tbase ); strcpy(textn, tbase   );
    strcat(textc, sbase ); strcat(textn, sbase   );
    strcat(textc, ".DDS"); strcat(textn, "_n.DDS");

    /* check if to write */
    if (IsOlder(name, writechk)) {
      logrf("writing \"%s\"\r", name);

      /* NIF starts on index 0 */
      int idx = 0, ifx = 0;

//    NiNodeRef root = new NiNode;
//    root->SetName("");

      /* normal treatment --------------------------------------------------------------- */
      if ((SectorVerticeO[ty][tx].size() < 0xFFFF) &&
	  (SectorFaceO   [ty][tx].size() < 0xFFFF)) {
	BSMultiBoundNodeRef root = new BSMultiBoundNode;
	BSMultiBoundRef mbnd = new BSMultiBound;
	BSMultiBoundAABBRef aabb = new BSMultiBoundAABB;
	NiTriShapeRef land = new NiTriShape;
	NiTriShapeDataRef ldta = new NiTriShapeData;
	BSLightingShaderPropertyRef lght = new BSLightingShaderProperty;
	BSShaderTextureSetRef txst = new BSShaderTextureSet;
	NifInfo info; bool hasw = false;
	float lscale = (float)min(resx, resy);
	float zmin = 0.0, zmax = -0.0;

	info.version = VER_20_2_0_7;
	info.endian = ENDIAN_LITTLE;
	info.userVersion = 12;
	info.userVersion2 = 83;
	/* mark the file to have been optimized (change version when adding more features) */
	info.creator = "OS01";

	/* no interfaces */
	lght->shaderType = 18;
	lght->textureTranslation1 = TexCoord(0.0f, 0.0f);
	lght->textureTranslation2 = TexCoord(1.0f, 1.0f);
	lght->shaderFlags1 = (SkyrimLightingShaderFlags1)((1 << 12)/*SF_UNKNOWN_3*/ | (1 << 22)/*SF_TREE_BILLBOARD*/ | (1 << 31)/*SF_ZBUFFER_TEST*/);
	lght->shaderFlags2 = (SkyrimLightingShaderFlags2)((1 << 1)/*SLSF2_1*/ | (1 << 0)/*SLSF2_ZBUFFER_WRITE*/);
	lght->textureSet = txst;

	txst->SetTexture(0, textc); // color
	txst->SetTexture(1, textn); // normal

	/* no interfaces */
	mbnd->data = aabb;
	root->multiBound = mbnd;

	root->SetName("chunk");
	root->AddChild((NiAVObjectRef)land);
	land->SetName("land");
	land->SetData(ldta);
	land->properties[0] = lght;
	land->unknownShort1 = 0;  // not 8
	land->SetFlags(14);
	land->SetLocalScale(lscale);
	land->SetLocalTranslation(Niflib::Vector3(
	  0.0f,
	  0.0f,
	  0.0f
	));

	/* NIF starts on index 0 */
	idx = 0;
	zmin = 0.0;
	zmax = 0.0;

	{
	  vector<Niflib::Vector3> passv;

	  for (itv = SectorVerticeO[ty][tx].begin(); itv != SectorVerticeO[ty][tx].end(); itv++) {
      	    /* assign index the moment of writing it out */
	    (*itv)->idx = idx++;

#if 0
	    /* record ocean-level */
	    if ((*itv)->z < 0.0)
	      hasw = true;
#endif

	    passv.push_back(Niflib::Vector3(
	      (float)(((*itv)->x -     0) / lscale),
	      (float)(((*itv)->y -     0) / lscale),
	      (float)(((*itv)->z - 14000) / lscale))
	    );

	    zmin = min(zmin, (float)((*itv)->z - 14000));
	    zmax = max(zmax, (float)((*itv)->z - 14000));
	  }

	  ldta->SetVertices(passv);
	}

	if (emituvs) {
	  ldta->SetUVSetCount(1);

	  vector<Niflib::TexCoord> passt;

	  /* these are sort of meaning-less as we have
	    * [0,1] texture coordinated _per tile_ later on
	    */
	  for (itv = SectorVerticeO[ty][tx].begin(); itv != SectorVerticeO[ty][tx].end(); itv++) {
	    passt.push_back(Niflib::TexCoord((float)(*itv)->tx, 1.0f - (float)(*itv)->ty));
	  }

	  ldta->SetUVSet(0, passt);
	}

	if (emitnrm) {
	  vector<Niflib::Vector3> passn;

	  for (itv = SectorVerticeO[ty][tx].begin(); itv != SectorVerticeO[ty][tx].end(); itv++) {
	    passn.push_back(Niflib::Vector3((float)(*itv)->nx, (float)(*itv)->ny, (float)(*itv)->nz));
	  }

	  ldta->SetNormals(passn);
	}

	{
	  vector<Niflib::Triangle> passi;

	  for (itf = SectorFaceO[ty][tx].begin(); itf != SectorFaceO[ty][tx].end(); itf++) {
	    passi.push_back(Niflib::Triangle(
	      (*itf)->v[0]->idx,
	      (*itf)->v[1]->idx,
	      (*itf)->v[2]->idx
	    ));
	  }

	  ldta->SetTriangles(passi);
	}

	/* no interfaces */
	aabb->minimum = Vector3(4096.0f * lscale * 0.5f, 4096.0f * lscale * 0.5f,        ((zmin + zmax) * 0.5f));
	aabb->maximum = Vector3(4096.0f * lscale * 0.5f, 4096.0f * lscale * 0.5f, zmax - ((zmin + zmax) * 0.5f));

	if (/*hasw ||*/ SectorWaters[ty][tx].size()) {
	  BSMultiBoundNodeRef woot = new BSMultiBoundNode;
	  BSMultiBoundRef mbnd = new BSMultiBound;
	  BSMultiBoundAABBRef aabb = new BSMultiBoundAABB;
	  NiTriShapeRef watr = new NiTriShape;
	  NiTriShapeDataRef wdta = new NiTriShapeData;

	  /* no interfaces */
	  mbnd->data = aabb;
	  woot->multiBound = mbnd;

	  root->AddChild((NiAVObjectRef)woot);
//	  woot->SetName("chunk");
	  woot->SetName("water");
	  woot->AddChild((NiAVObjectRef)watr);
//	  watr->SetName("water");
	  watr->SetData(wdta);
	  watr->unknownShort1 = 0;  // not 8
//	  watr->properties[0] = lght;
	  watr->SetFlags(14);
	  watr->SetLocalScale(lscale);
	  watr->SetLocalTranslation(Niflib::Vector3(
	    0.0f,
	    0.0f,
	    0.0f
	  ));

	  zmin = 0.0;
	  zmax = 0.0;

#if 0
	  for (itv = SectorVerticeO[ty][tx].begin(); itv != SectorVerticeO[ty][tx].end(); itv++) {
	    /* assign index the moment of writing it out */
	    (*itv)->idx = idx++;

	    /* record ocean-level (excluding far-borders) */
	    if (((*itv)->x != 131072.0) &&
	        ((*itv)->y != 131072.0) &&
		((*itv)->z <       0.0)) {
	      Point2d p(
		(float)(floor((*itv)->x * (1.0 / 4096.0)) * 4096.0),
		(float)(floor((*itv)->y * (1.0 / 4096.0)) * 4096.0)
	      );

	      class objWater wp;
	      class objWater *w;

	      set<class objWater *, struct W>::iterator i;

	      wp.wtx = p; w = NULL;

	      i = SectorWaters[ty][tx].find(&wp); if (i != SectorWaters[ty][tx].end()) w = *i;

	      if (!w) {
		w = new(&WPool) class objWater(); assert(w);
		w->wtx = p;

		w->x = p.x;
		w->y = p.y;
		w->z = 0.0;

		SectorWaters[ty][tx].insert(w);
	      }
	    }
	  }
#endif

	  vector<Niflib::Vector3> passw;
	  vector<Niflib::Triangle> passq;

	  set<class objWater *, struct W>::iterator itw;
	  for (itw = SectorWaters[ty][tx].begin(); itw != SectorWaters[ty][tx].end(); itw++) {
	    int idx1 = (int)passw.size();
	    passw.push_back(Niflib::Vector3(
	      (float)(((*itw)->x -     0) / lscale),
	      (float)(((*itw)->y -     0) / lscale),
	      (float)(((*itw)->z - 14000) / lscale))
	    );

	    int idx2 = (int)passw.size();
	    passw.push_back(Niflib::Vector3(
	      (float)(((*itw)->x +  4096) / lscale),
	      (float)(((*itw)->y +     0) / lscale),
	      (float)(((*itw)->z - 14000) / lscale))
	    );

	    int idx3 = (int)passw.size();
	    passw.push_back(Niflib::Vector3(
	      (float)(((*itw)->x +     0) / lscale),
	      (float)(((*itw)->y +  4096) / lscale),
	      (float)(((*itw)->z - 14000) / lscale))
	    );

	    int idx4 = (int)passw.size();
	    passw.push_back(Niflib::Vector3(
	      (float)(((*itw)->x +  4096) / lscale),
	      (float)(((*itw)->y +  4096) / lscale),
	      (float)(((*itw)->z - 14000) / lscale))
	    );

	    passq.push_back(Niflib::Triangle(
	      idx1,
	      idx2,
	      idx3
	    ));

	    passq.push_back(Niflib::Triangle(
	      idx4,
	      idx3,
	      idx2
	    ));

	    zmin = min(zmin, (float)((*itw)->z - 14000));
	    zmax = max(zmax, (float)((*itw)->z - 14000));
	  }

	  wdta->SetVertices(passw);
	  wdta->SetTriangles(passq);

	  /* no interfaces */
	  aabb->minimum = Vector3(4096.0f * lscale * 0.5f, 4096.0f * lscale * 0.5f,        ((zmin + zmax) * 0.5f));
	  aabb->maximum = Vector3(4096.0f * lscale * 0.5f, 4096.0f * lscale * 0.5f, zmax - ((zmin + zmax) * 0.5f));
	}

	WriteNifTree(name, root, info);
      }
      /* special treatment --------------------------------------------------------------- */
      else {
	throw runtime_error("The current tile contains too much triangles.");
      }
    }

    /* advance progress */
    SetProgress((numty - minty) * (ty - minty) + (tx - mintx) + 1);
  }
  }
}

void wrteBTR(SimplField& ter) {
  wrteBTR("");

#ifdef	SPLIT_ON_INJECTION
  wrteBTR(ter, "%s.%02d.%02d.%02d");
#endif
}
#endif

/* ---------------------------------------------------- */

#include <d3d9.h>
#include <d3dx9.h>

void wrteDXMesh(const char *name) {
}

void wrteDXMesh(SimplField& ter, const char *pattern) {
  D3DPRESENT_PARAMETERS Parameters;
  D3DDISPLAYMODE Mode;
  IDirect3D9 *pD3D;
  IDirect3DDevice9 *pD3DDevice;

  if (!(pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
    return;

  pD3D->GetAdapterDisplayMode(0, &Mode);

  memset(&Parameters, 0, sizeof(Parameters));

  Parameters.BackBufferWidth  = 1;
  Parameters.BackBufferHeight = 1;
  Parameters.BackBufferFormat = Mode.Format;
  Parameters.BackBufferCount  = 1;
  Parameters.SwapEffect       = D3DSWAPEFFECT_COPY;
  Parameters.Windowed         = TRUE;

  if (pD3D->CreateDevice(D3DADAPTER_DEFAULT,
      			  D3DDEVTYPE_NULLREF,
      			  GetConsoleWindow(),
      			  D3DCREATE_SOFTWARE_VERTEXPROCESSING,
      			  &Parameters,
      			  &pD3DDevice
      			) != D3D_OK) {
    pD3D->Release();
    return;
  }

  viterator itv;
  fiterator itf;

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

    SetTopic(emituvs ? "Saving UV tile {%d,%d}:" : "Saving non-UV tile {%d,%d}:", coordx, coordy);

    char name[MAXPATH];

    /**/ if (wchgame == 0)	// Oblivion
      _snprintf(name, sizeof(name) - 1, pattern, wdspace, coordx, coordy, min(resx, resy));
    else if (wchgame == 1)	// Skyrim
      _snprintf(name, sizeof(name) - 1, pattern, wdsname, min(resx, resy), coordx, coordy);

    /* lower-case */
    strlwr(name);
    strcat(name, ".x");

    /* check if to write */
    if (IsOlder(name, writechk) &&
	(SectorFaceO   [ty][tx].size() <= 65535) &&
	(SectorVerticeO[ty][tx].size() <= 65535)) {
      logrf("writing \"%s\"\r", name);

      /* X starts on index 0 */
      int idx = 0;

      LPD3DXMESH pMesh;
      if (D3DXCreateMeshFVF((DWORD)SectorFaceO   [ty][tx].size(),
			    (DWORD)SectorVerticeO[ty][tx].size(),
			    D3DXMESH_SYSTEMMEM,
			    (D3DFVF_XYZ | (emitnrm ? D3DFVF_NORMAL : 0) | (emituvs ? D3DFVF_TEX1 : 0)),
			    pD3DDevice,
			    &pMesh
			    ) != D3D_OK) {

	/* advance progress */
	SetProgress((numty - minty) * (ty - minty) + (tx - mintx) + 1);
	continue;
      }

      {
//     	LPDIRECT3DVERTEXBUFFER9 pIB;
//     	pMesh->GetVertexBuffer(&pVB);

      	LPVOID pData; float *pFloat;
      	pMesh->LockVertexBuffer(D3DLOCK_NOSYSLOCK, &pData);
      	pFloat = (float *)pData;

	for (itv = SectorVerticeO[ty][tx].begin(); itv != SectorVerticeO[ty][tx].end(); itv++) {
      	  /* assign index the moment of writing it out */
	  (*itv)->idx = idx++;

	  *pFloat++ = (float)(*itv)->x;
	  *pFloat++ = (float)(*itv)->y;
	  *pFloat++ = (float)(*itv)->z;

	  if (emitnrm) {
	    *pFloat++ = (float)(*itv)->nx;
	    *pFloat++ = (float)(*itv)->ny;
	    *pFloat++ = (float)(*itv)->nz;
	  }

	  if (emituvs) {
	    *pFloat++ = (float)(*itv)->tx;
	    *pFloat++ = (float)(*itv)->ty;
	  }
	}

	pMesh->UnlockVertexBuffer();
      }

      {
//    	LPDIRECT3DINDEXBUFFER9 pIB;
//    	pMesh->GetIndexBuffer(&pIB);

      	LPVOID pData; unsigned short int *pIndex;
      	pMesh->LockIndexBuffer(D3DLOCK_NOSYSLOCK, &pData);
      	pIndex = (unsigned short int *)pData;

	for (itf = SectorFaceO[ty][tx].begin(); itf != SectorFaceO[ty][tx].end(); itf++) {
	  *pIndex++ = (*itf)->v[0]->idx;
	  *pIndex++ = (*itf)->v[1]->idx;
	  *pIndex++ = (*itf)->v[2]->idx;
	}

	pMesh->UnlockIndexBuffer();
      }

      D3DXSaveMeshToX(
	name,
	pMesh,
	NULL,
	NULL,
	NULL,
	0,
	D3DXF_FILEFORMAT_BINARY | D3DXF_FILEFORMAT_COMPRESSED
      );

      pMesh->Release();
    }

    /* advance progress */
    SetProgress((numty - minty) * (ty - minty) + (tx - mintx) + 1);
  }
  }

  pD3DDevice->Release();
  pD3D->Release();
}

void wrteDXMesh(SimplField& ter)
{
  wrteDXMesh("");

#ifdef	SPLIT_ON_INJECTION
  wrteDXMesh(ter, "%02d.%02d.%02d.%02d");
#endif
}

void wrteGeometry(SimplField& ter, const char *pattern) {
  if (writeobj || writedx9 || writenif) {
//  write_mesh(ter);

    if (writenif) {
      /**/ if (wchgame == 0)	// Oblivion
	wrteNIF(ter, pattern);
      else if (wchgame == 1)	// Skyrim
	wrteBTR(ter, pattern);
    }

    if (writeobj)
      wrteWavefront(ter, pattern);
    if (writedx9)
      wrteDXMesh(ter, pattern);
  }
}

/* ---------------------------------------------------- */

bool skipTexture(const char *pattern, const char *pfx, int coordx, int coordy, int reso, bool xyz) {
  bool skip = true;

  if (writeppm || writepng || writedds) {
    char nbase[MAXPATH], name[MAXPATH];

    /**/ if (wchgame == 0)	// Oblivion
      _snprintf(nbase, sizeof(nbase) - 1, pattern, wdspace, coordx, coordy, reso);
    else if (wchgame == 1)	// Skyrim
      _snprintf(nbase, sizeof(nbase) - 1, pattern, wdsname, reso, coordx, coordy);

    /* change "_fn" to "_n" for Skyrim */
    if ((wchgame == 1))
      if (pfx && !strcmp(pfx, "_fn"))
	pfx = "_n";

    /* lower-case */
    strlwr(nbase);

    if (writeppm) {
      strcpy(name, nbase);
      strcat(name, pfx);
      strcat(name, ".ppm");

      skip = skip && !IsOlder(name, writechk);
    }

    if (writepng) {
      strcpy(name, nbase);
      strcat(name, pfx);
      strcat(name, ".png");

      skip = skip && !IsOlder(name, writechk);
    }

    if (writedds) {
      strcpy(name, nbase);
      strcat(name, pfx);
      strcat(name, ".dds");

      skip = skip && !IsOlder(name, writechk);
    }
  }

  return skip;
}

#define	addnote
#define	verbose		0
#define	optimizequick	0
#define	normalsteepness	2

LPDIRECT3DTEXTURE9 tnrm = NULL;
LPDIRECT3DTEXTURE9 thgt = NULL;
LPDIRECT3DTEXTURE9 tcol = NULL;

#include "scape-rasterize.C"
#include "scape-tex.cpp"

#define	PROGRESS	63
#include "scape-tex0x.C"
#include "scape-tex1x.C"
#include "scape-tex2x.C"

void freeTexture() {
  if (hmap_o) delete[] hmap_o; hmap_o = NULL;
  if (dmap  ) delete[] dmap  ; dmap   = NULL;

  if (tnrm) tnrm->Release(); tnrm = NULL;
  if (thgt) thgt->Release(); thgt = NULL;
  if (tcol) tcol->Release(); tcol = NULL;
}

bool writeppm = false;
bool writepng = false;
bool writedds = false;

void wrteTexture(LPDIRECT3DTEXTURE9 tex, const char *pattern, const char *pfx, int coordx, int coordy, int reso, bool xyz) {
  if (writeppm || writepng || writedds) {
    HRESULT res;
    char nbase[MAXPATH], name[MAXPATH];

    /**/ if (wchgame == 0)	// Oblivion
      _snprintf(nbase, sizeof(nbase) - 1, pattern, wdspace, coordx, coordy, reso);
    else if (wchgame == 1)	// Skyrim
      _snprintf(nbase, sizeof(nbase) - 1, pattern, wdsname, reso, coordx, coordy);

    /* lower-case */
    strlwr(nbase);

    /* change "_fn" to "_n" for Skyrim */
    if ((wchgame == 1))
      if (pfx && !strcmp(pfx, "_fn"))
	pfx = "_n";

    /* flip-y for Skyrim */
    if (wchgame == 1) {
      D3DSURFACE_DESC texd;
      D3DLOCKED_RECT texr;

      tex->GetLevelDesc(0, &texd);
      tex->LockRect(0, &texr, NULL, 0);

      /* xyz: D3DFMT_A16B16G16R16, color: D3DFMT_A8B8G8R8 */
      UCHAR *dTex = (UCHAR *)texr.pBits;
      UINT32 stride = texd.Width * 4 * (xyz ? 2 : 1);
      UCHAR *lTex = (UCHAR *)malloc(stride);
      /* do it for half of the picture, or do it twice :^P */
      for (UINT32 s = 0; s < (texd.Height >> 1); s++) {
	UINT32 loH = (                    s) * stride;
	UINT32 hiH = ((texd.Height - 1) - s) * stride;

	/* swap lines */
	memcpy(lTex      , dTex + loH, stride);
	memcpy(dTex + loH, dTex + hiH, stride);
	memcpy(dTex + hiH, lTex      , stride);
      }

      tex->UnlockRect(0);
      free(lTex);
    }

    if (writeppm) {
      strcpy(name, nbase);
      strcat(name, pfx);
      strcat(name, ".ppm");

      if (IsOlder(name, writechk)) {
	res = D3DXSaveTextureToFile(name, D3DXIFF_PPM, tex, NULL);
	if (res != D3D_OK) throw runtime_error("Failed to write texture");
      }
    }

    if (writepng) {
      strcpy(name, nbase);
      strcat(name, pfx);
      strcat(name, ".png");

      if (IsOlder(name, writechk)) {
	res = D3DXSaveTextureToFile(name, D3DXIFF_PNG, tex, NULL);
	if (res != D3D_OK) throw runtime_error("Failed to write texture");
      }
    }

    if (writedds) {
      strcpy(name, nbase);
      strcat(name, pfx);
      strcat(name, ".dds");

      if (IsOlder(name, writechk)) {
	/* preserve current textures-buffer */
	LPDIRECT3DTEXTURE9 tex_plus = tex;
	D3DSURFACE_DESC tex_dsc;

	tex->GetLevelDesc(0, &tex_dsc);
	if (1)
	  tex->AddRef();

	if (xyz) {
          /**/ if (wchgame == 0) {	// Oblivion
	    if (TextureCompressXYZ(&tex_plus, 0))
	      res = D3DXSaveTextureToFile(name, D3DXIFF_DDS, tex_plus, NULL);
	  }
          else if (wchgame == 1) {	// Skyrim
	    if (TextureCompressXZY(&tex_plus, 0))
	      res = D3DXSaveTextureToFile(name, D3DXIFF_DDS, tex_plus, NULL);
	  }
	}
	else {
	  if (TextureCompressRGB(&tex_plus, 0, true))
	    res = D3DXSaveTextureToFile(name, D3DXIFF_DDS, tex_plus, NULL);
	}

	if (tex_plus != tex)
	  tex_plus->Release();

	if (res != D3D_OK) throw runtime_error("Failed to write texture");
      }
    }
  }
}




void greedy_insert(SimplField& ter) {
  int p = (limit + 499) / 500;
  for (int i = 5; i <= limit && ter.select_new_point(); i++) {
    if ((i % p) == 0)
      SetProgress(i, ter.getCurrentError());
  }
}

void greedy_insert_error(SimplField& ter) {
  int p = (limit + 499) / 500;
  for (int i = 5; i <= limit && ter.select_new_points(5.0); i++) {
    if ((i % p) == 0)
      SetProgress(i, ter.getCurrentError());
  }
}


int main(int argc,char **argv) {
  repfile = stderr;
  logfile = stdout;

  srandom(0);
  parseCommandline(argc, argv);

  if (datadep)
    cout << "# doing data-dependent triangulation" << endl
	<< "#  with "
	<< (criterion==SUMINF ? "sum" : criterion==MAXINF ?
	    "max" : criterion==SUM2 ? "sqerr" : "abn")
	<< " criterion, threshold "
	<< qual_thresh << ", and fraction " << area_thresh
	<< endl;
  else
    cout << "# doing Delaunay triangulation" << endl;

  cout << "# emphasis=" << emphasis << " npoint=" << limit << endl;

  if( parallelInsert ) {
    cout << "# Using constant threshold parallel insert:  thresh=";
    cout << thresh << endl;
  }

  if( multinsert ) {
    cout << "# Using fractional threshold insert:  thresha="<<alpha;
    cout << endl;
  }

  // initialize Tootle
  if (TootleInit() != TOOTLE_OK)
    return 0;

  ifstream mntns(dataFile, ios::binary);
  HField H(mntns, texFile, greyFile);
  SimplField ter(&H);

  width  = H.getWidth();
  height = H.getHeight();

  if (rasterx > width ) rasterx = width;
  if (rastery > height) rastery = height;
  tilesx = (int)floor((Real)width  / rasterx),
  tilesy = (int)floor((Real)height / rastery);

  while ((Real)width  != (tilesx * rasterx))
    tilesx = (int)floor((Real)width  / --rasterx);
  while ((Real)height != (tilesy * rastery))
    tilesy = (int)floor((Real)height / --rastery);

#ifdef	SPLIT_ON_INJECTION
  /* damit, the heightfield is [0,width), not inclusive */
  for (int h = 0; h <= height; h += rastery)
  for (int w = 0; w <=  width; w += rasterx)
    ter.select_new_point(min(w, width - 1), min(h, height - 1));
#endif

  greedy_insert(ter);
//greedy_insert_error(ter);

#ifdef	MATCH_WITH_HIRES
  readPointsFile(ter);
#endif

  cout << "Writing meshes and textures ..." << endl;

  TransferGeometry(ter);
  TransferGeometry();
  RevisitGeometry();

  if (!nomodel) {
#ifdef	MATCH_WITH_HIRES
    wrtePointsFile(ter);
#endif

    wrteTIN(ter);
    wrteWavefront(ter);
    wrteNIF(ter);
    wrteDXMesh(ter);
  }

#ifdef	SPLIT_ON_INJECTION
  if (writeobjmaps || writefcemaps) {
    if ( writeloresmaps)
      wrteNormals0(writefcemaps, true, true, H);
    if (!writenoresmaps)
      wrteNormals1(writefcemaps, true, true, H);
    if ( writehiresmaps)
      wrteNormals2(writefcemaps, true, true, H);
  }
#else
  if (writeobjmaps || writefcemaps) {
    write_nrm1024(ter);
    if (writehiresmaps)
      write_nrm2047(ter);
  }
#endif

  // clean up tootle
  TootleCleanup();

  // You can output a PostScript version of the mesh by uncommenting the
  // following line.
  //output_ps(ter);

  return 0;
}
