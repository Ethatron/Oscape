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

#pragma comment(lib,"CBash")

#ifdef	COMPILING_STATIC
#if   _MSC_VER == 1500
#define LIBDIR	"vc9"
#elif _MSC_VER == 1600
#define LIBDIR	"vc10"
#endif

#ifndef	NDEBUG
#ifdef	_WIN64
#pragma comment(lib,"../../../NIFopt/zlib-1.2.3/" LIBDIR "/x64/Debug/libz")
#else
#pragma comment(lib,"../../../NIFopt/zlib-1.2.3/" LIBDIR "/Debug/libz")
#endif
#else
#ifdef	_WIN64
#pragma comment(lib,"../../../NIFopt/zlib-1.2.3/" LIBDIR "/x64/Release/libz")
#else
#pragma comment(lib,"../../../NIFopt/zlib-1.2.3/" LIBDIR "/Release/libz")
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

/* ---------------------------------------------------- */

void InitProgress(int rng, double err);
void SetProgress(int dne, double err);

void InitProgress(int rng);
void SetProgress(int dne);
void SetStatus(const char *status);
void SetTopic(const char *topic);
void SetTopic(const char *topic, int a, int b);

void InitProgress(int rng, int srng);
void SetProgress(int dne, int sdne);
void PollProgress();

bool IsOlder(const char *name, __int64 tschk);

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

#include "Oscape_CBash-io.C"

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

string weoutn; bool calcn = false;
string weouth; bool calch = false;
string weoutw; bool calcw = false;
string weoutx; bool calcx = false;
string weoutm; bool calcm = false;
string wename; int weid;

/* flip y */
#define rofx  0
#define rofy  1			// why, oh, why?
#define lofx  (     -1)		// first column is previous cell
#define lofy  (-32 - 1)		// first row is previous cell, y is inverted

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

#undef	TEXTURE1x1
#undef	TEXTURE2x2

/* global so it's much faster to search */
struct landtex {
//Ob::LTEXRecord *rec;
  union {
    void *rec;

    Ob::LTEXRecord *reco;
    Sk::LTEXRecord *recs;
  };

  LPDIRECT3DTEXTURE9 tex;
  unsigned long mem1x1[1*1];
  unsigned long mem2x2[2*2];
  unsigned char classification;
};

map<FORMID, struct landtex> ltex;

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

  return "Landscape\\dirt01.dds";
}

/* ............................................................................ */

template<
  class LTEXRecord /*= Ob::LTEXRecord*/
