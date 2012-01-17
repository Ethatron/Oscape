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

#define CBASH
#ifdef	CBASH

#define	_CRT_SECURE_NO_WARNINGS
#define	_CRT_NONSTDC_NO_DEPRECATE

#include <string>

using namespace std;

#include <windows.h>
#include <CBash.h>

#include "../globals.h"

#pragma comment(lib,"CBash")

#ifdef	COMPILING_STATIC
#if   _MSC_VER == 1500
#define LIBDIR	"vc9"
#elif _MSC_VER == 1600
#define LIBDIR	"vc10"
#endif

#ifndef	NDEBUG
#ifdef	_WIN64
#pragma comment(lib,"../../../NIFopt/io/zlib-1.2.3/" LIBDIR "/x64/Debug/libz")
#else
#pragma comment(lib,"../../../NIFopt/io/zlib-1.2.3/" LIBDIR "/Debug/libz")
#endif
#else
#ifdef	_WIN64
#pragma comment(lib,"../../../NIFopt/io/zlib-1.2.3/" LIBDIR "/x64/Release/libz")
#else
#pragma comment(lib,"../../../NIFopt/io/zlib-1.2.3/" LIBDIR "/Release/libz")
#endif
#endif
#else
RecordOp::RecordOp() : count(0), stop(false), result(false) {}
RecordOp::~RecordOp() {}
bool RecordOp::Accept(Record *&curRecord) { return false; }
UINT32 RecordOp::GetCount() { return count; }
void RecordOp::ResetCount() { count = 0; }
bool RecordOp::Stop() { return stop; }
bool RecordOp::GetResult() { return result; }
#endif

/* ---------------------------------------------------------------------------- */

#define	__USE_MISC
#include <random.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "random_r.c"
#include "random.c"

#ifdef __cplusplus
};
#endif

/* ---------------------------------------------------------------------------- */

#define skiphashcheck	true
#define verbose		false
#define optimizequick	false
#define processhidden	false

size_t processedinbytes;
size_t compressedinbytes;
size_t compresseddtbytes;
size_t compressedoubytes;
size_t virtualbsabytes;
size_t virtualbsafiles;

char *stristr(const char *s, const char *find);

#include <d3d9.h>
#include <d3dx9.h>

extern IDirect3D9 *pD3D;
extern IDirect3DDevice9 *pD3DDevice;

#include "io.C"

string wedata;
long weoffs = 0;
map<string, struct iodir *> websas;
map<string, LPDIRECT3DTEXTURE9> wetxts;

void TextureDBExit() {
  {
    /* release all open BSAs */
    map<string, struct iodir *>::iterator walk = websas.begin();
    while (walk != websas.end()) {
      if (walk->second)
	ioclosedir(walk->second);
      walk++;
    }
  }

  {
    /* release all loaded textures */
    map<string, LPDIRECT3DTEXTURE9>::iterator walk = wetxts.begin();
    while (walk != wetxts.end()) {
      if (walk->second)
	walk->second->Release();
      walk++;
    }
  }

  /* reset */
  websas.clear();
  wetxts.clear();
}

void TextureDBInit(const char *name = NULL, int lvl = 0) {
  if (!name) TextureDBExit(), name = wedata.data();
  size_t ilen = (name ? strlen(name) : 0);

  struct ioinfo info;
  if (!iostat(name, &info)) {
    /* input: directory */
    if (info.io_type & IO_DIRECTORY) {
      /* search only directly in the data-folder */
      bool isbsa = isext(name, "bsa");
      if ((lvl > 0) && !isbsa)
	return;

      const char *dle;

      /* strip trailing slash */
      if (!(dle = strrchr(name, '/')))
	if (!(dle = strrchr(name, '\\')))
	  dle = name - 1;

      dle += 1;

      /* update status */
      if (isbsa) {
	char tmp[256];
	sprintf(tmp, "Adding \"%s\" to search-db ...", dle);
	SetStatus(tmp);
      }

      /* walk */
      struct iodir *dir;
      if ((dir = ioopendir(name))) {
	struct iodirent *et;

	while ((et = ioreaddir(dir))) {
	  if (!strcmp(et->name, ".") ||
	      !strcmp(et->name, ".."))
	    continue;

	  char *nname = (char *)malloc(ilen + 1 + et->namelength + 1);

	  strcpy(nname, name);
	  strcat(nname, "\\");
	  strcat(nname, et->name);

	  /* recurse */
	  TextureDBInit(nname, lvl + 1);

	  free(nname);
	}

	if (!isbsa)
	  ioclosedir(dir);
	else
	  websas[name] = dir;
      }
    }
    /* input: file */
    else {
    }
  }

  if (!lvl)
    SetStatus("Done building search-db");
}

LPDIRECT3DTEXTURE9 TextureSearch(const char *name) {
  char fname[260];
  bool f = false;

  /* direct hit found */
  if (wetxts[name])
    return wetxts[name];

  /* update status */
  sprintf(fname, "Searching \"%s\" in db ...", name);
  SetStatus(fname);

  /* search in the directory */
  strcpy(fname, wedata.data());
  strcat(fname, "\\Textures\\");
  strcat(fname, name);

  struct ioinfo info;
  if (!iostat(fname, &info)) {
    f = true;
  }
  else {
    /* then search in all of the BSAs */
    map<string, struct iodir *>::iterator walk = websas.begin();
    while (walk != websas.end()) {
      strcpy(fname, walk->first.data());
      strcat(fname, "\\Textures\\");
      strcat(fname, name);

      if (!iostat(fname, &info)) {
	f = true;
	break;
      }

      walk++;
    }
  }

  /* update status */
  SetStatus("Extracting ...");

  /* did we find it? */
  if (!f)
    return NULL;

  /* read the DDS */
  void *fmem = NULL;
  UINT fsize = 0;

  /* read in the file-contents */
  struct iofile *file;
  if ((file = ioopenfile(fname, "rb"))) {
    fmem = malloc(fsize = (UINT)info.io_size);

    UINT rdsze = (UINT)
    ioreadfile(fmem, fsize, file);
    ioclosefile(file);

    if (rdsze != fsize) {
      return NULL;
    }
  }
  else {
    return NULL;
  }

  /* load memory into the texture */
  LPDIRECT3DTEXTURE9 base = NULL;
  HRESULT res;
  if ((res = D3DXCreateTextureFromFileInMemoryEx(
    pD3DDevice, fmem, fsize,
    D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
    0, D3DFMT_A8R8G8B8/*D3DFMT_UNKNOWN*/, D3DPOOL_MANAGED, D3DX_FILTER_NONE/*D3DX_DEFAULT*/,
    D3DX_FILTER_NONE/*D3DX_DEFAULT*/, 0, NULL, NULL,
    &base
  )) != D3D_OK)
    base = NULL;

  free(fmem);

  /* register, even if NULL */
  return (wetxts[string(name)] = base);
}

/* ############################################################################ */

#define	SUPERSAMPLE_NORMAL	1
#define	SUPERSAMPLE_HEIGHT	1
#define	SUPERSAMPLE_FEATURE	1
#define	SUPERSAMPLE_COLOR	4
#define	SUPERSAMPLE_LIMIT	4

#define	RESAMPLE_NORMAL		1
#define	RESAMPLE_HEIGHT		1
#define	RESAMPLE_FEATURE	1
#define	RESAMPLE_COLOR		1

#define	UPSAMPLE_NORMAL		(SUPERSAMPLE_NORMAL  / RESAMPLE_NORMAL )
#define	UPSAMPLE_HEIGHT		(SUPERSAMPLE_HEIGHT  / RESAMPLE_HEIGHT )
#define	UPSAMPLE_FEATURE	(SUPERSAMPLE_FEATURE / RESAMPLE_FEATURE)
#define	UPSAMPLE_COLOR		(SUPERSAMPLE_COLOR   / RESAMPLE_COLOR  )

int texsample = 1;
int srfsample = 1;

string weoutn; bool calcn = false;
string weouth; bool calch = false;
string weoutw; bool calcw = false;
string weoutx; bool calcx = false;
string weoutm; bool calcm = false;
string wename; int weid;

/* flip y */
#define rofx  0
#define rofy  1			// why, oh, why?
//efine lofx  (     -1)		// first column is previous cell
//efine lofy  (-32 - 1)		// first row is previous cell, y is inverted
#define lofx  (     -0)		// last column is next cell
#define lofy  (-32 - 0)		// last row is next cell, y is inverted

long wleftx =  1024 * 1024;
long wrghtx = -1024 * 1024;
long wtopy  =  1024 * 1024;
long wboty  = -1024 * 1024;
long offsx  = 0;
long offsy  = 0;
unsigned long sizex  = 0;
unsigned long sizey  = 0;
long cells  = 0;
long dells  = 0;

Collection *col;

/* ---------------------------------------------------------------------------- */

template<
  class WRLDRecord = Ob::WRLDRecord,
  class CELLRecord = Ob::CELLRecord,
  class LANDRecord = Ob::LANDRecord,
  class LTEXRecord = Ob::LTEXRecord
>
class ExtendsWorldOp : public RecordOp {
public:
  ExtendsWorldOp() : RecordOp() { }

  virtual bool Accept(Record *&curRecord) {
    WRLDRecord *wrld = (WRLDRecord *)curRecord;
    CELLRecord *cell;
    LANDRecord *land;

    if (!stricmp(wrld->EDID.value, wename.data())) {
      std::vector<Record *>::iterator walk = wrld->CELLS.begin();
      while (walk != wrld->CELLS.end()) {
	cell = (CELLRecord *)(*walk);
	land = (LANDRecord *)cell->LAND;

	/* determine extents */
	if (cell->XCLC.IsLoaded() && land) {
	  cells++;

	  if (wleftx > (cell->XCLC->posX + rofx))
	    wleftx = (cell->XCLC->posX + rofx);
	  if (wrghtx < (cell->XCLC->posX + rofx))
	    wrghtx = (cell->XCLC->posX + rofx);

	  if (wtopy > (cell->XCLC->posY + rofy))
	    wtopy = (cell->XCLC->posY + rofy);
	  if (wboty < (cell->XCLC->posY + rofy))
	    wboty = (cell->XCLC->posY + rofy);
	}

	/* next */
	walk++;
      }
    }

    return false;
  }
};

/* ---------------------------------------------------------------------------- */

/* global so it's much faster to search */
struct landtex {
//Ob::LTEXRecord *rec;
  union {
    void *rec;

    Ob::LTEXRecord *reco;
    Sk::LTEXRecord *recs;
  };

  LPDIRECT3DTEXTURE9 tex;
  unsigned long mem1x1[1*1+1];
  unsigned long mem2x2[2*2+1];
  unsigned long mem4x4[4*4+1];
  unsigned char classification;
};

#define BLEND_NONGAMMA
const float gamma_in  = 2.2f / 1.0f;
const float gamma_out = 1.0f / 2.2f;
float raisecontrast = 1.0f;
float upperbrightness = 1.0f;

map<FORMID, struct landtex> ltex;

/* ............................................................................ */

BOOL CreateDirectoryRecursive(const char *pathname);
void DumpTexture(const char *name, void *mem, int len) {
#if 0
  string out = weoutx << "\\" << name;
  string out = weoutx << "\\" << name;

  /* TODO: get me texture-dumps */
  if (CreateDirectoryRecursive(pathname)) {
  }
#endif
}

/* ............................................................................ */

template<
  class LTEXRecord /*= Ob::LTEXRecord*/
>
const char *GetTextureName(LTEXRecord *rec);

template<>
const char *GetTextureName<Ob::LTEXRecord>(Ob::LTEXRecord *rec) {
  static char tmp[256];
  if (rec) {
    /* entry does not include "Landscape" */
    strcpy(tmp, "Landscape\\");
    strcat(tmp, rec->ICON.value);

    return tmp;
  }

  return "Landscape\\default.dds";
}

template<>
const char *GetTextureName<Sk::LTEXRecord>(Sk::LTEXRecord *rec) {
  if (rec) {
    /* find the record */
    ModFile *WinningModFile;
    Record *WinningRecord;

    col->LookupWinningRecord(rec->TNAM.value, WinningModFile, WinningRecord, false);
    if (!WinningRecord)
      return NULL;

    /* entry includes "Landscape" */
    return ((Sk::TXSTRecord *)WinningRecord)->TX00.value;
  }

  /* heuristic */
  return "Landscape\\dirt02.dds";
}

/* ............................................................................ */

/* infinite filter size, always map from the highest resolution available */
#define FILTER_MIPMAP 0

template<
  class LTEXRecord /*= Ob::LTEXRecord*/
>
const char *LookupName(FORMID t) {
  struct landtex *ltex = &::ltex[t];

  /* what does a texture of 0 mean? (assume default.dds) */
  if (t && !ltex->rec) {
    /* find the record */
    ModFile *WinningModFile;
    Record *WinningRecord;

    col->LookupWinningRecord(t, WinningModFile, WinningRecord, false);
    if (!WinningRecord)
      return NULL;
    ltex->rec = (LTEXRecord *)WinningRecord;
  }

  LTEXRecord *rec;
  if (t && (rec = (LTEXRecord *)ltex->rec))
    return GetTextureName(rec);
  else
    return GetTextureName((LTEXRecord *)NULL);
}

