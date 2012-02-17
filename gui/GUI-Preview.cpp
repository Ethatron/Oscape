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

  /* ---------------------------------------------------------------------------- */
  void OscapeGUI::PaintH(wxPaintEvent& event) {
    wxPaintDC dc(OSPreview);

    wxRect rg = OSPreview->GetUpdateRegion().GetBox();
    int dw = OSPreview->GetClientSize().GetWidth();
    int dh = OSPreview->GetClientSize().GetHeight();
    int sw = hdc.GetSize().GetWidth();
    int sh = hdc.GetSize().GetHeight();

#if 0
    if (hdc.IsOk())
      dc.Blit(
	0,
	0,
	OSPreview->GetClientSize().GetWidth(),
	OSPreview->GetClientSize().GetHeight(),
	&hdc,
	0,
	0,
	wxCOPY
      );
#elif 1
    if (hdc.IsOk())
      dc.StretchBlit(
	0,
	0,
	dw,
	dh,
	&hdc,
	0,
	0,
	sw,
	sh,
	wxCOPY
      );
#else
    if (hdc.IsOk())
    if ((rg.GetWidth() > 0) &&
    	(rg.GetHeight() > 0))
      dc.StretchBlit(
	rg.GetLeft(),
	rg.GetTop(),
	rg.GetWidth(),
	rg.GetHeight(),
	&hdc,
	(rg.GetLeft() * sw) / dw,
	(rg.GetTop() * sh) / dh,
	(rg.GetWidth() * sw) / dw,
	(rg.GetHeight() * sh) / dh,
	wxCOPY
      );
#endif
  }

  void OscapeGUI::RedrawH() {
    wxString ph = OSFileHeightfieldIn1->GetPath();
    if (ph.IsNull())
      return;

    int which = OSPreviewSelector->GetSelection();
    wxString pw = OSPreviewSelector->GetString(which);

    wxFileName fnn(ph); fnn.ClearExt(); fnn.SetExt("nrm");
    wxFileName fnh(ph); fnh.ClearExt(); fnh.SetExt("raw");
    wxFileName fnx(ph); fnx.ClearExt(); fnx.SetExt("land");
    wxFileName fnm(ph); fnm.ClearExt(); fnm.SetExt("fmap");

    /**/ if (pw == "Heightfield") ph = fnh.GetFullPath();
    else if (pw == "Normals"    ) ph = fnn.GetFullPath();
    else if (pw == "Features"   ) ph = fnm.GetFullPath();
    else if (pw == "Surface"    ) ph = fnx.GetFullPath();
    else return;

    /* does it exist? */
    DWORD atr = GetFileAttributes(ph.data());
    if (atr == INVALID_FILE_ATTRIBUTES)
      return;

    int tlmt = 0;
    int tsze = 0;
    int csze = 0;
    /**/ if (OSGame->FindItem(wxID_OBLIVON)->IsChecked())
      tlmt = 1, tsze = 32, csze = 32;
    else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked())
      tlmt = 8, tsze =  4, csze = 32;	// "csze" down to 4 for the lowest LOD, up-to 32 for the highest

    /* guarantee at least 32x32 LODs */
    int rasterx = tsze * csze;
    int rastery = tsze * csze;
    int tilesx = 1;
    int tilesy = 1;
    int offsx = 1;
    int offsy = 1;
    int width = rWidth->GetValueAsVariant().GetInteger();
    int height = rHeight->GetValueAsVariant().GetInteger();
    int wdscape = 0;

    {
      /**/ if (pw == "Heightfield") OSStatusBar->SetStatusText(wxT("Skimming heightfield ..."), 0);
      else if (pw == "Normals"    ) OSStatusBar->SetStatusText(wxT("Skimming normals ..."    ), 0);
      else if (pw == "Features"   ) OSStatusBar->SetStatusText(wxT("Skimming feature-map ..."), 0);
      else if (pw == "Surface"    ) OSStatusBar->SetStatusText(wxT("Skimming surface-map ..."), 0);

      OFSTRUCT of;
      HANDLE oh = (HANDLE)OpenFile(ph, &of, OF_READ);
      DWORD lph = 0;
      DWORD len = GetFileSize(oh, &lph);
      HANDLE mh = CreateFileMapping(oh,
	NULL,
	PAGE_READONLY,
	0,
	0,
	NULL);

      char *mem = (char *)MapViewOfFile(mh,
	FILE_MAP_READ,
	0,
	0,
	0);

      if (oh && mh && mem) {
	int pixel = 0;

	/**/ if (pw == "Heightfield") pixel = sizeof(unsigned short);
	else if (pw == "Normals"    ) pixel = sizeof(unsigned char) * 3;
	else if (pw == "Features"   ) pixel = sizeof(unsigned char) * 1;
	else if (pw == "Surface"    ) pixel = sizeof(unsigned long);

	tilesx = (width  + (rasterx - 1)) / rasterx;
	tilesy = (height + (rastery - 1)) / rastery;

	offsx = tlmt * ((tilesx / tlmt) >> 1);
	offsy = tlmt * ((tilesy / tlmt) >> 1);

	int scanx = 64;
	int scany = 64;
	/**/ if (OSGame->FindItem(wxID_OBLIVON)->IsChecked())
	  scanx = 64, scany = 64;
	else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked())
//	  scanx = 64, scany = 64;	// "scan" down to 8 for the lowest LOD
	  scanx = 8, scany = 8;

	int pwidth  = tilesx * scanx;
	int pheight = tilesy * scany;
	int multx = rasterx / scanx;
	int multy = rastery / scany;

	/* compensate for super-sampled sizes */
	LONGLONG
	  rlen = lph;
	  rlen <<= 32;
	  rlen += len;
	LONGLONG
	  plen = width;
	  plen *= height;
	  plen *= pixel;

	while (plen < rlen) {
	  width *= 2;
	  height *= 2;
	  multx *= 2;
	  multy *= 2;
	  plen *= 4;
	}

	unsigned char *rgb;
	unsigned char *a;
	wxImage *im = &hgt;
	if (!im->IsOk() || (im->GetWidth() != pwidth) || (im->GetHeight() != pheight)) {
	  if (im->IsOk())
	    im->Destroy();

	  if (!im->Create(pwidth, pheight, false))
	    assert(NULL);
	}

	int w = im->GetWidth();
	int h = im->GetHeight();
	rgb = im->GetData();
	if (im->HasAlpha())
	  a = im->GetAlpha();

	/* don't allow wrong sizes */
	if (plen == rlen) {
	  /**/ if (pw == "Heightfield") {
	    for (int y = 0; y < pheight; y++)
	    for (int x = 0; x < pwidth; x++) {
	      int cy = min(y * multy, height - 1);
	      int cx = min(x * multx, width  - 1);

	      /* may exceed 32bit */
	      size_t cp = width; cp *= cy; cp += cx;
	      unsigned short el = ((unsigned short *)mem)[cp];

	      if (((y % scany) == 0) ||
		  ((x % scanx) == 0))
		el >>= 1;

	      if (((y * multy) > (height - 1)) ||
		  ((x * multx) > (width  - 1)))
		el = 512;

	      rgb[((((pheight - 1) - y) * pwidth) + (x)) * 3 + 0] = min(255, el / 24);
	      rgb[((((pheight - 1) - y) * pwidth) + (x)) * 3 + 1] = min(255, el / 24);
	      rgb[((((pheight - 1) - y) * pwidth) + (x)) * 3 + 2] = min(255, el / 24);
	    }

	    im->ConvertToGreyscale();
	  }
	  else if (pw == "Features") {
	    for (int y = 0; y < pheight; y++)
	    for (int x = 0; x < pwidth; x++) {
	      int cy = min(y * multy, height - 1);
	      int cx = min(x * multx, width  - 1);

	      /* may exceed 32bit */
	      size_t cp = width; cp *= cy; cp += cx;
	      unsigned char el = ((unsigned char *)mem)[cp];

	      if (((y % scany) == 0) ||
		  ((x % scanx) == 0))
		el >>= 1;

	      if (((y * multy) > (height - 1)) ||
		  ((x * multx) > (width  - 1)))
		el = 0;

	      rgb[((((pheight - 1) - y) * pwidth) + (x)) * 3 + 0] = min(255, el);
	      rgb[((((pheight - 1) - y) * pwidth) + (x)) * 3 + 1] = min(255, el);
	      rgb[((((pheight - 1) - y) * pwidth) + (x)) * 3 + 2] = min(255, el);
	    }

	    im->ConvertToGreyscale();
	  }
	  else if (pw == "Surface") {
	    for (int y = 0; y < pheight; y++)
	    for (int x = 0; x < pwidth; x++) {
	      int cy = min(y * multy, height - 1);
	      int cx = min(x * multx, width  - 1);

	      /* may exceed 32bit */
	      size_t cp = width; cp *= cy; cp += cx;
	      unsigned long el = ((unsigned long *)mem)[cp];
	      unsigned char r = (el >> 24) & 0xFF;
	      unsigned char g = (el >> 16) & 0xFF;
	      unsigned char b = (el >>  8) & 0xFF;

	      if (((y % scany) == 0) ||
		  ((x % scanx) == 0))
		r >>= 1, g >>= 1, b >>= 1;

	      if (((y * multy) > (height - 1)) ||
		  ((x * multx) > (width  - 1)))
		r = 0, g = 0, b = 0;

	      rgb[((((pheight - 1) - y) * pwidth) + (x)) * 3 + 0] = min(255, r);
	      rgb[((((pheight - 1) - y) * pwidth) + (x)) * 3 + 1] = min(255, g);
	      rgb[((((pheight - 1) - y) * pwidth) + (x)) * 3 + 2] = min(255, b);
	    }
	  }
	}

	hdc.SelectObject(wxBitmap(hgt));

//	this->Refresh();
	OSPreview->Update();
	OSPreview->Refresh();
      }

      if (mem)
	UnmapViewOfFile(mem);
      if (mh && (mh != INVALID_HANDLE_VALUE))
	CloseHandle(mh);
      if (oh && (oh != INVALID_HANDLE_VALUE))
	CloseHandle(oh);

      OSStatusBar->SetStatusText(wxT("Ready"), 0);
    }
  }

  void OscapeGUI::ChangeHeightfieldIn1(wxFileDirPickerEvent& event) {
    wxString ph = event.GetPath();

    ChangeHeightfieldIn1(ph);
    ChangeHeightfieldIn2(ph);
    ChangeBaseDirOut1(ph);
    ChangeBaseDirOut2(ph);
  }

  void OscapeGUI::ChangeHeightfieldIn1(wxString ph) {
    OSPreviewSelector->Hide();
    OSPreviewSelector->Clear();
    OSPreview->Hide();
    OSHeightfieldFirst1->Show();
    OSHeightfieldFirst1->GetParent()->Layout();
    OSHeightfieldInfos->Clear();

    formID =
    tSize = cSize =
    rWidth = rHeight =
    tLeft = tRight = tTop = tBottom = NULL;

    if (ph.IsNull())
      return;

    /* does it exist? */
    DWORD atr = GetFileAttributes(ph.data());
    if (atr == INVALID_FILE_ATTRIBUTES)
      return;

    {
      const char *HI = ph.data();
      OSFileHeightfieldIn1->SetPath(HI);
      OSFileHeightfieldIn2->SetPath(HI);
      RegSetKeyValue(Settings, GetGameKey(), "Heightfield In", RRF_RT_REG_SZ, HI, (DWORD)strlen(HI) + 1);
    }

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

    atr = GetFileAttributes(snn.data()); bool bnn = (atr != INVALID_FILE_ATTRIBUTES);
    atr = GetFileAttributes(snh.data()); bool bnh = (atr != INVALID_FILE_ATTRIBUTES);
    atr = GetFileAttributes(snw.data()); bool bnw = (atr != INVALID_FILE_ATTRIBUTES);
    atr = GetFileAttributes(snx.data()); bool bnx = (atr != INVALID_FILE_ATTRIBUTES);
    atr = GetFileAttributes(snm.data()); bool bnm = (atr != INVALID_FILE_ATTRIBUTES);

    if (bnh) OSPreviewSelector->Append("Heightfield");
//  if (bnw) OSPreviewSelector->Append("Water");
    if (bnn) OSPreviewSelector->Append("Normals");
    if (bnm) OSPreviewSelector->Append("Features");
    if (bnx) OSPreviewSelector->Append("Surface");

    OSPreviewSelector->SetSelection(0);
    OSPreviewSelector->Show();
    OSPreview->Show();
    OSHeightfieldFirst1->Hide();
    OSHeightfieldFirst1->GetParent()->Layout();

    float ol = readWaterLevel(snw.data());
    int tlmt = 0;
    int tsze = 0;
    int csze = 0;
    /**/ if (OSGame->FindItem(wxID_OBLIVON)->IsChecked())
      tlmt = 1, tsze = 32, csze = 32;
    else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked())
      tlmt = 8, tsze =  4, csze = 32;	// "csze" down to 4 for the lowest LOD, up-to 32 for the highest

    /* guarantee at least 32x32 LODs */
    int rasterx = tsze * csze;
    int rastery = tsze * csze;
    int tilesx = 1;
    int tilesy = 1;
    int offsx = 1;
    int offsy = 1;
    int width = 1;
    int height = 1;
    int wdscape = 0;

    /* search from this field first */
    string wsn = OSWorldspace->GetValue();
    wdscape = GetWorldspace(wsn, wdscape);

    /* then check against a file-fragment */
    wxFileName fn(ph); fn.ClearExt();
    wxString wsf = fn.GetName();
    wdscape = GetWorldspacePrefix(wsf, wdscape);

    /* then check against the file-name */
    wsn = fn.GetName();
    wdscape = GetWorldspace(wsn, wdscape);

    {
      OSStatusBar->SetStatusText(wxT("Analyzing heightfield ..."), 0);

      OFSTRUCT of;
      HANDLE oh = (HANDLE)OpenFile(ph, &of, OF_READ);
      DWORD len = GetFileSize(oh, NULL);
      int pixel = len >> 1;

      if (pixel) {
	bool hor = OSOrientation->GetValue();
	bool sqr =  !OSNonSquare->GetValue();

	width  = pixel / rasterx;
	height = pixel / width;

	double l = sqrt((double)width * width + (double)height * height);
	for (int h = height; h < pixel; h += rastery) {
	  int w = pixel / h;
	  double m = sqrt((double)w * w + (double)h * h);

	  if ( pixel == (w *  h))
	  if (( sqr) || (w != h))
	  if (( hor && (l >  m)) ||
	      (!hor && (l >= m))) {
	    l = m;

	    width  = w;
	    height = h;
	  }
	}

	tilesx = (width  + (rasterx - 1)) / rasterx;
	tilesy = (height + (rastery - 1)) / rastery;

	offsx = tlmt * ((tilesx / tlmt) >> 1);
	offsy = tlmt * ((tilesy / tlmt) >> 1);
      }

      CloseHandle(oh);

      OSStatusBar->SetStatusText(wxT("Ready"), 0);
    }

    wxPGProperty *p;
    OSHeightfieldInfos->AppendCategory("Dimensions");
    OSHeightfieldInfos->Append(p = wxIntProperty("Worldspace FormID", wxPG_LABEL, wdscape)); formID = p;
    OSHeightfieldInfos->Append(p = wxIntProperty("Tilesize (in cells)", wxPG_LABEL, tsze)); tSize = p;
    OSHeightfieldInfos->Append(p = wxIntProperty("Cellsize (in units)", wxPG_LABEL, csze)); cSize = p;
    wxPGId valProp =
    OSHeightfieldInfos->Append(wxParentProperty("Total", wxPG_LABEL));
    OSHeightfieldInfos->AppendIn(valProp, p = wxIntProperty("Width", wxPG_LABEL, width)); rWidth = p;
    OSHeightfieldInfos->AppendIn(valProp, p = wxIntProperty("Height", wxPG_LABEL, height)); rHeight = p;
    wxPGId tileProp =
    OSHeightfieldInfos->Append(wxParentProperty("Cells", wxPG_LABEL));
    OSHeightfieldInfos->AppendIn(tileProp, p = wxIntProperty("Left", wxPG_LABEL, (-offsx) * tsze)); tLeft = p;
    OSHeightfieldInfos->AppendIn(tileProp, p = wxIntProperty("Top", wxPG_LABEL, (-offsy) * tsze)); tTop = p;
    OSHeightfieldInfos->AppendIn(tileProp, p = wxIntProperty("Right", wxPG_LABEL, (tilesx - offsx - 1) * tsze)); tRight = p;
    OSHeightfieldInfos->AppendIn(tileProp, p = wxIntProperty("Bottom", wxPG_LABEL, (tilesy - offsy - 1) * tsze)); tBottom = p;

    if (bnx) {
      int ss = MaxSuperSampling(width, height);
      wxString s;

      if (ss > 0)
	s.Printf("%dx Supersampled", ss);
      else
	s = "Size mismatch (invalid file)";

      OSHeightfieldInfos->Append(p = wxStringProperty("Surface", wxPG_LABEL, s)); p->SetFlag(wxPG_PROP_READONLY);
    }

    wxPGId statProp =
    OSHeightfieldInfos->AppendCategory("Config");
