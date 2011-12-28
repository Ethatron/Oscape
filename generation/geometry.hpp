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

#include <vector>
//#include <map>
#include <set>
//#include <list>
//#include <queue>
//#include <string>
//#include <utility>

using namespace std;

/* ---------------------------------------------------- */

class objWater;
class objVertex;
class objFace;

#include "../libmemory.hpp"

extern class MemQueue<4> WPool;
extern class MemQueue<4> VPool;
extern class MemQueue<4> FPool;

typedef	std::vector<class objWater  *, MemAllocator<class objWater  *> >	wvector;
typedef	std::vector<class objVertex *, MemAllocator<class objVertex *> >	vvector;
typedef	std::vector<class objFace   *, MemAllocator<class objFace   *> >	fvector;

//class MemAllocator<objWater  *> WAllc(&WPool);
//class MemAllocator<objVertex *> VAllc(&VPool);
//class MemAllocator<objFace   *> FAllc(&FPool);

/* ---------------------------------------------------- */

class objWater : public MemQueueable {
public:
  objWater() { }

  /* the set's key, can change to whatever */
  Point2d wtx;
  /* the vertex's local object coordinates */
  Point2d op; Real oz;

  Real x, y, z;

  bool ocean;

  bool operator < (const class objWater &other) const
  {
    return memcmp(&this->wtx, &other.wtx, sizeof(Point2d)) < 0;
  }
};

class objVertex : public MemQueueable {
public:
  objVertex() : f(&FPool) { idx = 0; }

  /* the set's key, can change to whatever */
  Point2d vtx;
  /* the vertex's local object coordinates */
  Point2d op; Real oz;

  Real nx, ny, nz;
  Real tx, ty;
  Real x, y, z;

  int idx;

  fvector f;

  bool operator < (const class objVertex &other) const
  {
    return memcmp(&this->vtx, &other.vtx, sizeof(Point2d)) < 0;
#if 0
    return (1024.0 * 1024.0 * dimx *       vtx.y +       vtx.x) <
	   (1024.0 * 1024.0 * dimx * other.vtx.y + other.vtx.x);
#elif 0
    return
      (((unsigned __int64)(((unsigned long int *)(&      vtx.y))[1]) <<  0) ^
       ((unsigned __int64)(((unsigned long int *)(&      vtx.x))[1]) << 32) ^
       ((unsigned __int64)(((unsigned long int *)(&      vtx.y))[0]) <<  0) ^
       ((unsigned __int64)(((unsigned long int *)(&      vtx.x))[0]) << 32)) <
      (((unsigned __int64)(((unsigned long int *)(&other.vtx.y))[1]) <<  0) ^
       ((unsigned __int64)(((unsigned long int *)(&other.vtx.x))[1]) << 32) ^
       ((unsigned __int64)(((unsigned long int *)(&other.vtx.y))[0]) <<  0) ^
       ((unsigned __int64)(((unsigned long int *)(&other.vtx.x))[0]) << 32));
#endif
  }

  void fill();
};

class objFace : public MemQueueable {
public:
  objFace() { narea = 0.0; v[0] = v[1] = v[2] = NULL; }

  Real nx, ny, nz;
  Real narea;

  class objVertex *v[3];

  void fill();
};

/* ---------------------------------------------------- */

struct W {
  bool operator()(const class objWater *s1, const class objWater *s2) const
  {
    return memcmp(&s1->wtx, &s2->wtx, sizeof(Point2d)) < 0;
  }
};

struct V {
  bool operator()(const class objVertex *s1, const class objVertex *s2) const
  {
    return memcmp(&s1->vtx, &s2->vtx, sizeof(Point2d)) < 0;
#if 0
    return (1024.0 * 1024.0 * dimx * s1->vtx.y + s1->vtx.x) <
	   (1024.0 * 1024.0 * dimx * s2->vtx.y + s2->vtx.x);
#elif 0
    return
      (((unsigned __int64)(((unsigned long int *)(&s1->vtx.y))[1]) <<  0) ^
       ((unsigned __int64)(((unsigned long int *)(&s1->vtx.x))[1]) << 32) ^
       ((unsigned __int64)(((unsigned long int *)(&s1->vtx.y))[0]) <<  0) ^
       ((unsigned __int64)(((unsigned long int *)(&s1->vtx.x))[0]) << 32)) <
      (((unsigned __int64)(((unsigned long int *)(&s2->vtx.y))[1]) <<  0) ^
       ((unsigned __int64)(((unsigned long int *)(&s2->vtx.x))[1]) << 32) ^
       ((unsigned __int64)(((unsigned long int *)(&s2->vtx.y))[0]) <<  0) ^
       ((unsigned __int64)(((unsigned long int *)(&s2->vtx.x))[0]) << 32));
#endif
  }
};

/* ---------------------------------------------------- */

extern std::set<class objWater *, struct W> Waters;
extern std::set<class objVertex *, struct V> Vertices;
extern std::vector<class objFace *> Faces;

#ifdef	SPLIT_ON_INJECTION
extern std::set<class objWater *, struct W> SectorWaters[128][128];
extern std::set<class objVertex *, struct V> SectorVertices[128][128];
extern std::vector<class objFace *> SectorFaces[128][128];
#endif

#if 0
#define	SectorVerticeO	SectorVertices
#define	SectorFaceO	SectorFaces

typedef std::set<class objVertex *, struct V>::iterator viterator;
typedef std::vector<class objFace *>::iterator fiterator;
#else
extern std::vector<class objVertex *> SectorVerticeO[128][128];
extern std::vector<class objFace *> SectorFaceO[128][128];
extern std::vector<unsigned int> SectorRemapO;

typedef std::vector<class objWater *>::iterator witerator;
typedef std::vector<class objVertex *>::iterator viterator;
typedef std::vector<class objFace *>::iterator fiterator;
#endif

/* ---------------------------------------------------- */

class SimplField;

void RegisterWater(
  const Point2d& _p, Real lvl, bool o);
void RegisterFace(
  const Point2d& _p1, Real _z1,
  const Point2d& _p2, Real _z2,
  const Point2d& _p3, Real _z3);
void RegisterFace(
  const Point2d& _p1,
  const Point2d& _p2,
  const Point2d& _p3, void *closure);

void CalculateGeometryNormals();

void TransferGeometry(SimplField &ter);
void TransferGeometry();
void RevisitGeometry();