template<
  class LTEXRecord /*= Ob::LTEXRecord*/
>
unsigned long *LookupTexture(FORMID t, int supersample, int supersize) {
  struct landtex *ltex = &::ltex[t];

  /* what does a texture of 0 mean? (assume default.dds) */
  if (t && !ltex->rec) {
    /* find the record */
    ModFile *WinningModFile;
    Record *WinningRecord;

    col->LookupWinningRecord(t, WinningModFile, WinningRecord, false);
    if (!WinningRecord)
      return NULL;
    ltex->rec = (LTEXRecord *)WinningRecord;
  }

  if (!ltex->tex) {
    /* read in the color */
    const char *name = NULL;
    LPDIRECT3DTEXTURE9 tex = NULL;
    LTEXRecord *rec;

    if (t && (rec = (LTEXRecord *)ltex->rec))
      name = GetTextureName(rec);
    else
      name = GetTextureName((LTEXRecord *)NULL);

    if (!name)
      return NULL;
    if (!(ltex->tex = tex = TextureSearch(name)))
      return NULL;

    DWORD low;
    D3DSURFACE_DESC texo;
    D3DLOCKED_RECT texs;
    ULONG *sTex;
    float r, g, b, a;

    /* 1x1 ------------------------------------------------------- */
    if (supersample >= 1) {
      low = min(max(tex->GetLevelCount(), 1) - 1, FILTER_MIPMAP);

      tex->GetLevelDesc(low, &texo);
      tex->LockRect(low, &texs, NULL, 0);
      sTex = (ULONG *)texs.pBits;

      r = 0, g = 0, b = 0, a = 0;
      for (int y = 0; y < (int)texo.Height; y += 1)
      for (int x = 0; x < (int)texo.Width ; x += 1) {
	ULONG t = sTex[(y * texo.Width) + x];

	/* gamma corrected */
	r += powf((float)((t >> 16) & 0xFF) / 255.0f, gamma_in);
	g += powf((float)((t >>  8) & 0xFF) / 255.0f, gamma_in);
	b += powf((float)((t >>  0) & 0xFF) / 255.0f, gamma_in);
	a +=      (float)((t >> 24) & 0xFF) / 255.0f	       ;
      }

      /* gamma corrected */
      r /= (texo.Width * texo.Height);
      g /= (texo.Width * texo.Height);
      b /= (texo.Width * texo.Height);
      a /= (texo.Width * texo.Height);

#ifndef	BLEND_NONGAMMA
      /* adjustment */
      r /= upperbrightness;
      g /= upperbrightness;
      b /= upperbrightness;
      a /= 1.0f           ;

      /* gamma corrected */
      r = powf(r, gamma_out);
      g = powf(g, gamma_out);
      b = powf(b, gamma_out);
      a =      a            ;
#endif

#define	normalize(c, pos)  (min((unsigned long int)floor((c * 255.0f) + 0.5), 0xFF) << pos)
      ltex->mem1x1[0] =
	normalize(r, 24) |
	normalize(g, 16) |
	normalize(b,  8) |
	normalize(a,  0);
#undef	normalize

      tex->UnlockRect(low);
    }

    /* tile 1x1 into 2x2 */
    for (int m = 0; m < (2 * 2); m += 1)
      ltex->mem2x2[m] = ltex->mem1x1[0];

    /* 2x2 ------------------------------------------------------- */
    if (supersample >= 2) {
      low = min(max(tex->GetLevelCount(), 2) - 2, FILTER_MIPMAP);

      tex->GetLevelDesc(low, &texo);
      tex->LockRect(low, &texs, NULL, 0);
      sTex = (ULONG *)texs.pBits;

      for (int m = 0; m < (2 * 2); m += 1) {
	r = 0, g = 0, b = 0, a = 0;

	int by = (!(m & 2) ? 0 : (int)texo.Height / 2);
	int bx = (!(m & 1) ? 0 : (int)texo.Width  / 2);

	int my = (!(m & 2) ? (int)texo.Height / 2 : (int)texo.Height);
	int mx = (!(m & 1) ? (int)texo.Width  / 2 : (int)texo.Width );

	for (int y = by; y < my; y += 1)
	for (int x = bx; x < mx; x += 1) {
	  ULONG t = sTex[(y * texo.Width) + x];

	  /* gamma corrected */
	  r += powf((float)((t >> 16) & 0xFF) / 255.0f, gamma_in);
	  g += powf((float)((t >>  8) & 0xFF) / 255.0f, gamma_in);
	  b += powf((float)((t >>  0) & 0xFF) / 255.0f, gamma_in);
	  a +=      (float)((t >> 24) & 0xFF) / 255.0f	       ;
	}

	/* gamma corrected */
	r /= ((mx - bx) * (my - by));
	g /= ((mx - bx) * (my - by));
	b /= ((mx - bx) * (my - by));
	a /= ((mx - bx) * (my - by));

#ifndef	BLEND_NONGAMMA
	/* adjustment */
	r /= upperbrightness;
	g /= upperbrightness;
	b /= upperbrightness;
	a /= 1.0f           ;

	/* gamma corrected */
	r = powf(r, gamma_out);
	g = powf(g, gamma_out);
	b = powf(b, gamma_out);
	a =      a            ;
#endif

#define	normalize(c, pos)  (min((unsigned long int)floor((c * 255.0f) + 0.5), 0xFF) << pos)
	ltex->mem2x2[m] =
	  normalize(r, 24) |
	  normalize(g, 16) |
	  normalize(b,  8) |
	  normalize(a,  0);
#undef	normalize
      }

      tex->UnlockRect(low);
    }

    /* tile 2x2 into 4x4 */
    for (int m = 0; m < 2; m += 1) {
      ltex->mem4x4[8 + m * 4 + 0] = ltex->mem4x4[m * 4 + 0] = ltex->mem2x2[m * 2 + 0];
      ltex->mem4x4[8 + m * 4 + 1] = ltex->mem4x4[m * 4 + 1] = ltex->mem2x2[m * 2 + 1];
      ltex->mem4x4[8 + m * 4 + 2] = ltex->mem4x4[m * 4 + 2] = ltex->mem2x2[m * 2 + 0];
      ltex->mem4x4[8 + m * 4 + 3] = ltex->mem4x4[m * 4 + 3] = ltex->mem2x2[m * 2 + 1];
    }

    /* 4x4 ------------------------------------------------------- */
    if (supersample >= 4) {
      low = min(max(tex->GetLevelCount(), 3) - 3, FILTER_MIPMAP);

      tex->GetLevelDesc(low, &texo);
      tex->LockRect(low, &texs, NULL, 0);
      sTex = (ULONG *)texs.pBits;

//    fprintf(stderr, "\ngenerating %s ...\n", name);

      for (int m = 0; m < (4 * 4); m += 1) {
	r = 0, g = 0, b = 0, a = 0;

	int by = (int)(texo.Height * (((m >> 2) & 3) + 0)) / 4;
	int bx = (int)(texo.Width  * (((m >> 0) & 3) + 0)) / 4;

	int my = (int)(texo.Height * (((m >> 2) & 3) + 1)) / 4;
	int mx = (int)(texo.Width  * (((m >> 0) & 3) + 1)) / 4;

//	fprintf(stderr, " rectangle %2d: [%4d,%4d]->[%4d,%4d]=>[%d,%4d] ...", m, bx, by, mx, my, mx - bx, my - by);

	for (int y = by; y < my; y += 1)
	for (int x = bx; x < mx; x += 1) {
	  ULONG t = sTex[(y * texo.Width) + x];

	  /* gamma corrected */
	  r += powf((float)((t >> 16) & 0xFF) / 255.0f, gamma_in);
	  g += powf((float)((t >>  8) & 0xFF) / 255.0f, gamma_in);
	  b += powf((float)((t >>  0) & 0xFF) / 255.0f, gamma_in);
	  a +=      (float)((t >> 24) & 0xFF) / 255.0f	       ;
	}

	/* gamma corrected */
	r /= ((mx - bx) * (my - by));
	g /= ((mx - bx) * (my - by));
	b /= ((mx - bx) * (my - by));
	a /= ((mx - bx) * (my - by));

#ifndef	BLEND_NONGAMMA
	/* adjustment */
	r /= upperbrightness;
	g /= upperbrightness;
	b /= upperbrightness;
	a /= 1.0f           ;

	/* gamma corrected */
	r = powf(r, gamma_out);
	g = powf(g, gamma_out);
	b = powf(b, gamma_out);
	a =      a            ;
#endif

//	fprintf(stderr, " %.3f %.3f %.3f %.3f\n", r, g, b, a);

#define	normalize(c, pos)  (min((unsigned long int)floor((c * 255.0f) + 0.5), 0xFF) << pos)
	ltex->mem4x4[m] =
	  normalize(r, 24) |
	  normalize(g, 16) |
	  normalize(b,  8) |
	  normalize(a,  0);
#undef	normalize
      }

      tex->UnlockRect(low);
    }

    /* total average is the last value */
    ltex->mem1x1[ 1] = ltex->mem1x1[0];
    ltex->mem2x2[ 4] = ltex->mem1x1[0];
    ltex->mem4x4[16] = ltex->mem1x1[0];

    /* debugging */
    DumpTexture(name, ltex->mem1x1, 1 * 1);
    DumpTexture(name, ltex->mem2x2, 2 * 2);
    DumpTexture(name, ltex->mem4x4, 4 * 4);
  }

  if (supersize == 1)
    return ltex->mem1x1;
  if (supersize == 2)
    return ltex->mem2x2;
  if (supersize == 4)
    return ltex->mem4x4;

  return NULL;
}

template<
  class LTEXRecord /*= Ob::LTEXRecord*/
>
unsigned char ClassifyTexture(FORMID t) {
  struct landtex *ltex = &::ltex[t];

  /* what does a texture of 0 mean? (assume default.dds) */
  if (t && !ltex->rec) {
    /* find the record */
    ModFile *WinningModFile;
    Record *WinningRecord;

    col->LookupWinningRecord(t, WinningModFile, WinningRecord, false);
    if (!WinningRecord)
      return NULL;
    ltex->rec = (LTEXRecord *)WinningRecord;
  }

  if (!ltex->classification) {
    /* read in the name */
    const char *name = NULL;
    LTEXRecord *rec;

    if (t && (rec = (LTEXRecord *)ltex->rec))
      name = GetTextureName(rec);
    else
      name = GetTextureName((LTEXRecord *)NULL);

    if (!name)
      return 0;

    /* neutral */
    unsigned char cls = 128;

    /* 1 is least solid nature, 255 is most solid man-made */
    /**/ if (stristr(name, "terrainskstoneground"))
      cls = 255;

    else if (stristr(name, "default" )) cls = 1;
    else if (stristr(name, "ocean"   )) cls = 1;
    else if (stristr(name, "beach"   )) cls = 1;
    else if (stristr(name, "sand"    )) cls = 1;
    else if (stristr(name, "mud"     )) cls = 1;
    else if (stristr(name, "river"   )) cls = 64;
    else if (stristr(name, "grass"   )) cls = 128;
    else if (stristr(name, "rock"    )) cls = 140;
    else if (stristr(name, "mountain")) cls = 140;
    else if (stristr(name, "snow"    )) cls = 192;
    else if (stristr(name, "cobble"  )) cls = 255;
    else if (stristr(name, "road"    )) cls = 255;
    else if (stristr(name, "bridge"  )) cls = 255;
    else if (stristr(name, "street"  )) cls = 255;
    else if (stristr(name, "path"    )) cls = 255;

    ltex->classification = cls;
  }

  return ltex->classification;
}

class WindowedWorldOp : public RecordOp {
  /* the window the memory represents */
  long begin, range;

public:
  WindowedWorldOp(long b, long r) : RecordOp() {
    begin = b;
    range = r;

    /* don't straddle cells */
    assert((begin % 32) == 0);
    assert((range % 32) == 0);
  }

  /* cell range check (relative to memory) */
  inline bool cchk(int cellx, int celly) {
    celly -= begin;

//  if ((cellx < 0) || (cellx >= sizex))
//    return false;
    if ((celly < 0) || (celly >= range))
      return false;

    return true;
  }

  /* real range check (relative to memory) */
  inline bool rchk(int realx, int realy, int su = 1) {
    /* translate local "begin" into up-sampled "begin" */
    realy -= begin * su;

    if ((realx < 0) || (realx >= (sizex * su)))
      return false;
    if ((realy < 0) || (realy >= (range * su)))
      return false;

    return true;
  }

//#define rpos(realx, realy) ((/*(sizey - 1) -*/ realy) * sizex) + (realx)
  /* real position (relative to memory) */
  inline unsigned long rpos(int realx, int realy, int su = 1) {
    /* translate local "begin" into up-sampled "begin" */
    realy -= begin * su;

    return ((realy) * (sizex * su)) + (realx);
  }
};

