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

#include "../globals.h"
#include "../scape/simplfield.H"
#include "../generation/geometry.hpp"

#include <windows.h>

#define	NIFLIB_STATIC_LINK
#define NIFLIB_QUOTEDIR(__D)  #__D
//#define NIFLIB_BASEDIR(__D)	NIFLIB_QUOTEDIR(../NIFtools/trunk/niflib/)
#define NIFLIB_BASEDIR(__L)	NIFLIB_QUOTEDIR(../NIFtools (Ethatron)/niflib/__L)

#include NIFLIB_BASEDIR(include/niflib.h)
#include NIFLIB_BASEDIR(include/obj/NiNode.h)
#include NIFLIB_BASEDIR(include/obj/NiTriShape.h)
#include NIFLIB_BASEDIR(include/obj/NiTriShapeData.h)
#include NIFLIB_BASEDIR(include/obj/NiTriStrips.h)
#include NIFLIB_BASEDIR(include/obj/NiTriStripsData.h)

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

/* ---------------------------------------------------- */

void readNIF(const char *pattern) {
  viterator itv;
  fiterator itf;

  // 1k == 32, 3k == 96, 512 == 16 */
  int resx = restx;
  int resy = resty;

  // round down, negative side would be smaller than positive side
  int offx = offtx;
  int offy = offty;

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
    strcat(name, ".nif");

    if (GetFileAttributes(name) != INVALID_FILE_ATTRIBUTES) {
      vector<NiObjectRef> list;
      vector<NiAVObjectRef> chld;
      NiTriShapeRef rootsh;
      NiTriShapeDataRef datash;
      NiTriStripsRef rootst;
      NiTriStripsDataRef datast;
      NifInfo info;

      /* clear first */
      memset(&info, 0, sizeof(info));

      /* read the NIF */
      list = ReadNifList(name, &info);
      if (list.size()) {
	vector<Niflib::Vector3> v;
	vector<Niflib::Triangle> f;
	float s = 1.0f;

	/* find the root */
	rootsh = DynamicCast<NiTriShape>(FindRoot(list));
	if (rootsh) {
	  datash = DynamicCast<NiTriShapeData>(rootsh->GetData());
	  if (datash) {
	    v = datash->GetVertices();
	    f = datash->GetTriangles();
	    s = max(1.0f, rootsh->scale);
	  }
	}

	/* find the root */
	rootst = DynamicCast<NiTriStrips>(FindRoot(list));
	if (rootst) {
	  datast = DynamicCast<NiTriStripsData>(rootst->GetData());
	  if (datast) {
	    v = datast->GetVertices();
	    f = datast->GetTriangles();
	    s = max(1.0f, rootst->scale);
	  }
	}

	if (v.size())
	if (f.size()) {
	  vector<Niflib::Triangle>::iterator itf;
	  for (itf = f.begin(); itf != f.end(); itf++) {
	    int i1 = (*itf).v1; Point2d p1(v[i1].x, v[i1].y); Real z1 = v[i1].z;
	    int i2 = (*itf).v2; Point2d p2(v[i2].x, v[i2].y); Real z2 = v[i2].z;
	    int i3 = (*itf).v3; Point2d p3(v[i3].x, v[i3].y); Real z3 = v[i3].z;

	    /* possibly bad precision, just be safe */
	    p1.x = max(0.0, min(p1.x, 131072.0 / s));
	    p1.y = max(0.0, min(p1.y, 131072.0 / s));

	    p2.x = max(0.0, min(p2.x, 131072.0 / s));
	    p2.y = max(0.0, min(p2.y, 131072.0 / s));

	    p3.x = max(0.0, min(p3.x, 131072.0 / s));
	    p3.y = max(0.0, min(p3.y, 131072.0 / s));

	    p1.x = (p1.x * s) + ((tx /*- offx*/) * rasterx) * sizescale;
	    p1.y = (p1.y * s) + ((ty /*- offy*/) * rastery) * sizescale;
	    z1   = (z1   * s) + 0;

	    p2.x = (p2.x * s) + ((tx /*- offx*/) * rasterx) * sizescale;
	    p2.y = (p2.y * s) + ((ty /*- offy*/) * rasterx) * sizescale;
	    z2   = (z2   * s) + 0;

	    p3.x = (p3.x * s) + ((tx /*- offx*/) * rasterx) * sizescale;
	    p3.y = (p3.y * s) + ((ty /*- offy*/) * rastery) * sizescale;
	    z3   = (z3   * s) + 0;

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

    /* advance progress */
    SetProgress((numty - minty) * (ty - minty) + (tx - mintx) + 1);
  }
  }
}

void readNIF() {
  readNIF("");

#ifdef	SPLIT_ON_INJECTION
  readNIF("%s.%02d.%02d.%02d");
#endif
}

/* ---------------------------------------------------- */

void wrteNIF(const char *name) {
}

void wrteNIF(SimplField& ter, const char *pattern) {
  viterator itv;
  fiterator itf;

  // 1k == 32, 3k == 96, 512 == 16 */
  int resx = restx;
  int resy = resty;
  // round down, negative side would be smaller than positive side
  int offx = offtx;
  int offy = offty;

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
	root->SetLocalScale(1.0f);
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
	  node->SetLocalScale(1.0f);
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
