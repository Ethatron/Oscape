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

  DWORD __stdcall HeightfieldGenerate(LPVOID lp) {
    if (gui)
      gui->HeightfieldGenerate();
    return 0;
  }

  /* ---------------------------------------------------------------------------- */
  void OscapeGUI::ChangeHeightfieldIn2(wxFileDirPickerEvent& event) {
    wxString ph = event.GetPath();

    ChangeHeightfieldIn1(ph);
    ChangeHeightfieldIn2(ph);
    ChangeBaseDirOut1(ph);
    ChangeBaseDirOut2(ph);
  }

  void OscapeGUI::ChangeHeightfieldIn2(wxString ph) {
    OSEmphasis->Disable();
    OSSelectGenerator->Hide();
    OSHeightfieldFirst2->Show();
    OSHeightfieldFirst2->GetParent()->Layout();

    if (ph.IsNull())
      return;

    wxFileName fnn(ph); fnn.ClearExt(); fnn.SetExt("nrm");
    wxFileName fnh(ph); fnh.ClearExt(); fnh.SetExt("raw");
    wxFileName fnw(ph); fnw.ClearExt(); fnw.SetExt("water");
    wxFileName fnx(ph); fnx.ClearExt(); fnx.SetExt("land");
    wxFileName fnm(ph); fnm.ClearExt(); fnm.SetExt("fmap");

    wxString snn = fnn.GetFullPath();
    wxString snh = fnh.GetFullPath();
    wxString snw = fnw.GetFullPath();
    wxString snx = fnx.GetFullPath();
    wxString snm = fnm.GetFullPath();

    /* does it exist? */
    DWORD atr = GetFileAttributes(snh.data());
    if (atr == INVALID_FILE_ATTRIBUTES)
      return;

    {
      const char *HI = snh.data();
      OSFileHeightfieldIn1->SetPath(HI);
      OSFileHeightfieldIn2->SetPath(HI);
      RegSetKeyValue(Settings, GetGameKey(), "Heightfield In", RRF_RT_REG_SZ, HI, (DWORD)strlen(HI) + 1);
    }

    OSEmphasis->Enable(GetFileAttributes(snm.data()) != INVALID_FILE_ATTRIBUTES);
    if (GetFileAttributes(snx.data()) != INVALID_FILE_ATTRIBUTES)
      OSColors->Enable();
    else
      OSColors->Disable(), OSColors->SetValue(false), OSPanelColors->Hide();

    OSSelectGenerator->Show();
    OSHeightfieldFirst2->Hide();
    OSHeightfieldFirst2->GetParent()->Layout();
  }

  void OscapeGUI::ChangePointsIn2(wxFileDirPickerEvent& event) {
    wxString ph = event.GetPath();

    ChangePointsIn1(ph);
    ChangePointsIn2(ph);
  }

  void OscapeGUI::ChangePointsIn2(wxString ph) {
    OSPointsClear2->Enable(FALSE);

    {
      const char *PI = ph.data();
      OSFilePoints1->SetPath(PI);
      OSFilePoints2->SetPath(PI);
      RegSetKeyValue(Settings, GetGameKey(), "Points In", RRF_RT_REG_SZ, PI, (DWORD)strlen(PI) + 1);
    }

    if (ph.IsNull())
      return;

    OSPointsClear2->Enable(TRUE);
  }

  void OscapeGUI::ClearPoints2(wxCommandEvent& event) {
    ChangePointsIn1("");
    ChangePointsIn2("");
  }

  void OscapeGUI::ChangeBaseDirOut2(wxFileDirPickerEvent& event) {
    wxString ph = event.GetPath();

    ChangeBaseDirOut1(ph);
    ChangeBaseDirOut2(ph);
  }

  void OscapeGUI::ChangeBaseDirOut2(wxString ph) {
    OSHeightfieldGenerate->Enable(FALSE);

    /* does it exist? */
    DWORD atr = GetFileAttributes(ph.data());
    if ((atr != INVALID_FILE_ATTRIBUTES)) {
      if (!(atr & FILE_ATTRIBUTE_DIRECTORY)) {
	wxFileName fn(ph);
	ph = fn.GetPath();
      }
    }

    if (ph.IsNull())
      return;
    else {
      const char *BO = ph.data();
      OSBaseDirOut1->SetPath(BO);
      OSBaseDirOut2->SetPath(BO);
      RegSetKeyValue(Settings, GetGameKey(), "Base directory Out", RRF_RT_REG_SZ, BO, (DWORD)strlen(BO) + 1);
    }

    /* scan directory and compare timestamps
     * disable all resolutions (mesh & textures) which have newer timestamps
     * if directory has max resolution with newer timestamp disable 1/1
     * else make all resolutions avaiable (re-calc)
     */
    VerifyHeightfieldOut();

    OSHeightfieldGenerate->Enable(TRUE);
  }

  /* ---------------------------------------------------------------------------- */
  wxString OscapeGUI::GetTarget(wxString val) {
    size_t ln;
    if ((ln = val.Index(' ')) != wxNOT_FOUND)
      val = val.SubString(0, ln - 1);
    return val;
  }

  wxString OscapeGUI::GetTarget() {
    return GetTarget(OSTarget->GetValue());
  }

  int OscapeGUI::MaxSuperSampling(int width, int height) {
    /* detect super-sampled sizes */
    wxString ph = OSFileHeightfieldIn1->GetPath();
    wxFileName fnx(ph); fnx.ClearExt(); fnx.SetExt("land");
    ph = fnx.GetFullPath(); WIN32_FILE_ATTRIBUTE_DATA atr;
    if (GetFileAttributesEx(ph.data(), GetFileExInfoStandard, &atr)) {
      int width = rWidth->GetValueAsVariant().GetInteger();
      int height = rHeight->GetValueAsVariant().GetInteger();

      LONGLONG
	rlen = atr.nFileSizeHigh;
      rlen <<= 32;
      rlen += atr.nFileSizeLow;
      LONGLONG
	plen = width;
      plen *= height;
      plen *= 4;

      int ss = 1;
      while (plen < rlen) {
	plen *= 4;
	ss *= 2;
      }

      if (plen == rlen)
	return ss;

      /* otherwise size-mismatch */
      return -1;
    }

    /* doesn't exist */
    return 0;
  }

  void OscapeGUI::MaxTarget(int &_maxp, int &_maxx, int &_ntls) {
    int rasterx = tSize->GetValueAsVariant().GetInteger() * cSize->GetValueAsVariant().GetInteger();
    int rastery = tSize->GetValueAsVariant().GetInteger() * cSize->GetValueAsVariant().GetInteger();

    int dotilex = tLeft->GetValueAsVariant().GetInteger() / tSize->GetValueAsVariant().GetInteger();
    int dotiley = tTop ->GetValueAsVariant().GetInteger() / tSize->GetValueAsVariant().GetInteger();

    int nmtilex = tRight ->GetValueAsVariant().GetInteger() / tSize->GetValueAsVariant().GetInteger() - dotilex + 1;
    int nmtiley = tBottom->GetValueAsVariant().GetInteger() / tSize->GetValueAsVariant().GetInteger() - dotiley + 1;

    /* maximum number of triangles on a grid */
    int maxt = ((rastery + 1) - 1) * ((rasterx + 1) - 1) * 2;
    /* (a - 1)² * 2 */
    int maxp = floor(sqrtf(65536 / 2) - 1.0f), maxx;

    /* multiplied by number of available tiles */
    maxp *= maxp;
    maxp *= nmtilex;
    maxp *= nmtiley;
    maxx  = maxp;

    /* return */
    _maxp = maxp;
    _maxx = maxx;
    _ntls = nmtilex * nmtiley;
  }

  void OscapeGUI::VerifyHeightfieldIn() {
    /* detect super-sampled sizes */
    int ss = MaxSuperSampling(
      rWidth ->GetValueAsVariant().GetInteger(),
      rHeight->GetValueAsVariant().GetInteger()
    );

    OSColorLow    ->Enable(ss > 0);
    OSColorRegular->Enable(ss > 0);
    OSColorHigh   ->Enable(ss > 1);
    OSColorUltra  ->Enable(ss > 2);

    if (!OSColorLow    ->IsEnabled()) OSColorLow    ->SetValue(false);
    if (!OSColorRegular->IsEnabled()) OSColorRegular->SetValue(false);
    if (!OSColorHigh   ->IsEnabled()) OSColorHigh   ->SetValue(false);
    if (!OSColorUltra  ->IsEnabled()) OSColorUltra  ->SetValue(false);

    /* -------------------------------------------------------------------- */
    wxString val = GetTarget();
    long l = 0; val.ToLong(&l);

    OSTarget->Clear();

    /* -------------------------------------------------------------------- */
    int maxp, maxx, ntls; MaxTarget(maxp, maxx, ntls);
    int maxs, maxm, it; maxm = maxp;

    if (OSGame->FindItem(wxID_OBLIVON)->IsChecked()) {
      OSTarget->Append("220000 (~3/2 of vanilla Cyrodiil)");
      OSTarget->Append("155000 (~3/3 of vanilla Cyrodiil)");
      OSTarget->Append("110000 (~2/3 of vanilla Cyrodiil)");
      OSTarget->Append("55000 (~1/3 of vanilla Cyrodiil)");
      OSTarget->Append("615000 (~4/3 of vanilla TWMP)");
      OSTarget->Append("450000 (~3/3 of vanilla TWMP)");
      OSTarget->Append("290000 (~2/3 of vanilla TWMP)");
      OSTarget->Append("150000 (~1/3 of vanilla TWMP)");

      /* multiplied by number of resolutions (limited by available tiles) */
      maxp = 12288;
      while ((maxp * 2) <= (12288 * ntls))
	maxp *= 2;

      if (maxp != (12288 * ntls)) {
	maxs = maxp / ntls; it = 1;
	while (maxp > 1024) {
	  char buf[64]; sprintf(buf, "%d (1/%d of %d per tile)", maxp, it, maxs); maxp = (maxp + 1) >> 1; it *= 2;

	  OSTarget->Append(buf);
	}
      }

      /* multiplied by number of available tiles */
      maxp = 12288;
      maxp *= ntls;

      /**/ {
	maxs = maxp / ntls; it = 1;
	while (maxp > 1024) {
	  char buf[64]; sprintf(buf, "%d (1/%d of %d per tile)", maxp, it, maxs); maxp = (maxp + 1) >> 1; it *= 2;

	  OSTarget->Append(buf);
	}
      }

      /* total maximum */
      maxp = maxm;

      /**/ {
	maxs = maxp / ntls; it = 1;
	while (maxp > 1024) {
	  char buf[64]; sprintf(buf, "%d (1/%d of %d per tile)", maxp, it, maxs); maxp = (maxp + 1) >> 1; it *= 2;

	  OSTarget->Append(buf);
	}
      }
    }
    else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked()) {
      OSTarget->Append("1500000 (~3/2 of vanilla Skyrim)");
      OSTarget->Append("1000000 (~3/3 of vanilla Skyrim)");
      OSTarget->Append("650000 (~2/3 of vanilla Skyrim)");
      OSTarget->Append("320000 (~1/3 of vanilla Skyrim)");

      /* multiplied by number of resolutions (limited by available tiles) */
      maxp = 1000;
      while ((maxp * 2) <= (1000 * ntls))
	maxp *= 2;

      if (maxp != (1000 * ntls)) {
	maxs = maxp / ntls; it = 1;
	while (maxp > 1000) {
	  char buf[64]; sprintf(buf, "%d (1/%d of %d per tile)", maxp, it, maxs); maxp = (maxp + 1) >> 1; it *= 2;

	  OSTarget->Append(buf);
	}
      }

      /* multiplied by number of available tiles */
      maxp = 1000;
      maxp *= ntls;

      /**/ {
	maxs = maxp / ntls; it = 1;
	while (maxp > 1000) {
	  char buf[64]; sprintf(buf, "%d (1/%d of %d per tile)", maxp, it, maxs); maxp = (maxp + 1) >> 1; it *= 2;

	  OSTarget->Append(buf);
	}
      }

      /* total maximum */
      maxp = maxm;

      /**/ {
	maxs = maxp / ntls; it = 1;
	while (maxp > 1000) {
	  char buf[64]; sprintf(buf, "%d (1/%d of %d per tile)", maxp, it, maxs); maxp = (maxp + 1) >> 1; it *= 2;

	  OSTarget->Append(buf);
	}
      }
    }

    /* -------------------------------------------------------------------- */
    OSTarget->SetValue(val);
    if (l > maxx) {
      char buf[32]; sprintf(buf, "%d", maxx);

      OSTarget->SetValue(buf);
    }

    VerifyTarget(l);
  }

  void OscapeGUI::VerifyHeightfieldOut() {
    wxVariant v;

    if (!formID)
      return;
    v = formID->GetValueAsVariant();
    if (v.IsNull())
      return;

    wdspace = v.GetInteger();

    long l = 0; if (GetTarget().ToLong(&l)) limit = l;
    double r = 0.0; if (OSTermination->GetValue().ToDouble(&r)) termination = r;

    VerifyTarget(l);

    /* -------------------------------------------------------------------- */
    wxString dataPth = OSFileHeightfieldIn1->GetPath();
    wxString dataPts = OSFilePoints1->GetPath();
    wxString dataDir = OSBaseDirOut1->GetPath();

    WIN32_FILE_ATTRIBUTE_DATA infoh; BOOL hex =
    GetFileAttributesEx(dataPth.data(), GetFileExInfoStandard, &infoh);

    {
      char temps[256];
      int target = limit;

      sprintf(temps, "%s\\LOD-%d\\%02d.pts", dataDir.data(), limit / 1, wdspace);

      WIN32_FILE_ATTRIBUTE_DATA infor; BOOL rex =
      GetFileAttributesEx(temps, GetFileExInfoStandard, &infor);

      /* newer seed-resolution */
      if (hex && rex && (*((__int64 *)&infoh.ftLastWriteTime) < *((__int64 *)&infor.ftLastWriteTime)))
	OSMlod1->Enable(true);
      else
	OSMlod1->Enable(false), OSMlod1->SetValue(true);
    }
  }

  void OscapeGUI::ChangeAlgorithm(wxCommandEvent& event) {
    OSQThreshold->Enable(event.GetSelection() == 0);
  }

  void OscapeGUI::ChangeTarget(wxCommandEvent& event) {
    wxString ph;
    long l = 0;

    ph = GetTarget(event.GetString());
    event.SetString(ph);
    if (!ph.ToLong(&l))
      return;

    /* -------------------------------------------------------------------- */
    int maxp, maxx, ntls; MaxTarget(maxp, maxx, ntls);

    if (l > maxx) {
      ph.Printf("%d", maxx);
      event.SetString(ph);
    }

    {
      const char *TG = ph.data();
      RegSetKeyValue(Settings, GetGameKey(), "Target", RRF_RT_REG_DWORD, TG, (DWORD)strlen(TG) + 1);
    }

    VerifyHeightfieldOut();
  }

  void OscapeGUI::VerifyTarget(int target) {
    OSMlod1->Enable(false);
    OSMlod2->Enable(false);
    OSMlod3->Enable(false);
    OSMlod4->Enable(false);
    OSMlod5->Enable(false);
    OSMlod6->Enable(false);
    OSMlod7->Enable(false);
    OSMlod8->Enable(false);
    OSNlod1->Enable(false);
    OSNlod2->Enable(false);
    OSNlod3->Enable(false);
    OSNlod4->Enable(false);
    OSClod1->Enable(false);
    OSClod2->Enable(false);
    OSClod3->Enable(false);
    OSClod4->Enable(false);

    int r = 0;
    while (target > 1024) {
      target = (target + 1) >> 1;
      r++;
    }

    /* Skyrim has a native limit of 4 (32,16,8,4), could be extended */
    if (OSGame->FindItem(wxID_SKYRIM)->IsChecked())
      r = min(4, r);

/*  OSMlod1->Enable(r > 0); if (r <= 0) OSMlod1->SetValue(false);*/
    OSMlod2->Enable(r > 1); if (r <= 1) OSMlod2->SetValue(false);
    OSMlod3->Enable(r > 2); if (r <= 2) OSMlod3->SetValue(false);
    OSMlod4->Enable(r > 3); if (r <= 3) OSMlod4->SetValue(false);
    OSMlod5->Enable(r > 4); if (r <= 4) OSMlod5->SetValue(false);
    OSMlod6->Enable(r > 5); if (r <= 5) OSMlod6->SetValue(false);
    OSMlod7->Enable(r > 6); if (r <= 6) OSMlod7->SetValue(false);
    OSMlod8->Enable(r > 7); if (r <= 7) OSMlod8->SetValue(false);

    OSMeshUVs->Enable(true);
    if (OSGame->FindItem(wxID_SKYRIM)->IsChecked()) {
      OSMeshUVs->Enable(false);
      OSMeshUVs->SetValue(true);

      OSNlod1->Enable(r > 0);
      OSNlod2->Enable(r > 1);
      OSNlod3->Enable(r > 2);
      OSNlod4->Enable(r > 3);
      OSClod1->Enable(r > 0);
      OSClod2->Enable(r > 1);
      OSClod3->Enable(r > 2);
      OSClod4->Enable(r > 3);
    }
  }

  /* ---------------------------------------------------------------------------- */
  void OscapeGUI::ChangeMeshes(wxCommandEvent& event) {
    ChangeMeshes();

    RegSetKeyValue(Settings, GetGameKey(), "Do Meshes", RRF_RT_REG_DWORD, OSMeshes->GetValue() ? "1" : "0", 2);
  }

  void OscapeGUI::ChangeNormals(wxCommandEvent& event) {
    ChangeNormals();

    RegSetKeyValue(Settings, GetGameKey(), "Do Normals", RRF_RT_REG_DWORD, OSNormals->GetValue() ? "1" : "0", 2);
  }

  void OscapeGUI::ChangeColors(wxCommandEvent& event) {
    ChangeColors();

    RegSetKeyValue(Settings, GetGameKey(), "Do Colors", RRF_RT_REG_DWORD, OSColors->GetValue() ? "1" : "0", 2);
  }

  void OscapeGUI::ChangeHeightmap(wxCommandEvent& event) {
    ChangeHeightmap();

    RegSetKeyValue(Settings, GetGameKey(), "Do Heightmap", RRF_RT_REG_DWORD, OSHeightmap->GetValue() ? "1" : "0", 2);
  }

  void OscapeGUI::ChangeMeshes() {
    if (OSMeshes->GetValue())
      OSPanelMeshes->Show();
    else
      OSPanelMeshes->Hide();

    OSPanelMeshes->GetParent()->Layout();
    OSSelectGenerator->SetVirtualSize(OSPanelMeshes->GetParent()->GetBestVirtualSize());
  }

  void OscapeGUI::ChangeNormals() {
    if (OSNormals->GetValue())
      OSPanelNormals->Show();
    else
      OSPanelNormals->Hide();

    OSPanelNormals->GetParent()->Layout();
    OSSelectGenerator->SetVirtualSize(OSPanelNormals->GetParent()->GetBestVirtualSize());
  }

  void OscapeGUI::ChangeColors() {
    if (OSColors->GetValue())
      OSPanelColors->Show();
    else
      OSPanelColors->Hide();

    OSPanelColors->GetParent()->Layout();
    OSSelectGenerator->SetVirtualSize(OSPanelColors->GetParent()->GetBestVirtualSize());
  }

  void OscapeGUI::ChangeHeightmap() {
    if (OSHeightmap->GetValue())
      OSPanelHeightmap->Show();
    else
      OSPanelHeightmap->Hide();

    OSPanelHeightmap->GetParent()->Layout();
    OSSelectGenerator->SetVirtualSize(OSPanelHeightmap->GetParent()->GetBestVirtualSize());
  }

  void OscapeGUI::CheckFloat(wxCommandEvent& event) {
    wxString n = event.GetString();
    double r;
    if (!n.ToDouble(&r))
      event.SetString("0.0");
  }

  void OscapeGUI::CheckInt(wxCommandEvent& event) {
    wxString n = event.GetString();
    long r;
    if (!n.ToLong(&r))
      event.SetString("0");
  }

  void OscapeGUI::HeightfieldGenerate(wxCommandEvent& event) {
    OSStatusBar->SetStatusText(wxT("Running generator ..."), 0);
    wxBusyCursor wait;
    prog = new OscapePrg(this);
    int ret = prog->Enter(::HeightfieldGenerate);
    delete prog;
    OSStatusBar->SetStatusText(wxT("Ready"), 0);
    prog = NULL;
    if (ret != 666)
      return;

    /* next tab */
    OSToolSwitch->SetSelection(3);

    /* and activate */
    ChangeBaseDirIn(OSBaseDirOut1->GetPath());
  }

  bool OscapeGUI::SanitizeGeneration() {
    long target;

    if (GetTarget().ToLong(&target)) {
      int wds = formID->GetValueAsVariant().GetInteger();
      wxString dataDir = OSBaseDirOut1->GetPath();
      bool makeclean = false, haspt = false;
      vector<string> dirs;
      char temps[256];
      long l = target;

      /* get some maximum */
      while (l < 0x3FFFFFFF)
	l = l << 1;

      while (l > 1024) {
	l = (l + 1) >> 1;

	/* collect all directories (also remnants) */
	sprintf(temps, "%s\\LOD-%d", dataDir.data(), l, wds);

	WIN32_FILE_ATTRIBUTE_DATA infod; BOOL dex =
	GetFileAttributesEx(temps, GetFileExInfoStandard, &infod);

	if (dex)
	  dirs.push_back(temps);

	/* collect all point-files */
	sprintf(temps, "%s\\LOD-%d\\%02d.pts", dataDir.data(), l, wds);

	WIN32_FILE_ATTRIBUTE_DATA infop; BOOL pex =
	  GetFileAttributesEx(temps, GetFileExInfoStandard, &infop);

	if (pex) {
	  haspt = true;
	  if (l != target)
	    makeclean = true;
	}
      }

      if (makeclean || !haspt) {
	SHFILEOPSTRUCT op;
	char buf[32768], *ptr = buf;

	memset(&op, 0, sizeof(op));
	memset(buf, 0, sizeof(buf));

	vector<string>::iterator walk = dirs.begin();
	while (walk != dirs.end()) {
	  strcpy(ptr, walk->data());
	  ptr += strlen(ptr) + 1;

	  walk++;
	}

//	op.hwnd = GetHwnd();
	op.wFunc = FO_DELETE;
	op.pFrom = buf;
	op.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_SIMPLEPROGRESS;
	op.lpszProgressTitle = "Oscape: Cleaning old targets";

	int ret = SHFileOperation(&op);
	if (op.fAnyOperationsAborted)
	  return false;
      }
    }

    return true;
  }

  void OscapeGUI::DefineDimensions(int tsze, int csze) {
    rasterx = tsze * csze;
    rastery = tsze * csze;

    rwsizex = rWidth ->GetValueAsVariant().GetInteger();
    rwsizey = rHeight->GetValueAsVariant().GetInteger();

    width   = rwsizex;
    height  = rwsizey;

    tilesx  = width  / rasterx;
    tilesy  = height / rastery;

    dotilex = tLeft->GetValueAsVariant().GetInteger() / tsze;
    dotiley = tTop ->GetValueAsVariant().GetInteger() / tsze;

    nmtilex = tRight ->GetValueAsVariant().GetInteger() / tsze - dotilex + 1;
    nmtiley = tBottom->GetValueAsVariant().GetInteger() / tsze - dotiley + 1;

    dotilex = dotilex + (tilesx >> 1);
    dotiley = dotiley + (tilesy >> 1);
  }

  void OscapeGUI::HeightfieldGenerate() {
    if (!SanitizeGeneration()) {
      prog->Leave(666);
      return;
    }

    /* -------------------------------------------------------------------- */
    wdspace = formID->GetValueAsVariant().GetInteger();

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

    /* -------------------------------------------------------------------- */
    long l = 0; double r = 0.0;
    int tlmt, tsze, csze;

    tlmt = (OSGame->FindItem(wxID_SKYRIM)->IsChecked() ? 8 : 1);
    tsze = tSize->GetValueAsVariant().GetInteger();
    csze = cSize->GetValueAsVariant().GetInteger();

    optimizemesh = OSMeshOpt->GetValue();
    nobasin = !OSMeshBasin->GetValue();
    datadep = (OSAlgorithm->GetSelection() == 0 ? 1 : 0);
    switch (QSError->GetSelection()) {
      case 0: criterion = SUMINF; break;
      case 1: criterion = MAXINF; break;
      case 2: criterion = SUM2; break;
      case 3: criterion = ABN; break;
    }

    if (GetTarget().ToLong(&l)) limit = l;

    if (OSQThreshold ->GetValue().ToDouble(&r)) qual_thresh = max(0.001, min(r, 0.999));
    if (OSAThreshold ->GetValue().ToDouble(&r)) area_thresh = max(0.0, r);
    if (OSTermination->GetValue().ToDouble(&r)) termination = max(0.0, r);
    if (OSEmphasis   ->GetValue().ToDouble(&r)) emphasis = max(0.0, min(r, 10000.0));

    if (!OSEmphasis->IsEnabled()) emphasis = 0.0;

    /* -------------------------------------------------------------------- */
    wxString dataPth = OSFileHeightfieldIn1->GetPath();
    wxString dataPts = OSFilePoints1->GetPath();
    wxString dataDir = OSBaseDirOut1->GetPath();

    wxFileName fnn(dataPth); fnn.ClearExt(); fnn.SetExt("nrm");
    wxFileName fnh(dataPth); fnh.ClearExt(); fnh.SetExt("raw");
    wxFileName fnw(dataPth); fnw.ClearExt(); fnw.SetExt("water");
    wxFileName fnx(dataPth); fnx.ClearExt(); fnx.SetExt("land");
    wxFileName fnm(dataPth); fnm.ClearExt(); fnm.SetExt("fmap");

    wxString greyFPth = fnm.GetFullPath();
    wxString dataFPth = fnh.GetFullPath();
    wxString watrFPth = fnw.GetFullPath();
    wxString colrFPth = fnx.GetFullPath();

    texFile = NULL;
    greyFile = greyFPth.data();
    colrFile = colrFPth.data();
    dataFile = dataFPth.data();
    watrFile = watrFPth.data();

//  if (currentRes != maxResolution_in_dataDir)
//  if (is_selected_1/1)
//    RecursiveDelete(dataDir.data());

    /* -------------------------------------------------------------------- */
    int majortasks = 0, majordone = 0;

    /* reading heightfield */
    if (OSMeshes->GetValue() ||
	OSNormals->GetValue() ||
	OSHeightmap->GetValue())
      majortasks++;

    /* generating meshes (generate mesh, add points, subdivide and optimize, write meshes) */
    if (OSMeshes->GetValue()) {
      int m = 1 + 3;

      if (dataPts.length() > 0) m++;
      if (OSMeshUVs->Get3StateValue() != wxCHK_CHECKED  ) m++;
      if (OSMeshUVs->Get3StateValue() != wxCHK_UNCHECKED) m++;

      if (OSHeightmap->GetValue()) {
//	if (OSHeightmapPPM    ->GetValue()) m++;
//	if (OSHeightmapDDS    ->GetValue()) m++;
//	if (OSHeightmapPNG    ->GetValue()) m++;

	if (OSHeightmapLow    ->GetValue()) m++;
	if (OSHeightmapRegular->GetValue()) m++;
	if (OSHeightmapHigh   ->GetValue()) m++;
      }

      if (OSMlod1->GetValue()) majortasks += m;
      if (OSMlod2->GetValue()) majortasks += m + 1;
      if (OSMlod3->GetValue()) majortasks += m + 1;
      if (OSMlod4->GetValue()) majortasks += m + 1;
      if (OSMlod5->GetValue()) majortasks += m + 1;
      if (OSMlod6->GetValue()) majortasks += m + 1;
      if (OSMlod7->GetValue()) majortasks += m + 1;
      if (OSMlod8->GetValue()) majortasks += m + 1;
    }

    /* generate textures (PPM, DDS, PNG) */
    if (OSNormals->GetValue()) {
      int m = 0;

      if (OSNlod1->GetValue() && (tlmt >=   1)) m += 1;
      if (OSNlod2->GetValue() && (tlmt >=   2)) m += 1;
      if (OSNlod3->GetValue() && (tlmt >=   4)) m += 1;
      if (OSNlod4->GetValue() && (tlmt >=   8)) m += 1;

//    if (OSNormalPPM    ->GetValue()) m++;
//    if (OSNormalDDS    ->GetValue()) m++;
//    if (OSNormalPNG    ->GetValue()) m++;

      if (OSNormalLow    ->GetValue()) majortasks += m;
      if (OSNormalRegular->GetValue()) majortasks += m;
      if (OSNormalHigh   ->GetValue()) majortasks += m;
    }

    /* reading surfacemap */
    if (OSColors->GetValue())
      majortasks++;

    /* generate textures (PPM, DDS, PNG) */
    if (OSColors->GetValue()) {
      int m = 0;

      if (OSClod1->GetValue() && (tlmt >=   1)) m += 1;
      if (OSClod2->GetValue() && (tlmt >=   2)) m += 1;
      if (OSClod3->GetValue() && (tlmt >=   4)) m += 1;
      if (OSClod4->GetValue() && (tlmt >=   8)) m += 1;

//    if (OSColorPPM    ->GetValue()) m++;
//    if (OSColorDDS    ->GetValue()) m++;
//    if (OSColorPNG    ->GetValue()) m++;

      if (OSColorLow    ->GetValue()) majortasks += m;
      if (OSColorRegular->GetValue()) majortasks += m;
      if (OSColorHigh   ->GetValue()) majortasks += m;
      if (OSColorUltra  ->GetValue()) majortasks += m;
    }

    /* -------------------------------------------------------------------- */
    prog->StartProgress(majortasks);

    /* -------------------------------------------------------------------- */
    if (OSMeshes->GetValue() ||
	OSNormals->GetValue() ||
	OSHeightmap->GetValue()) {
      prog->InitProgress("Initializing:", 0, "Reading heightfield:", 0.0, majordone++, 1);

      /* -------------------------------------------------------------------- */
      tlmt = (OSGame->FindItem(wxID_SKYRIM)->IsChecked() ? 8 : 1);
      tsze = tSize->GetValueAsVariant().GetInteger();
      csze = cSize->GetValueAsVariant().GetInteger();

      DefineDimensions(tsze, csze);

      ifstream mntns(dataFile, ios::binary);
      HField H(mntns, texFile, greyFile);

      WIN32_FILE_ATTRIBUTE_DATA infoh; BOOL hex =
      GetFileAttributesEx(dataFile, GetFileExInfoStandard, &infoh);
      writechk = (hex ? *((__int64 *)&infoh.ftLastWriteTime) : 0);
      if (OSOverwrite->GetValue())
	writechk = 0;

      /* -------------------------------------------------------------------- */
      if (OSMeshes->GetValue()) {
	writetin = false;
	writeobj = OSMeshOBJ->GetValue();
	writenif = OSMeshNIF->GetValue();
	writedx9 = OSMeshDX ->GetValue();

	class _tripple {
public:
	  _tripple(int l, Real t, int s) :
	  limit(l), termination(t), tsze(s) {}
	  int limit; Real termination; int tsze;
	};

	vector< _tripple > ress; size_t r = 0; Real pw;

	if (OSMlodHalf->GetValue()) pw = 1.0;
	if (OSMlodQuat->GetValue()) pw = 1.5;
	if (OSMlodCube->GetValue()) pw = 2.0;

	if (OSMlod1->GetValue()) ress.push_back(_tripple((int)floor((1.0 * limit) / pow(  1.0, pw)), termination *   1, tsze * min(  1, tlmt)));
	if (OSMlod2->GetValue()) ress.push_back(_tripple((int)floor((1.0 * limit) / pow(  2.0, pw)), termination *   2, tsze * min(  2, tlmt)));
	if (OSMlod3->GetValue()) ress.push_back(_tripple((int)floor((1.0 * limit) / pow(  4.0, pw)), termination *   4, tsze * min(  4, tlmt)));
	if (OSMlod4->GetValue()) ress.push_back(_tripple((int)floor((1.0 * limit) / pow(  8.0, pw)), termination *   8, tsze * min(  8, tlmt)));
	if (OSMlod5->GetValue()) ress.push_back(_tripple((int)floor((1.0 * limit) / pow( 16.0, pw)), termination *  16, tsze * min( 16, tlmt)));
	if (OSMlod6->GetValue()) ress.push_back(_tripple((int)floor((1.0 * limit) / pow( 32.0, pw)), termination *  32, tsze * min( 32, tlmt)));
	if (OSMlod7->GetValue()) ress.push_back(_tripple((int)floor((1.0 * limit) / pow( 64.0, pw)), termination *  64, tsze * min( 64, tlmt)));
	if (OSMlod8->GetValue()) ress.push_back(_tripple((int)floor((1.0 * limit) / pow(128.0, pw)), termination * 128, tsze * min(128, tlmt)));

	if ((r = ress.size()) > 0) {
	  char temps[256], base[1024];
	  int target = limit;

	  for (size_t n = 0; n < r; n++) {
	    tsze        = ress[n].tsze;
	    limit       = ress[n].limit;
	    termination = ress[n].termination;

	    /* -------------------------------------------------------------------- */
	    DefineDimensions(tsze, csze);
	    srandom(limit);

	    _controlfp(_RC_DOWN, _MCW_RC);	// round to -8
//	    _controlfp(_PC_53, _MCW_PC);	// round to -8

#if 0
	    // replace default output buffer with string buffer
	    ofstream dbg("debug.log", ios::binary);
	    std::streambuf* old_rdbuf = std::cout.rdbuf();
	    //std::stringbuf new_rdbuf;
	    //std::cout.rdbuf(&new_rdbuf);
	    std::cout.rdbuf(dbg.rdbuf());

	    debug = 0;
#endif
	    try {

	      /* let's go */
	      prog->InitProgress("Resolution %d, generating mesh:", limit, "Selecting points (current mesh error %f):", -1.0, majordone++, limit);

	      /* create the base directory */
	      sprintf(base, "%s\\LOD-%d", dataDir.data(), limit);
	      CreateDirectory(base, NULL);

	      /* prepare the skiplist for faster processing */
	      skiplist.clear();

	      if (OSMeshUVs->Get3StateValue() != wxCHK_CHECKED) {
		strcpy(temps, base);
		strcat(temps, "\\UVoff\\");
		strcat(temps, wpattern);

		skiplist.push_back(temps);
	      }

	      if (OSMeshUVs->Get3StateValue() != wxCHK_UNCHECKED) {
		strcpy(temps, base);
		strcat(temps, "\\UVon\\");
		strcat(temps, wpattern);

		skiplist.push_back(temps);
	      }

	      /* initial heightfield-class */
	      SimplField ter(&H);

#ifdef	SPLIT_ON_INJECTION
	      /* add highest resolution raster-points */
	      for (int h = 0; h <= height; h += rastery)
	      for (int w = 0; w <=  width; w += rasterx)
		/* damit, the heightfield is [0,width), not inclusive */
		ter.select_new_point(min(w, width - 1), min(h, height - 1));

	      /* add water-point for cells crossing 0.0 */
	      for (int h = 0; h <  height; h += 32)
	      for (int w = 0; w <   width; w += 32) {
		/* TODO: this is not really correct, as from the simplification
		 *       a triangle can cross zero in this cell even though there
		 *       is no water in this cell
		 */
		for (int hh = h; hh <= (h + 32); hh += 1)
		for (int ww = w; ww <= (w + 32); ww += 1) {
		  /* damit, the heightfield is [0,width), not inclusive */
		  Real z = H.getZ(min(ww, width - 1), min(hh, height - 1));
		  z = z * heightscale - heightshift;

		  /* must cross (means smaller than 0.0) */
		  if (z < 0.0) {
		    Point2d p(w, h);

		    RegisterWater(p, 0.0, true);

		    /* bail out */
		    hh = h + 32;
		    ww = w + 32;
		  }
		}
	      }
#endif

#if 0
	      debug = 0;//(limit == 2025 ? 500 : 0);
#endif

	      /* serial insertion */
	      greedy_insert(ter);

	      /* parallel insertion (consume all above given error) */
//	      greedy_insert_error(ter);

	      /* water read the points in */
	      {
		prog->InitProgress("Resolution %d, generating mesh:", limit, "Placing water levels:", 0.0, majordone, 1);

		readWaterFile(ter, watrFile);
	      }

	      /* low resolutions read the points in */
	      if (limit < target) {
		prog->InitProgress("Resolution %d, generating mesh:", limit, "Placing hi-res points (current mesh error %f):", ter.getCurrentError(), majordone++, 1);

		sprintf(temps, "%s\\LOD-%d\\%02d.pts", dataDir.data(), target, wdspace);
		readPointsFile(ter, temps);
	      }

	      /* custom points are also read in */
	      if (dataPts.length() > 0) {
		prog->InitProgress("Resolution %d, generating mesh:", limit, "Placing custom points (current mesh error %f):", ter.getCurrentError(), majordone++, 1);

		readPointsFile(ter, dataPts.data());
	      }

	      sprintf(base, "%s\\LOD-%d", dataDir.data(), limit);

	      prog->InitProgress("Resolution %d, optimizing mesh:", limit, "Reordering faces:", 0.0, majordone++, 1);

	      /* prepare writing */
	      TransferGeometry(ter);

	      prog->InitProgress("Resolution %d, optimizing mesh:", limit, "Tiling faces:", 0.0, majordone++, 1);

	      /* prepare writing */
	      TransferGeometry();

	      prog->InitProgress("Resolution %d, optimizing mesh:", limit, "Optimizing tiles:", 0.0, majordone++, 1);

	      /* prepare writing */
	      RevisitGeometry();

	      /* go, write */
	      if (OSMeshUVs->Get3StateValue() != wxCHK_CHECKED) {
		sprintf(temps, "%s\\UVoff", base);
		CreateDirectory(temps, NULL);

		strcpy(temps, base);
		strcat(temps, "\\UVoff\\");
		strcat(temps, wpattern);
		emituvs = false;

		prog->InitProgress("Resolution %d, saving mesh:", limit, "Saving non-UV tiles:", 0.0, majordone++, 1);

		wrteGeometry(ter, temps);
	      }

	      /* go, write */
	      if (OSMeshUVs->Get3StateValue() != wxCHK_UNCHECKED) {
		sprintf(temps, "%s\\UVon", base);
		CreateDirectory(temps, NULL);

		strcpy(temps, base);
		strcat(temps, "\\UVon\\");
		strcat(temps, wpattern);
		emituvs = true;

		prog->InitProgress("Resolution %d, saving mesh:", limit, "Saving UV tiles:", 0.0, majordone++, 1);

		wrteGeometry(ter, temps);
	      }

	      /* the highest resolution writes the points out */
	      if (limit == target) {
		sprintf(temps, "%s\\LOD-%d\\%02d.pts", dataDir.data(), target, wdspace);
		wrtePointsFile(ter, temps);
	      }

	      /* done */
	      freeGeometry();
	    }
	    catch (exception &e) {
	      /* failure */
	      freeGeometry();

	      if (strcmp(e.what(), "ExitThread")) {
		wxMessageDialog d(prog, e.what(), "Oscape error");
		d.ShowModal();
	      }

	      prog->Leave(0);
	      return;
	    }

#if 0
	    // restore the default buffer before destroying the new one
	    //std::string s(new_rdbuf.str());
	    std::cout.rdbuf(old_rdbuf);
	    //FILE *f = fopen("debug.log", "w");
	    //fwrite(s.data(), 1, s.size(), f);
	    //fclose(f);
#endif
	  }
	}
      }

      /* -------------------------------------------------------------------- */
      tlmt = (OSGame->FindItem(wxID_SKYRIM)->IsChecked() ? 8 : 1);
      tsze = tSize->GetValueAsVariant().GetInteger();
      csze = cSize->GetValueAsVariant().GetInteger();

      DefineDimensions(tsze, csze);

      infoh; hex =
      GetFileAttributesEx(dataFile, GetFileExInfoStandard, &infoh);
      writechk = (hex ? *((__int64 *)&infoh.ftLastWriteTime) : 0);
      if (OSOverwrite->GetValue())
	writechk = 0;

      if (OSNormals->GetValue()) {
	writeppm = OSNormalPPM->GetValue();
	writepng = OSNormalPNG->GetValue();
	writedds = OSNormalDDS->GetValue();

	class _single {
public:
	  _single(int s) :
	  tsze(s) {}
	  int tsze;
	};

	vector< _single > ress; size_t r = 0;

	if (OSNlod1->GetValue() && (tlmt >=   1)) ress.push_back(_single(tsze *   1));
	if (OSNlod2->GetValue() && (tlmt >=   2)) ress.push_back(_single(tsze *   2));
	if (OSNlod3->GetValue() && (tlmt >=   4)) ress.push_back(_single(tsze *   4));
	if (OSNlod4->GetValue() && (tlmt >=   8)) ress.push_back(_single(tsze *   8));

	if ((r = ress.size()) > 0) {
	  char temps[256], base[1024];
	  int target = limit;

	  for (size_t n = 0; n < r; n++) {
	    tsze       = ress[n].tsze;

	    /* -------------------------------------------------------------------- */
	    DefineDimensions(tsze, csze);
	    srandom(limit);

	    _controlfp(_RC_DOWN, _MCW_RC);	// round to -8
//	    _controlfp(_PC_53, _MCW_PC);	// round to -8

	    try {

	      if (OSNormalLow    ->GetValue()) {
		/* create the base directory */
		sprintf(base, "%s\\TEX-%d", dataDir.data(), 512);
		CreateDirectory(base, NULL);

		strcpy(temps, base);
		strcat(temps, "\\");
		strcat(temps, wpattern);

		prog->InitProgress("Resolution %d, calculating normal-maps:", 512, "Tiling normals:", 0.0, majordone++, 1);
		wrteNormals0(false, OSNormalLow    ->GetValue(), false, H, temps);
	      }

	      if (OSNormalRegular->GetValue()) {
		/* create the base directory */
		sprintf(base, "%s\\TEX-%d", dataDir.data(), 1024);
		CreateDirectory(base, NULL);

		strcpy(temps, base);
		strcat(temps, "\\");
		strcat(temps, wpattern);

		prog->InitProgress("Resolution %d, calculating normal-maps:", 1024, "Tiling normals:", 0.0, majordone++, 1);
		wrteNormals1(false, OSNormalRegular->GetValue(), false, H, temps);
	      }

	      if (OSNormalHigh   ->GetValue()) {
		/* create the base directory */
		sprintf(base, "%s\\TEX-%d", dataDir.data(), 2048);
		CreateDirectory(base, NULL);

		strcpy(temps, base);
		strcat(temps, "\\");
		strcat(temps, wpattern);

		prog->InitProgress("Resolution %d, calculating normal-maps:", 2048, "Tiling normals:", 0.0, majordone++, 1);
		wrteNormals2(false, OSNormalHigh   ->GetValue(), false, H, temps);
	      }

	      freeTexture();
	    }
	    catch (exception &e) {
	      freeTexture();

	      if (strcmp(e.what(), "ExitThread")) {
		wxMessageDialog d(prog, e.what(), "Oscape error");
		d.ShowModal();
	      }

	      prog->Leave(0);
	      return;
	    }
	  }
	}
      }
    }

    /* -------------------------------------------------------------------- */
    if (OSColors->GetValue()) {
      prog->InitProgress("Initializing:", 0, "Reading surface-map:", 0.0, majordone++, 1);

      /* -------------------------------------------------------------------- */
      tlmt = (OSGame->FindItem(wxID_SKYRIM)->IsChecked() ? 8 : 1);
      tsze = tSize->GetValueAsVariant().GetInteger();
      csze = cSize->GetValueAsVariant().GetInteger();

      DefineDimensions(tsze, csze);

      WIN32_FILE_ATTRIBUTE_DATA infoh; BOOL hex =
      GetFileAttributesEx(colrFile, GetFileExInfoStandard, &infoh);
      writechk = (hex ? *((__int64 *)&infoh.ftLastWriteTime) : 0);
      if (OSOverwrite->GetValue())
	writechk = 0;

//    ifstream mntns(colrFile, ios::binary);
//    CField C(mntns);
      CView C(colrFile, width, height);

      /* -------------------------------------------------------------------- */
      if (OSColors->GetValue()) {
	writeppm = OSColorPPM->GetValue();
	writepng = OSColorPNG->GetValue();
	writedds = OSColorDDS->GetValue();

	class _single {
public:
	  _single(int s) :
	  tsze(s) {}
	  int tsze;
	};

	vector< _single > ress; size_t r = 0;

	if (OSClod1->GetValue() && (tlmt >=   1)) ress.push_back(_single(tsze *   1));
	if (OSClod2->GetValue() && (tlmt >=   2)) ress.push_back(_single(tsze *   2));
	if (OSClod3->GetValue() && (tlmt >=   4)) ress.push_back(_single(tsze *   4));
	if (OSClod4->GetValue() && (tlmt >=   8)) ress.push_back(_single(tsze *   8));

	if ((r = ress.size()) > 0) {
	  char temps[256], base[1024];
	  int target = limit;

	  for (size_t n = 0; n < r; n++) {
	    tsze       = ress[n].tsze;

	    /* -------------------------------------------------------------------- */
	    DefineDimensions(tsze, csze);
	    srandom(limit);

	    _controlfp(_RC_DOWN, _MCW_RC);	// round to -8
//	    _controlfp(_PC_53, _MCW_PC);	// round to -8

	    try {

	      if (OSColorLow    ->GetValue()) {
		/* create the base directory */
		sprintf(base, "%s\\TEX-%d", dataDir.data(), 512);
		CreateDirectory(base, NULL);

		strcpy(temps, base);
		strcat(temps, "\\");
		strcat(temps, wpattern);

		prog->InitProgress("Resolution %d, calculating color-maps:", 512, "Tiling colors:", 0.0, majordone++, 1);
		wrteColors0(OSColorLow    ->GetValue(), C, temps);
	      }

	      if (OSColorRegular->GetValue()) {
		/* create the base directory */
		sprintf(base, "%s\\TEX-%d", dataDir.data(), 1024);
		CreateDirectory(base, NULL);

		strcpy(temps, base);
		strcat(temps, "\\");
		strcat(temps, wpattern);

		prog->InitProgress("Resolution %d, calculating color-maps:", 1024, "Tiling colors:", 0.0, majordone++, 1);
		wrteColors1(OSColorRegular->GetValue(), C, temps);
	      }

	      if (OSColorHigh   ->GetValue()) {
		/* create the base directory */
		sprintf(base, "%s\\TEX-%d", dataDir.data(), 2048);
		CreateDirectory(base, NULL);

		strcpy(temps, base);
		strcat(temps, "\\");
		strcat(temps, wpattern);

		prog->InitProgress("Resolution %d, calculating color-maps:", 2048, "Tiling colors:", 0.0, majordone++, 1);
		wrteColors2(OSColorHigh   ->GetValue(), C, temps);
	      }

	      if (OSColorUltra  ->GetValue()) {
		/* create the base directory */
		sprintf(base, "%s\\TEX-%d", dataDir.data(), 4096);
		CreateDirectory(base, NULL);

		strcpy(temps, base);
		strcat(temps, "\\");
		strcat(temps, wpattern);

		prog->InitProgress("Resolution %d, calculating color-maps:", 4096, "Tiling colors:", 0.0, majordone++, 1);
		wrteColors3(OSColorUltra  ->GetValue(), C, temps);
	      }

	      freeTexture();
	    }
	    catch (exception &e) {
	      freeTexture();

	      if (strcmp(e.what(), "ExitThread")) {
		wxMessageDialog d(prog, e.what(), "Oscape error");
		d.ShowModal();
	      }

	      prog->Leave(0);
	      return;
	    }
	  }
	}
      }
    }

    prog->Leave(666);
  }