/* ---------------------------------------------------------------------------- */

template<
  class WRLDRecord = Ob::WRLDRecord,
  class CELLRecord = Ob::CELLRecord,
  class LANDRecord = Ob::LANDRecord,
  class LTEXRecord = Ob::LTEXRecord
>
class ExtractNWorldOp : public WindowedWorldOp {
  unsigned char *nm;

public:
  ExtractNWorldOp(void *mnm, long b, long r) : WindowedWorldOp(b, r) {
    nm = (unsigned char *)mnm;
  }

  virtual bool Accept(Record *&curRecord) {
    WRLDRecord *wrld = (WRLDRecord *)curRecord;
    CELLRecord *cell;
    LANDRecord *land;

    if (!stricmp(wrld->EDID.value, wename.data())) {
      std::vector<Record *>::iterator walk = wrld->CELLS.begin();
      while (walk != wrld->CELLS.end()) {
	cell = (CELLRecord *)(*walk);
	land = (LANDRecord *)cell->LAND;

	/* make sure it's winning */
	if (cell->XCLC.IsLoaded() && land && !land->IsWinningDetermined()) {
	  ModFile *WinningModFile;
	  Record *WinningRecord;

	  col->LookupWinningRecord(land->formID, WinningModFile, WinningRecord, false);
	}
/*
	if (cell->XCLC.IsLoaded() && land && !land->IsWinning()) {
	  printf("skipping");
	}
*/
	if (cell->XCLC.IsLoaded() && land && land->IsWinning()) {
	  long leftx = (cell->XCLC->posX + rofx);
	  long topy  = (cell->XCLC->posY + rofy);
	  long leftc = (offsx + leftx) * 32;
	  long topc  = (offsy + topy ) * 32;

	  /* check cell-center against range, no problems with the overlap that way */
	  if (!cchk(leftc + 16 + lofx, topc + 16 + lofy)) {
	    /* next */ walk++; continue; }

	  SetTopic("Extracting normals from cell {%d,%d}", leftx, topy);
	  SetProgress(dells++);

	  if (land->VNML.IsLoaded()) {
	    for (int y = 0; y <= 32; y ++)
	    for (int x = 0; x <= 32; x ++) {
	      /* AFAIK cells are centered on their coordinates [-16,16] */
	      int realx = leftc + x + lofx;
	      int realy = topc  + y + lofy;

	      if (rchk(realx, realy)) {
		unsigned long realpos = rpos(realx, realy);

		nm[realpos * 3 + 0] = (unsigned char)land->VNML->VNML[y][x].x;
		nm[realpos * 3 + 1] = (unsigned char)land->VNML->VNML[y][x].y;
		nm[realpos * 3 + 2] = (unsigned char)land->VNML->VNML[y][x].z;
	      }
	    }
	  }
	}

	/* next */
	walk++;
      }
    }

    return false;
  }
};

template<
  class WRLDRecord = Ob::WRLDRecord,
  class CELLRecord = Ob::CELLRecord,
  class LANDRecord = Ob::LANDRecord,
  class LTEXRecord = Ob::LTEXRecord
>
class ExtractHWorldOp : public WindowedWorldOp {
  unsigned short *hf;

public:
  ExtractHWorldOp(void *mhf, long b, long r) : WindowedWorldOp(b, r) {
    hf = (unsigned short *)mhf;
  }

  virtual bool Accept(Record *&curRecord) {
    WRLDRecord *wrld = (WRLDRecord *)curRecord;
    CELLRecord *cell;
    LANDRecord *land;

    if (!stricmp(wrld->EDID.value, wename.data())) {
      std::vector<Record *>::iterator walk = wrld->CELLS.begin();
      while (walk != wrld->CELLS.end()) {
	cell = (CELLRecord *)(*walk);
	land = (LANDRecord *)cell->LAND;

	/* make sure it's winning */
	if (cell->XCLC.IsLoaded() && land && !land->IsWinningDetermined()) {
	  ModFile *WinningModFile;
	  Record *WinningRecord;

	  col->LookupWinningRecord(land->formID, WinningModFile, WinningRecord, false);
	}
/*
	if (cell->XCLC.IsLoaded() && land && !land->IsWinning()) {
	  printf("skipping");
	}
*/
	if (cell->XCLC.IsLoaded() && land && land->IsWinning()) {
	  long leftx = (cell->XCLC->posX + rofx);
	  long topy  = (cell->XCLC->posY + rofy);
	  long leftc = (offsx + leftx) * 32;
	  long topc  = (offsy + topy ) * 32;

	  /* check cell-center against range, no problems with the overlap that way */
	  if (!cchk(leftc + 16 + lofx, topc + 16 + lofy)) {
	    /* next */ walk++; continue; }

	  SetTopic("Extracting heights from cell {%d,%d}", leftx, topy);
	  SetProgress(dells++);

	  if (land->VHGT.IsLoaded()) {
	    for (int y = 0; y <= 32; y ++)
	    for (int x = 0; x <= 32; x ++) {
	      /* AFAIK cells are centered on their coordinates [-16,16] */
	      int realx = leftc + x + lofx;
	      int realy = topc  + y + lofy;

	      if (rchk(realx, realy)) {
		unsigned long realpos = rpos(realx, realy);

#define scale 1.0f  // 8.0f
		/* delta coded heights (for compression) */
		float hv = land->VHGT->offset * scale;
		for (int h = 0; h <= y; ++h)
		  hv += (land->VHGT->VHGT[h][0] * scale);
		for (int w = 1; w <= x; ++w)
		  hv += (land->VHGT->VHGT[y][w] * scale);
#undef	scale

#define shift 0x400
		hf[realpos] = (unsigned short)min(((long)floor(hv + 0.5)) + shift + weoffs, 0xFFFF);
#undef	shift
	      }
	    }
	  }
	}

	/* next */
	walk++;
      }
    }

    return false;
  }
};

template<
  class WRLDRecord = Ob::WRLDRecord,
  class CELLRecord = Ob::CELLRecord,
  class LANDRecord = Ob::LANDRecord,
  class LTEXRecord = Ob::LTEXRecord
>
class ExtractWWorldOp : public WindowedWorldOp {
  FILE *wf;

public:
  ExtractWWorldOp(void *mwf, long b, long r) : WindowedWorldOp(b, r) {
    wf = (FILE *)mwf;
  }

  virtual bool Accept(Record *&curRecord) {
    WRLDRecord *wrld = (WRLDRecord *)curRecord;
    CELLRecord *cell;
    LANDRecord *land;

    if (!stricmp(wrld->EDID.value, wename.data())) {
      std::vector<Record *>::iterator walk = wrld->CELLS.begin();
      while (walk != wrld->CELLS.end()) {
	cell = (CELLRecord *)(*walk);
	land = (LANDRecord *)cell->LAND;

	/* make sure it's winning */
	if (cell->XCLC.IsLoaded() && land && !land->IsWinningDetermined()) {
	  ModFile *WinningModFile;
	  Record *WinningRecord;

	  col->LookupWinningRecord(land->formID, WinningModFile, WinningRecord, false);
	}
/*
	if (cell->XCLC.IsLoaded() && land && !land->IsWinning()) {
	  printf("skipping");
	}
*/
	if (cell->XCLC.IsLoaded() && land && land->IsWinning()) {
	  long leftx = (cell->XCLC->posX + rofx);
	  long topy  = (cell->XCLC->posY + rofy);
	  long leftc = (offsx + leftx) * 32;
	  long topc  = (offsy + topy ) * 32;

	  /* check cell-center against range, no problems with the overlap that way */
	  if (!cchk(leftc + 16 + lofx, topc + 16 + lofy)) {
	    /* next */ walk++; continue; }

	  SetTopic("Extracting water-heights from cell {%d,%d}", leftx, topy);
	  SetProgress(dells++);

	  if (1) {
	    fprintf(wf, "%4d %4d 0x%08x %f\n",
	      cell->XCLC->posX,
	      cell->XCLC->posY,
	      *((long  *)&cell->XCLW.value),
	      *((float *)&cell->XCLW.value)
	    );

	    /* XCLW: FFFF7F7F == no water */

	    //	    fprintf(stdout, "XCLW: 0x%08x\n", *((long *)&cell->XCLW.value));
	    //	    fprintf(stdout, "XWCN: 0x%08x\n", *((long *)&cell->XWCN.value));
	    //	    fprintf(stdout, "XWCU: 0x%08x 0x%08x 0x%08x 0x%08x\n", *((long *)&cell->XWCU.value[ 0]), *((long *)&cell->XWCU.value[ 4]), *((long *)&cell->XWCU.value[ 8]), *((long *)&cell->XWCU.value[12]));
	    //	    fprintf(stdout, "XWCU: 0x%08x 0x%08x 0x%08x 0x%08x\n", *((long *)&cell->XWCU.value[16]), *((long *)&cell->XWCU.value[20]), *((long *)&cell->XWCU.value[24]), *((long *)&cell->XWCU.value[28]));
	  }

	}

	/* next */
	walk++;
      }
    }

    return false;
  }
};

#define EXTRACT_GROUNDLAYER	1
#define EXTRACT_BASELAYER	1
#define EXTRACT_BLENDLAYER	1
#define EXTRACT_OVERLAYER	1

template<
  class WRLDRecord = Ob::WRLDRecord,
  class CELLRecord = Ob::CELLRecord,
  class LANDRecord = Ob::LANDRecord,
  class LTEXRecord = Ob::LTEXRecord
>
class ExtractMWorldOp : public WindowedWorldOp {
  unsigned char *tx;

public:
  ExtractMWorldOp(void *mtx, long b, long r) : WindowedWorldOp(b, r) {
    tx = (unsigned char *)mtx;
  }

