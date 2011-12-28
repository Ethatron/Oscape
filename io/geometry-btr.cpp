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
#include NIFLIB_BASEDIR(include/obj/BSMultiBoundNode.h)
#include NIFLIB_BASEDIR(include/obj/BSMultiBound.h)
#include NIFLIB_BASEDIR(include/obj/BSMultiBoundAABB.h)
#include NIFLIB_BASEDIR(include/obj/BSLightingShaderProperty.h)
#include NIFLIB_BASEDIR(include/obj/BSShaderTextureSet.h)

using namespace Niflib;

NiObjectRef FindRoot(vector<NiObjectRef> const & objects);/* {
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
}*/

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

void readBTR(const char *pattern) {
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

    if (GetFileAttributes(name) != INVALID_FILE_ATTRIBUTES) {
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
		float s = max(1.0f, land->scale);

		vector<Niflib::Triangle>::iterator itf;
		for (itf = f.begin(); itf != f.end(); itf++) {
		  int i1 = (*itf).v1; Point2d p1(v[i1].x, v[i1].y); Real z1 = v[i1].z;
		  int i2 = (*itf).v2; Point2d p2(v[i2].x, v[i2].y); Real z2 = v[i2].z;
		  int i3 = (*itf).v3; Point2d p3(v[i3].x, v[i3].y); Real z3 = v[i3].z;

		  /* bad precision, man, who wrote that tool? */
		  p1.x = max(0.0, min(p1.x, 131072.0 / s));
		  p1.y = max(0.0, min(p1.y, 131072.0 / s));

		  p2.x = max(0.0, min(p2.x, 131072.0 / s));
		  p2.y = max(0.0, min(p2.y, 131072.0 / s));

		  p3.x = max(0.0, min(p3.x, 131072.0 / s));
		  p3.y = max(0.0, min(p3.y, 131072.0 / s));

		  p1.x = (p1.x * s) + ((tx /*- offx*/) * rasterx) * sizescale;
		  p1.y = (p1.y * s) + ((ty /*- offy*/) * rastery) * sizescale;
		  z1   = (z1   * s) + 14000;

		  p2.x = (p2.x * s) + ((tx /*- offx*/) * rasterx) * sizescale;
		  p2.y = (p2.y * s) + ((ty /*- offy*/) * rasterx) * sizescale;
		  z2   = (z2   * s) + 14000;

		  p3.x = (p3.x * s) + ((tx /*- offx*/) * rasterx) * sizescale;
		  p3.y = (p3.y * s) + ((ty /*- offy*/) * rastery) * sizescale;
		  z3   = (z3   * s) + 14000;

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

void readBTR() {
  readBTR("");

#ifdef	SPLIT_ON_INJECTION
  readBTR("%s.%02d.%02d.%02d");
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
	lght->emissiveSaturation = 1.0f;
	lght->emissiveColor = Color3(0.0f, 0.0f, 0.0f);
	lght->transparency = 1.0f;
	lght->glossiness = 1.0f;
	lght->specularStrength = 1.0f;
	lght->specularColor = Color3(1.0f, 1.0f, 1.0f);

	txst->SetTexture(0, textc); // color
	txst->SetTexture(1, textn); // normal
//	txst->SetTexture(2, "");
//	txst->SetTexture(3, "");
//	txst->SetTexture(4, "");
//	txst->SetTexture(5, "");
//	txst->SetTexture(6, "");
//	txst->SetTexture(7, "");
//	txst->SetTexture(8, "");

	/* no interfaces */
	mbnd->data = aabb;
	root->multiBound = mbnd;
	root->SetFlags(14);
	root->unknownShort1 = 0;
	root->unknownInt = 1;

	root->SetName("chunk");
	root->AddChild((NiAVObjectRef)land);
	ldta->SetConsistencyFlags(CT_STATIC);
//	ldta->numUvMask = (min(resx, resy) < 32 ? 0x0000 : 0x1000); // what the fuck?
	ldta->numUvMask = (min(resx, resy) < 32 ? 0x0000 : 0x0000);
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
	  woot->SetFlags(14);
	  woot->unknownShort1 = 0;
	  woot->unknownInt = 1;

	  root->AddChild((NiAVObjectRef)woot);
//	  woot->SetName("chunk");
	  woot->SetName("water");
	  woot->AddChild((NiAVObjectRef)watr);
	  wdta->SetConsistencyFlags(CT_MUTABLE);
	  wdta->numUvMask = (min(resx, resy) < 32 ? 0x0000 : 0x0000);
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
