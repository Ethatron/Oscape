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

#include "texture.hpp"
#include "texture-dds.hpp"

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