  virtual bool Accept(Record *&curRecord) {
    WRLDRecord *wrld = (WRLDRecord *)curRecord;
    CELLRecord *cell;
    LANDRecord *land;

    if (!stricmp(wrld->EDID.value, wename.data())) {
      std::vector<Record *>::iterator walk = wrld->CELLS.begin();
      while (walk != wrld->CELLS.end()) {
	cell = (CELLRecord *)(*walk);
	land = (LANDRecord *)cell->LAND;

	/* make sure it's winning */
	if (cell->XCLC.IsLoaded() && land && !land->IsWinningDetermined()) {
	  ModFile *WinningModFile;
	  Record *WinningRecord;

	  col->LookupWinningRecord(land->formID, WinningModFile, WinningRecord, false);
	}
/*
	if (cell->XCLC.IsLoaded() && land && !land->IsWinning()) {
	  printf("skipping");
	}
*/
	if (cell->XCLC.IsLoaded() && land && land->IsWinning()) {
	  long leftx = (cell->XCLC->posX + rofx);
	  long topy  = (cell->XCLC->posY + rofy);
	  long leftc = (offsx + leftx) * 32;
	  long topc  = (offsy + topy ) * 32;
	  float bbuf[33][33] = {0};

	  /* check cell-center against range, no problems with the overlap that way */
	  if (!cchk(leftc + 16 + lofx, topc + 16 + lofy)) {
	    /* next */ walk++; continue; }

	  SetTopic("Extracting importance from cell {%d,%d}", leftx, topy);
	  SetProgress(dells++);

	  if (EXTRACT_GROUNDLAYER) {
	    /* search for the texture and get the average color of it */
	    unsigned char tex = ClassifyTexture<LTEXRecord>(0);

	    /* don't apply missing textures (instead of applying black) */
	    if (tex) {
	      for (int y = 0; y <= 32; y++)
	      for (int x = 0; x <= 32; x++) {
		unsigned char ctext = tex;

		bbuf[y][x] = ctext;
	      }
	    }
	  }

	  if (land->VTEX.IsLoaded()) {
	    std::vector<FORMID>::iterator walk = land->VTEX.value.begin();
	    while (walk != land->VTEX.value.end()) {
	      /* search for the texture and get the average color of it */
	      unsigned char tex = ClassifyTexture<LTEXRecord>(*walk);

	      walk++;
	    }
	  }

	  if (EXTRACT_BASELAYER)
	  if (land->BTXT.IsLoaded()) {
	    std::vector<LANDRecord::LANDGENTXT *>::iterator walk = land->BTXT.value.begin();
	    while (walk != land->BTXT.value.end()) {
	      /* search for the texture and get the average color of it */
	      unsigned char tex = ClassifyTexture<LTEXRecord>((*walk)->texture);

	      /* don't apply missing textures (instead of applying black) */
	      if (tex) {
		int offsx;
		int offsy;

		__assume(
		  ((*walk)->quadrant >= 0) &&
		  ((*walk)->quadrant <= 3)
		);

		/* bottom/top are y-flipped */
		switch ((*walk)->quadrant) {
		  case 0: /* eBottomLeft  */ offsx =  0; offsy =  0; break;
		  case 1: /* eBottomRight */ offsx = 16; offsy =  0; break;
		  case 2: /* eTopLeft     */ offsx =  0; offsy = 16; break;
		  case 3: /* eTopRight    */ offsx = 16; offsy = 16; break;
		}

		/* copy over */
		for (int lposy = 0; lposy <= 16; lposy++)
		for (int lposx = 0; lposx <= 16; lposx++) {
		  int y = offsy + lposy;
		  int x = offsx + lposx;

		  unsigned char ctext = tex;

		  /* blending with alpha from texture looks bad,
		   * in theory the alpha should be a max()-map anyway
		   */
#define blend(B, a, A, b)	(				\
		      ((1.0f - (B/* * A / 0xFF*/)) * a) +	\
		      ((       (B/* * A / 0xFF*/)) * b)		\
		    )

		  bbuf[y][x] = blend(1.0f, bbuf[y][x], alpha, ctext);
#undef	blend
		}
	      }

	      walk++;
	    }
	  }

	  /* overlay one layer after the other */
	  if (EXTRACT_BLENDLAYER)
	  if (land->Layers.IsLoaded()) {
	    std::set<SINT16> layers; std::set<SINT16>::iterator ls;
	    std::vector<LANDRecord::LANDLAYERS *>::iterator srch;

	    if ((cell->XCLC->posX == (0 + 1)) && (cell->XCLC->posY == (-20 + 1))) {
	      int i = 0;
	    }

	    srch = land->Layers.value.begin();
	    while (srch != land->Layers.value.end()) {
	      LANDRecord::LANDGENTXT *walk = &(*srch)->ATXT.value;
	      layers.insert(walk->layer);

	      srch++;
	    }

	    ls = layers.begin();
	    while (ls != layers.end()) {
	      srch = land->Layers.value.begin();
	      while (srch != land->Layers.value.end()) {
		LANDRecord::LANDGENTXT *walk = &(*srch)->ATXT.value;
		if (walk->layer == *ls) {
		  /* search for the texture and get the average color of it */
		  unsigned char tex = ClassifyTexture<LTEXRecord>((*walk).texture);

		  /* don't apply missing textures (instead of applying black) */
		  if (tex) {
		    int offsx;
		    int offsy;

		    __assume(
		      ((*walk).quadrant >= 0) &&
		      ((*walk).quadrant <= 3)
		    );

		    /* bottom/top are y-flipped */
		    switch ((*walk).quadrant) {
		      case 0: /* eBottomLeft  */ offsx =  0; offsy =  0; break;
		      case 1: /* eBottomRight */ offsx = 16; offsy =  0; break;
		      case 2: /* eTopLeft     */ offsx =  0; offsy = 16; break;
		      case 3: /* eTopRight    */ offsx = 16; offsy = 16; break;
		    }

		    /* blend per-pixel */
		    std::vector<LANDRecord::LANDVTXT>::iterator over = (*srch)->VTXT.value.begin();
		    while (over != (*srch)->VTXT.value.end()) {
		      float opc = over->opacity;

		      __assume(
			(over->position >= 0) &&
			(over->position < (17 * 17))
		      );

		      int lposy = over->position / 17;
		      int lposx = over->position % 17;

		      int y = offsy + lposy;
		      int x = offsx + lposx;

		      unsigned char ctext = tex;

		      /* blending with alpha from texture looks bad,
		       * in theory the alpha should be a max()-map anyway
		       */
#define blend(B, a, A, b)	(				\
			  ((1.0f - (B/* * A / 0xFF*/)) * a) +	\
			  ((       (B/* * A / 0xFF*/)) * b)	\
			)

		      bbuf[y][x] = blend(opc, bbuf[y][x], alpha, ctext);
#undef	blend

		      over++;
		    }
		  }
		}

		srch++;
	      }

	      ls++;
	    }
	  }

	  /* place the cell into the destination */
	  for (int y = 0; y <= 32; y++)
	  for (int x = 0; x <= 32; x++) {
	    int realx = leftc + x + lofx;
	    int realy = topc  + y + lofy;

	    if (rchk(realx, realy)) {
	      unsigned long realpos = rpos(realx, realy);

#define round(clr)	min((unsigned long)floor(clr + 0.5), 0xFF)

	      unsigned char
	      color  = 0;
	      color |= round(bbuf[y][x]);
#undef	round

	      tx[realpos] = color;
	    }
	  }

#undef	extract
	}

	/* next */
	walk++;
      }
    }

    return false;
  }
};

template<
  class WRLDRecord = Ob::WRLDRecord,
  class CELLRecord = Ob::CELLRecord,
  class LANDRecord = Ob::LANDRecord,
  class LTEXRecord = Ob::LTEXRecord
>
class ExtractCWorldOp : public WindowedWorldOp {
  unsigned long *tx;

private:
  unsigned long *LookupTexture(FORMID t, int ss) {
    return ::LookupTexture<LTEXRecord>(t, texsample, ss);
  }

  /* --------------------------------------------------------------------------------------- */

#define extract(c, pos)		(float)((c >> pos) & 0xFF)
#define randomf()		((1.0f / RAND_MAX) * random())
#define multipleof(a, b)	(b * ((a + (b - 1)) / b))
//#define PREMULTIPLIED

  /* blending with alpha from texture looks bad,
    * in theory the alpha should be a max()-map anyway
    */
#define blend(B, a, A, b) (				\
    ((1.0f - (B/* * A / 0xFF*/)) * a) +			\
    ((       (B/* * A / 0xFF*/)) * b)			\
  )

  /* 4 * 33 * 4 * 33 * 4 * 4 = 278784 */
  float bbuf[33 * SUPERSAMPLE_LIMIT][multipleof(33 * SUPERSAMPLE_LIMIT, 16)][4];
//float fbuf[33 * SUPERSAMPLE_LIMIT][multipleof(33 * SUPERSAMPLE_LIMIT, 16)][4];
  float blnd[17                    ][multipleof(17                    , 16)];

  /* --------------------------------------------------------------------------------------- */
  /* randomize between a and b (that is between a super-sampled texture and it's base color) */
  template<const int ss>
  float interpol(float a, float b) {
    const float rnd = randomf();

    return a + ((b - a) * rnd);
  }

  template<>
  float interpol<1>(float a, float b) {
    return a;
  }

  /* super sample color from bbuf */
  template<const int ss>
  float sup(int x, int y, int sx, int sy, float v00, float v10, float v01, float v11) {
//efine modulo(v, m)	(v % (m    ))
#define modulo(v, m)	(v & (m - 1))
    /* (sy & 0) ^= (sy % 1)
     * (sy & 1) ^= (sy % 2)
     * (sy & 3) ^= (sy % 4)
     */
    float wy = (float)(ss - modulo(sy, ss));
    float vy = (float)(     modulo(sy, ss));
    float wx = (float)(ss - modulo(sx, ss));
    float vx = (float)(     modulo(sx, ss));

    /* jitter the blend-factor a half/full-sample around */
#define jitters		1.0f
    float jitterl = randomf() * (jitters / ss);
    float jitterr = randomf() * (jitters / ss);
    float jitterx = randomf() * (jitters / ss);

    float valyl =
      v00          * (wy - jitterl) +
      v10          * (vy + jitterl);
    float valyr =
      v01          * (wy - jitterr) +
      v11          * (vy + jitterr);
    float valx =
      (valyl / ss) * (wx - jitterx) +
      (valyr / ss) * (vx + jitterx);
    float val =
      (valx  / ss);

    return val;
  }

  template<>
  float sup<1>(int x, int y, int sx, int sy, float v00, float v10, float v01, float v11) {
    return v00;
  }

  /* super sample color from bbuf */
  template<const int ss>
  float sup(int x, int y, int sx, int sy, int off) {
    return sup<ss>(x, y, sx, sy,
      bbuf[(y + 0) * ss][(x + 0) * ss][off],
      bbuf[(y + 1) * ss][(x + 0) * ss][off],
      bbuf[(y + 0) * ss][(x + 1) * ss][off],
      bbuf[(y + 1) * ss][(x + 1) * ss][off]);
  }

  /* --------------------------------------------------------------------------------------- */
  /* get dimensions and location of a quadrant */
  void DimsQuad(UINT8 quad, int &offsx, int &numsx, int &offsy, int &numsy) {
    __assume(
      (quad >= 0) &&
      (quad <= 3)
    );

    /* bottom/top are y-flipped */
    switch (quad) {
      case 0: /* eBottomLeft  */ offsx =  0; offsy =  0; numsx = 16; numsy = 16; break;
      case 1: /* eBottomRight */ offsx = 16; offsy =  0; numsx = 17; numsy = 16; break;
      case 2: /* eTopLeft     */ offsx =  0; offsy = 16; numsx = 16; numsy = 17; break;
      case 3: /* eTopRight    */ offsx = 16; offsy = 16; numsx = 17; numsy = 17; break;
    }
  }

#define NONSS_RANGEX  33
#define NONSS_RANGEY  33
#define NONSS_QUADRX  17
#define NONSS_QUADRY  17

