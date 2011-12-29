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

#include "GUI.hpp"

  char *stristr(const char *s, const char *find);

  /* ---------------------------------------------------------------------------- */
  DWORD __stdcall HeightfieldRecover(LPVOID lp) {
    if (gui)
      gui->HeightfieldRecover();
    return 0;
  }

  /* ---------------------------------------------------------------------------- */
  void OscapeGUI::ResetLPluginDir(bool init) {
    if (init) {
      LPath[0] = 0; DWORD LPL = 1023;
      RegGetValue(Settings, GetGameKey(), "LOD Directory", RRF_RT_REG_SZ, NULL, &LPath, &LPL), LPL = 1023;

      if (!LPath[0]) {
	strcpy(LPath, IPath);

	/**/ if (OSGame->FindItem(wxID_OBLIVON)->IsChecked())
	  strcat(LPath, "\\Meshes\\Landscape\\LOD");
	else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked())
	  strcat(LPath, "\\Meshes\\Terrain\\Tamriel");
      }

      size_t LLen = strlen(LPath) - 1;
      while (LPath[LLen] == '\\')
	LPath[LLen--] = '\0';

      OSLODDir->SetPath(LPath);
    }

    int ws, cx, cy, rs;
    char PPath[1024];
    set<int> lws;
    set<int> lrs;

    lodfils.clear();
    lsorted.clear();

    if (OSGame->FindItem(wxID_OBLIVON)->IsChecked()) {
#if 0
      /**/ if (OSRevHeight->GetValue()) {
  //    _snprintf(PPath, sizeof(PPath) - 1, "%s\\Meshes\\Landscape\\LOD\\*.nif", LPath);
	_snprintf(PPath, sizeof(PPath) - 1, "%s\\*.nif", LPath);
      }
      else if (OSRevNormals->GetValue()) {
  //    _snprintf(PPath, sizeof(PPath) - 1, "%s\\Textures\\LandscapeLOD\\Generated\\*.dds", LPath);
	_snprintf(PPath, sizeof(PPath) - 1, "%s\\*.dds", LPath);
      }
      else if (OSRevColors->GetValue()) {
  //    _snprintf(PPath, sizeof(PPath) - 1, "%s\\Textures\\LandscapeLOD\\Generated\\*.dds", LPath);
	_snprintf(PPath, sizeof(PPath) - 1, "%s\\*.dds", LPath);
      }
#else
  //  _snprintf(PPath, sizeof(PPath) - 1, "%s\\Textures\\LandscapeLOD\\Generated\\*.nif", LPath);
  //  _snprintf(PPath, sizeof(PPath) - 1, "%s\\Meshes\\Landscape\\LOD\\*.*", LPath);
      _snprintf(PPath, sizeof(PPath) - 1, "%s\\*.*", LPath);
#endif

      HANDLE LFiles; WIN32_FIND_DATA LFound;
      if ((LFiles = FindFirstFileEx(PPath, FindExInfoBasic, &LFound, FindExSearchNameMatch, NULL, 0)) != INVALID_HANDLE_VALUE) {
	do {
	  const char *subfile = LFound.cFileName;
	  if (subfile && sscanf(subfile, "%d.%d.%d.%d", &ws, &cx, &cy, &rs) == 4) {
	    lws.insert(ws);
	    lrs.insert(rs);

	    time_t       tme = LFound.ftLastWriteTime.dwHighDateTime;
	    tme <<= 32; tme |= LFound.ftLastWriteTime.dwLowDateTime;
	    lodfile *set = &lodfils[string(LFound.cFileName)];

	    set->tme = tme;
	    set->act = true;
	  }
	} while (FindNextFile(LFiles, &LFound));

	FindClose(LFiles);
      }
    }
    else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked()) {
#if 0
      /**/ if (OSRevHeight->GetValue()) {
  //    _snprintf(PPath, sizeof(PPath) - 1, "%s\\Meshes\\Terrain\\*.btr", LPath);
	_snprintf(PPath, sizeof(PPath) - 1, "%s\\*.btr", LPath);
      }
      else if (OSRevNormals->GetValue()) {
  //    _snprintf(PPath, sizeof(PPath) - 1, "%s\\Textures\\Terrain\\*.dds", LPath);
	_snprintf(PPath, sizeof(PPath) - 1, "%s\\*.dds", LPath);
      }
      else if (OSRevColors->GetValue()) {
  //    _snprintf(PPath, sizeof(PPath) - 1, "%s\\Textures\\Terrain\\*.dds", LPath);
	_snprintf(PPath, sizeof(PPath) - 1, "%s\\*.dds", LPath);
      }
#else
  //  _snprintf(PPath, sizeof(PPath) - 1, "%s\\Meshes\\Terrain\\*", LPath);
  //  _snprintf(PPath, sizeof(PPath) - 1, "%s\\Textures\\Terrain\\*", LPath);
      _snprintf(PPath, sizeof(PPath) - 1, "%s\\*.*", LPath);
#endif

      HANDLE LFiles; WIN32_FIND_DATA LFound;
      if ((LFiles = FindFirstFileEx(PPath, FindExInfoBasic, &LFound, FindExSearchNameMatch, NULL, 0)) != INVALID_HANDLE_VALUE) {
	do {
	  const char *subfile = strchr(LFound.cFileName, '.');
	  if (subfile && sscanf(subfile, ".%d.%d.%d", &rs, &cx, &cy) == 3) {
	    ws = GetWorldspacePrefix(LFound.cFileName);
	    if (ws)
	      lws.insert(ws);
	    lrs.insert(rs);

	    time_t       tme = LFound.ftLastWriteTime.dwHighDateTime;
	    tme <<= 32; tme |= LFound.ftLastWriteTime.dwLowDateTime;
	    lodfile *set = &lodfils[string(LFound.cFileName)];

	    set->tme = tme;
	    set->act = true;
	  }
	} while (FindNextFile(LFiles, &LFound));

	FindClose(LFiles);
      }
    }

    map<string, lodfile>::iterator walk = lodfils.begin();
    while (walk != lodfils.end())
      lsorted.push_back(walk++);