>
unsigned long *LookupTexture(FORMID t) {
  /* what does a texture of 0 mean? (assume default.dds) */
  if (t && !ltex[t].rec) {
    /* find the record */
    ModFile *WinningModFile;
    Record *WinningRecord;

    col->LookupWinningRecord(t, WinningModFile, WinningRecord, false);
    if (!WinningRecord)
      return NULL;
    ltex[t].rec = (LTEXRecord *)WinningRecord;
  }

  if (!ltex[t].tex) {
    /* read in the color */
    const char *name = NULL;
    LPDIRECT3DTEXTURE9 tex = NULL;
    LTEXRecord *rec;

    if (t && (rec = (LTEXRecord *)ltex[t].rec))
      name = GetTextureName(rec);
    else
      name = GetTextureName((LTEXRecord *)NULL);

    if (!name)
      return NULL;
    if (!(ltex[t].tex = tex = TextureSearch(name)))
      return NULL;

    /* 1x1 ------------------------------------------------------- */
    DWORD low = max(tex->GetLevelCount(), 1) - 1;
    D3DSURFACE_DESC texo;
    D3DLOCKED_RECT texs;

    tex->GetLevelDesc(low, &texo);
    tex->LockRect(low, &texs, NULL, 0);
    ULONG *sTex = (ULONG *)texs.pBits;

    unsigned long r = 0, g = 0, b = 0, a = 0;
    for (int y = 0; y < (int)texo.Height; y += 1)
    for (int x = 0; x < (int)texo.Width ; x += 1) {
      ULONG t = sTex[(y * texo.Width) + x];

      r += (t >> 16) & 0xFF;
      g += (t >>  8) & 0xFF;
      b += (t >>  0) & 0xFF;
      a += (t >> 24) & 0xFF;
    }

#define	normalize(c, pos)  (((c / (texo.Width * texo.Height)) & 0xFF) << pos)
    ltex[t].mem1x1[0] =
      normalize(r, 24) |
      normalize(g, 16) |
      normalize(b,  8) |
      normalize(a,  0);
#undef	normalize

    tex->UnlockRect(low);

    /* 2x2 ------------------------------------------------------- */
    low = max(tex->GetLevelCount(), 2) - 2;

    tex->GetLevelDesc(low, &texo);
    tex->LockRect(low, &texs, NULL, 0);
    sTex = (ULONG *)texs.pBits;

    for (int m = 0; m < 4; m += 1) {
      r = 0, g = 0, b = 0, a = 0;

      int by = (!(m & 1) ? 0 : (int)texo.Height / 2);
      int bx = (!(m & 2) ? 0 : (int)texo.Height / 2);

      int my = (!(m & 1) ? (int)texo.Height / 2 : (int)texo.Height);
      int mx = (!(m & 2) ? (int)texo.Height / 2 : (int)texo.Width );

      for (int y = by; y < my; y += 1)
      for (int x = bx; x < mx; x += 1) {
	ULONG t = sTex[(y * texo.Width) + x];

	r += (t >> 16) & 0xFF;
	g += (t >>  8) & 0xFF;
	b += (t >>  0) & 0xFF;
	a += (t >> 24) & 0xFF;
      }

#define	normalize(c, pos)  (((c / ((mx - bx) * (my - by))) & 0xFF) << pos)
      ltex[t].mem2x2[m] =
	normalize(r, 24) |
	normalize(g, 16) |
	normalize(b,  8) |
	normalize(a,  0);
#undef	normalize
    }

    tex->UnlockRect(low);
  }

#ifdef TEXTURE2x2
  return ltex[t].mem2x2;
#endif
  return ltex[t].mem1x1;
}

template<
  class LTEXRecord /*= Ob::LTEXRecord*/