#define YESSS_RANGEX  32
#define YESSS_RANGEY  32
#define YESSS_QUADRX  16
#define YESSS_QUADRY  16

  void DimsQuadNS(UINT8 quad, int &offsx, int &numsx, int &offsy, int &numsy) {
    /* get the location */
    DimsQuad(quad, offsx, numsx, offsy, numsy);

    numsx = min(numsx, NONSS_QUADRX);
    numsy = min(numsy, NONSS_QUADRY);
  }

  void DimsQuadYS(UINT8 quad, int &offsx, int &numsx, int &offsy, int &numsy) {
    /* get the location */
    DimsQuad(quad, offsx, numsx, offsy, numsy);

    numsx = min(numsx, YESSS_QUADRX);
    numsy = min(numsy, YESSS_QUADRY);
  }

  /* --------------------------------------------------------------------------------------- */
  template<const int ss>
  void CopyTileNS(unsigned long *tex) {
    /* read beyond last value (1x1 value) */
    const unsigned long btext = tex[ss*ss];

    const float r = extract(btext, 24);
    const float g = extract(btext, 16);
    const float b = extract(btext,  8);
//  const float a = extract(btext,  0);

    for (int y = 0; y < NONSS_RANGEY; y++)
    for (int x = 0; x < NONSS_RANGEX; x++) {
      /* read into fully super-sampled buffer */
      unsigned long *ctex = tex;

      for (int sy = ((y + 0) * ss); sy < ((y + 1) * ss); sy++)
      for (int sx = ((x + 0) * ss); sx < ((x + 1) * ss); sx++) {
	unsigned long ctext = *ctex++;

	float R = extract(ctext, 24);
	float G = extract(ctext, 16);
	float B = extract(ctext,  8);
//	float A = extract(ctext,  0);

	R = interpol<ss>(R, r);
	G = interpol<ss>(G, g);
	B = interpol<ss>(B, b);
//	A = interpol<ss>(A, a);

	bbuf[sy][sx][0] = R;
	bbuf[sy][sx][1] = G;
	bbuf[sy][sx][2] = B;
//	bbuf[sy][sx][3] = A;
      }
    }
  }

  /* copy super-sampled */
  template<const int ss>
  void CopyQuadNS(unsigned long *tex, UINT8 quad) {
    /* read beyond last value (1x1 value) */
    const unsigned long btext = tex[ss*ss];

    const float r = extract(btext, 24);
    const float g = extract(btext, 16);
    const float b = extract(btext,  8);
//  const float a = extract(btext,  0);

    int offsx, numsx;
    int offsy, numsy;

    /* get the location */
    DimsQuadNS(quad, offsx, numsx, offsy, numsy);

    /* copy over */
    for (int lposy = 0; lposy < numsy; lposy++)
    for (int lposx = 0; lposx < numsx; lposx++) {
      /* read into fully super-sampled buffer */
      unsigned long *ctex = tex;

      const int y = offsy + lposy;
      const int x = offsx + lposx;
      for (int sy = ((y + 0) * ss); sy < ((y + 1) * ss); sy++)
      for (int sx = ((x + 0) * ss); sx < ((x + 1) * ss); sx++) {
	unsigned long ctext = *ctex++;

	float R = extract(ctext, 24);
	float G = extract(ctext, 16);
	float B = extract(ctext,  8);
//	float A = extract(ctext,  0);

	R = interpol<ss>(R, r);
	G = interpol<ss>(G, g);
	B = interpol<ss>(B, b);
//	A = interpol<ss>(A, a);

	bbuf[sy][sx][0] = blend(1.0f, bbuf[sy][sx][0], alpha, R);
	bbuf[sy][sx][1] = blend(1.0f, bbuf[sy][sx][1], alpha, G);
	bbuf[sy][sx][2] = blend(1.0f, bbuf[sy][sx][2], alpha, B);
//	bbuf[sy][sx][3] = blend(1.0f, bbuf[sy][sx][3], alpha, A);
      }
    }
  }

  /* blend super-sampled */
  template<const int ss>
  void BlndQuadNS(unsigned long *tex, UINT8 quad) {
    /* read beyond last value (1x1 value) */
    const unsigned long btext = tex[ss*ss];

    const float r = extract(btext, 24);
    const float g = extract(btext, 16);
    const float b = extract(btext,  8);
//  const float a = extract(btext,  0);

    int offsx, numsx;
    int offsy, numsy;

    /* get the location */
    DimsQuadYS(quad, offsx, numsx, offsy, numsy);

    /* blend over */
    for (int lposy = 0; lposy < numsy; lposy++)
    for (int lposx = 0; lposx < numsx; lposx++) {
      /* blend with fully super-sampled buffer */
      unsigned long *ctex = tex;

      const int y = offsy + lposy;
      const int x = offsx + lposx;
      for (int sy = ((y + 0) * ss); sy < ((y + 1) * ss); sy++)
      for (int sx = ((x + 0) * ss); sx < ((x + 1) * ss); sx++) {
	unsigned long ctext = *ctex++;

	float R = extract(ctext, 24);
	float G = extract(ctext, 16);
	float B = extract(ctext,  8);
//	float A = extract(ctext,  0);

	R = interpol<ss>(R, r);
	G = interpol<ss>(G, g);
	B = interpol<ss>(B, b);
//	A = interpol<ss>(A, a);

	/* supersample alpha */
	float opc = sup<ss>(x, y, sx, sy,
	  blnd[lposy + 0][lposx + 0],
	  blnd[lposy + 1][lposx + 0],
	  blnd[lposy + 0][lposx + 1],
	  blnd[lposy + 1][lposx + 1]);

#ifdef	PREMULTIPLIED
	fbuf[sy][sx][0] = opc * R + (1.0f - opc) * fbuf[sy][sx][0];
	fbuf[sy][sx][1] = opc * G + (1.0f - opc) * fbuf[sy][sx][1];
	fbuf[sy][sx][2] = opc * B + (1.0f - opc) * fbuf[sy][sx][2];
  	fbuf[sy][sx][3] = opc     + (1.0f - opc) * fbuf[sy][sx][3];
#else
	bbuf[sy][sx][0] = blend(opc, bbuf[sy][sx][0], alpha, R);
	bbuf[sy][sx][1] = blend(opc, bbuf[sy][sx][1], alpha, G);
	bbuf[sy][sx][2] = blend(opc, bbuf[sy][sx][2], alpha, B);
//	bbuf[sy][sx][3] = blend(opc, bbuf[sy][sx][3], alpha, A);
#endif
      }
    }
  }

  template<const int ss>
  void CmbnTileNS() {
    /* base-layer */
    for (int y = 0; y < YESSS_RANGEY; y++)
    for (int x = 0; x < YESSS_RANGEX; x++) {
      for (int sy = ((y + 0) * ss); sy < ((y + 1) * ss); sy++)
      for (int sx = ((x + 0) * ss); sx < ((x + 1) * ss); sx++) {
	/* supersample base-color */
	float red = fbuf[sy][sx][0];
	float grn = fbuf[sy][sx][1];
	float blu = fbuf[sy][sx][2];
	float alp = fbuf[sy][sx][3];

	bbuf[sy][sx][0] = red + (1.0f - alp) * bbuf[sy][sx][0];
	bbuf[sy][sx][1] = grn + (1.0f - alp) * bbuf[sy][sx][1];
	bbuf[sy][sx][2] = blu + (1.0f - alp) * bbuf[sy][sx][2];
//	bbuf[sy][sx][3] = alp + (1.0f - alp) * bbuf[sy][sx][3];
      }
    }
  }

  /* --------------------------------------------------------------------------------------- */

  /* blend super-sampled */
  template<const int ss>
  void BlndQuadMS(unsigned long *tex, UINT8 quad) {
    /* read beyond last value (1x1 value) */
    const unsigned long btext = tex[ss*ss];

    const float r = extract(btext, 24);
    const float g = extract(btext, 16);
    const float b = extract(btext,  8);
    //  const float a = extract(btext,  0);

    int offsx, numsx;
    int offsy, numsy;

    /* get the location */
    DimsQuadYS(quad, offsx, numsx, offsy, numsy);

    /* blend over */
    for (int lposy = 0; lposy < numsy; lposy++)
    for (int lposx = 0; lposx < numsx; lposx++) {
      /* blend with fully super-sampled buffer */
      unsigned long *ctex = tex;

      const int y = offsy + lposy;
      const int x = offsx + lposx;
      for (int sy = ((y + 0) * ss); sy < ((y + 1) * ss); sy++)
      for (int sx = ((x + 0) * ss); sx < ((x + 1) * ss); sx++) {
	unsigned long ctext = *ctex++;

	float R = extract(ctext, 24);
	float G = extract(ctext, 16);
	float B = extract(ctext,  8);
//	float A = extract(ctext,  0);

	R = interpol<ss>(R, r);
	G = interpol<ss>(G, g);
	B = interpol<ss>(B, b);
//	A = interpol<ss>(A, a);

	/* supersample alpha */
	float opc = sup<ss>(x, y, sx, sy,
	  blnd[lposy + 0][lposx + 0],
	  blnd[lposy + 1][lposx + 0],
	  blnd[lposy + 0][lposx + 1],
	  blnd[lposy + 1][lposx + 1]);

	/* half-blend by distance from interpolated center */
	int dy = ss - abs(ss - 2 * modulo(sy, ss));
	int dx = ss - abs(ss - 2 * modulo(sx, ss));

	opc *= min(dy, dx) + max(dy, dx);
	opc /= ss * 2 * (1.5f + randomf());

#ifdef	PREMULTIPLIED
	fbuf[sy][sx][0] = opc * R + (1.0f - opc) * fbuf[sy][sx][0];
	fbuf[sy][sx][1] = opc * G + (1.0f - opc) * fbuf[sy][sx][1];
	fbuf[sy][sx][2] = opc * B + (1.0f - opc) * fbuf[sy][sx][2];
	fbuf[sy][sx][3] = opc     + (1.0f - opc) * fbuf[sy][sx][3];
#else
	bbuf[sy][sx][0] = blend(opc, bbuf[sy][sx][0], alpha, R);
	bbuf[sy][sx][1] = blend(opc, bbuf[sy][sx][1], alpha, G);
	bbuf[sy][sx][2] = blend(opc, bbuf[sy][sx][2], alpha, B);
//	bbuf[sy][sx][3] = blend(opc, bbuf[sy][sx][3], alpha, A);
#endif
      }
    }
  }

  /* --------------------------------------------------------------------------------------- */
  template<const int ss>
  void CopyTileRS(unsigned long *tex) {
    /* read beyond last value (1x1 value) */
    const unsigned long btext = tex[ss*ss];

    const float r = extract(btext, 24);
    const float g = extract(btext, 16);
    const float b = extract(btext,  8);
//  const float a = extract(btext,  0);

    for (int y = 0; y < NONSS_RANGEY; y++)
    for (int x = 0; x < NONSS_RANGEX; x++) {
      /* read into fully super-sampled buffer */
      unsigned long *ctex = tex;

      int sy = ((y + 0) * ss);
      int sx = ((x + 0) * ss); {
	unsigned long ctext = *ctex++;

	float R = extract(ctext, 24);
	float G = extract(ctext, 16);
	float B = extract(ctext,  8);
//	float A = extract(ctext,  0);

	R = interpol<ss>(R, r);
	G = interpol<ss>(G, g);
	B = interpol<ss>(B, b);
//	A = interpol<ss>(A, a);

	bbuf[sy][sx][0] = R;
	bbuf[sy][sx][1] = G;
	bbuf[sy][sx][2] = B;
//	bbuf[sy][sx][3] = A;
      }
    }
  }

  /* copy normal-sampled */
  template<const int ss>
  void CopyQuadRS(unsigned long *tex, UINT8 quad) {
    /* read beyond last value (1x1 value) */
    const unsigned long btext = tex[ss*ss];

    const float r = extract(btext, 24);
    const float g = extract(btext, 16);
    const float b = extract(btext,  8);
//  const float a = extract(btext,  0);

    int offsx, numsx;
    int offsy, numsy;

    /* get the location */
    DimsQuadNS(quad, offsx, numsx, offsy, numsy);

    /* copy over */
    for (int lposy = 0; lposy < numsy; lposy++)
    for (int lposx = 0; lposx < numsx; lposx++) {
      /* blend with normal-sampled buffer */
      unsigned long *ctex = tex;

      const int y = offsy + lposy;
      const int x = offsx + lposx;
      int sy = ((y + 0) * ss);
      int sx = ((x + 0) * ss); {
	unsigned long ctext = *ctex++;

	float R = extract(ctext, 24);
	float G = extract(ctext, 16);
	float B = extract(ctext,  8);
//	float A = extract(ctext,  0);

	R = interpol<ss>(R, r);
	G = interpol<ss>(G, g);
	B = interpol<ss>(B, b);
//	A = interpol<ss>(A, a);

	bbuf[sy][sx][0] = blend(1.0f, bbuf[sy][sx][0], alpha, R);
	bbuf[sy][sx][1] = blend(1.0f, bbuf[sy][sx][1], alpha, G);
	bbuf[sy][sx][2] = blend(1.0f, bbuf[sy][sx][2], alpha, B);
//	bbuf[sy][sx][3] = blend(1.0f, bbuf[sy][sx][3], alpha, A);
      }
    }
  }

  /* blend normal-sampled */
  template<const int ss>
  void BlndQuadRS(unsigned long *tex, UINT8 quad) {
    /* read beyond last value (1x1 value) */
    const unsigned long btext = tex[ss*ss];

    const float r = extract(btext, 24);
    const float g = extract(btext, 16);
    const float b = extract(btext,  8);
//  const float a = extract(btext,  0);

    int offsx, numsx;
    int offsy, numsy;

    /* get the location */
    DimsQuadNS(quad, offsx, numsx, offsy, numsy);

    /* blend over */
    for (int lposy = 0; lposy < numsy; lposy++)
    for (int lposx = 0; lposx < numsx; lposx++) {
      /* blend with normal-sampled buffer */
      unsigned long *ctex = tex;

      const int y = offsy + lposy;
      const int x = offsx + lposx;
      int sy = ((y + 0) * ss);
      int sx = ((x + 0) * ss); {
	unsigned long ctext = *ctex++;

	float R = extract(ctext, 24);
	float G = extract(ctext, 16);
	float B = extract(ctext,  8);
//	float A = extract(ctext,  0);

	R = interpol<ss>(R, r);
	G = interpol<ss>(G, g);
	B = interpol<ss>(B, b);
//	A = interpol<ss>(A, a);

	float opc = blnd[lposy][lposx];

#ifdef	PREMULTIPLIED
	fbuf[sy][sx][0] = opc * R + (1.0f - opc) * fbuf[sy][sx][0];
	fbuf[sy][sx][1] = opc * G + (1.0f - opc) * fbuf[sy][sx][1];
	fbuf[sy][sx][2] = opc * B + (1.0f - opc) * fbuf[sy][sx][2];
	fbuf[sy][sx][3] = opc     + (1.0f - opc) * fbuf[sy][sx][3];
#else
	bbuf[sy][sx][0] = blend(opc, bbuf[sy][sx][0], alpha, R);
	bbuf[sy][sx][1] = blend(opc, bbuf[sy][sx][1], alpha, G);
	bbuf[sy][sx][2] = blend(opc, bbuf[sy][sx][2], alpha, B);
//	bbuf[sy][sx][3] = blend(opc, bbuf[sy][sx][3], alpha, A);
#endif
      }
    }
  }

  /* --------------------------------------------------------------------------------------- */
  template<const int ss>
  void FillTileRS() {
    /* base-layer */
    for (int y = 0; y < YESSS_RANGEY; y++)
    for (int x = 0; x < YESSS_RANGEX; x++) {
      for (int sy = ((y + 0) * ss); sy < ((y + 1) * ss); sy++)
      for (int sx = ((x + 0) * ss); sx < ((x + 1) * ss); sx++) {
	/* supersample base-color */
	float red = sup<ss>(x, y, sx, sy, 0);
	float grn = sup<ss>(x, y, sx, sy, 1);
	float blu = sup<ss>(x, y, sx, sy, 2);
//	float alp = sup<ss>(x, y, sx, sy, 3);

	bbuf[sy][sx][0] = red;
	bbuf[sy][sx][1] = grn;
	bbuf[sy][sx][2] = blu;
//	bbuf[sy][sx][3] = alp;
      }
    }
  }

  template<int ss, class LANDRecord>
  void MultTileNS(LANDRecord *land) {
    for (int y = 0; y < 32; y ++)
    for (int x = 0; x < 32; x ++) {
      /* multiply with fully super-sampled buffer */
      for (int sy = ((y + 0) * ss); sy < ((y + 1) * ss); sy++)
      for (int sx = ((x + 0) * ss); sx < ((x + 1) * ss); sx++) {
	/* supersample vertex-color */
	float red = sup<ss>(x, y, sx, sy,
	  (float)land->VCLR->VCLR[y + 0][x + 0].red,
	  (float)land->VCLR->VCLR[y + 1][x + 0].red,
	  (float)land->VCLR->VCLR[y + 0][x + 1].red,
	  (float)land->VCLR->VCLR[y + 1][x + 1].red);
	float grn = sup<ss>(x, y, sx, sy,
	  (float)land->VCLR->VCLR[y + 0][x + 0].green,
	  (float)land->VCLR->VCLR[y + 1][x + 0].green,
	  (float)land->VCLR->VCLR[y + 0][x + 1].green,
	  (float)land->VCLR->VCLR[y + 1][x + 1].green);
	float blu = sup<ss>(x, y, sx, sy,
	  (float)land->VCLR->VCLR[y + 0][x + 0].blue,
	  (float)land->VCLR->VCLR[y + 1][x + 0].blue,
	  (float)land->VCLR->VCLR[y + 0][x + 1].blue,
	  (float)land->VCLR->VCLR[y + 1][x + 1].blue);

#define multiply(a, b, pos)	((a * b) / 0xFF)

	bbuf[sy][sx][0] = multiply(bbuf[sy][sx][0], red, 24);
	bbuf[sy][sx][1] = multiply(bbuf[sy][sx][1], grn, 16);
	bbuf[sy][sx][2] = multiply(bbuf[sy][sx][2], blu,  8);
//	bbuf[sy][sx][3] = multiply(bbuf[sy][sx][3], 255,  0);
#undef	multiply

      }
    }
  }