//  sort(lsorted.begin(), lsorted.end(), lodfile::compare);
//  sort(lsorted.begin(), lsorted.end(), lodfile::compare);

    OSLODWorldspace->Clear();
    set<int>::iterator wpush = lws.begin(); int idx = 0;
    while (wpush != lws.end()) {
      int ws = *wpush;
      if (wspacef.count(ws) > 0) {
	OSLODWorldspace->Append(*(wspacef[ws]));
	OSLODWorldspace->SetClientData(idx, (void *)ws); idx++;
      }

      wpush++;
    }

    OSLODResolution->Clear();
    set<int>::iterator rpush = lrs.begin(); idx = 0;
    while (rpush != lrs.end()) {
      int rs = *rpush;
      /**/ if (rs == 4) {
	OSLODResolution->Append("From 4x4 Cells");
	OSLODResolution->SetClientData(idx, (void *)rs); idx++;
      }
      else if (rs == 8) {
	OSLODResolution->Append("From 8x8 Cells");
	OSLODResolution->SetClientData(idx, (void *)rs); idx++;
      }
      else if (rs == 16) {
	OSLODResolution->Append("From 16x16 Cells");
	OSLODResolution->SetClientData(idx, (void *)rs); idx++;
      }
      else if (rs == 32) {
	OSLODResolution->Append("From 32x32 Cells");
	OSLODResolution->SetClientData(idx, (void *)rs); idx++;
      }

      rpush++;
    }

    OSLODWorldspace->SetSelection(0);
    OSLODResolution->SetSelection(0);
  }

  void OscapeGUI::ResetLPluginList() {
    activel = 0;

    int r = OSLODResolution->GetSelection();
    int w = OSLODWorldspace->GetSelection();
    int rs = (r != -1 ? (int)OSLODResolution->GetClientData(r) : 0);
    int ws = (w != -1 ? (int)OSLODWorldspace->GetClientData(w) : 0);
    wxString Sn, Sr, Sw;
    wxString On, Or, Ow;

    /**/ if (OSRevHeight->GetValue()) {
      Sn.Printf("xyz");
      Sr.Printf(".btr");
      Sw.Printf("%s.%d.", (ws != 0 ? (*(wspacef[ws])).data() : ""), rs);
      On.Printf("xyz", rs);
      Or.Printf(".%d.nif", rs);
      Ow.Printf("%d.", ws);
    }
    else if (OSRevNormals->GetValue()) {
      Sn.Printf("xyz");
      Sr.Printf("_n.dds");
      Sw.Printf("%s.%d.", (ws != 0 ? (*(wspacef[ws])).data() : ""), rs);
      On.Printf("xyz", rs);
      Or.Printf(".%d_fn.dds", rs);
      Ow.Printf("%d.", ws);
    }
    else if (OSRevColors->GetValue()) {
      Sn.Printf("_n.dds");
      Sr.Printf(".dds");
      Sw.Printf("%s.%d.", (ws != 0 ? (*(wspacef[ws])).data() : ""), rs);
      On.Printf(".%d_fn.dds", rs);
      Or.Printf(".%d.dds", rs);
      Ow.Printf("%d.", ws);
    }

    Sw = Sw.MakeLower();

    OSLODList->Clear(); int idx = 0;
    vector< map<string, lodfile>::iterator >::iterator push = lsorted.begin();
    while (push != lsorted.end()) {
      wxString file((*push)->first); file = file.MakeLower();
      bool valid = false;

      /* Oblivion */
      if (file.StartsWith(Ow) && file.EndsWith(Or) && !file.EndsWith(On))
	valid = true;
      /* Skyrim */
      if (file.StartsWith(Sw) && file.EndsWith(Sr) && !file.EndsWith(Sn))
	valid = true;

      if (valid) {
//	wxOwnerDrawn *id;

	OSLODList->Append(file);
	OSLODList->Check(idx, (*push)->second.act); idx++;
//	id = OSLODList->GetItem(idx); idx++;
//	id->SetCheckable(true);
	if ((*push)->second.act) {
	  activel++;
	}
      }

      push++;
    }

    ResetLButtons();
  }

  /* ---------------------------------------------------------------------------- */
  void OscapeGUI::ChangeToHeightfield(wxCommandEvent& event) {
    ResetLPluginList();
    ResetLButtons();

    RegSetKeyValue(Settings, GetGameKey(), "LOD Type", RRF_RT_REG_SZ, "0", 2);
  }

  void OscapeGUI::ChangeToNormalmap(wxCommandEvent& event) {
    ResetLPluginList();
    ResetLButtons();

    RegSetKeyValue(Settings, GetGameKey(), "LOD Type", RRF_RT_REG_SZ, "1", 2);
  }

  void OscapeGUI::ChangeToColormap(wxCommandEvent& event) {
    ResetLPluginList();
    ResetLButtons();

    RegSetKeyValue(Settings, GetGameKey(), "LOD Type", RRF_RT_REG_SZ, "2", 2);
  }

  /* ---------------------------------------------------------------------------- */
  void OscapeGUI::ChangeLODDir(wxFileDirPickerEvent& event) {
    strcpy(LPath, OSLODDir->GetTextCtrlValue().data());
//  strcat(LPath, "\\");

    ResetLPluginDir();
    ResetLPluginList();
    ResetLButtons();

    RegSetKeyValue(Settings, GetGameKey(), "LOD Directory", RRF_RT_REG_SZ, LPath, (DWORD)strlen(LPath) + 1);
  }

  void OscapeGUI::ChangeLODWorldspace(wxCommandEvent& event) {
    ResetLButtons();

#if 0
    int w = event.GetSelection();
    map<string, lodfile>::iterator *lw = (map<string, lodfile>::iterator *)(OSLODWorldspace->GetClientData(w));

    wxString ph = (*lw)->first;
    if (!ph.IsNull()) {
      const char *WS = ph.data();
      RegSetKeyValue(Settings, GetGameKey(), "Last LOD Worldspace", RRF_RT_REG_SZ, WS, (DWORD)strlen(WS) + 1);
    }
#endif

    ChangeLODWorldspace();
  }

  void OscapeGUI::ChangeLODWorldspace() {
    ResetLPluginList();
    ResetLButtons();
  }

  void OscapeGUI::ChangeLODRes(wxCommandEvent& event) {
    ResetLButtons();

    /* make it ('1' + x) */
    int r = 1 + event.GetSelection();

    wxString ph; ph.Printf("%d", r);
    if (!ph.IsNull()) {
      const char *WS = ph.data();
      RegSetKeyValue(Settings, GetGameKey(), "Last LOD Resolution", RRF_RT_REG_SZ, WS, (DWORD)strlen(WS) + 1);
    }

    ChangeLODRes();
  }

  void OscapeGUI::ChangeLODRes() {
    ResetLPluginList();
    ResetLButtons();
  }

  void OscapeGUI::ChangeRecoveryOut(wxFileDirPickerEvent& event) {
    ResetHButtons();

    wxString ph = event.GetPath();
    if (!ph.IsNull()) {
      const char *HO = ph.data();
      RegSetKeyValue(Settings, GetGameKey(), "Recovery Out", RRF_RT_REG_SZ, HO, (DWORD)strlen(HO) + 1);
    }
  }

  /* ---------------------------------------------------------------------------- */
  void OscapeGUI::HeightfieldRecover(wxCommandEvent& event) {
    OSStatusBar->SetStatusText(wxT("Running recovery ..."), 0);
    wxBusyCursor wait;
    prog = new OscapePrg(this);
    int ret = prog->Enter(::HeightfieldRecover);
    delete prog;
    OSStatusBar->SetStatusText(wxT("Ready"), 0);
    prog = NULL;
    if (ret != 666)
      return;
  }

  void OscapeGUI::DefineDimensions(int tsze, int csze, int tmxx, int tmxy) {
    rasterx = tsze * csze;
    rastery = tsze * csze;

    tilesx  = tmxx / tsze;
    tilesy  = tmxy / tsze;

    width   = tilesx * rasterx;
    height  = tilesy * rastery;

    rwsizex = width;
    rwsizey = height;

    nmtilex = tilesx;
    nmtiley = tilesy;

    dotilex = 0;
    dotiley = 0;
  }

  void OscapeGUI::HeightfieldRecover() {
    /* move the result-file around */
    wxString ph = OSFileRecoveryOut->GetPath();
    wxFileName fnh(ph); fnh.ClearExt();

    /**/ if (OSRevHeight->GetValue())
      fnh.SetExt("raw");
    else if (OSRevNormals->GetValue())
      fnh.SetExt("nrm");
    else if (OSRevColors->GetValue())
      fnh.SetExt("land");

    int r = OSLODResolution->GetSelection();
    int w = OSLODWorldspace->GetSelection();
    int rs = (r != -1 ? (int)OSLODResolution->GetClientData(r) : 0);
    int ws = (w != -1 ? (int)OSLODWorldspace->GetClientData(w) : 0);

    /* -------------------------------------------------------------------- */
    wdspace = ws;
    writechk = 0;

#define BASINSHIFT_OBLIVION	(8192 - 0 -  512) / (2 * 4)
#define BASINSHIFT_SKYRIM	(8192 + 0 -  512) / (2 * 4)

#define HEIGHTSHIFT_OBLIVION	(8192.0f -     0.0f)
#define HEIGHTSHIFT_SKYRIM	(8192.0f + 14848.0f)  // 18040 + 5000 (why?)

    /* put the string there and identify the game */
    if (OSGame->FindItem(wxID_OBLIVON)->IsChecked())
      sprintf(wdsname, "%02d", wdspace),
      wpattern = "%02d.%02d.%02d.%02d",
      wchgame = 0,
      basinshift = BASINSHIFT_OBLIVION,
      heightshift = HEIGHTSHIFT_OBLIVION;
    else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked())
      sprintf(wdsname, "%s", wspacef[wdspace]->data()),
      wpattern = "%s.%d.%d.%d",
      wchgame = 1,
      basinshift = BASINSHIFT_SKYRIM,
      heightshift = HEIGHTSHIFT_SKYRIM;

    int mxt = 0, nxt = 0, cx;
    int myt = 0, nyt = 0, cy;

    int num = OSLODList->GetCount();
    for (int n = 0; n < num; n++) {
      if (OSLODList->IsChecked(n)) {
	wxOwnerDrawn *id = OSLODList->GetItem(n);
	const wxString nam = id->GetName();
	const char *sub = nam.data();

	if (sub && (stristr(sub, wdsname) == sub)) {
	  sub += strlen(wdsname);

	  if (OSGame->FindItem(wxID_OBLIVON)->IsChecked()) {
	    if (sub && sscanf(sub, ".%d.%d.%d", &cx, &cy, &rs) == 3) {
	      if (cx >= 0) mxt = max(mxt, (cx + rs + (rs - 1)) / rs);
	      else         nxt = min(nxt, (cx +  0 - (rs - 1)) / rs);
	      if (cy >= 0) myt = max(myt, (cy + rs + (rs - 1)) / rs);
	      else         nyt = min(nyt, (cy +  0 - (rs - 1)) / rs);
	    }
	  }
	  else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked()) {
	    if (sub && sscanf(sub, ".%d.%d.%d", &rs, &cx, &cy) == 3) {
	      if (cx >= 0) mxt = max(mxt, (cx + rs + (rs - 1)) / rs);
	      else         nxt = min(nxt, (cx +  0 - (rs - 1)) / rs);
	      if (cy >= 0) myt = max(myt, (cy + rs + (rs - 1)) / rs);
	      else         nyt = min(nyt, (cy +  0 - (rs - 1)) / rs);
	    }
	  }
	}
      }
    }

    int tlmt, tsze, csze;

    tlmt = 32 / rs;
    tsze = rs;
    csze = 32;

    int rx = mxt % tlmt; if (rx < 0) mxt -= tlmt + rx; else if (rx > 0) mxt += tlmt - rx; 
    int ry = mxt % tlmt; if (ry < 0) mxt -= tlmt + ry; else if (ry > 0) mxt += tlmt - ry; 

        rx = nxt % tlmt; if (rx < 0) nxt -= tlmt + rx; else if (rx > 0) nxt += tlmt - rx; 
        ry = nxt % tlmt; if (ry < 0) nxt -= tlmt + ry; else if (ry > 0) nxt += tlmt - ry; 

    if (mxt < (-nxt - 1)) mxt = -nxt - 1;
    if (myt < (-nyt - 1)) myt = -nyt - 1;

    /**/ if (OSRevHeight->GetValue())
      prog->StartProgress(3);
    else
      prog->StartProgress(1);

    try {

      DefineDimensions(tsze, csze, (mxt - nxt) * rs, (myt - nyt) * rs);

      char temps[256], base[1024];

      /* create the base directory */
      sprintf(base, "%s\\", LPath);

      strcpy(temps, base);
      strcat(temps, wpattern);

      /**/ if (OSRevHeight->GetValue()) {
	prog->InitProgress("Recovering:", 0, "Parsing LODs:", 0.0, 0, OSLODList->GetCount());

	/**/ if (OSGame->FindItem(wxID_OBLIVON)->IsChecked())
	  readNIF(temps);
	else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked())
	  readBTR(temps);

	prog->InitProgress("Recovering:", 1, "Tiling faces:", 0.0, 1, 1);

	/* prepare writing */
	CalculateGeometryNormals();
	TransferGeometry();

	prog->InitProgress("Recovering:", 1, "Rasterizing tiles:", 0.0, 2, 1);

	wrteRecovery1(fnh.GetFullPath(), width, height);
      }
      else if (OSRevNormals->GetValue()) {
	prog->InitProgress("Recovering:", 0, "Parsing LODs:", 0.0, 0, OSLODList->GetCount());

	wrteRecovery1(fnh.GetFullPath(), width, height, temps);
      }
      else if (OSRevColors->GetValue()) {
	prog->InitProgress("Recovering:", 0, "Parsing LODs:", 0.0, 0, OSLODList->GetCount());

	wrteRecovery1(fnh.GetFullPath(), width, height, temps);
      }

      /* done */
      freeGeometry();
      freeTexture();
    }
    catch (exception &e) {
      /* failure */
      freeGeometry();
      freeTexture();

      if (strcmp(e.what(), "ExitThread")) {
	wxMessageDialog d(prog, e.what(), "Oscape error");
	d.ShowModal();
      }

      prog->Leave(0);
      return;
    }

    prog->Leave(666);
  }