>
unsigned char ClassifyTexture(FORMID t) {
  /* what does a texture of 0 mean? (assume default.dds) */
  if (t && !ltex[t].rec) {
    /* find the record */
    ModFile *WinningModFile;
    Record *WinningRecord;

    col->LookupWinningRecord(t, WinningModFile, WinningRecord, false);
    if (!WinningRecord)
      return NULL;
    ltex[t].rec = (LTEXRecord *)WinningRecord;
  }

  if (!ltex[t].classification) {
    /* read in the name */
    const char *name = NULL;
    LTEXRecord *rec;

    if (t && (rec = (LTEXRecord *)ltex[t].rec))
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

    ltex[t].classification = cls;
  }

  return ltex[t].classification;
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
  inline bool rchk(int realx, int realy) {
    realy -= begin;

    if ((realx < 0) || (realx >= sizex))
      return false;
    if ((realy < 0) || (realy >= range))
      return false;

    return true;
  }

//#define rpos(realx, realy) ((/*(sizey - 1) -*/ realy) * sizex) + (realx)
  /* real position (relative to memory) */
  inline unsigned long rpos(int realx, int realy) {
    realy -= begin;

    return ((realy) * sizex) + (realx);
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

	if (cell->XCLC.IsLoaded() && land && !land->IsWinning()) {
	  printf("skipping");
	}

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

	if (cell->XCLC.IsLoaded() && land && !land->IsWinning()) {
	  printf("skipping");
	}

	if (cell->XCLC.IsLoaded() && land && land->IsWinning()) {
	  long leftx = (cell->XCLC->posX + rofx);
	  long topy  = (cell->XCLC->posY + rofy);
	  long leftc = (offsx + leftx) * 32;
	  long topc  = (offsy + topy ) * 32;

	  if ((cell->XCLC->posX == 0) && (cell->XCLC->posY == 0)) {
	    printf("stopping");

	    /* XCLW: FFFF7F7F == no water */
	    
//	    fprintf(stdout, "XCLW: 0x%08x\n", *((long *)&cell->XCLW.value));
//	    fprintf(stdout, "XWCN: 0x%08x\n", *((long *)&cell->XWCN.value));
//	    fprintf(stdout, "XWCU: 0x%08x 0x%08x 0x%08x 0x%08x\n", *((long *)&cell->XWCU.value[ 0]), *((long *)&cell->XWCU.value[ 4]), *((long *)&cell->XWCU.value[ 8]), *((long *)&cell->XWCU.value[12]));
//	    fprintf(stdout, "XWCU: 0x%08x 0x%08x 0x%08x 0x%08x\n", *((long *)&cell->XWCU.value[16]), *((long *)&cell->XWCU.value[20]), *((long *)&cell->XWCU.value[24]), *((long *)&cell->XWCU.value[28]));
	  }

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

	if (cell->XCLC.IsLoaded() && land && !land->IsWinning()) {
	  printf("skipping");
	}

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

	if (cell->XCLC.IsLoaded() && land && !land->IsWinning()) {
	  printf("skipping");
	}

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
	    for (int l = 0; l < 0x8; l++) {
	      std::vector<LANDRecord::LANDLAYERS *>::iterator srch = land->Layers.value.begin();
	      while (srch != land->Layers.value.end()) {
		LANDRecord::LANDGENTXT *walk = &(*srch)->ATXT.value;
		if (walk->layer == l) {
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

	if (cell->XCLC.IsLoaded() && land && !land->IsWinning()) {
	  printf("skipping");
	}

	if (cell->XCLC.IsLoaded() && land && land->IsWinning()) {
	  long leftx = (cell->XCLC->posX + rofx);
	  long topy  = (cell->XCLC->posY + rofy);
	  long leftc = (offsx + leftx) * 32;
	  long topc  = (offsy + topy ) * 32;
	  float bbuf[33][33][4] = {0};

	  /* check cell-center against range, no problems with the overlap that way */
	  if (!cchk(leftc + 16 + lofx, topc + 16 + lofy)) {
	    /* next */ walk++; continue; }

	  SetTopic("Extracting surfaces from cell {%d,%d}", leftx, topy);
	  SetProgress(dells++);

#define extract(c, pos)		(float)((c >> pos) & 0xFF)

	  if (EXTRACT_GROUNDLAYER) {
	    /* search for the texture and get the average color of it */
	    unsigned long *tex = LookupTexture<LTEXRecord>(0);
	    /* don't apply missing textures (instead of applying black) */
	    if (tex) {
	      for (int y = 0; y <= 32; y++)
	      for (int x = 0; x <= 32; x++) {
		unsigned long ctext = *tex;
#ifdef TEXTURE2x2
		ctext = tex[((x & 1) << 0) | ((y & 1) << 1)];
#endif

		bbuf[y][x][0] = extract(ctext, 24);
		bbuf[y][x][1] = extract(ctext, 16);
		bbuf[y][x][2] = extract(ctext,  8);
		bbuf[y][x][3] = extract(ctext,  0);
	      }
	    }
	  }

	  if (land->VTEX.IsLoaded()) {
	    std::vector<FORMID>::iterator walk = land->VTEX.value.begin();
	    while (walk != land->VTEX.value.end()) {
	      /* search for the texture and get the average color of it */
	      unsigned long *tex = LookupTexture<LTEXRecord>(*walk);

	      walk++;
	    }
	  }

	  if (EXTRACT_BASELAYER)
	  if (land->BTXT.IsLoaded()) {
	    std::vector<LANDRecord::LANDGENTXT *>::iterator walk = land->BTXT.value.begin();
	    while (walk != land->BTXT.value.end()) {
	      /* search for the texture and get the average color of it */
	      unsigned long *tex = LookupTexture<LTEXRecord>((*walk)->texture);
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

		  unsigned long ctext = *tex;
#ifdef TEXTURE2x2
		  ctext = tex[((x & 1) << 0) | ((y & 1) << 1)];
#endif
		  float alpha = extract(ctext, 0);

		  /* blending with alpha from texture looks bad,
		   * in theory the alpha should be a max()-map anyway
		   */
#define blend(B, a, A, b, pos)	(						\
		      ((1.0f - (B/* * A / 0xFF*/)) * ((a       )       )) +	\
		      ((       (B/* * A / 0xFF*/)) * ((b >> pos) & 0xFF))	\
		    )

		  bbuf[y][x][0] = blend(1.0f, bbuf[y][x][0], alpha, ctext, 24);
		  bbuf[y][x][1] = blend(1.0f, bbuf[y][x][1], alpha, ctext, 16);
		  bbuf[y][x][2] = blend(1.0f, bbuf[y][x][2], alpha, ctext,  8);
		  bbuf[y][x][3] = blend(1.0f, bbuf[y][x][3], alpha, ctext,  0);
#undef	blend
		}
	      }

	      walk++;
	    }
	  }

	  /* overlay one layer after the other */
	  if (EXTRACT_BLENDLAYER)
	  if (land->Layers.IsLoaded()) {
	    for (int l = 0; l < 0x8; l++) {
	      std::vector<LANDRecord::LANDLAYERS *>::iterator srch = land->Layers.value.begin();
	      while (srch != land->Layers.value.end()) {
		LANDRecord::LANDGENTXT *walk = &(*srch)->ATXT.value;
		if (walk->layer == l) {
		  /* search for the texture and get the average color of it */
		  unsigned long *tex = LookupTexture<LTEXRecord>((*walk).texture);
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

		      unsigned long ctext = *tex;
#ifdef TEXTURE2x2
		      ctext = tex[((x & 1) << 0) | ((y & 1) << 1)];
#endif
		      float alpha = extract(ctext, 0);

		      /* blending with alpha from texture looks bad,
		       * in theory the alpha should be a max()-map anyway
		       */
#define blend(B, a, A, b, pos)	(						\
			  ((1.0f - (B/* * A / 0xFF*/)) * ((a       )       )) +	\
			  ((       (B/* * A / 0xFF*/)) * ((b >> pos) & 0xFF))	\
			)

		      bbuf[y][x][0] = blend(opc, bbuf[y][x][0], alpha, ctext, 24);
		      bbuf[y][x][1] = blend(opc, bbuf[y][x][1], alpha, ctext, 16);
		      bbuf[y][x][2] = blend(opc, bbuf[y][x][2], alpha, ctext,  8);
		      bbuf[y][x][3] = blend(opc, bbuf[y][x][3], alpha, ctext,  0);
#undef	blend

		      over++;
		    }
		  }
		}

		srch++;
	      }
	    }
	  }

	  /* multiply both colors in the cell */
	  if (EXTRACT_OVERLAYER)
	  if (land->VCLR.IsLoaded()) {
	    for (int y = 0; y <= 32; y ++)
	    for (int x = 0; x <= 32; x ++) {

#define multiply(a, b, pos)	((a * b) / 0xFF)

	      bbuf[y][x][0] = multiply(bbuf[y][x][0], land->VCLR->VCLR[y][x].red  , 24);
	      bbuf[y][x][1] = multiply(bbuf[y][x][1], land->VCLR->VCLR[y][x].green, 16);
	      bbuf[y][x][2] = multiply(bbuf[y][x][2], land->VCLR->VCLR[y][x].blue ,  8);
	      bbuf[y][x][3] = multiply(bbuf[y][x][3], 0xFF                        ,  0);
#undef	multiply

	    }
	  }

	  /* place the cell into the destination */
	  for (int y = 0; y <= 32; y++)
	  for (int x = 0; x <= 32; x++) {
	    int realx = leftc + x + lofx;
	    int realy = topc  + y + lofy;

	    if (rchk(realx, realy)) {
	      unsigned long realpos = rpos(realx, realy);

#define round(clr, pos)	(min((unsigned long)floor(clr + 0.5), 0xFF) << pos)

	      unsigned long
	      color  = 0;
	      color |= round(bbuf[y][x][0], 24);
	      color |= round(bbuf[y][x][1], 16);
	      color |= round(bbuf[y][x][2],  8);
	      color |= round(bbuf[y][x][3],  0);
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

/* besides enabling much less memory-consumption it also allows us to access
 * >2GB files on 32bit operating systems
 */
#define PARTITION_ROWS	(1024UL)
#define PARTITION_SIZE	(1024UL * str)
#define PARTITION_OFFSh	(DWORD)(((unsigned __int64)sy * str) >> 32)
#define PARTITION_OFFSl	(DWORD)(((unsigned __int64)sy * str) >>  0)

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

//DWORD lenh = GetFileSize(oh, NULL);
  DWORD str =         sizex * sizeof(unsigned char) * 3;
  DWORD len = sizey * sizex * sizeof(unsigned char) * 3;

  FILE_ZERO_DATA_INFORMATION z;
  z.FileOffset.QuadPart = 0;
  z.BeyondFinalZero.QuadPart = len;

  /* mark all zeros (cool for zeroes) */
  s = DeviceIoControl(oh, FSCTL_SET_ZERO_DATA, &z, sizeof(z), NULL, 0, &ss, NULL);

  SetFilePointer(oh, len, NULL, FILE_BEGIN); BOOL sfs = SetEndOfFile(oh);
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
	  ExtractNWorldOp<Ob::WRLDRecord, Ob::CELLRecord, Ob::LANDRecord, Ob::LTEXRecord> ewo(mem, sy, PARTITION_ROWS);
	  for (SINT32 n = 0; n < num; ++n) {
	    ModFile *mf = GetModIDByLoadOrder(col, n);

	    mf->VisitRecords(REV32(WRLD), ewo);
	  }
	} break;
	case eIsSkyrim: {
	  ExtractNWorldOp<Sk::WRLDRecord, Sk::CELLRecord, Sk::LANDRecord, Sk::LTEXRecord> ewo(mem, sy, PARTITION_ROWS);
	  for (SINT32 n = 0; n < num; ++n) {
	    ModFile *mf = GetModIDByLoadOrder(col, n);

	    mf->VisitRecords(REV32(WRLD), ewo);
	  }
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

//DWORD lenh = GetFileSize(oh, NULL);
  DWORD str =         sizex * sizeof(unsigned short) * 1;
  DWORD len = sizey * sizex * sizeof(unsigned short) * 1;

  FILE_ZERO_DATA_INFORMATION z;
  z.FileOffset.QuadPart = 0;
  z.BeyondFinalZero.QuadPart = len;

  /* mark all zeros (cool for zeroes) */
  s = DeviceIoControl(oh, FSCTL_SET_ZERO_DATA, &z, sizeof(z), NULL, 0, &ss, NULL);

  SetFilePointer(oh, len, NULL, FILE_BEGIN); BOOL sfs = SetEndOfFile(oh);
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
	  ExtractHWorldOp<Ob::WRLDRecord, Ob::CELLRecord, Ob::LANDRecord, Ob::LTEXRecord> ewo(mem, sy, PARTITION_ROWS);
	  for (SINT32 n = 0; n < num; ++n) {
	    ModFile *mf = GetModIDByLoadOrder(col, n);

	    mf->VisitRecords(REV32(WRLD), ewo);
	  }
	} break;
	case eIsSkyrim: {
	  ExtractHWorldOp<Sk::WRLDRecord, Sk::CELLRecord, Sk::LANDRecord, Sk::LTEXRecord> ewo(mem, sy, PARTITION_ROWS);
	  for (SINT32 n = 0; n < num; ++n) {
	    ModFile *mf = GetModIDByLoadOrder(col, n);

	    mf->VisitRecords(REV32(WRLD), ewo);
	  }
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
	  ExtractWWorldOp<Ob::WRLDRecord, Ob::CELLRecord, Ob::LANDRecord, Ob::LTEXRecord> ewo(file, sy, PARTITION_ROWS);
	  for (SINT32 n = 0; n < num; ++n) {
	    ModFile *mf = GetModIDByLoadOrder(col, n);

	    mf->VisitRecords(REV32(WRLD), ewo);
	  }
	} break;
	case eIsSkyrim: {
	  ExtractWWorldOp<Sk::WRLDRecord, Sk::CELLRecord, Sk::LANDRecord, Sk::LTEXRecord> ewo(file, sy, PARTITION_ROWS);
	  for (SINT32 n = 0; n < num; ++n) {
	    ModFile *mf = GetModIDByLoadOrder(col, n);

	    mf->VisitRecords(REV32(WRLD), ewo);
	  }
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

//DWORD lenh = GetFileSize(oh, NULL);
  DWORD str =         sizex * sizeof(unsigned char) * 1;
  DWORD len = sizey * sizex * sizeof(unsigned char) * 1;

  FILE_ZERO_DATA_INFORMATION z;
  z.FileOffset.QuadPart = 0;
  z.BeyondFinalZero.QuadPart = len;

  /* mark all zeros (cool for zeroes) */
  s = DeviceIoControl(oh, FSCTL_SET_ZERO_DATA, &z, sizeof(z), NULL, 0, &ss, NULL);

  SetFilePointer(oh, len, NULL, FILE_BEGIN); BOOL sfs = SetEndOfFile(oh);
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
	  ExtractMWorldOp<Ob::WRLDRecord, Ob::CELLRecord, Ob::LANDRecord, Ob::LTEXRecord> ewo(mem, sy, PARTITION_ROWS);
	  for (SINT32 n = 0; n < num; ++n) {
	    ModFile *mf = GetModIDByLoadOrder(col, n);

	    mf->VisitRecords(REV32(WRLD), ewo);
	  }
	} break;
	case eIsSkyrim: {
	  ExtractMWorldOp<Sk::WRLDRecord, Sk::CELLRecord, Sk::LANDRecord, Sk::LTEXRecord> ewo(mem, sy, PARTITION_ROWS);
	  for (SINT32 n = 0; n < num; ++n) {
	    ModFile *mf = GetModIDByLoadOrder(col, n);

	    mf->VisitRecords(REV32(WRLD), ewo);
	  }
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

//DWORD lenh = GetFileSize(oh, NULL);
  DWORD str =         sizex * sizeof(unsigned long) * 1;
  DWORD len = sizey * sizex * sizeof(unsigned long) * 1;

  FILE_ZERO_DATA_INFORMATION z;
  z.FileOffset.QuadPart = 0;
  z.BeyondFinalZero.QuadPart = len;

  /* mark all zeros (cool for zeroes) */
  s = DeviceIoControl(oh, FSCTL_SET_ZERO_DATA, &z, sizeof(z), NULL, 0, &ss, NULL);

  SetFilePointer(oh, len, NULL, FILE_BEGIN); BOOL sfs = SetEndOfFile(oh);
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
	  ExtractCWorldOp<Ob::WRLDRecord, Ob::CELLRecord, Ob::LANDRecord, Ob::LTEXRecord> ewo(mem, sy, PARTITION_ROWS);
	  for (SINT32 n = 0; n < num; ++n) {
	    ModFile *mf = GetModIDByLoadOrder(col, n);

	    mf->VisitRecords(REV32(WRLD), ewo);
	  }
	} break;
	case eIsSkyrim: {
	  ExtractCWorldOp<Sk::WRLDRecord, Sk::CELLRecord, Sk::LANDRecord, Sk::LTEXRecord> ewo(mem, sy, PARTITION_ROWS);
	  for (SINT32 n = 0; n < num; ++n) {
	    ModFile *mf = GetModIDByLoadOrder(col, n);

	    mf->VisitRecords(REV32(WRLD), ewo);
	  }
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
      ExtendsWorldOp<Ob::WRLDRecord, Ob::CELLRecord, Ob::LANDRecord, Ob::LTEXRecord> wo;
      for (SINT32 n = 0; n < num; ++n) {
	ModFile *mf = GetModIDByLoadOrder(col, n);

	mf->VisitRecords(REV32(WRLD), wo);
      }
    } break;
    case eIsSkyrim: {
      ExtendsWorldOp<Sk::WRLDRecord, Sk::CELLRecord, Sk::LANDRecord, Sk::LTEXRecord> wo;
      for (SINT32 n = 0; n < num; ++n) {
	ModFile *mf = GetModIDByLoadOrder(col, n);

	mf->VisitRecords(REV32(WRLD), wo);
      }
    } break;
  }

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
