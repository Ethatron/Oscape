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
