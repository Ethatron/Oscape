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

int width, height;
int tilesx, tilesy;

__int64 writechk = 0;
int wdspace = 60;
char wdsname[256] = "tamriel";
int wchgame = 0;
char *wpattern = "%02d.%02d.%02d.%02d";

unsigned short basinshift  = (8192 - 512) / (2 * 4);
Real heightscale =    2.0f *    4.0f/*Annwyn*/;
Real heightshift = 8192.0f -    0.0f/*Annwyn*/;
Real heightadjust1 = 4.0f / (4.0f)/*Oblivion*/;
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
      char nbase[256], name[256];

      /* codification is:
       * "worldspace.tilex*32.tiley*32.32"
       * worldspace == 60 == Tamriel
       */
      /**/ if (wchgame == 0)	// Oblivion
        sprintf(nbase, walk->data(), wdspace, coordx, coordy, reso);
      else if (wchgame == 1)	// Skyrim
        sprintf(nbase, walk->data(), wdsname, reso, coordx, coordy);

      /* lower-case */
      strlwr(nbase);

      if (writeobj) {
	strcpy(name, nbase); strcat(name, ".obj");
	skip = skip && !IsOlder(name, writechk);
      }

      if (writenif) {
	strcpy(name, nbase); strcat(name, ".nif");
	skip = skip && !IsOlder(name, writechk);
      }

      if (writedx9) {
	strcpy(name, nbase); strcat(name, ".x");
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

#ifdef	MATCH_WITH_HIRES
void readPointsFile(SimplField& ter, const char *name) {
  int width  = ter.getHField()->getWidth();
  int height = ter.getHField()->getHeight();

  FILE *pts = fopen(name, "rb");
  if (!pts)
    return;

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
  int x, y, bv = 0, cnt = 0;
  while (fgets(buf, 256, pts) != NULL) {
//while (fscanf(pts, "%d %d\n", &x, &y) == 2) {
    /* comments / sections allowed */
    if ((buf[0] != '\0') &&
        (buf[0] != ';') &&
        (buf[0] != '[') &&
        (buf[0] != '#')) {
      /* can we also interprete it? */
      if (sscanf(buf, "%d %d\n", &x, &y) == 2) {
	x = (int)floor((Real)x / sizescale); x = min(x, width  - 1);
	y = (int)floor((Real)y / sizescale); y = min(y, height - 1);

	logrf("%d/%d: ", bv, cnt);
//	bv += ter.select_new_point(x, y) ? 1 : 0;
	bv += ter.select_fix_point(x, y) ? 1 : 0;
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

  int gw = ter.getHField()->getWidth();
  int gh = ter.getHField()->getHeight();
  int bv = 0;

  set<class objVertex *, struct V>::const_iterator itv;
  for (itv = Vertices.begin(); itv != Vertices.end(); itv++) {
    class objVertex *vo = (*itv);

    Real sx = floor((1.0f / rasterx) * vo->vtx.x) * rasterx;
    Real sy = floor((1.0f / rastery) * vo->vtx.y) * rastery;

    /* collect tile-border vertices */
    if ((vo->vtx.x == sx) ||
	(vo->vtx.y == sy) ||
	(vo->vtx.x == gw) ||
	(vo->vtx.y == gh)) {
      bv++;

      fprintf(pts, "%d %d\n", (int)(vo->x), (int)(vo->y));
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

    char name[256];

    /**/ if (wchgame == 0)	// Oblivion
      sprintf(name, pattern, wdspace, coordx, coordy, min(resx, resy));
    else if (wchgame == 1)	// Skyrim
      sprintf(name, pattern, wdsname, min(resx, resy), coordx, coordy);

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

using namespace Niflib;

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

    char name[256];

    /**/ if (wchgame == 0)	// Oblivion
      sprintf(name, pattern, wdspace, coordx, coordy, min(resx, resy));
    else if (wchgame == 1)	// Skyrim
      sprintf(name, pattern, wdsname, min(resx, resy), coordx, coordy);

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

    char name[256];

    /**/ if (wchgame == 0)	// Oblivion
      sprintf(name, pattern, wdspace, coordx, coordy, min(resx, resy));
    else if (wchgame == 1)	// Skyrim
      sprintf(name, pattern, wdsname, min(resx, resy), coordx, coordy);

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

    if (writeobj)
      wrteWavefront(ter, pattern);
    if (writenif)
      wrteNIF(ter, pattern);
    if (writedx9)
      wrteDXMesh(ter, pattern);
  }
}

/* ---------------------------------------------------- */

bool skipTexture(const char *pattern, const char *pfx, int coordx, int coordy, int reso, bool xyz) {
  bool skip = true;

  if (writeppm || writepng || writedds) {
    char nbase[256], name[256];

    /**/ if (wchgame == 0)	// Oblivion
      sprintf(nbase, pattern, wdspace, coordx, coordy, reso);
    else if (wchgame == 1)	// Skyrim
      sprintf(nbase, pattern, wdsname, reso, coordx, coordy);

    /* lower-case */
    strlwr(nbase);

    if (writeppm) {
      strcpy(name, nbase); strcat(name, pfx); strcat(name, ".ppm");
      skip = skip && !IsOlder(name, writechk);
    }

    if (writepng) {
      strcpy(name, nbase); strcat(name, pfx); strcat(name, ".png");
      skip = skip && !IsOlder(name, writechk);
    }

    if (writedds) {
      strcpy(name, nbase); strcat(name, pfx); strcat(name, ".dds");
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
    char nbase[256], name[256];

    /**/ if (wchgame == 0)	// Oblivion
      sprintf(nbase, pattern, wdspace, coordx, coordy, reso);
    else if (wchgame == 1)	// Skyrim
      sprintf(nbase, pattern, wdsname, reso, coordx, coordy);

    /* lower-case */
    strlwr(nbase);

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
      for (UINT32 s = 0; s < texd.Height; s++) {
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
      strcpy(name, nbase); strcat(name, pfx); strcat(name, ".ppm");

      if (IsOlder(name, writechk)) {
	res = D3DXSaveTextureToFile(name, D3DXIFF_PPM, tex, NULL);
	if (res != D3D_OK) throw runtime_error("Failed to write texture");
      }
    }

    if (writepng) {
      strcpy(name, nbase); strcat(name, pfx); strcat(name, ".png");

      if (IsOlder(name, writechk)) {
	res = D3DXSaveTextureToFile(name, D3DXIFF_PNG, tex, NULL);
	if (res != D3D_OK) throw runtime_error("Failed to write texture");
      }
    }

    if (writedds) {
      strcpy(name, nbase); strcat(name, pfx); strcat(name, ".dds");

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