//  OSHeightfieldInfos->Append(p = wxIntProperty("Minimum", wxPG_LABEL, -8192)); p->SetFlag(wxPG_PROP_READONLY);
    OSHeightfieldInfos->Append(p = wxFloatProperty("Seallevel", wxPG_LABEL, ol)); fSealevel = p;
//  OSHeightfieldInfos->Append(p = wxIntProperty("Maximum", wxPG_LABEL, 55600)); p->SetFlag(wxPG_PROP_READONLY);

//  SetHeightfield(&hgt, _rt);
    RedrawH();

    OSHeightfieldFirst1->GetParent()->Layout();

    VerifyHeightfieldIn();
  }

  void OscapeGUI::ChangePreview(wxCommandEvent& event) {
    RedrawH();
  }

  void OscapeGUI::ChangeOrientation(wxCommandEvent& event) {
    ChangeHeightfieldIn1(OSFileHeightfieldIn1->GetPath());

    RegSetKeyValue(Settings, GetGameKey(), "Orientation", RRF_RT_REG_DWORD, OSOrientation->GetValue() ? "H" : "V", 2);
  }

  void OscapeGUI::ChangeNonSquare(wxCommandEvent& event) {
    ChangeHeightfieldIn1(OSFileHeightfieldIn1->GetPath());

    RegSetKeyValue(Settings, GetGameKey(), "NonSquare", RRF_RT_REG_DWORD, OSNonSquare->GetValue() ? "N" : "S", 2);
  }

  void OscapeGUI::ChangeHeightfieldInfos(wxPropertyGridEvent& event) {
    wxPGId p = event.GetProperty();

    if ((p == rWidth ->GetId()) ||
        (p == rHeight->GetId()))
      RedrawH();

    if ((p == rWidth ->GetId()) ||
	(p == rHeight->GetId()) ||
	(p == tLeft  ->GetId()) ||
	(p == tTop   ->GetId()) ||
	(p == tRight ->GetId()) ||
	(p == tBottom->GetId()))
      VerifyHeightfieldIn();
  }

  void OscapeGUI::HeightfieldAccept(wxCommandEvent& event) {
    /* next tab */
    OSToolSwitch->SetSelection(2);
  }

  /* ---------------------------------------------------------------------------- */
  void OscapeGUI::ChangePointsIn1(wxFileDirPickerEvent& event) {
    wxString ph = event.GetPath();

    ChangePointsIn1(ph);
    ChangePointsIn2(ph);
  }

  void OscapeGUI::ChangePointsIn1(wxString ph) {
    OSAdditionalPoints->ClearAll();
    OSAdditionalPoints->Enable(FALSE);
    OSPointsClear1->Enable(FALSE);

    {
      const char *PI = ph.data();
      OSFilePoints1->SetPath(PI);
      OSFilePoints2->SetPath(PI);
      RegSetKeyValue(Settings, GetGameKey(), "Points In", RRF_RT_REG_SZ, PI, (DWORD)strlen(PI) + 1);
    }

    if (ph.IsNull())
      return;

    OSAdditionalPoints->Enable(TRUE);
    OSStatusBar->SetStatusText(wxT("Reading point-file ..."), 0);

    int pos = 0;
    int row = 0;
    int col = 0;

    FILE *points;
    char PBuffer[261], line[64];
    if (!fopen_s(&points, ph, "rt")) {
      while (!feof(points)) {
	if (fgets(PBuffer, 260, points)) {
	  size_t lastpos = strlen(PBuffer) - 1;

	  if (PBuffer[lastpos] == 10 ||
	      PBuffer[lastpos] == 13)
	    PBuffer[lastpos] = 0;

	  /* allow comments and sections */
	  if ((PBuffer[0] != '\0') &&
	      (PBuffer[0] != ';') &&
	      (PBuffer[0] != '[') &&
	      (PBuffer[0] != '#')) {
	    string pb(PBuffer);

	    if ((pos & 0x3F) == 0)
	      sprintf(line, "Reading point-file line %d", pos),
	      OSStatusBar->SetStatusText(line, 0);

	    OSAdditionalPoints->InsertItem(pos++, pb);
	  }
	}
      }

      fclose(points);
    }

    OSStatusBar->SetStatusText(wxT("Ready"), 0);
    OSPointsClear1->Enable(TRUE);
  }

  void OscapeGUI::ClearPoints1(wxCommandEvent& event) {
    ChangePointsIn1("");
    ChangePointsIn2("");
  }

  void OscapeGUI::ChangeBaseDirOut1(wxFileDirPickerEvent& event) {
    wxString ph = event.GetPath();

    ChangeBaseDirOut1(ph);
    ChangeBaseDirOut2(ph);
  }

  void OscapeGUI::ChangeBaseDirOut1(wxString ph) {
    OSHeightfieldAccept->Enable(FALSE);

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

    OSHeightfieldAccept->Enable(verified);
  }