#undef	extract
#undef	interpol
#undef	blend

public:
  ExtractCWorldOp(void *mtx, long b, long r) : WindowedWorldOp(b, r) {
    tx = (unsigned long *)mtx;
  }

  virtual bool Accept(Record *&curRecord) {
    WRLDRecord *wrld = (WRLDRecord *)curRecord;
    CELLRecord *cell;
    LANDRecord *land;

    if (!stricmp(wrld->EDID.value, wename.data())) {
      std::vector<Record *>::iterator walk = wrld->CELLS.begin();
      while (walk != wrld->CELLS.end()) {
	cell = (CELLRecord *)(*walk);
	land = (LANDRecord *)cell->LAND;

	/* make sure it's winning */
	if (cell->XCLC.IsLoaded() && land && !land->IsWinningDetermined()) {
	  ModFile *WinningModFile;
	  Record *WinningRecord;

	  col->LookupWinningRecord(land->formID, WinningModFile, WinningRecord, false);
	}
/*
	if (cell->XCLC.IsLoaded() && land && !land->IsWinning()) {
	  printf("skipping");
	}
*/
	if (cell->XCLC.IsLoaded() && land && land->IsWinning()) {
	  const long leftx = (cell->XCLC->posX + rofx);
	  const long topy  = (cell->XCLC->posY + rofy);
	  const long leftc = (offsx + leftx) * 32;
	  const long topc  = (offsy + topy ) * 32;

	  /* max. supersample into the required buffer */
	  const int ss = max(texsample, srfsample);
	  __assume((ss == 1) || (ss == 2) || (ss == 4));

	  memset(bbuf, 0, sizeof(bbuf));
//	  memset(fbuf, 0, sizeof(fbuf));

	  /* check cell-center against range, no problems with the overlap that way */
	  if (!cchk(leftc + 16 + lofx, topc + 16 + lofy)) {
	    /* next */ walk++; continue; }

	  SetTopic("Extracting surfaces from cell {%d,%d}", leftx, topy);
	  SetProgress(dells++);

	  if (EXTRACT_GROUNDLAYER) {
	    /* search for the texture and get the average color of it */
	    unsigned long *tex = LookupTexture(0, ss);

	    /* don't apply missing textures (instead of applying black) */
	    if (tex) {
	      switch (ss) {
		case 1: CopyTileNS<1>(tex); break;
		case 2: CopyTileNS<2>(tex); break;
		case 4: CopyTileNS<4>(tex); break;
	      }
	    }
	  }

	  if (land->VTEX.IsLoaded()) {
	    std::vector<FORMID>::iterator walk = land->VTEX.value.begin();
	    while (walk != land->VTEX.value.end()) {
	      /* search for the texture and get the average color of it */
	      unsigned long *tex = LookupTexture(*walk, ss);

	      walk++;
	    }
	  }

	  if (EXTRACT_BASELAYER)
	  if (land->BTXT.IsLoaded()) {
	    std::vector<LANDRecord::LANDGENTXT *>::iterator walk = land->BTXT.value.begin();
	    while (walk != land->BTXT.value.end()) {
	      /* search for the texture and get the average color of it */
	      unsigned long *tex = LookupTexture((*walk)->texture, ss);

	      /* don't apply missing textures (instead of applying black) */
	      if (tex) {
		switch (ss) {
		  case 1: CopyQuadNS<1>(tex, (*walk)->quadrant); break;
		  case 2: CopyQuadNS<2>(tex, (*walk)->quadrant); break;
		  case 4: CopyQuadNS<4>(tex, (*walk)->quadrant); break;
		}
	      }

	      walk++;
	    }
	  }

	  /* overlay one layer after the other */
	  if (EXTRACT_BLENDLAYER)
	  if (land->Layers.IsLoaded()) {
	    std::set<SINT16> layers; std::set<SINT16>::iterator ls;
	    std::vector<LANDRecord::LANDLAYERS *>::iterator srch;

	    srch = land->Layers.value.begin();
	    while (srch != land->Layers.value.end()) {
	      LANDRecord::LANDGENTXT *walk = &(*srch)->ATXT.value;
	      layers.insert(walk->layer);

	      srch++;
	    }

#define	TWOPASS_BASE
#ifdef	TWOPASS_BASE
	    if (ss > 1) {
	      /* the idea is the following:
	       * - when we super-sample the opacity value we end up with
	       *   half-transparent pixels between the original values:
	       *   a[0,1] = 1.0, b[1,0] = 1.0	-> p[a,b] non-transparent
	       *           ab[0.5,0.5]		-> half transparent
	       *
	       * - now what we do is, we fill the pixels of the background-
	       *   layer that will be used for super-sampled alpha with an
	       *   upsampled non-supersampled version of the entire process
	       * - that way the half-transparent pixel blends with the final
	       *   color (though under-sampled of course) instead of blending
	       *   with the base-layer
	       *
	       * the problem occured on -19,1 where the base-layer is snow
	       * in one quadrant and all super-sampled pixels became white
	       */

	      ls = layers.begin();
	      while (ls != layers.end()) {
		std::vector<LANDRecord::LANDLAYERS *>::iterator srch = land->Layers.value.begin();
		while (srch != land->Layers.value.end()) {
		  LANDRecord::LANDGENTXT *walk = &(*srch)->ATXT.value;
		  if (walk->layer == *ls) {
		    /* search for the texture and get the average color of it */
		    unsigned long *tex = LookupTexture((*walk).texture, ss);

		    /* don't apply missing textures (instead of applying black) */
		    if (tex) {
		      memset(blnd, 0, sizeof(blnd));

		      /* blend per-pixel */
		      std::vector<LANDRecord::LANDVTXT>::iterator over = (*srch)->VTXT.value.begin();
		      while (over != (*srch)->VTXT.value.end()) {
			float opc = over->opacity;

			__assume(
			  (over->position >= 0) &&
			  (over->position < (17 * 17))
			);

			int lposy = over->position / 17;
			int lposx = over->position % 17;

			blnd[lposy][lposx] = opc;

			over++;
		      }

		      switch (ss) {
			case 1: BlndQuadMS<1>(tex, (*walk).quadrant); break;
			case 2: BlndQuadMS<2>(tex, (*walk).quadrant); break;
			case 4: BlndQuadMS<4>(tex, (*walk).quadrant); break;
		      }
		    }
		  }

		  srch++;
		}

		ls++;
	      }

	      /* base-layer
	      switch (ss) {
		case 1: FillTileRS<1>(); break;
		case 2: FillTileRS<2>(); break;
		case 4: FillTileRS<4>(); break;
	      }
	       */

//#define	TWOPASS_RESTORE
#ifdef	TWOPASS_RESTORE
	      /* - now we restore all non-supersampled pixels by their original values
	       *   this allows the non-supersampled opacities (which are correct!) to
	       *   blend against the original layer
	       */

	      if (EXTRACT_GROUNDLAYER) {
		/* search for the texture and get the average color of it */
		unsigned long *tex = LookupTexture(0, ss);

		/* don't apply missing textures (instead of applying black) */
		if (tex) {
		  switch (ss) {
		    case 1: CopyTileRS<1>(tex); break;
		    case 2: CopyTileRS<2>(tex); break;
		    case 4: CopyTileRS<4>(tex); break;
		  }
		}
	      }

	      if (EXTRACT_BASELAYER)
	      if (land->BTXT.IsLoaded()) {
		std::vector<LANDRecord::LANDGENTXT *>::iterator walk = land->BTXT.value.begin();
		while (walk != land->BTXT.value.end()) {
		  /* search for the texture and get the average color of it */
		  unsigned long *tex = LookupTexture((*walk)->texture, ss);

		  /* don't apply missing textures (instead of applying black) */
		  if (tex) {
		    switch (ss) {
		      case 1: CopyQuadRS<1>(tex, (*walk)->quadrant); break;
		      case 2: CopyQuadRS<2>(tex, (*walk)->quadrant); break;
		      case 4: CopyQuadRS<4>(tex, (*walk)->quadrant); break;
		    }
		  }

		  walk++;
		}
	      }
#endif
	    }
#endif

	    ls = layers.begin();
	    while (ls != layers.end()) {
	      std::vector<LANDRecord::LANDLAYERS *>::iterator srch = land->Layers.value.begin();
	      while (srch != land->Layers.value.end()) {
		LANDRecord::LANDGENTXT *walk = &(*srch)->ATXT.value;
		if (walk->layer == *ls) {
		  /* search for the texture and get the average color of it */
		  unsigned long *tex = LookupTexture((*walk).texture, ss);

		  /* don't apply missing textures (instead of applying black) */
		  if (tex) {
		    memset(blnd, 0, sizeof(blnd));

		    /* blend per-pixel */
		    std::vector<LANDRecord::LANDVTXT>::iterator over = (*srch)->VTXT.value.begin();
		    while (over != (*srch)->VTXT.value.end()) {
		      float opc = over->opacity;

		      __assume(
			(over->position >= 0) &&
			(over->position < (17 * 17))
		      );

		      int lposy = over->position / 17;
		      int lposx = over->position % 17;

		      blnd[lposy][lposx] = opc;

		      over++;
		    }

		    switch (ss) {
		      case 1: BlndQuadNS<1>(tex, (*walk).quadrant); break;
		      case 2: BlndQuadNS<2>(tex, (*walk).quadrant); break;
		      case 4: BlndQuadNS<4>(tex, (*walk).quadrant); break;
		    }
		  }
		}

		srch++;
	      }

	      ls++;
	    }

#ifdef	PREMULTIPLIED
	    /* base-layer */
	    switch (ss) {
	      case 1: CmbnTileNS<1>(); break;
	      case 2: CmbnTileNS<2>(); break;
	      case 4: CmbnTileNS<4>(); break;
	    }
#endif
	  }

	  /* multiply both colors in the cell */
	  if (EXTRACT_OVERLAYER)
	  if (land->VCLR.IsLoaded()) {
	    /* base-layer */
	    switch (ss) {
	      case 1: MultTileNS<1>(land); break;
	      case 2: MultTileNS<2>(land); break;
	      case 4: MultTileNS<4>(land); break;
	    }
	  }

	  /* downsample */
	  const int sr = ss / srfsample;
	  const int su =      srfsample;

	  /* place the cell into the destination */
	  for (int y = 0; y < 32; y++)
	  for (int x = 0; x < 32; x++) {
	    for (int sy = 0; sy < su; sy++)
	    for (int sx = 0; sx < su; sx++) {
	      int realy = (topc  + y + lofy) * su + sy;
	      int realx = (leftc + x + lofx) * su + sx;

	      if (rchk(realx, realy, su)) {
		unsigned long realpos = rpos(realx, realy, su);

		float red = 0.0f;
		float grn = 0.0f;
		float blu = 0.0f;
//		float alp = 0.0f;

		/* downsample by "sr" */
		for (int ry = 0; ry < sr; ry++)
		for (int rx = 0; rx < sr; rx++) {
		  red += bbuf[(y * ss) + (sy * sr) + ry][(x * ss) + (sx * sr) + rx][0];
		  grn += bbuf[(y * ss) + (sy * sr) + ry][(x * ss) + (sx * sr) + rx][1];
		  blu += bbuf[(y * ss) + (sy * sr) + ry][(x * ss) + (sx * sr) + rx][2];
//		  alp += bbuf[(y * ss) + (sy * sr) + ry][(x * ss) + (sx * sr) + rx][3];
		}

		red /= sr * sr;
		grn /= sr * sr;
		blu /= sr * sr;
//		alp /= sr * sr;

#ifdef	BLEND_NONGAMMA
		/* adjustment */
		red /= 255.0f;
		grn /= 255.0f;
		blu /= 255.0f;
//		alp /= 255.0f;

		/* gamma corrected */
		red = powf(red, raisecontrast);
		grn = powf(grn, raisecontrast);
		blu = powf(blu, raisecontrast);
//		alp =      alp                ;

		/* adjustment */
		red /= upperbrightness;
		grn /= upperbrightness;
		blu /= upperbrightness;
//		alp /= 1.0f           ;

		/* gamma corrected */
		red = powf(red, gamma_out);
		grn = powf(grn, gamma_out);
		blu = powf(blu, gamma_out);
//		alp =      alp            ;

		/* adjustment */
		red *= 255.0f;
		grn *= 255.0f;
		blu *= 255.0f;
//		alp *= 255.0f;
#endif

#define round(clr, pos)	(min((unsigned long)floor(clr + 0.5f), 0xFF) << pos)

		unsigned long
		color  = 0;
		color |= round(red, 24);
		color |= round(grn, 16);
		color |= round(blu,  8);
//		color |= round(alp,  0);
#undef	round
		/* opaque alpha */
		color |= 0xFF;

		tx[realpos] = color;
	      }
	    }
	  }

#undef	extract
	}

	/* next */
	walk++;
      }
    }

    return false;
  }
};

