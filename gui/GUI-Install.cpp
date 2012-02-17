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

  /*
   * Find the first occurrence of find in s, ignore case.
   */
  char *stristr(const char *s, const char *find) {
    char c, sc;
    size_t len;

    if ((c = *find++) != 0) {
      c = tolower((unsigned char)c);
      len = strlen(find);
      do {
        do {
  	if ((sc = *s++) == 0)
  	  return (NULL);
        } while ((char)tolower((unsigned char)sc) != c);
      } while (strnicmp(s, find, len) != 0);

      s--;
    }

    return ((char *)s);
  }

  BOOL CreateDirectoryRecursive(const char *pathname) {
    size_t len, l;
    char buf[1024];
    BOOL ret = FALSE;

    len = strlen(pathname);
    strcpy(buf, pathname);

    /* iterate down */
    while (!(ret = CreateDirectory(buf, NULL))) {
      char *sep = strrchr(buf, '\\');
      if (!sep)
        break;
      *sep = '\0';
    }

    /* back up again */
    while ((l = strlen(buf)) != len) {
      char *sep = buf + l;
      *sep = '\\';
      if (!(ret = CreateDirectory(buf, NULL)))
        break;
    }

    return ret;
  }

  /* ---------------------------------------------------------------------------- */
  DWORD __stdcall HeightfieldInstall(LPVOID lp) {
    if (gui)
      gui->HeightfieldInstall();
    return 0;
  }

  /* ---------------------------------------------------------------------------- */
  void OscapeGUI::CreateWorldspacePage(int wsv, struct sset *config) {
    string wsn = "unknown";
    map<int, worldset::iterator >::iterator srch = wspacef.begin();
    while (srch != wspacef.end()) {
      if (srch->first == wsv) {
	wsn = *(srch->second);
	break;
      }

      srch++;
    }

    /* make the elements adressable */
//efine wID(id)     -(wxID_Base + id)
#define wID(id)      (wxWindowID)(wxID_Base + (wxID_Page * 16) + id)
#define LEVEL0_INST 0
#define LEVEL1_INST 1
#define LEVEL2_INST 2
#define LEVEL3_INST 3
#define LEVEL0UV_INST 0
#define LEVEL1UV_INST 1
#define LEVEL2UV_INST 2
#define LEVEL3UV_INST 3
#define LEVEL0_MRES 4
#define LEVEL1_MRES 5
#define LEVEL2_MRES 6
#define LEVEL3_MRES 7
#define LEVEL0N_TRES 8
#define LEVEL1N_TRES 9
#define LEVEL2N_TRES 10
#define LEVEL3N_TRES 11
#define LEVEL0C_TRES 12
#define LEVEL1C_TRES 13
#define LEVEL2C_TRES 14
#define LEVEL3C_TRES 15

    size_t wxID_Page = OSInstallWS->GetPageCount();
    int wxID_Base = /*(wsv << 4) + 1*/ 16384;
//  wxWindow *w = this->FindWindowById(wxID_Base);
//  assert(w == NULL);

    m_scrolledWindow4 = new wxScrolledWindow( OSInstallWS, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
    m_scrolledWindow4->SetScrollRate( 5, 5 );
    wxBoxSizer* bSizer52;
    bSizer52 = new wxBoxSizer( wxVERTICAL );

    /* ----------------------------------------------------------------------- */
    wxStaticBoxSizer* OSLevel0;
    OSLevel0 = new wxStaticBoxSizer( new wxStaticBox( m_scrolledWindow4, wxID_ANY, wxT("Level0") ), wxVERTICAL );

    wxBoxSizer* bSizer28;
    bSizer28 = new wxBoxSizer( wxHORIZONTAL );

    OSInstallLevel0 = new wxCheckBox( m_scrolledWindow4, wID(LEVEL0_INST), wxT("Install"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer28->Add( OSInstallLevel0, 0, wxALL, 5 );

    OSInstallLevel0UVs = new wxCheckBox( m_scrolledWindow4, wID(LEVEL0UV_INST), wxT("with UVs"), wxDefaultPosition, wxDefaultSize, 0 );
    OSInstallLevel0UVs->Hide();

    bSizer28->Add( OSInstallLevel0UVs, 0, wxALL, 5 );

    OSLevel0->Add( bSizer28, 1, wxEXPAND, 5 );

    wxFlexGridSizer* fgSizer1;
    fgSizer1 = new wxFlexGridSizer( 1, 2, 0, 0 );
//    fgSizer1->AddGrowableCol( 0 );
    fgSizer1->AddGrowableCol( 1 );
    fgSizer1->SetFlexibleDirection( wxBOTH );
    fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_staticText40 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxT("Mesh-resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText40->Wrap( -1 );
    fgSizer1->Add( m_staticText40, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxArrayString OSInstallLevel0MeshResChoices;
    OSInstallLevel0MeshRes = new wxChoice( m_scrolledWindow4, wID(LEVEL0_MRES), wxDefaultPosition, wxDefaultSize, OSInstallLevel0MeshResChoices, 0 );
    OSInstallLevel0MeshRes->SetSelection( 0 );
    fgSizer1->Add( OSInstallLevel0MeshRes, 0, wxALL|wxEXPAND, 5 );

    m_staticText401 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxT("Texture-resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText401->Wrap( -1 );
    fgSizer1->Add( m_staticText401, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxArrayString OSInstallLevel0TextResNChoices;
    OSInstallLevel0TextResN = new wxChoice( m_scrolledWindow4, wID(LEVEL0N_TRES), wxDefaultPosition, wxDefaultSize, OSInstallLevel0TextResNChoices, 0 );
    OSInstallLevel0TextResN->SetSelection( 0 );
    fgSizer1->Add( OSInstallLevel0TextResN, 0, wxALL|wxEXPAND, 5 );

    m_staticText4014 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText4014->Wrap( -1 );
    fgSizer1->Add( m_staticText4014, 0, wxALL, 5 );

    wxArrayString OSInstallLevel0TextResCChoices;
    OSInstallLevel0TextResC = new wxChoice( m_scrolledWindow4, wID(LEVEL0C_TRES), wxDefaultPosition, wxDefaultSize, OSInstallLevel0TextResCChoices, 0 );
    OSInstallLevel0TextResC->SetSelection( 0 );
    fgSizer1->Add( OSInstallLevel0TextResC, 0, wxALL|wxEXPAND, 5 );

    OSLevel0->Add( fgSizer1, 0, wxEXPAND, 5 );

    bSizer52->Add( OSLevel0, 0, wxEXPAND, 5 );

    /* ----------------------------------------------------------------------- */
    wxStaticBoxSizer* OSLevel1;
    OSLevel1 = new wxStaticBoxSizer( new wxStaticBox( m_scrolledWindow4, wxID_ANY, wxT("Level1") ), wxVERTICAL );

    wxBoxSizer* bSizer281;
    bSizer281 = new wxBoxSizer( wxHORIZONTAL );

    OSInstallLevel1 = new wxCheckBox( m_scrolledWindow4, wID(LEVEL1_INST), wxT("Install"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer281->Add( OSInstallLevel1, 0, wxALL, 5 );

    OSInstallLevel1UVs = new wxCheckBox( m_scrolledWindow4, wID(LEVEL1UV_INST), wxT("with UVs"), wxDefaultPosition, wxDefaultSize, 0 );
    OSInstallLevel1UVs->Hide();

    bSizer281->Add( OSInstallLevel1UVs, 0, wxALL, 5 );

    OSLevel1->Add( bSizer281, 1, wxEXPAND, 5 );

    wxFlexGridSizer* fgSizer2;
    fgSizer2 = new wxFlexGridSizer( 1, 2, 0, 0 );
//    fgSizer2->AddGrowableCol( 0 );
    fgSizer2->AddGrowableCol( 1 );
    fgSizer2->SetFlexibleDirection( wxBOTH );
    fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_staticText402 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxT("Mesh-resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText402->Wrap( -1 );
    fgSizer2->Add( m_staticText402, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxArrayString OSInstallLevel1MeshResChoices;
    OSInstallLevel1MeshRes = new wxChoice( m_scrolledWindow4, wID(LEVEL1_MRES), wxDefaultPosition, wxDefaultSize, OSInstallLevel1MeshResChoices, 0 );
    OSInstallLevel1MeshRes->SetSelection( 0 );
    fgSizer2->Add( OSInstallLevel1MeshRes, 0, wxALL|wxEXPAND, 5 );

    m_staticText4011 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxT("Texture-resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText4011->Wrap( -1 );
    fgSizer2->Add( m_staticText4011, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxArrayString OSInstallLevel1TextResNChoices;
    OSInstallLevel1TextResN = new wxChoice( m_scrolledWindow4, wID(LEVEL1N_TRES), wxDefaultPosition, wxDefaultSize, OSInstallLevel1TextResNChoices, 0 );
    OSInstallLevel1TextResN->SetSelection( 0 );
    fgSizer2->Add( OSInstallLevel1TextResN, 0, wxALL|wxEXPAND, 5 );

    m_staticText40111 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText40111->Wrap( -1 );
    fgSizer2->Add( m_staticText40111, 0, wxALL, 5 );

    wxArrayString OSInstallLevel1TextResCChoices;
    OSInstallLevel1TextResC = new wxChoice( m_scrolledWindow4, wID(LEVEL1C_TRES), wxDefaultPosition, wxDefaultSize, OSInstallLevel1TextResCChoices, 0 );
    OSInstallLevel1TextResC->SetSelection( 0 );
    fgSizer2->Add( OSInstallLevel1TextResC, 0, wxALL|wxEXPAND, 5 );

    OSLevel1->Add( fgSizer2, 0, wxEXPAND, 5 );

    bSizer52->Add( OSLevel1, 0, wxEXPAND, 5 );

    /* ----------------------------------------------------------------------- */
    wxStaticBoxSizer* OSLevel2;
    OSLevel2 = new wxStaticBoxSizer( new wxStaticBox( m_scrolledWindow4, wxID_ANY, wxT("Level2") ), wxVERTICAL );

    wxBoxSizer* bSizer282;
    bSizer282 = new wxBoxSizer( wxHORIZONTAL );

    OSInstallLevel2 = new wxCheckBox( m_scrolledWindow4, wID(LEVEL2_INST), wxT("Install"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer282->Add( OSInstallLevel2, 0, wxALL, 5 );

    OSInstallLevel2UVs = new wxCheckBox( m_scrolledWindow4, wID(LEVEL2UV_INST), wxT("with UVs"), wxDefaultPosition, wxDefaultSize, 0 );
    OSInstallLevel2UVs->Hide();

    bSizer282->Add( OSInstallLevel2UVs, 0, wxALL, 5 );

    OSLevel2->Add( bSizer282, 1, wxEXPAND, 5 );

    wxFlexGridSizer* fgSizer3;
    fgSizer3 = new wxFlexGridSizer( 1, 2, 0, 0 );
//    fgSizer3->AddGrowableCol( 0 );
    fgSizer3->AddGrowableCol( 1 );
    fgSizer3->SetFlexibleDirection( wxBOTH );
    fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_staticText403 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxT("Mesh-resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText403->Wrap( -1 );
    fgSizer3->Add( m_staticText403, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxArrayString OSInstallLevel2MeshResChoices;
    OSInstallLevel2MeshRes = new wxChoice( m_scrolledWindow4, wID(LEVEL2_MRES), wxDefaultPosition, wxDefaultSize, OSInstallLevel2MeshResChoices, 0 );
    OSInstallLevel2MeshRes->SetSelection( 0 );
    fgSizer3->Add( OSInstallLevel2MeshRes, 0, wxALL|wxEXPAND, 5 );

    m_staticText4012 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxT("Texture-resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText4012->Wrap( -1 );
    fgSizer3->Add( m_staticText4012, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxArrayString OSInstallLevel2TextResNChoices;
    OSInstallLevel2TextResN = new wxChoice( m_scrolledWindow4, wID(LEVEL2N_TRES), wxDefaultPosition, wxDefaultSize, OSInstallLevel2TextResNChoices, 0 );
    OSInstallLevel2TextResN->SetSelection( 0 );
    fgSizer3->Add( OSInstallLevel2TextResN, 0, wxALL|wxEXPAND, 5 );

    m_staticText40121 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText40121->Wrap( -1 );
    fgSizer3->Add( m_staticText40121, 0, wxALL, 5 );

    wxArrayString OSInstallLevel2TextResCChoices;
    OSInstallLevel2TextResC = new wxChoice( m_scrolledWindow4, wID(LEVEL2C_TRES), wxDefaultPosition, wxDefaultSize, OSInstallLevel2TextResCChoices, 0 );
    OSInstallLevel2TextResC->SetSelection( 0 );
    fgSizer3->Add( OSInstallLevel2TextResC, 0, wxALL|wxEXPAND, 5 );

    OSLevel2->Add( fgSizer3, 0, wxEXPAND, 5 );

    bSizer52->Add( OSLevel2, 0, wxEXPAND, 5 );

    /* ----------------------------------------------------------------------- */
    wxStaticBoxSizer* OSLevel3;
    OSLevel3 = new wxStaticBoxSizer( new wxStaticBox( m_scrolledWindow4, wxID_ANY, wxT("Level3") ), wxVERTICAL );

    wxBoxSizer* bSizer283;
    bSizer283 = new wxBoxSizer( wxHORIZONTAL );

    OSInstallLevel3 = new wxCheckBox( m_scrolledWindow4, wID(LEVEL3_INST), wxT("Install"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer283->Add( OSInstallLevel3, 0, wxALL, 5 );

    OSInstallLevel3UVs = new wxCheckBox( m_scrolledWindow4, wID(LEVEL3UV_INST), wxT("with UVs"), wxDefaultPosition, wxDefaultSize, 0 );
    OSInstallLevel3UVs->Hide();

    bSizer283->Add( OSInstallLevel3UVs, 0, wxALL, 5 );

    OSLevel3->Add( bSizer283, 1, wxEXPAND, 5 );

    wxFlexGridSizer* fgSizer4;
    fgSizer4 = new wxFlexGridSizer( 1, 2, 0, 0 );
//    fgSizer4->AddGrowableCol( 0 );
    fgSizer4->AddGrowableCol( 1 );
    fgSizer4->SetFlexibleDirection( wxBOTH );
    fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_staticText404 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxT("Mesh-resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText404->Wrap( -1 );
    fgSizer4->Add( m_staticText404, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxArrayString OSInstallLevel3MeshResChoices;
    OSInstallLevel3MeshRes = new wxChoice( m_scrolledWindow4, wID(LEVEL3_MRES), wxDefaultPosition, wxDefaultSize, OSInstallLevel3MeshResChoices, 0 );
    OSInstallLevel3MeshRes->SetSelection( 0 );
    fgSizer4->Add( OSInstallLevel3MeshRes, 0, wxALL|wxEXPAND, 5 );

    m_staticText4013 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxT("Texture-resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText4013->Wrap( -1 );
    fgSizer4->Add( m_staticText4013, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxArrayString OSInstallLevel3TextResNChoices;
    OSInstallLevel3TextResN = new wxChoice( m_scrolledWindow4, wID(LEVEL3N_TRES), wxDefaultPosition, wxDefaultSize, OSInstallLevel3TextResNChoices, 0 );
    OSInstallLevel3TextResN->SetSelection( 0 );
    fgSizer4->Add( OSInstallLevel3TextResN, 0, wxALL|wxEXPAND, 5 );

    m_staticText40131 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText40131->Wrap( -1 );
    fgSizer4->Add( m_staticText40131, 0, wxALL, 5 );

    wxArrayString OSInstallLevel3TextResCChoices;
    OSInstallLevel3TextResC = new wxChoice( m_scrolledWindow4, wID(LEVEL3C_TRES), wxDefaultPosition, wxDefaultSize, OSInstallLevel3TextResCChoices, 0 );
    OSInstallLevel3TextResC->SetSelection( 0 );
    fgSizer4->Add( OSInstallLevel3TextResC, 0, wxALL|wxEXPAND, 5 );

    OSLevel3->Add( fgSizer4, 0, wxEXPAND, 5 );

    bSizer52->Add( OSLevel3, 0, wxEXPAND, 5 );

    /* ----------------------------------------------------------------------- */
    m_scrolledWindow4->SetSizer( bSizer52 );
    m_scrolledWindow4->Layout();
    bSizer52->Fit( m_scrolledWindow4 );
    OSInstallWS->AddPage( m_scrolledWindow4, wxT(wsn), false );

    OSInstallLevel0MeshRes->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( OscapeGUI::ChangeLevel0MeshRes ), NULL, this );
    OSInstallLevel1MeshRes->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( OscapeGUI::ChangeLevel1MeshRes ), NULL, this );
    OSInstallLevel2MeshRes->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( OscapeGUI::ChangeLevel2MeshRes ), NULL, this );
    OSInstallLevel3MeshRes->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( OscapeGUI::ChangeLevel3MeshRes ), NULL, this );

    /* ####################################################################### */
    char buf[256]; DWORD bufl = 256;

    buf[0] = 0; RegGetValue(Settings, GetGameKey("Installer", wsn.data()), "Install0", RRF_RT_REG_SZ, NULL, buf, &bufl);
    if (buf[0]) OSInstallLevel0->SetValue(buf[0] == '1'); bufl = 1023;
    buf[0] = 0; RegGetValue(Settings, GetGameKey("Installer", wsn.data()), "Install1", RRF_RT_REG_SZ, NULL, buf, &bufl);
    if (buf[0]) OSInstallLevel1->SetValue(buf[0] == '1'); bufl = 1023;
    buf[0] = 0; RegGetValue(Settings, GetGameKey("Installer", wsn.data()), "Install2", RRF_RT_REG_SZ, NULL, buf, &bufl);
    if (buf[0]) OSInstallLevel2->SetValue(buf[0] == '1'); bufl = 1023;
    buf[0] = 0; RegGetValue(Settings, GetGameKey("Installer", wsn.data()), "Install3", RRF_RT_REG_SZ, NULL, buf, &bufl);
    if (buf[0]) OSInstallLevel3->SetValue(buf[0] == '1'); bufl = 1023;

    /* --------------------------------------------------------------------- */
    unsigned long int res_, ress, rs;
    int numN, numY;
    int addm = 0, fndn = 0, fndc = 0, divm = 1, texn = 0, texc = 0,
	ackm = 0, tckn = 0, tckc = 0, resm = 0, resn = 0, resc = 0;

    set<unsigned long int>::const_iterator walk = config->seed.begin();
    while (walk != config->seed.end()) {
      divm = 1, addm = 0;

      /**/ if (OSGame->FindItem(wxID_OBLIVON)->IsChecked())
	rs = 32;
      else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked())
	rs = 4;

      res_ = *walk;
      ress = *walk;
      while (res_ > 1024) {
	numN = config->mesh_nu_n[(res_ << 8) | rs];
	numY = config->mesh_yu_n[(res_ << 8) | rs];

	if (numN > 0) {
	  sprintf(buf, "1/%d: %d points, no UVs, %d tiles", divm, res_, numN);

	  OSInstallLevel0MeshRes->Append(buf, (void *)-((int)res_));

	  /* prefer the highest number of points */
	  if ((ackm == 0) || ((ackm == 1) && (res_ > resm)))
	    OSInstallLevel0MeshRes->SetSelection(OSInstallLevel0MeshRes->GetCount() - 1), ackm = 1, ress = res_;

	  resm = max(resm, res_);
	}

	if (numY > 0) {
	  sprintf(buf, "1/%d: %d points, UVs, %d tiles", divm, res_, numY);

	  OSInstallLevel0MeshRes->Append(buf, (void *) ((int)res_));

	  /* prefer the highest number of points */
	  if ((ackm == 0) || ((ackm == 1) && (res_ > resm)))
	    OSInstallLevel0MeshRes->SetSelection(OSInstallLevel0MeshRes->GetCount() - 1), ackm = 1, ress = res_;

	  resm = max(resm, res_);
	}

	addm += (numN || numY) ? 1 : 0;
	res_ /= 2;
	divm *= 2;
      }

      walk++;
    }

    {
      /**/ if (OSGame->FindItem(wxID_OBLIVON)->IsChecked())
	rs = 32;
      else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked())
	rs = 4;

      res_ = 2048, fndn = 0;
      while (res_ > 64) {
	numN = config->textn_d[(res_ << 8) | rs];
	numY = config->textn_p[(res_ << 8) | rs];

	/* if there are resolution, all lower ones are available as well */
	if ((numN + numY + fndn) > 0) {
	  if (!max(numN, numY))
	    sprintf(buf, "%dx%d normals, automatic", (res_ * rs) / 32, (res_ * rs) / 32);
	  else
	    sprintf(buf, "%dx%d normals, %d tiles", (res_ * rs) / 32, (res_ * rs) / 32, max(numN, numY));

	  OSInstallLevel0TextResN->Append(buf, (void *) ((int)res_));
	  if ((tckn == 0) || ((tckn == 1) && (res_ > resn)))
	    OSInstallLevel0TextResN->SetSelection(OSInstallLevel0TextResN->GetCount() - 1), tckn = 1;

	  resn = max(resn, res_);
	}

	texn += (numN || numY || fndn) ? 1 : 0;
	fndn += (numN || numY) ? 1 : 0;
	res_ /= 2;
      }

      res_ = 4096, fndc = 0;
      while (res_ > 64) {
	numN = config->textc_d[(res_ << 8) | rs];
	numY = config->textc_p[(res_ << 8) | rs];

	/* if there are resolution, all lower ones are available as well */
	if ((numN + numY + fndc) > 0) {
	  if (!max(numN, numY))
	    sprintf(buf, "%dx%d colors, automatic", (res_ * rs) / 32, (res_ * rs) / 32);
	  else
	    sprintf(buf, "%dx%d colors, %d tiles", (res_ * rs) / 32, (res_ * rs) / 32, max(numN, numY));

	  OSInstallLevel0TextResC->Append(buf, (void *) ((int)res_));
	  if ((tckc == 0) || ((tckc == 1) && (res_ > resc)))
	    OSInstallLevel0TextResC->SetSelection(OSInstallLevel0TextResC->GetCount() - 1), tckc = 1;

	  resc = max(resc, res_);
	}

	texc += (numN || numY || fndc) ? 1 : 0;
	fndc += (numN || numY) ? 1 : 0;
	res_ /= 2;
      }

      if (addm <= 0) OSInstallLevel0MeshRes ->Enable(FALSE);
      if (texn <= 0) OSInstallLevel0TextResN->Enable(FALSE);
      if (texc <= 0) OSInstallLevel0TextResC->Enable(FALSE);
    }

    /* --------------------------------------------------------------------- */
    divm = 1, addm = 0, texn = 0, texc = 0;

    walk = config->seed.begin();
    while (walk != config->seed.end()) {
      divm = 1, addm = 0;

      res_ = *walk;
      while (res_ > 1024) {
	/* direct calculation */
	switch (*walk / res_) {
	  case 1: ackm = 0; break;
	  case 2: ackm = 1; break;
	  case 4: ackm = 2; break;
	  case 8: ackm = 3; break;
	}

	/**/ if (OSGame->FindItem(wxID_OBLIVON)->IsChecked()) {
	  rs = 32;

	  /* DirectX */
	  numN = config->mesh_nu_x[(res_ << 8) | rs];
	  numY = config->mesh_yu_x[(res_ << 8) | rs];
	}
	else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked()) {
	  rs = 4 << ackm;

	  /* BTR */
	  numN = config->mesh_nu_n[(res_ << 8) | rs];
	  numY = config->mesh_yu_n[(res_ << 8) | rs];
	}

	if (numN > 0) {
	  sprintf(buf, "1/%d: %d points, no UVs, %d tiles", divm, res_, numN);

	  /**/ if (OSGame->FindItem(wxID_OBLIVON)->IsChecked()) {
	    OSInstallLevel1MeshRes->Append(buf, (void *)-((int)res_));
	    OSInstallLevel2MeshRes->Append(buf, (void *)-((int)res_));
	    OSInstallLevel3MeshRes->Append(buf, (void *)-((int)res_));
	  }
	  else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked()) {
	    if ((ackm == 1) && (res_ < resm))
	      OSInstallLevel1MeshRes->Append(buf, (void *) ((int)res_));
	    if ((ackm == 2) && (res_ < resm))
	      OSInstallLevel2MeshRes->Append(buf, (void *) ((int)res_));
	    if ((ackm == 3) && (res_ < resm))
	      OSInstallLevel3MeshRes->Append(buf, (void *) ((int)res_));
	  }
	}

	if (numY > 0) {
	  sprintf(buf, "1/%d: %d points, UVs, %d tiles", divm, res_, numY);

	  /**/ if (OSGame->FindItem(wxID_OBLIVON)->IsChecked()) {
	    OSInstallLevel1MeshRes->Append(buf, (void *) ((int)res_));
	    OSInstallLevel2MeshRes->Append(buf, (void *) ((int)res_));
	    OSInstallLevel3MeshRes->Append(buf, (void *) ((int)res_));
	  }
	  else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked()) {
	    if ((ackm == 1) && (res_ < resm))
	      OSInstallLevel1MeshRes->Append(buf, (void *) ((int)res_));
	    if ((ackm == 2) && (res_ < resm))
	      OSInstallLevel2MeshRes->Append(buf, (void *) ((int)res_));
	    if ((ackm == 3) && (res_ < resm))
	      OSInstallLevel3MeshRes->Append(buf, (void *) ((int)res_));
	  }
	}

	if (numN + numY) {
	  /* prefer multiples of the first selected number of points */
	  if (((ackm == 3) && (res_ < resm)) || ((ackm == 4) && (res_ == (ress / res_) * res_)))
	    OSInstallLevel3MeshRes->SetSelection(OSInstallLevel3MeshRes->GetCount() - 1), ackm = 4;
	  if (((ackm == 2) && (res_ < resm)) || ((ackm == 3) && (res_ == (ress / res_) * res_)))
	    OSInstallLevel2MeshRes->SetSelection(OSInstallLevel2MeshRes->GetCount() - 1), ackm = 3;
	  if (((ackm == 1) && (res_ < resm)) || ((ackm == 2) && (res_ == (ress / res_) * res_)))
	    OSInstallLevel1MeshRes->SetSelection(OSInstallLevel1MeshRes->GetCount() - 1), ackm = 2;
	}

	addm += (numN || numY) ? 1 : 0;
	res_ /= 2;
	divm *= 2;
      }

      walk++;
    }

    {
      res_ = resn, fndn = (resn ? 1 : 0);
      while (res_ > 64) {
	/**/ if (OSGame->FindItem(wxID_OBLIVON)->IsChecked()) {
	  rs = 32;

	  numN = config->textn_d[(res_ << 8) | rs];
	  numY = config->textn_p[(res_ << 8) | rs];

	  /* if there are resolution, all lower ones are available as well */
	  if ((numN + numY + fndn) > 0) {
	    /* if there is a higher resolution and this one got none, we can calculate them */
	    if (!max(numN, numY))
	      sprintf(buf, "%dx%d normals, automatic", (res_ * rs) / 32, (res_ * rs) / 32);
	    else
	      sprintf(buf, "%dx%d normals, %d tiles", (res_ * rs) / 32, (res_ * rs) / 32, max(numN, numY));

	    OSInstallLevel1TextResN->Append(buf, (void *) ((int)res_));
	    OSInstallLevel2TextResN->Append(buf, (void *) ((int)res_));
	    OSInstallLevel3TextResN->Append(buf, (void *) ((int)res_));
	  }
	}
	else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked()) {
	  rs = 8;

	  numN = config->textn_d[(res_ << 8) | rs];
	  numY = config->textn_p[(res_ << 8) | rs];

	  /* if there are resolution, all lower ones are available as well */
	  if ((numN + numY + fndn) > 0) {
	    /* if there is a higher resolution and this one got none, we can calculate them */
	    if (!max(numN, numY))
	      sprintf(buf, "%dx%d normals, automatic", (res_ * rs) / 32, (res_ * rs) / 32);
	    else
	      sprintf(buf, "%dx%d normals, %d tiles", (res_ * rs) / 32, (res_ * rs) / 32, max(numN, numY));

	    OSInstallLevel1TextResN->Append(buf, (void *) ((int)res_));
	  }

	  rs = 16;

	  numN = config->textn_d[(res_ << 8) | rs];
	  numY = config->textn_p[(res_ << 8) | rs];

	  /* if there are resolution, all lower ones are available as well */
	  if ((numN + numY + fndn) > 0) {
	    /* if there is a higher resolution and this one got none, we can calculate them */
	    if (!max(numN, numY))
	      sprintf(buf, "%dx%d normals, automatic", (res_ * rs) / 32, (res_ * rs) / 32);
	    else
	      sprintf(buf, "%dx%d normals, %d tiles", (res_ * rs) / 32, (res_ * rs) / 32, max(numN, numY));

	    OSInstallLevel2TextResN->Append(buf, (void *) ((int)res_));
	  }

	  rs = 32;

	  numN = config->textn_d[(res_ << 8) | rs];
	  numY = config->textn_p[(res_ << 8) | rs];

	  /* if there are resolution, all lower ones are available as well */
	  if ((numN + numY + fndn) > 0) {
	    /* if there is a higher resolution and this one got none, we can calculate them */
	    if (!max(numN, numY))
	      sprintf(buf, "%dx%d normals, automatic", (res_ * rs) / 32, (res_ * rs) / 32);
	    else
	      sprintf(buf, "%dx%d normals, %d tiles", (res_ * rs) / 32, (res_ * rs) / 32, max(numN, numY));

	    OSInstallLevel3TextResN->Append(buf, (void *) ((int)res_));
	  }

	  rs = 4 << tckn;

	  numN = config->textn_d[(res_ << 8) | 8];
	  numY = config->textn_p[(res_ << 8) | 8];
	}

	if ((numN + numY + fndn) > 0) {
	  if ((tckn == 3) && (res_ < resn))
	    OSInstallLevel3TextResN->SetSelection(OSInstallLevel3TextResN->GetCount() - 1), tckn = 4;
	  if ((tckn == 2) && (res_ < resn))
	    OSInstallLevel2TextResN->SetSelection(OSInstallLevel2TextResN->GetCount() - 1), tckn = 3;
	  if ((tckn == 1) && (res_ < resn))
	    OSInstallLevel1TextResN->SetSelection(OSInstallLevel1TextResN->GetCount() - 1), tckn = 2;
	}

	texn += (numN || numY || fndn) ? 1 : 0;
	fndn += (numN || numY) ? 1 : 0;
	res_ /= 2;
      }

      res_ = resc, fndc = (resc ? 1 : 0);
      while (res_ > 64) {
	/**/ if (OSGame->FindItem(wxID_OBLIVON)->IsChecked()) {
	  rs = 32;

	  numN = config->textc_d[(res_ << 8) | rs];
	  numY = config->textc_p[(res_ << 8) | rs];

	  /* if there are resolution, all lower ones are available as well */
	  if ((numN + numY + fndc) > 0) {
	    /* if there is a higher resolution and this one got none, we can calculate them */
	    if (!max(numN, numY))
	      sprintf(buf, "%dx%d colors, automatic", (res_ * rs) / 32, (res_ * rs) / 32);
	    else
	      sprintf(buf, "%dx%d colors, %d tiles", (res_ * rs) / 32, (res_ * rs) / 32, max(numN, numY));

	    OSInstallLevel1TextResC->Append(buf, (void *) ((int)res_));
	    OSInstallLevel2TextResC->Append(buf, (void *) ((int)res_));
	    OSInstallLevel3TextResC->Append(buf, (void *) ((int)res_));
	  }
	}
	else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked()) {
	  rs = 8;

	  numN = config->textc_d[(res_ << 8) | rs];
	  numY = config->textc_p[(res_ << 8) | rs];

	  /* if there are resolution, all lower ones are available as well */
	  if ((numN + numY + fndc) > 0) {
	    /* if there is a higher resolution and this one got none, we can calculate them */
	    if (!max(numN, numY))
	      sprintf(buf, "%dx%d colors, automatic", (res_ * rs) / 32, (res_ * rs) / 32);
	    else
	      sprintf(buf, "%dx%d colors, %d tiles", (res_ * rs) / 32, (res_ * rs) / 32, max(numN, numY));

	    OSInstallLevel1TextResC->Append(buf, (void *) ((int)res_));
	  }

	  rs = 16;

	  numN = config->textc_d[(res_ << 8) | rs];
	  numY = config->textc_p[(res_ << 8) | rs];

	  /* if there are resolution, all lower ones are available as well */
	  if ((numN + numY + fndc) > 0) {
	    /* if there is a higher resolution and this one got none, we can calculate them */
	    if (!max(numN, numY))
	      sprintf(buf, "%dx%d colors, automatic", (res_ * rs) / 32, (res_ * rs) / 32);
	    else
	      sprintf(buf, "%dx%d colors, %d tiles", (res_ * rs) / 32, (res_ * rs) / 32, max(numN, numY));

	    OSInstallLevel2TextResC->Append(buf, (void *) ((int)res_));
	  }

	  rs = 32;

	  numN = config->textc_d[(res_ << 8) | rs];
	  numY = config->textc_p[(res_ << 8) | rs];

	  /* if there are resolution, all lower ones are available as well */
	  if ((numN + numY + fndc) > 0) {
	    /* if there is a higher resolution and this one got none, we can calculate them */
	    if (!max(numN, numY))
	      sprintf(buf, "%dx%d colors, automatic", (res_ * rs) / 32, (res_ * rs) / 32);
	    else
	      sprintf(buf, "%dx%d colors, %d tiles", (res_ * rs) / 32, (res_ * rs) / 32, max(numN, numY));

	    OSInstallLevel3TextResC->Append(buf, (void *) ((int)res_));
	  }

	  rs = 4 << tckc;

	  numN = config->textc_d[(res_ << 8) | rs];
	  numY = config->textc_p[(res_ << 8) | rs];
	}

	if ((numN + numY + fndc) > 0) {
	  if ((tckc == 3) && (res_ < resc))
	    OSInstallLevel3TextResC->SetSelection(OSInstallLevel3TextResC->GetCount() - 1), tckc = 4;
	  if ((tckc == 2) && (res_ < resc))
	    OSInstallLevel2TextResC->SetSelection(OSInstallLevel2TextResC->GetCount() - 1), tckc = 3;
	  if ((tckc == 1) && (res_ < resc))
	    OSInstallLevel1TextResC->SetSelection(OSInstallLevel1TextResC->GetCount() - 1), tckc = 2;
	}

	texc += (numN || numY || fndc) ? 1 : 0;
	fndc += (numN || numY) ? 1 : 0;
	res_ /= 2;
      }
    }

    OSInstallLevel0MeshRes ->Append("None", (void *)-((int)0));
    OSInstallLevel0TextResN->Append("None", (void *)-((int)0));
    OSInstallLevel0TextResC->Append("None", (void *)-((int)0));

    OSInstallLevel1MeshRes ->Append("None", (void *)-((int)0));
    OSInstallLevel1TextResN->Append("None", (void *)-((int)0));
    OSInstallLevel1TextResC->Append("None", (void *)-((int)0));

    OSInstallLevel2MeshRes ->Append("None", (void *)-((int)0));
    OSInstallLevel2TextResN->Append("None", (void *)-((int)0));
    OSInstallLevel2TextResC->Append("None", (void *)-((int)0));

    OSInstallLevel3MeshRes ->Append("None", (void *)-((int)0));
    OSInstallLevel3TextResN->Append("None", (void *)-((int)0));
    OSInstallLevel3TextResC->Append("None", (void *)-((int)0));

    /* --------------------------------------------------------------------- */
    {
      if (ackm <= 1) {
	if (!texn && !texc)
	           OSLevel1->ShowItems(FALSE),
		   OSInstallLevel1->SetValue(FALSE);
	else       OSInstallLevel1MeshRes ->Enable(FALSE); }
      if (!texn)   OSInstallLevel1TextResN->Enable(FALSE);
      if (!texc)   OSInstallLevel1TextResC->Enable(FALSE);

      if (ackm <= 2) {
	if (!texn && !texc)
		   OSLevel2->ShowItems(FALSE),
		   OSInstallLevel2->SetValue(FALSE);
	else       OSInstallLevel2MeshRes ->Enable(FALSE); }
      if (!texn)   OSInstallLevel2TextResN->Enable(FALSE);
      if (!texc)   OSInstallLevel2TextResC->Enable(FALSE);

      if (ackm <= 3) {
	if (!texn && !texc)
		   OSLevel3->ShowItems(FALSE),
		   OSInstallLevel3->SetValue(FALSE);
	else       OSInstallLevel3MeshRes ->Enable(FALSE); }
      if (!texn)   OSInstallLevel3TextResN->Enable(FALSE);
      if (!texc)   OSInstallLevel3TextResC->Enable(FALSE);

//    OSInstallLevel0MeshRes->SetSelection(0);
//    OSInstallLevel1MeshRes->SetSelection(1);
//    OSInstallLevel2MeshRes->SetSelection(2);
//    OSInstallLevel3MeshRes->SetSelection(3);
    }
  }

  void OscapeGUI::ChangeLevel0MeshRes(wxCommandEvent& event) {
    /* applies only to skyrim */
    if (OSGame->FindItem(wxID_OBLIVON)->IsChecked())
      return;

    int wxID_Base = /*(wsv << 4) + 1*/ 16384;
    int wxID_Page = OSInstallWS->GetSelection();
    wxWindow *pg = OSInstallWS->GetPage(wxID_Page);

    wxChoice   *m0 = (wxChoice   *)pg->FindWindowById(wID(LEVEL0_MRES));
    wxChoice   *m1 = (wxChoice   *)pg->FindWindowById(wID(LEVEL1_MRES));
    wxChoice   *m2 = (wxChoice   *)pg->FindWindowById(wID(LEVEL2_MRES));
    wxChoice   *m3 = (wxChoice   *)pg->FindWindowById(wID(LEVEL3_MRES));

//  int mS = event.GetSelection();
    int mS0 = (int)event.GetClientData();

    int c1 = m1->GetCount();
    for (int c = 0; c < c1; c++) {
      int mS1 = (int)m1->GetClientData(c);
      if (mS1 == (mS0 >> 1)) {
	m1->SetSelection(c); break; }
    }

    int c2 = m2->GetCount();
    for (int c = 0; c < c2; c++) {
      int mS2 = (int)m2->GetClientData(c);
      if (mS2 == (mS0 >> 2)) {
	m2->SetSelection(c); break; }
    }

    int c3 = m3->GetCount();
    for (int c = 0; c < c3; c++) {
      int mS3 = (int)m3->GetClientData(c);
      if (mS3 == (mS0 >> 3)) {
	m3->SetSelection(c); break; }
    }
  }

  void OscapeGUI::ChangeLevel1MeshRes(wxCommandEvent& event) {
    /* applies only to skyrim */
    if (OSGame->FindItem(wxID_OBLIVON)->IsChecked())
      return;

    int wxID_Base = /*(wsv << 4) + 1*/ 16384;
    int wxID_Page = OSInstallWS->GetSelection();
    wxWindow *pg = OSInstallWS->GetPage(wxID_Page);

    wxChoice   *m0 = (wxChoice   *)pg->FindWindowById(wID(LEVEL0_MRES));
    wxChoice   *m1 = (wxChoice   *)pg->FindWindowById(wID(LEVEL1_MRES));
    wxChoice   *m2 = (wxChoice   *)pg->FindWindowById(wID(LEVEL2_MRES));
    wxChoice   *m3 = (wxChoice   *)pg->FindWindowById(wID(LEVEL3_MRES));

//  int mS = event.GetSelection();
    int mS1 = (int)event.GetClientData();

    int c0 = m0->GetCount();
    for (int c = 0; c < c0; c++) {
      int mS0 = (int)m0->GetClientData(c);
      if (mS0 == (mS1 << 1)) {
	m0->SetSelection(c); break; }
    }

    int c2 = m2->GetCount();
    for (int c = 0; c < c2; c++) {
      int mS2 = (int)m2->GetClientData(c);
      if (mS2 == (mS1 >> 1)) {
	m2->SetSelection(c); break; }
    }

    int c3 = m3->GetCount();
    for (int c = 0; c < c3; c++) {
      int mS3 = (int)m3->GetClientData(c);
      if (mS3 == (mS1 >> 2)) {
	m3->SetSelection(c); break; }
    }
  }

  void OscapeGUI::ChangeLevel2MeshRes(wxCommandEvent& event) {
    /* applies only to skyrim */
    if (OSGame->FindItem(wxID_OBLIVON)->IsChecked())
      return;

    int wxID_Base = /*(wsv << 4) + 1*/ 16384;
    int wxID_Page = OSInstallWS->GetSelection();
    wxWindow *pg = OSInstallWS->GetPage(wxID_Page);

    wxChoice   *m0 = (wxChoice   *)pg->FindWindowById(wID(LEVEL0_MRES));
    wxChoice   *m1 = (wxChoice   *)pg->FindWindowById(wID(LEVEL1_MRES));
    wxChoice   *m2 = (wxChoice   *)pg->FindWindowById(wID(LEVEL2_MRES));
    wxChoice   *m3 = (wxChoice   *)pg->FindWindowById(wID(LEVEL3_MRES));

//  int mS = event.GetSelection();
    int mS2 = (int)event.GetClientData();

    int c0 = m0->GetCount();
    for (int c = 0; c < c0; c++) {
      int mS0 = (int)m0->GetClientData(c);
      if (mS0 == (mS2 << 2)) {
	m0->SetSelection(c); break; }
    }

    int c1 = m1->GetCount();
    for (int c = 0; c < c1; c++) {
      int mS1 = (int)m1->GetClientData(c);
      if (mS1 == (mS2 << 1)) {
	m1->SetSelection(c); break; }
    }

    int c3 = m3->GetCount();
    for (int c = 0; c < c3; c++) {
      int mS3 = (int)m3->GetClientData(c);
      if (mS3 == (mS2 >> 1)) {
	m3->SetSelection(c); break; }
    }
  }

  void OscapeGUI::ChangeLevel3MeshRes(wxCommandEvent& event) {
    /* applies only to skyrim */
    if (OSGame->FindItem(wxID_OBLIVON)->IsChecked())
      return;

    int wxID_Base = /*(wsv << 4) + 1*/ 16384;
    int wxID_Page = OSInstallWS->GetSelection();
    wxWindow *pg = OSInstallWS->GetPage(wxID_Page);

    wxChoice   *m0 = (wxChoice   *)pg->FindWindowById(wID(LEVEL0_MRES));
    wxChoice   *m1 = (wxChoice   *)pg->FindWindowById(wID(LEVEL1_MRES));
    wxChoice   *m2 = (wxChoice   *)pg->FindWindowById(wID(LEVEL2_MRES));
    wxChoice   *m3 = (wxChoice   *)pg->FindWindowById(wID(LEVEL3_MRES));

//  int mS = event.GetSelection();
    int mS3 = (int)event.GetClientData();

    int c0 = m0->GetCount();
    for (int c = 0; c < c0; c++) {
      int mS0 = (int)m0->GetClientData(c);
      if (mS0 == (mS3 << 3)) {
	m0->SetSelection(c); break; }
    }

    int c1 = m1->GetCount();
    for (int c = 0; c < c1; c++) {
      int mS1 = (int)m1->GetClientData(c);
      if (mS1 == (mS3 << 2)) {
	m1->SetSelection(c); break; }
    }

    int c2 = m2->GetCount();
    for (int c = 0; c < c2; c++) {
      int mS2 = (int)m2->GetClientData(c);
      if (mS2 == (mS3 << 1)) {
	m2->SetSelection(c); break; }
    }
  }

  void OscapeGUI::ChangeBaseDirIn(wxFileDirPickerEvent& event) {
    ChangeBaseDirIn(event.GetPath());
  }

  void OscapeGUI::ChangeBaseDirIn(wxString ph) {
    OSInstallWS->Hide();
    OSHeightfieldFirst3->SetLabel("Select a directory first");
    OSHeightfieldFirst3->Show();
    OSHeightfieldFirst3->GetParent()->Layout();

    if (ph.IsNull())
      return;
    {
      const char *BO = ph.data();
      OSBaseDirIn->SetPath(BO);
      RegSetKeyValue(Settings, GetGameKey(), "Base directory In", RRF_RT_REG_SZ, BO, (DWORD)strlen(BO) + 1);
    }

    OSStatusBar->SetStatusText(wxT("Skimming directory ..."), 0);

    map<long, struct sset> wsset;

    // worldspace, mesh-res, texture-res

    /* look for all the directories */
    char temp[1024], base[1024];
    sprintf(base, "%s\\*", ph.data(), 1024);
    long res; int ws, cx, cy, rs;

    HANDLE IFiles; WIN32_FIND_DATA IFound;
    HANDLE RFiles; WIN32_FIND_DATA RFound;
    if ((IFiles = FindFirstFileEx(base, FindExInfoBasic, &IFound, FindExSearchNameMatch, NULL, 0)) != INVALID_HANDLE_VALUE) {
      do {
	const char *file = IFound.cFileName;

	if ((file == stristr(file, "LOD-"))) {
	  if (sscanf(file + 4, "%d", &res) == 1) {
	    sprintf(temp, "LOD-%d", res);
	    if (!stricmp(file, temp)) {
	      sprintf(temp, "%s\\LOD-%d\\*.pts", ph.data(), res);
	      if ((RFiles = FindFirstFileEx(temp, FindExInfoBasic, &RFound, FindExSearchNameMatch, NULL, 0)) != INVALID_HANDLE_VALUE) {
		do {
		  /* Any */
		  const char *subfile = RFound.cFileName;
		  if (sscanf(subfile, "%d.pts", &ws) == 1) {
		    struct sset *match = &wsset[ws];
		    match->seed.insert(res);
		  }
		} while (FindNextFile(RFiles, &RFound));

		FindClose(RFiles);
	      }

	      sprintf(temp, "%s\\LOD-%d\\UVon\\*", ph.data(), res);
	      if ((RFiles = FindFirstFileEx(temp, FindExInfoBasic, &RFound, FindExSearchNameMatch, NULL, 0)) != INVALID_HANDLE_VALUE) {
		do {
		  const char *subfile = RFound.cFileName;

		  /* Oblivion */
		  if (OSGame->FindItem(wxID_OBLIVON)->IsChecked()) {
		    if (sscanf(subfile, "%d.%d.%d.%d", &ws, &cx, &cy, &rs) == 4) {
		      struct sset *match = &wsset[ws];
		      if (stristr(subfile, ".x"  )) match->mesh_yu_x[(res << 8) | rs]++;
		      if (stristr(subfile, ".nif")) match->mesh_yu_n[(res << 8) | rs]++;
		    }
		  }

		  /* Skyrim */
		  if (OSGame->FindItem(wxID_SKYRIM)->IsChecked()) {
		    if ((subfile = strchr(RFound.cFileName, '.')))
		    if ((ws = GetWorldspacePrefix(RFound.cFileName)))
		    if (sscanf(subfile, ".%d.%d.%d", &rs, &cx, &cy) == 3) {
		      struct sset *match = &wsset[ws];
		      if (stristr(subfile, ".x"  )) match->mesh_yu_x[(res << 8) | rs]++;
		      if (stristr(subfile, ".btr")) match->mesh_yu_n[(res << 8) | rs]++;
		    }
		  }
		} while (FindNextFile(RFiles, &RFound));

		FindClose(RFiles);
	      }

	      sprintf(temp, "%s\\LOD-%d\\UVoff\\*", ph.data(), res);
	      if ((RFiles = FindFirstFileEx(temp, FindExInfoBasic, &RFound, FindExSearchNameMatch, NULL, 0)) != INVALID_HANDLE_VALUE) {
		do {
		  const char *subfile = RFound.cFileName;

		  /* Oblivion */
		  if (OSGame->FindItem(wxID_OBLIVON)->IsChecked()) {
		    if (sscanf(subfile, "%d.%d.%d.%d", &ws, &cx, &cy, &rs) == 4) {
		      struct sset *match = &wsset[ws];
		      if (stristr(subfile, ".x"  )) match->mesh_nu_x[(res << 8) | rs]++;
		      if (stristr(subfile, ".nif")) match->mesh_nu_n[(res << 8) | rs]++;
		    }
		  }

		  /* Skyrim */
		  if (OSGame->FindItem(wxID_SKYRIM)->IsChecked()) {
		    if ((subfile = strchr(RFound.cFileName, '.')))
		    if ((ws = GetWorldspacePrefix(RFound.cFileName)))
		    if (sscanf(subfile, ".%d.%d.%d", &rs, &cx, &cy) == 3) {
		      struct sset *match = &wsset[ws];
		      if (stristr(subfile, ".x"  )) match->mesh_nu_x[(res << 8) | rs]++;
		      if (stristr(subfile, ".btr")) match->mesh_nu_n[(res << 8) | rs]++;
		    }
		  }
		} while (FindNextFile(RFiles, &RFound));

		FindClose(RFiles);
	      }

//	      wsset.push_back(res);
	    }
	  }
	}

	if ((file == stristr(file, "TEX-"))) {
	  if (sscanf(file + 4, "%d", &res) == 1) {
	    sprintf(temp, "TEX-%d", res);
	    if (!stricmp(file, temp)) {
	      sprintf(temp, "%s\\TEX-%d\\*", ph.data(), res);
	      if ((RFiles = FindFirstFileEx(temp, FindExInfoBasic, &RFound, FindExSearchNameMatch, NULL, 0)) != INVALID_HANDLE_VALUE) {
		do {
		  const char *subfile = RFound.cFileName;

		  /* Oblivion */
		  if (OSGame->FindItem(wxID_OBLIVON)->IsChecked()) {
		    if (sscanf(subfile, "%d.%d.%d.%d", &ws, &cx, &cy, &rs) == 4) {
		      struct sset *match = &wsset[ws];

		      /* a bit weak, but works */
		      if (stristr(subfile, ".dds")) {
			if (stristr(subfile, "_fn"))
			  match->textn_d[(res << 8) | rs]++;
			else
			  match->textc_d[(res << 8) | rs]++;
		      }

		      /* a bit weak, but works */
		      if (stristr(subfile, ".png")) {
			if (stristr(subfile, "_fn"))
			  match->textn_p[(res << 8) | rs]++;
			else
			  match->textc_p[(res << 8) | rs]++;
		      }
		    }
		  }

		  /* Skyrim */
		  if (OSGame->FindItem(wxID_SKYRIM)->IsChecked()) {
		    if ((subfile = strchr(RFound.cFileName, '.')))
		    if ((ws = GetWorldspacePrefix(RFound.cFileName)))
		    if (sscanf(subfile, ".%d.%d.%d", &rs, &cx, &cy) == 3) {
		      struct sset *match = &wsset[ws];

		      /* a bit weak, but works */
		      if (stristr(subfile, ".dds")) {
			if (stristr(subfile, "_n"))
			  match->textn_d[(res << 8) | rs]++;
			else
			  match->textc_d[(res << 8) | rs]++;
		      }

		      /* a bit weak, but works */
		      if (stristr(subfile, ".png")) {
			if (stristr(subfile, "_n"))
			  match->textn_p[(res << 8) | rs]++;
			else
			  match->textc_p[(res << 8) | rs]++;
		      }
		    }
		  }
		} while (FindNextFile(RFiles, &RFound));

		FindClose(RFiles);
	      }

//	      wsset.push_back(res);
	    }
	  }
	}

      } while (FindNextFile(IFiles, &IFound));

      FindClose(IFiles);
    }

    if (wsset.size() == 0) {
      OSHeightfieldFirst3->SetLabel("No installable files found");
      OSStatusBar->SetStatusText(wxT("Ready"), 0);
      return;
    }

    OSInstallWS->DeleteAllPages();
    map<long, struct sset>::iterator walk = wsset.begin();
    while (walk != wsset.end())
      CreateWorldspacePage(walk->first, &walk->second), walk++;

    OSStatusBar->SetStatusText(wxT("Ready"), 0);

    OSInstallWS->Show();
    OSHeightfieldFirst3->Hide();
    OSHeightfieldFirst3->GetParent()->Layout();
  }

  void OscapeGUI::ChangePlugoutDir(wxFileDirPickerEvent& event) {
    ChangePlugoutDir(event.GetPath());
  }

  void OscapeGUI::ChangePlugoutDir(wxString ph) {
    OSHeightfieldInstall->Enable(FALSE);

    if (ph.IsNull())
      return;

    OSHeightfieldInstall->Enable(TRUE);
  }

  void OscapeGUI::HeightfieldInstall(wxCommandEvent& event) {
    OSStatusBar->SetStatusText(wxT("Running installer ..."), 0);
    wxBusyCursor wait;
    prog = new OscapePrg(this);
    int ret = prog->Enter(::HeightfieldInstall);
    delete prog;
    OSStatusBar->SetStatusText(wxT("Ready"), 0);
    prog = NULL;
    if (ret != 666)
      return;
  }

  bool OscapeGUI::SynchronizeInstall(int wsv, int res, const char *subdir, int rs, const char *lid) {
    wxString ph = OSBaseDirIn->GetPath();
    HANDLE RFiles; WIN32_FIND_DATA RFound;
    vector< pair<string, string> > copy;
    vector< pair<string, string> >::const_iterator walk;
    int dne = 0;
    wxString wss = *(wspacef[wsv]);
    char tpc[256], trgm[261];
    char base[1024], temp[1024];

    /* --------------------------------------------------------------------- */
    sprintf(tpc, "Level %s meshes:", lid);
    prog->InitProgress(NULL, 0, tpc, 0.0, installdone++, 1);

    /**/ if (OSGame->FindItem(wxID_OBLIVON)->IsChecked()) {
      strcpy(trgm, OSPlugoutDir->GetPath().data());
      strcat(trgm, "\\Meshes\\Landscape\\LOD");
      if (subdir)
	strcat(trgm, "\\"),
	strcat(trgm, subdir);
      CreateDirectoryRecursive(trgm);
      strcat(trgm, "\\");

      /* look for all the directories */
      sprintf(temp, "%s\\LOD-%d\\%s\\"         , ph.data(), abs(res), res > 0 ? "UVon" : "UVoff"                           );
      sprintf(base, "%s\\LOD-%d\\%s\\%02d.*.%s", ph.data(), abs(res), res > 0 ? "UVon" : "UVoff", wsv, subdir ? "x" : "nif");
    }
    else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked()) {
      strcpy(trgm, OSPlugoutDir->GetPath().data());
      strcat(trgm, "\\Meshes\\Terrain");
      if (wss.length())
	strcat(trgm, "\\"),
	strcat(trgm, wss.data());
      CreateDirectoryRecursive(trgm);
      strcat(trgm, "\\");

      /* look for all the directories */
      sprintf(temp, "%s\\LOD-%d\\%s\\"          , ph.data(), abs(res), res > 0 ? "UVon" : "UVoff"                       );
      sprintf(base, "%s\\LOD-%d\\%s\\%s.%d.*.%s", ph.data(), abs(res), res > 0 ? "UVon" : "UVoff", wss.data(), rs, "btr");
    }

    copy.clear();
    if ((RFiles = FindFirstFileEx(base, FindExInfoBasic, &RFound, FindExSearchNameMatch, NULL, 0)) != INVALID_HANDLE_VALUE) {
      string in = temp;
      string ou = trgm;

      do {
	copy.push_back(pair<string, string>(
	  in + RFound.cFileName,
	  ou + RFound.cFileName
	));
      } while (FindNextFile(RFiles, &RFound));

      FindClose(RFiles);
    }

    /* --------------------------------------------------------------------- */
    prog->InitProgress((int)copy.size());
    dne = 0; walk = copy.begin();
    while (walk != copy.end()) {
      while (!CopyFile(walk->first.data(), walk->second.data(), FALSE)) {
	char buf[256]; sprintf(buf, "Failed to copy file \"%s\". Retry?", strrchr(walk->second.data(), '\\') + 1);
	wxMessageDialog d(this, buf, "Oscape error", wxYES_NO | wxCANCEL | wxCENTRE);
	int ret = d.ShowModal();
	if (ret == wxID_CANCEL)
	  return false;
	if (ret == wxID_NO)
	  break;
      }

      prog->SetProgress(dne += 1);
      walk++;
    }

    return true;
  }

  bool OscapeGUI::SynchronizeInstall(int wsv, bool n, int res, const char *subdir, int rs, const char *lid) {
    wxString ph = OSBaseDirIn->GetPath();
    HANDLE RFiles; WIN32_FIND_DATA RFound;
    vector< pair<string, string> > copy;
    vector< pair<string, string> >::const_iterator walk;
    int dne = 0;
    wxString wss = *(wspacef[wsv]);
    char tpc[256], trgt[261];
    char base[1024], temp[1024];
    const char *ext = "";

    /* --------------------------------------------------------------------- */
    sprintf(tpc, "Level %s %s textures:", lid, n ? "normal" : "color");
    prog->InitProgress(NULL, 0, tpc, 0.0, installdone++, 1);

    /**/ if (OSGame->FindItem(wxID_OBLIVON)->IsChecked()) {
      strcpy(trgt, OSPlugoutDir->GetPath().data());
      strcat(trgt, "\\Textures\\LandscapeLOD\\Generated");
      if (subdir)
	strcat(trgt, "\\"),
	strcat(trgt, subdir);
      CreateDirectoryRecursive(trgt);
      strcat(trgt, "\\");

      /* look for all the directories */
      sprintf(temp, "%s\\TEX-%d\\"          , ph.data(), abs(res)     );
      sprintf(base, "%s\\TEX-%d\\%02d.*.dds", ph.data(), abs(res), wsv);

      ext = "_fn";
    }
    else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked()) {
      strcpy(trgt, OSPlugoutDir->GetPath().data());
      strcat(trgt, "\\Textures\\Terrain");
      if (wss.length())
	strcat(trgt, "\\"),
	strcat(trgt, wss.data());
      CreateDirectoryRecursive(trgt);
      strcat(trgt, "\\");

      /* look for all the directories */
      sprintf(temp, "%s\\TEX-%d\\"           , ph.data(), abs(res)                );
      sprintf(base, "%s\\TEX-%d\\%s.%d.*.dds", ph.data(), abs(res), wss.data(), rs);

      ext = "_n";
    }

    copy.clear();
    if ((RFiles = FindFirstFileEx(base, FindExInfoBasic, &RFound, FindExSearchNameMatch, NULL, 0)) != INVALID_HANDLE_VALUE) {
      string in = temp;
      string ou = trgt;

      do {
	if (( n &&  stristr(RFound.cFileName, ext)) ||
	    (!n && !stristr(RFound.cFileName, ext)))
	  copy.push_back(pair<string, string>(
	    in + RFound.cFileName,
	    ou + RFound.cFileName
	  ));
      } while (FindNextFile(RFiles, &RFound));

      FindClose(RFiles);
    }

    /* --------------------------------------------------------------------- */
    prog->InitProgress((int)copy.size());
    dne = 0; walk = copy.begin();
    while (walk != copy.end()) {
      while (!CopyFile(walk->first.data(), walk->second.data(), FALSE)) {
	char buf[256]; sprintf(buf, "Failed to copy file \"%s\". Retry?", strrchr(walk->second.data(), '\\') + 1);
	wxMessageDialog d(this, buf, "Oscape error", wxYES_NO | wxCANCEL | wxCENTRE);
	int ret = d.ShowModal();
	if (ret == wxID_CANCEL)
	  return false;
	if (ret == wxID_NO)
	  break;
      }

      prog->SetProgress(dne += 1);
      walk++;
    }

    /* --------------------------------------------------------------------- */
    if (copy.size() == 0) {
      /* recalculate levels after conversion */
      int w = (res * rs) / 32;
      int h = (res * rs) / 32;
      int levels = 1; {
	int ww = w;
	int hh = h;
	while ((ww > 1) && (hh > 1)) {
	  ww = (ww + 1) >> 1;
	  hh = (hh + 1) >> 1;

	  levels++;
	}
      }

      /* now we search:
       * - same resolution PNGs?
       * - higher resolution PNGs?
       * - higher resolution DDSs?
       */
      int tcnv = res;
      int tmax = n ? 2048 : 4096;
      while (tcnv <= tmax) {
	/* higher resolution PPMs? */
	/**/ if (OSGame->FindItem(wxID_OBLIVON)->IsChecked()) {
	  sprintf(temp, "%s\\TEX-%d\\"          , ph.data(), abs(tcnv)     );
	  sprintf(base, "%s\\TEX-%d\\%02d.*.ppm", ph.data(), abs(tcnv), wsv);
	}
	else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked()) {
	  sprintf(temp, "%s\\TEX-%d\\"           , ph.data(), abs(tcnv)                );
	  sprintf(base, "%s\\TEX-%d\\%s.%d.*.ppm", ph.data(), abs(tcnv), wss.data(), rs);
	}

	copy.clear();
	if ((RFiles = FindFirstFileEx(base, FindExInfoBasic, &RFound, FindExSearchNameMatch, NULL, 0)) != INVALID_HANDLE_VALUE) {
	  string in = temp;
	  string ou = trgt;

	  do {
	    if (( n &&  stristr(RFound.cFileName, ext)) ||
		(!n && !stristr(RFound.cFileName, ext)))
	      copy.push_back(pair<string, string>(
		in + RFound.cFileName,
		ou + RFound.cFileName
	      ));
	  } while (FindNextFile(RFiles, &RFound));

	  FindClose(RFiles);
	}

	if (copy.size() != 0)
	  break;

	/* higher resolution PNGs? */
	/**/ if (OSGame->FindItem(wxID_OBLIVON)->IsChecked()) {
	  sprintf(temp, "%s\\TEX-%d\\"          , ph.data(), abs(tcnv)     );
	  sprintf(base, "%s\\TEX-%d\\%02d.*.png", ph.data(), abs(tcnv), wsv);
	}
	else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked()) {
	  sprintf(temp, "%s\\TEX-%d\\"           , ph.data(), abs(tcnv)                );
	  sprintf(base, "%s\\TEX-%d\\%s.%d.*.png", ph.data(), abs(tcnv), wss.data(), rs);
	}

	copy.clear();
	if ((RFiles = FindFirstFileEx(base, FindExInfoBasic, &RFound, FindExSearchNameMatch, NULL, 0)) != INVALID_HANDLE_VALUE) {
	  string in = temp;
	  string ou = trgt;

	  do {
	    if (( n &&  stristr(RFound.cFileName, ext)) ||
		(!n && !stristr(RFound.cFileName, ext)))
	      copy.push_back(pair<string, string>(
		in + RFound.cFileName,
		ou + RFound.cFileName
	      ));
	  } while (FindNextFile(RFiles, &RFound));

	  FindClose(RFiles);
	}

	if (copy.size() != 0)
	  break;

	/* higher resolution DDSs? */
	/**/ if (OSGame->FindItem(wxID_OBLIVON)->IsChecked()) {
	  sprintf(temp, "%s\\TEX-%d\\"          , ph.data(), abs(tcnv)     );
	  sprintf(base, "%s\\TEX-%d\\%02d.*.dds", ph.data(), abs(tcnv), wsv);
	}
	else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked()) {
	  sprintf(temp, "%s\\TEX-%d\\"           , ph.data(), abs(tcnv)                );
	  sprintf(base, "%s\\TEX-%d\\%s.%d.*.dds", ph.data(), abs(tcnv), wss.data(), rs);
	}

	copy.clear();
	if ((RFiles = FindFirstFileEx(base, FindExInfoBasic, &RFound, FindExSearchNameMatch, NULL, 0)) != INVALID_HANDLE_VALUE) {
	  string in = temp;
	  string ou = trgt;

	  do {
	    if (( n &&  stristr(RFound.cFileName, ext)) ||
	        (!n && !stristr(RFound.cFileName, ext)))
	      copy.push_back(pair<string, string>(
		in + RFound.cFileName,
		ou + RFound.cFileName
	      ));
	  } while (FindNextFile(RFiles, &RFound));

	  FindClose(RFiles);
	}

	if (copy.size() != 0)
	  break;

	tcnv <<= 1;
      }

      if (copy.size() != 0) {
	prog->InitProgress((int)copy.size());
	dne = 0; walk = copy.begin();
	while (walk != copy.end()) {
	  LPDIRECT3DTEXTURE9 base = NULL;
	  LPDIRECT3DTEXTURE9 trns = NULL;
	  HRESULT ret;

	  while ((ret = D3DXCreateTextureFromFileEx(
	    pD3DDevice, walk->first.data(),
	    D3DX_DEFAULT, D3DX_DEFAULT, 0/*D3DX_DEFAULT*/,
	    0, D3DFMT_UNKNOWN, D3DPOOL_SYSTEMMEM, D3DX_FILTER_NONE/*D3DX_DEFAULT*/,
	    D3DX_FILTER_NONE/*D3DX_DEFAULT*/, 0, NULL, NULL,
	    &base
	  )) != D3D_OK) {
	    char buf[256]; sprintf(buf, "Failed to read file \"%s\". Retry?", strrchr(walk->first.data(), '\\') + 1);
	    wxMessageDialog d(this, buf, "Oscape error", wxYES_NO | wxCANCEL | wxCENTRE);
	    int ret = d.ShowModal();
	    if (ret == wxID_CANCEL)
	      return false;
	    if (ret == wxID_NO)
	      break;
	  }

	  if (!base) {
	    prog->SetProgress(dne += 1);
	    walk++;

	    continue;
	  }

	  /* copy out mip-levels, no recalculation */
	  if (stristr(walk->first.data(), ".dds")) {
	    D3DSURFACE_DESC based;
	    base->GetLevelDesc(0, &based);

	    /* any resolution smaller than the target can simply be passed */
	    if (based.Width > (UINT)w) {
	      /* for higher resolution we have to copy out the trimmed mip-chain */
	      if ((ret = D3DXCreateTexture(
		pD3DDevice,
		w, h, 0,
		0, based.Format, D3DPOOL_SYSTEMMEM, &trns
	      )) != D3D_OK)
		goto terminal_error;

	      int baselvl = 0;
	      int trnslvl = 0;
	      while (based.Width > (UINT)w) {
		based.Width = (based.Width + 1) >> 1;
		baselvl++;
	      }

	      while (trnslvl < levels) {
		LPDIRECT3DSURFACE9 stex, srep;

		if (base->GetSurfaceLevel(baselvl, &stex) != D3D_OK)
		  goto terminal_error;
		if (trns->GetSurfaceLevel(trnslvl, &srep) != D3D_OK)
		  goto terminal_error;

		if (D3DXLoadSurfaceFromSurface(srep, NULL, NULL, stex, NULL, NULL, D3DX_FILTER_NONE, 0) != D3D_OK)
		  goto terminal_error;

		stex->Release();
		srep->Release();

		baselvl++;
		trnslvl++;
	      }

	      base->Release();
	      base = trns;
	    }
	  }
	  /* do the recalculation */
	  else if (stristr(walk->first.data(), ".ppm") ||
		   stristr(walk->first.data(), ".png")) {
	    D3DSURFACE_DESC based;
	    base->GetLevelDesc(0, &based);

	    if ((ret = D3DXCreateTexture(
		pD3DDevice,
		w, h, 0,
		0, based.Format, D3DPOOL_SYSTEMMEM, &trns
	      )) != D3D_OK)
	      goto terminal_error;

	    LPDIRECT3DSURFACE9 stex, srep;

	    if (base->GetSurfaceLevel(0, &stex) != D3D_OK)
	      goto terminal_error;
	    if (trns->GetSurfaceLevel(0, &srep) != D3D_OK)
	      goto terminal_error;

	    if (D3DXLoadSurfaceFromSurface(srep, NULL, NULL, stex, NULL, NULL, D3DX_FILTER_TRIANGLE, 0) != D3D_OK)
	      goto terminal_error;

	    stex->Release();
	    srep->Release();

	    base->Release();
	    base = trns;

	    if (n) {
	      if (OSGame->FindItem(wxID_OBLIVON)->IsChecked()) {
		if (!TextureCompressXYZ(&base, 0))
		  goto terminal_error;
	      }
	      else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked()) {
		if (!TextureCompressXZY(&base, 0))
		  goto terminal_error;
	      }
	    }
	    else {
	      if (!TextureCompressRGB(&base, 0, true))
		goto terminal_error;
	    }
	  }

	  char out[260];
	  strcpy(out, walk->second.data());
	  strcpy(strrchr(out, '.'), ".dds");

	  while ((ret = D3DXSaveTextureToFile(
	    out, D3DXIFF_DDS, base, NULL
	  )) != D3D_OK) {
	    wxMessageDialog d(this, "Failed to write file. Retry?", "Oscape error", wxYES_NO | wxCANCEL | wxCENTRE);
	    int ret = d.ShowModal();
	    if (ret == wxID_CANCEL) {
	      base->Release(); return false; }
	    if (ret == wxID_NO)
	      break;
	  }

	  base->Release();
	  prog->SetProgress(dne += 1);
	  walk++;

	  continue;

terminal_error: {
	    if (base) base->Release();
	    if (trns) trns->Release();

	    char buf[256]; sprintf(buf, "Failed to convert file \"%s\". Retry?", strrchr(walk->second.data(), '\\') + 1);
	    wxMessageDialog d(this, buf, "Oscape error", wxYES | wxCANCEL | wxCENTRE);
	    int ret = d.ShowModal();
	    if (ret == wxID_CANCEL)
	      return false;

	    prog->SetProgress(dne += 1);
	    walk++;
	  }
	}
      }
    }

    return true;
  }

  bool OscapeGUI::SynchronizeInstall(int wsv, int mres, int tres, int cres, const char *subdir, int rs, const char *lid) {
    installdone -= 2;

    if (mres && !SynchronizeInstall(wsv,        mres, subdir, rs, lid))
      return false;
    if (tres && !SynchronizeInstall(wsv, true , tres, subdir, rs, lid))
      return false;
    if (cres && !SynchronizeInstall(wsv, false, cres, subdir, rs, lid))
      return false;

    return true;
  }

  void OscapeGUI::HeightfieldInstall() {
    size_t pages = OSInstallWS->GetPageCount();
    installdone = 0;

    prog->StartProgress((int)(pages * 12) + 1);
    prog->InitProgress("Installing:", 0, "Preparing:", 0.0, installdone, 1);

    for (size_t wxID_Page = 0; wxID_Page < pages; wxID_Page++) {
      wxWindow *pg = OSInstallWS->GetPage(wxID_Page);
      string wsn = OSInstallWS->GetPageText(wxID_Page);
      char tpc[256]; sprintf(tpc, "Installing \"%s\":", wsn.data());
      int wsv;

      map<int, worldset::iterator >::iterator srch = wspacef.begin();
      while (srch != wspacef.end()) {
	if (*(srch->second) == wsn) {
	  wsv = srch->first;
	  break;
	}

	srch++;
      }

      if (wsv) {
	int wxID_Base = /*(wsv << 4) + 1*/ 16384;

	wxCheckBox *i0 = (wxCheckBox *)pg->FindWindowById(wID(LEVEL0_INST));
	wxCheckBox *i1 = (wxCheckBox *)pg->FindWindowById(wID(LEVEL1_INST));
	wxCheckBox *i2 = (wxCheckBox *)pg->FindWindowById(wID(LEVEL2_INST));
	wxCheckBox *i3 = (wxCheckBox *)pg->FindWindowById(wID(LEVEL3_INST));
	wxChoice   *m0 = (wxChoice   *)pg->FindWindowById(wID(LEVEL0_MRES));
	wxChoice   *m1 = (wxChoice   *)pg->FindWindowById(wID(LEVEL1_MRES));
	wxChoice   *m2 = (wxChoice   *)pg->FindWindowById(wID(LEVEL2_MRES));
	wxChoice   *m3 = (wxChoice   *)pg->FindWindowById(wID(LEVEL3_MRES));
	wxChoice   *t0 = (wxChoice   *)pg->FindWindowById(wID(LEVEL0N_TRES));
	wxChoice   *t1 = (wxChoice   *)pg->FindWindowById(wID(LEVEL1N_TRES));
	wxChoice   *t2 = (wxChoice   *)pg->FindWindowById(wID(LEVEL2N_TRES));
	wxChoice   *t3 = (wxChoice   *)pg->FindWindowById(wID(LEVEL3N_TRES));
	wxChoice   *c0 = (wxChoice   *)pg->FindWindowById(wID(LEVEL0C_TRES));
	wxChoice   *c1 = (wxChoice   *)pg->FindWindowById(wID(LEVEL1C_TRES));
	wxChoice   *c2 = (wxChoice   *)pg->FindWindowById(wID(LEVEL2C_TRES));
	wxChoice   *c3 = (wxChoice   *)pg->FindWindowById(wID(LEVEL3C_TRES));

	bool inst0 = (i0 ? i0->GetValue() : false);
	bool inst1 = (i1 ? i1->GetValue() : false);
	bool inst2 = (i2 ? i2->GetValue() : false);
	bool inst3 = (i3 ? i3->GetValue() : false);

	int mres0 = (m0 && m0->GetCount() ? (int)m0->GetClientData(m0->GetSelection()) : -1);
	int mres1 = (m1 && m1->GetCount() ? (int)m1->GetClientData(m1->GetSelection()) : -1);
	int mres2 = (m2 && m2->GetCount() ? (int)m2->GetClientData(m2->GetSelection()) : -1);
	int mres3 = (m3 && m3->GetCount() ? (int)m3->GetClientData(m3->GetSelection()) : -1);

	int tres0 = (t0 && t0->GetCount() ? (int)t0->GetClientData(t0->GetSelection()) : -1);
	int tres1 = (t1 && t1->GetCount() ? (int)t1->GetClientData(t1->GetSelection()) : -1);
	int tres2 = (t2 && t2->GetCount() ? (int)t2->GetClientData(t2->GetSelection()) : -1);
	int tres3 = (t3 && t3->GetCount() ? (int)t3->GetClientData(t3->GetSelection()) : -1);

	int cres0 = (c0 && c0->GetCount() ? (int)c0->GetClientData(c0->GetSelection()) : -1);
	int cres1 = (c1 && c1->GetCount() ? (int)c1->GetClientData(c1->GetSelection()) : -1);
	int cres2 = (c2 && c2->GetCount() ? (int)c2->GetClientData(c2->GetSelection()) : -1);
	int cres3 = (c3 && c3->GetCount() ? (int)c3->GetClientData(c3->GetSelection()) : -1);

	try {
	  int cap = 0;

	  /**/ if (OSGame->FindItem(wxID_OBLIVON)->IsChecked())
	    cap = 32;
	  else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked())
	    cap = 4;

	  prog->InitProgress(tpc, 0, "Level zero:", 0.0, installdone, 1); installdone += 3;
	  if (inst0)
	    if (!SynchronizeInstall(wsv, mres0, tres0, cres0, NULL     , max(cap,  4), "zero"))
	      break;

	  prog->InitProgress(tpc, 0, "Level one:", 0.0, installdone, 1); installdone += 3;
	  if (inst1)
	    if (!SynchronizeInstall(wsv, mres1, tres1, cres1, "farnear", max(cap,  8), "one"))
	      break;

	  prog->InitProgress(tpc, 0, "Level two:", 0.0, installdone, 1); installdone += 3;
	  if (inst2)
	    if (!SynchronizeInstall(wsv, mres2, tres2, cres2, "farfar" , max(cap, 16), "two"))
	      break;

	  prog->InitProgress(tpc, 0, "Level three:", 0.0, installdone, 1); installdone += 3;
	  if (inst3)
	    if (!SynchronizeInstall(wsv, mres3, tres3, cres3, "farinf" , max(cap, 32), "three"))
	      break;

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

	RegSetKeyValue(Settings, GetGameKey("Installer", wsn.data()), "Install0", RRF_RT_REG_SZ, inst0 ? "1" : "0", 2);
	RegSetKeyValue(Settings, GetGameKey("Installer", wsn.data()), "Install1", RRF_RT_REG_SZ, inst1 ? "1" : "0", 2);
	RegSetKeyValue(Settings, GetGameKey("Installer", wsn.data()), "Install2", RRF_RT_REG_SZ, inst2 ? "1" : "0", 2);
	RegSetKeyValue(Settings, GetGameKey("Installer", wsn.data()), "Install3", RRF_RT_REG_SZ, inst3 ? "1" : "0", 2);
      }
      else
	prog->InitProgress(tpc, 0, "Skipping:", 0.0, installdone += 8, 1);
    }

    prog->Leave(666);
  }