/* besides enabling much less memory-consumption it also allows us to access
 * >2GB files on 32bit operating systems
 */
#define PARTITION_ROWS	((1024UL))
#define PARTITION_SIZE	((1024UL * su) * str)
#define PARTITION_OFFSh	(DWORD)(((unsigned __int64)(sy * su) * str) >> 32)
#define PARTITION_OFFSl	(DWORD)(((unsigned __int64)(sy * su) * str) >>  0)

/* ---------------------------------------------------------------------------- */
void NExtract(SINT32 num) {
  bool rethrow = false;
  char rethrowing[256];

  /* create output file */
//OFSTRUCT of; HANDLE ohx = (HANDLE)OpenFile(weoutx.data(), &of, OF_READWRITE);
  HANDLE oh = CreateFile(weoutn.data(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (oh == (HANDLE)HFILE_ERROR) throw runtime_error("Failed to open output file");

  /* mark them sparse (cool for zeroes) */
  DWORD ss; BOOL
  s = DeviceIoControl(oh, FSCTL_SET_SPARSE, NULL, 0, NULL, 0, &ss, NULL);

  const int su = UPSAMPLE_NORMAL;
  LONG     str = sizeof(unsigned char) * 3; str *= sizex * su;
  LONGLONG len = sizeof(unsigned char) * 3; len *= sizex * su; len *= sizey * su;
  LONG     leh = (LONG)(len >> 32);

  FILE_ZERO_DATA_INFORMATION z;
  z.FileOffset.QuadPart = 0;
  z.BeyondFinalZero.QuadPart = len;

  /* mark all zeros (cool for zeroes) */
  s = DeviceIoControl(oh, FSCTL_SET_ZERO_DATA, &z, sizeof(z), NULL, 0, &ss, NULL);

  SetFilePointer(oh, (LONG)(len >> 0), &leh, FILE_BEGIN); BOOL sfs = SetEndOfFile(oh);
  if (!sfs) throw runtime_error("Failed to resize output file");

  HANDLE mh = CreateFileMapping(oh, NULL, PAGE_READWRITE, 0, 0, NULL);
  if (!mh) throw runtime_error("Failed to map output file");

  for (unsigned long sy = 0; sy < sizey; sy += PARTITION_ROWS) {
    char *mem = (char *)MapViewOfFile(mh, FILE_MAP_ALL_ACCESS, PARTITION_OFFSh, PARTITION_OFFSl, PARTITION_SIZE);
    if (!mem) throw runtime_error("Failed to allocate output file");

    /* this may be 65535 if we want to mark them invalid
    memset(mem, 0x00000000, len); */

    SetTopic("Extracting normals from cells:");
    SetStatus("Extracting normals ...");

    /* intercept for clean-up */
    try {
      switch (col->CollectionType) {
	case eIsOblivion: {
	  ExtractNWorldOp<Ob::WRLDRecord, Ob::CELLRecord, Ob::LANDRecord, Ob::LTEXRecord> *ewo = new 
	  ExtractNWorldOp<Ob::WRLDRecord, Ob::CELLRecord, Ob::LANDRecord, Ob::LTEXRecord>(mem, sy, PARTITION_ROWS);
	  for (SINT32 n = 0; n < num; ++n) {
	    ModFile *mf = GetModIDByLoadOrder(col, n);

	    mf->VisitRecords(REV32(WRLD), *ewo);
	  }

	  delete ewo;
	} break;
	case eIsSkyrim: {
	  ExtractNWorldOp<Sk::WRLDRecord, Sk::CELLRecord, Sk::LANDRecord, Sk::LTEXRecord> *ewo = new 
	  ExtractNWorldOp<Sk::WRLDRecord, Sk::CELLRecord, Sk::LANDRecord, Sk::LTEXRecord>(mem, sy, PARTITION_ROWS);
	  for (SINT32 n = 0; n < num; ++n) {
	    ModFile *mf = GetModIDByLoadOrder(col, n);

	    mf->VisitRecords(REV32(WRLD), *ewo);
	  }

	  delete ewo;
	} break;
      }
    }
    catch (exception &e) {
      strcpy(rethrowing, e.what());
      rethrow = true;

      /* force loop-finish (allow clean-up) */
      sy = sizey;
    }

    UnmapViewOfFile(mem);
  }

  CloseHandle(mh);
  CloseHandle(oh);

  /* after total cleanup, pass the error down */
  if (rethrow) {
    DeleteFile(weoutn.data());
    throw runtime_error(rethrowing);
  }
}

void HExtract(SINT32 num) {
  bool rethrow = false;
  char rethrowing[256];

  /* create output file */
//OFSTRUCT of; HANDLE ohx = (HANDLE)OpenFile(weoutx.data(), &of, OF_READWRITE);
  HANDLE oh = CreateFile(weouth.data(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (oh == (HANDLE)HFILE_ERROR) throw runtime_error("Failed to open output file");

  /* mark them sparse (cool for zeroes) */
  DWORD ss; BOOL
  s = DeviceIoControl(oh, FSCTL_SET_SPARSE, NULL, 0, NULL, 0, &ss, NULL);

  const int su = UPSAMPLE_HEIGHT;
  LONG     str = sizeof(unsigned short) * 1; str *= sizex * su;
  LONGLONG len = sizeof(unsigned short) * 1; len *= sizex * su; len *= sizey * su;
  LONG     leh = (LONG)(len >> 32);

  FILE_ZERO_DATA_INFORMATION z;
  z.FileOffset.QuadPart = 0;
  z.BeyondFinalZero.QuadPart = len;

  /* mark all zeros (cool for zeroes) */
  s = DeviceIoControl(oh, FSCTL_SET_ZERO_DATA, &z, sizeof(z), NULL, 0, &ss, NULL);

  SetFilePointer(oh, (LONG)(len >> 0), &leh, FILE_BEGIN); BOOL sfs = SetEndOfFile(oh);
  if (!sfs) throw runtime_error("Failed to resize output file");

  HANDLE mh = CreateFileMapping(oh, NULL, PAGE_READWRITE, 0, 0, NULL);
  if (!mh) throw runtime_error("Failed to map output file");

  for (unsigned long sy = 0; sy < sizey; sy += PARTITION_ROWS) {
    char *mem = (char *)MapViewOfFile(mh, FILE_MAP_ALL_ACCESS, PARTITION_OFFSh, PARTITION_OFFSl, PARTITION_SIZE);
    if (!mem) throw runtime_error("Failed to allocate output file");

    /* this may be 65535 if we want to mark them invalid
    memset(mem, 0x00000000, len); */

    SetTopic("Extracting heights from cells:");
    SetStatus("Extracting heights ...");

    /* intercept for clean-up */
    try {
      switch (col->CollectionType) {
	case eIsOblivion: {
	  ExtractHWorldOp<Ob::WRLDRecord, Ob::CELLRecord, Ob::LANDRecord, Ob::LTEXRecord> *ewo = new 
	  ExtractHWorldOp<Ob::WRLDRecord, Ob::CELLRecord, Ob::LANDRecord, Ob::LTEXRecord>(mem, sy, PARTITION_ROWS);
	  for (SINT32 n = 0; n < num; ++n) {
	    ModFile *mf = GetModIDByLoadOrder(col, n);

	    mf->VisitRecords(REV32(WRLD), *ewo);
	  }

	  delete ewo;
	} break;
	case eIsSkyrim: {
	  ExtractHWorldOp<Sk::WRLDRecord, Sk::CELLRecord, Sk::LANDRecord, Sk::LTEXRecord> *ewo = new 
	  ExtractHWorldOp<Sk::WRLDRecord, Sk::CELLRecord, Sk::LANDRecord, Sk::LTEXRecord>(mem, sy, PARTITION_ROWS);
	  for (SINT32 n = 0; n < num; ++n) {
	    ModFile *mf = GetModIDByLoadOrder(col, n);

	    mf->VisitRecords(REV32(WRLD), *ewo);
	  }

	  delete ewo;
	} break;
      }
    }
    catch (exception &e) {
      strcpy(rethrowing, e.what());
      rethrow = true;

      /* force loop-finish (allow clean-up) */
      sy = sizey;
    }

    UnmapViewOfFile(mem);
  }

  CloseHandle(mh);
  CloseHandle(oh);

  /* after total cleanup, pass the error down */
  if (rethrow) {
    DeleteFile(weouth.data());
    throw runtime_error(rethrowing);
  }
}

void WExtract(SINT32 num) {
  bool rethrow = false;
  char rethrowing[256];

  /* create output file */
  FILE *file = fopen(weoutw.data(), "wb");
  if (file == NULL) throw runtime_error("Failed to open output file");

  for (unsigned long sy = 0; sy < sizey; sy += PARTITION_ROWS) {
    /* this may be 65535 if we want to mark them invalid
    memset(mem, 0x00000000, len); */

    SetTopic("Extracting water-heights from cells:");
    SetStatus("Extracting water-heights ...");

    /* intercept for clean-up */
    try {
      switch (col->CollectionType) {
	case eIsOblivion: {
	  ExtractWWorldOp<Ob::WRLDRecord, Ob::CELLRecord, Ob::LANDRecord, Ob::LTEXRecord> *ewo = new 
	  ExtractWWorldOp<Ob::WRLDRecord, Ob::CELLRecord, Ob::LANDRecord, Ob::LTEXRecord>(file, sy, PARTITION_ROWS);
	  for (SINT32 n = 0; n < num; ++n) {
	    ModFile *mf = GetModIDByLoadOrder(col, n);

	    mf->VisitRecords(REV32(WRLD), *ewo);
	  }

	  delete ewo;
	} break;
	case eIsSkyrim: {
	  ExtractWWorldOp<Sk::WRLDRecord, Sk::CELLRecord, Sk::LANDRecord, Sk::LTEXRecord> *ewo = new 
	  ExtractWWorldOp<Sk::WRLDRecord, Sk::CELLRecord, Sk::LANDRecord, Sk::LTEXRecord>(file, sy, PARTITION_ROWS);
	  for (SINT32 n = 0; n < num; ++n) {
	    ModFile *mf = GetModIDByLoadOrder(col, n);

	    mf->VisitRecords(REV32(WRLD), *ewo);
	  }

	  delete ewo;
	} break;
      }
    }
    catch (exception &e) {
      strcpy(rethrowing, e.what());
      rethrow = true;

      /* force loop-finish (allow clean-up) */
      sy = sizey;
    }
  }

  fclose(file);

  /* after total cleanup, pass the error down */
  if (rethrow) {
    DeleteFile(weoutw.data());
    throw runtime_error(rethrowing);
  }
}

void MExtract(SINT32 num) {
  bool rethrow = false;
  char rethrowing[256];

  /* create output file */
//OFSTRUCT of; HANDLE ohx = (HANDLE)OpenFile(weoutx.data(), &of, OF_READWRITE);
  HANDLE oh = CreateFile(weoutm.data(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (oh == (HANDLE)HFILE_ERROR) throw runtime_error("Failed to open output file");

  /* mark them sparse (cool for zeroes) */
  DWORD ss; BOOL
  s = DeviceIoControl(oh, FSCTL_SET_SPARSE, NULL, 0, NULL, 0, &ss, NULL);

  const int su = UPSAMPLE_FEATURE;
  LONG     str = sizeof(unsigned char) * 1; str *= sizex * su;
  LONGLONG len = sizeof(unsigned char) * 1; len *= sizex * su; len *= sizey * su;
  LONG     leh = (LONG)(len >> 32);

  FILE_ZERO_DATA_INFORMATION z;
  z.FileOffset.QuadPart = 0;
  z.BeyondFinalZero.QuadPart = len;

  /* mark all zeros (cool for zeroes) */
  s = DeviceIoControl(oh, FSCTL_SET_ZERO_DATA, &z, sizeof(z), NULL, 0, &ss, NULL);

  SetFilePointer(oh, (LONG)(len >> 0), &leh, FILE_BEGIN); BOOL sfs = SetEndOfFile(oh);
  if (!sfs) throw runtime_error("Failed to resize output file");

  HANDLE mh = CreateFileMapping(oh, NULL, PAGE_READWRITE, 0, 0, NULL);
  if (!mh) throw runtime_error("Failed to map output file");

  for (unsigned long sy = 0; sy < sizey; sy += PARTITION_ROWS) {
    char *mem = (char *)MapViewOfFile(mh, FILE_MAP_ALL_ACCESS, PARTITION_OFFSh, PARTITION_OFFSl, PARTITION_SIZE);
    if (!mem) throw runtime_error("Failed to allocate output file");

    /* this may be 65535 if we want to mark them invalid
    memset(mem, 0x00000000, len); */

    SetTopic("Extracting importance from cells:");
    SetStatus("Extracting importance ...");

    /* intercept for clean-up */
    try {
      switch (col->CollectionType) {
	case eIsOblivion: {
	  ExtractMWorldOp<Ob::WRLDRecord, Ob::CELLRecord, Ob::LANDRecord, Ob::LTEXRecord> *ewo = new 
	  ExtractMWorldOp<Ob::WRLDRecord, Ob::CELLRecord, Ob::LANDRecord, Ob::LTEXRecord>(mem, sy, PARTITION_ROWS);
	  for (SINT32 n = 0; n < num; ++n) {
	    ModFile *mf = GetModIDByLoadOrder(col, n);

	    mf->VisitRecords(REV32(WRLD), *ewo);
	  }

	  delete ewo;
	} break;
	case eIsSkyrim: {
	  ExtractMWorldOp<Sk::WRLDRecord, Sk::CELLRecord, Sk::LANDRecord, Sk::LTEXRecord> *ewo = new 
	  ExtractMWorldOp<Sk::WRLDRecord, Sk::CELLRecord, Sk::LANDRecord, Sk::LTEXRecord>(mem, sy, PARTITION_ROWS);
	  for (SINT32 n = 0; n < num; ++n) {
	    ModFile *mf = GetModIDByLoadOrder(col, n);

	    mf->VisitRecords(REV32(WRLD), *ewo);
	  }

	  delete ewo;
	} break;
      }
    }
    catch (exception &e) {
      strcpy(rethrowing, e.what());
      rethrow = true;

      /* force loop-finish (allow clean-up) */
      sy = sizey;
    }

    UnmapViewOfFile(mem);
  }

  CloseHandle(mh);
  CloseHandle(oh);

  /* after total cleanup, pass the error down */
  if (rethrow) {
    DeleteFile(weoutm.data());
    throw runtime_error(rethrowing);
  }
}

void CExtract(SINT32 num) {
  bool rethrow = false;
  char rethrowing[256];

  SetTopic("Building search-db:");
  SetStatus("Building search-db ...");

  /* intercept for clean-up */
  try {
    TextureDBInit();
  }
  catch (exception &e) {
    TextureDBExit();

    throw runtime_error(e.what());
  }

  /* create output file */
//OFSTRUCT of; HANDLE ohx = (HANDLE)OpenFile(weoutx.data(), &of, OF_READWRITE);
  HANDLE oh = CreateFile(weoutx.data(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (oh == (HANDLE)HFILE_ERROR) throw runtime_error("Failed to open output file");

  /* mark them sparse (cool for zeroes) */
  DWORD ss; BOOL
  s = DeviceIoControl(oh, FSCTL_SET_SPARSE, NULL, 0, NULL, 0, &ss, NULL);

  const int su = srfsample;//UPSAMPLE_COLOR;
  LONG     str = sizeof(unsigned long); str *= sizex * su;
  LONGLONG len = sizeof(unsigned long); len *= sizex * su; len *= sizey * su;
  LONG     leh = (LONG)(len >> 32);

  FILE_ZERO_DATA_INFORMATION z;
  z.FileOffset.QuadPart = 0;
  z.BeyondFinalZero.QuadPart = len;

  /* mark all zeros (cool for zeroes) */
  s = DeviceIoControl(oh, FSCTL_SET_ZERO_DATA, &z, sizeof(z), NULL, 0, &ss, NULL);

  SetFilePointer(oh, (LONG)(len >> 0), &leh, FILE_BEGIN); BOOL sfs = SetEndOfFile(oh);
  if (!sfs) throw runtime_error("Failed to resize output file");

  HANDLE mh = CreateFileMapping(oh, NULL, PAGE_READWRITE, 0, 0, NULL);
  if (!mh) throw runtime_error("Failed to map output file");

  for (unsigned long sy = 0; sy < sizey; sy += PARTITION_ROWS) {
    char *mem = (char *)MapViewOfFile(mh, FILE_MAP_ALL_ACCESS, PARTITION_OFFSh, PARTITION_OFFSl, PARTITION_SIZE);
    if (!mem) throw runtime_error("Failed to allocate output file");

    /* this may be 65535 if we want to mark them invalid
    memset(mem, 0x00000000, len); */

    SetTopic("Extracting surfaces from cells:");
    SetStatus("Extracting surfaces ...");

    /* intercept for clean-up */
    try {
      switch (col->CollectionType) {
	case eIsOblivion: {
	  ExtractCWorldOp<Ob::WRLDRecord, Ob::CELLRecord, Ob::LANDRecord, Ob::LTEXRecord> *ewo = new 
	  ExtractCWorldOp<Ob::WRLDRecord, Ob::CELLRecord, Ob::LANDRecord, Ob::LTEXRecord>(mem, sy, PARTITION_ROWS);
	  for (SINT32 n = 0; n < num; ++n) {
	    ModFile *mf = GetModIDByLoadOrder(col, n);

	    mf->VisitRecords(REV32(WRLD), *ewo);
	  }

	  delete ewo;
	} break;
	case eIsSkyrim: {
	  ExtractCWorldOp<Sk::WRLDRecord, Sk::CELLRecord, Sk::LANDRecord, Sk::LTEXRecord> *ewo = new 
	  ExtractCWorldOp<Sk::WRLDRecord, Sk::CELLRecord, Sk::LANDRecord, Sk::LTEXRecord>(mem, sy, PARTITION_ROWS);
	  for (SINT32 n = 0; n < num; ++n) {
	    ModFile *mf = GetModIDByLoadOrder(col, n);

	    mf->VisitRecords(REV32(WRLD), *ewo);
	  }

	  delete ewo;
	} break;
      }
    }
    catch (exception &e) {
      strcpy(rethrowing, e.what());
      rethrow = true;

      /* force loop-finish (allow clean-up) */
      sy = sizey;
    }

    UnmapViewOfFile(mem);
  }

  CloseHandle(mh);
  CloseHandle(oh);

  TextureDBExit();

  /* after total cleanup, pass the error down */
  if (rethrow) {
    DeleteFile(weoutx.data());
    throw runtime_error(rethrowing);
  }
}

/* ---------------------------------------------------------------------------- */
DWORD __stdcall ExtractFromCollection(LPVOID lp) {
  SetStatus("Loading plugins ...");
  col = (Collection *)lp;
  ltex.clear();

  SetStatus("Skimming plugins for heights ...");
  SINT32 num = GetLoadOrderNumMods(col);

  /* determine extents */
  wleftx =  1024 * 1024;
  wrghtx = -1024 * 1024;
  wtopy  =  1024 * 1024;
  wboty  = -1024 * 1024;

  switch (col->CollectionType) {
    case eIsOblivion: {
      raisecontrast = 1.0f;
      upperbrightness = pow(255.0f / 255.0f, 2.2f);

      ExtendsWorldOp<Ob::WRLDRecord, Ob::CELLRecord, Ob::LANDRecord, Ob::LTEXRecord> *wo = new
      ExtendsWorldOp<Ob::WRLDRecord, Ob::CELLRecord, Ob::LANDRecord, Ob::LTEXRecord>();
      for (SINT32 n = 0; n < num; ++n) {
	ModFile *mf = GetModIDByLoadOrder(col, n);

	mf->VisitRecords(REV32(WRLD), *wo);
      }

      delete wo;
    } break;
    case eIsSkyrim: {
      raisecontrast = 1.25f;
      upperbrightness = pow(124.0f / 255.0f, 2.2f);

      ExtendsWorldOp<Sk::WRLDRecord, Sk::CELLRecord, Sk::LANDRecord, Sk::LTEXRecord> *wo = new
      ExtendsWorldOp<Sk::WRLDRecord, Sk::CELLRecord, Sk::LANDRecord, Sk::LTEXRecord>();
      for (SINT32 n = 0; n < num; ++n) {
	ModFile *mf = GetModIDByLoadOrder(col, n);

	mf->VisitRecords(REV32(WRLD), *wo);
      }

      delete wo;
    } break;
  }

  /* align to multiples of 32 cells */
  int tlmt, tsze, csze;

  tlmt = 32;
  tsze = 32;
  csze = 32;

  /* must include "0" (origin) */
  wrghtx = max(0, wrghtx);
  wleftx = min(0, wleftx);
  wboty  = max(0, wboty );
  wtopy  = min(0, wtopy );

  int rx = wrghtx % tlmt; if (rx < 0) wrghtx -= tlmt + rx; else if (rx > 0) wrghtx += tlmt - rx; 
  int ry = wboty  % tlmt; if (ry < 0) wboty  -= tlmt + ry; else if (ry > 0) wboty  += tlmt - ry; 

      rx = wleftx % tlmt; if (rx < 0) wleftx -= tlmt + rx; else if (rx > 0) wleftx += tlmt - rx; 
      ry = wtopy  % tlmt; if (ry < 0) wtopy  -= tlmt + ry; else if (ry > 0) wtopy  += tlmt - ry; 

  /* make sure both extends are of the same magnitude (0 at center) */
  if (wrghtx < (-wleftx - 0   )) wrghtx = -wleftx - 0;	  /* [-64,32] -> [-64,64] */
  if (wboty  < (-wtopy  - 0   )) wboty  = -wtopy  - 0;	  /* [-32,64] -> [-32,64] */
  if (wleftx > (-wrghtx + tlmt)) wleftx = -wrghtx + tlmt; /* [  0,64] -> [-32,64] */
  if (wtopy  > (-wboty  + tlmt)) wtopy  = -wboty  + tlmt; /* [ 32,64] -> [-32,64] */

  sizex = wrghtx - wleftx;
  sizey = wboty  - wtopy;

  /* get higher deviation from 0 */
  offsx  = (tlmt * ((sizex / tlmt) >> 1));
  offsy  = (tlmt * ((sizey / tlmt) >> 1));

#if 0
  /* get higher deviation from 0 */
  offsx  = 0;
  offsy  = 0;

  /* because of (size >> 1) rounddown the positive side can be one tile bigger */
  long extx = min(wleftx, -(wrghtx - 32));
  long exty = min(wtopy , -(wboty  - 32));

  while (extx < 0) extx += 32, offsx += 32;
  while (exty < 0) exty += 32, offsy += 32;

//sizex = (offsx << 1);
//sizey = (offsy << 1);

//while (sizex < (offsx + wrghtx)) sizex += 32;
//while (sizey < (offsy + wboty )) sizey += 32;

  /* because of (size >> 1) rounddown the positive side can be one tile bigger */
  sizex = (offsx + max(wrghtx, offsx));
  sizey = (offsy + max(wboty , offsy));

  if (sizex % 32) sizex += 32 - (sizex % 32);
  if (sizey % 32) sizey += 32 - (sizey % 32);

//sizex += 1;
//sizey += 1;
#endif

  /* we always want multiple of 1024 */
  sizex *= 32;
  sizey *= 32;

  if (!sizex || !sizey) {
    throw runtime_error("There is no data about this worldspace in the active plugin-list.");
  }

  int numpasses = 0;

  numpasses += calcn ? 1 : 0;
  numpasses += calch ? 1 : 0;
  numpasses += calcw ? 1 : 0;
  numpasses += calcx ? 1 : 0;
  numpasses += calcm ? 1 : 0;

  InitProgress(cells * numpasses, 0.0); dells = 0;
  srandom(cells * numpasses);

  if (calcn) NExtract(num);
  if (calch) HExtract(num);
  if (calcw) WExtract(num);
  if (calcm) MExtract(num);
  if (calcx) CExtract(num);

  SetStatus("Ready");
  ltex.clear();
  return 0;
}

#endif
