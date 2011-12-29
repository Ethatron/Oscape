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

class OscapeGUI *gui;

// ----------------------------------------------------------------------------
  int OscapeGUI::GetWorldspace(wxString ph, int wsel) {
    ph = ph.MakeLower();
    map<int, worldset::iterator >::iterator srch = wspacef.begin();
    while (srch != wspacef.end()) {
      wxString eq = *(srch->second);
      eq = eq.MakeLower();
      if (ph.IsSameAs(eq)) {
	wsel = srch->first;
	break;
      }

      srch++;
    }

    return wsel;
  }

  int OscapeGUI::GetWorldspacePrefix(wxString ph, int wsel) {
    ph = ph.MakeLower();
    map<int, worldset::iterator >::iterator srch = wspacef.begin();
    while (srch != wspacef.end()) {
      wxString mtch = *(srch->second);
      mtch = mtch.MakeLower();
      if (ph.First(mtch) != wxNOT_FOUND) {
	wsel = srch->first;
	break;
      }

      srch++;
    }

    return wsel;
  }

  /* ---------------------------------------------------------------------------- */
  const char *OscapeGUI::GetGameKey(const char *subsub) {
    const char *subkey = NULL;

    if (OSGame->FindItem(wxID_OBLIVON)->IsChecked())
      subkey = "Oblivion";
    else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked())
      subkey = "Skyrim";

    if (subsub && subsub[0]) {
      static char submrg[256];

      strcpy(submrg, subkey);
      strcat(submrg, "\\");
      strcat(submrg, subsub);

      return submrg;
    }

    return subkey;
  }

  const char *OscapeGUI::GetGameKey(const char *prefix, const char *subsub) {
    static char submrg[256];

    strcpy(submrg, prefix);
    strcat(submrg, "\\");
    strcat(submrg, subsub);

    return GetGameKey(submrg);
  }

  /* ---------------------------------------------------------------------------- */
  void OscapeGUI::ChangeToOblivion(wxCommandEvent& event) {
    ResetHPluginDir(true);
    ResetHPluginList();
    LoadHPluginList();

    ResetLPluginDir(true);
    ResetLPluginList();

    ReadMemory();

    ChangeMeshes();
    ChangeNormals();
    ChangeColors();
    ChangeHeightmap();
    ChangeLODRes();

    ChangeHeightfieldIn1(OSFileHeightfieldIn1->GetPath());
    ChangeHeightfieldIn2(OSFileHeightfieldIn2->GetPath());

    RegSetKeyValue(Settings, NULL, "Game", RRF_RT_REG_SZ, "0", 2);
  }

  void OscapeGUI::ChangeToSkyrim(wxCommandEvent& event) {
    ResetHPluginDir(true);
    ResetHPluginList();
    LoadHPluginList();

    ResetLPluginDir(true);
    ResetLPluginList();

    ReadMemory();

    ChangeMeshes();
    ChangeNormals();
    ChangeColors();
    ChangeHeightmap();
    ChangeLODRes();

    ChangeHeightfieldIn1(OSFileHeightfieldIn1->GetPath());
    ChangeHeightfieldIn2(OSFileHeightfieldIn2->GetPath());

    RegSetKeyValue(Settings, NULL, "Game", RRF_RT_REG_SZ, "1", 2);
  }

  /* ---------------------------------------------------------------------------- */
  void OscapeGUI::ResetHButtons() {
    /* enable button */
    OSWorldspaceFill->Enable((actives != 0));
    OSPluginExtract->Enable((actives != 0) && (OSFileHeightfieldOut->GetPath() != "") && (OSWorldspace->GetValue() != ""));
    OSHeightfieldInstall->Enable((OSBaseDirIn->GetPath() != "") && (OSPlugoutDir->GetPath() != ""));
    OSPointsClear1->Enable(OSFilePoints1->GetPath() != "");
    OSPointsClear1->Enable(OSFilePoints1->GetPath() != "");
  }

  void OscapeGUI::ResetLButtons() {
    /* enable button */
    OSLODRecover->Enable((activel != 0) && (OSFileRecoveryOut->GetPath() != ""));
  }

  /* ---------------------------------------------------------------------------- */
  void OscapeGUI::SetStatus(const char *status) {
    OSStatusBar->SetStatusText(status);
  }

  void OscapeGUI::ReadMemory() {
    char TS[1024]; DWORD TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, GetGameKey(), "LOD Directory", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSLODDir->SetPath(TS); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, GetGameKey(), "LOD Type", RRF_RT_REG_SZ, NULL, TS, &TSL);
    switch (TS[0]) {
      default:
      case '0': OSRevHeight->SetValue(true); break;
      case '1': OSRevNormals->SetValue(true); break;
      case '2': OSRevColors->SetValue(true); break;
    } TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, GetGameKey(), "Last LOD Resolution", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSLODResolution->SetSelection(TS[0] - '1'); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, GetGameKey(), "Recovery Out", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSFileRecoveryOut->SetPath(TS); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, GetGameKey(), "Last worldspace", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSWorldspace->SetValue(TS); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, GetGameKey(), "Heightfield Out", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSFileHeightfieldOut->SetPath(TS); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, GetGameKey(), "Heightfield In", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSFileHeightfieldIn1->SetPath(TS); TSL = 1023;
    if (TS[0]) OSFileHeightfieldIn2->SetPath(TS); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, GetGameKey(), "Points In", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSFilePoints1->SetPath(TS); TSL = 1023;
    if (TS[0]) OSFilePoints2->SetPath(TS); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, GetGameKey(), "Base directory Out", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSBaseDirOut1->SetPath(TS); TSL = 1023;
    if (TS[0]) OSBaseDirOut2->SetPath(TS); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, GetGameKey(), "Base directory In", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSBaseDirIn->SetPath(TS); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, GetGameKey(), "Autosave", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSPluginAutosave->SetValue(TS[0] == '1'); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, GetGameKey(), "Do Meshes", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSMeshes->SetValue(TS[0] == '1'); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, GetGameKey(), "Do Normals", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSNormals->SetValue(TS[0] == '1'); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, GetGameKey(), "Do Colors", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSColors->SetValue(TS[0] == '1'); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, GetGameKey(), "Do Heightmap", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSHeightmap->SetValue(TS[0] == '1'); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, GetGameKey(), "Defaults", RRF_RT_REG_SZ, NULL, TS, &TSL);
    switch (TS[0]) {
      default:
	OSMeshUVs->SetValue(true);
	OSMeshNIF->SetValue(true);
	OSMeshDX->SetValue(false);
	OSMlod1->SetValue(true ); OSMlod2->SetValue(true ); OSMlod3->SetValue(true ); OSMlod4->SetValue(true );
	OSMlod5->SetValue(false); OSMlod6->SetValue(false); OSMlod7->SetValue(false); OSMlod8->SetValue(false);
	OSNlod1->SetValue(true ); OSNlod2->SetValue(true ); OSNlod3->SetValue(true ); OSNlod4->SetValue(true );
	OSClod1->SetValue(true ); OSClod2->SetValue(true ); OSClod3->SetValue(true ); OSClod4->SetValue(true );
	if (OSGame->FindItem(wxID_SKYRIM)->IsChecked())
	  break;

      case '0':
	OSDefaults->FindItem(wxID_DEFV, NULL)->Check(true);
	OSMeshUVs->SetValue(true);
	OSMeshNIF->SetValue(true);
	OSMeshDX->SetValue(false);
	OSMlod1->SetValue(true ); OSMlod2->SetValue(false); OSMlod3->SetValue(false); OSMlod4->SetValue(false);
	OSMlod5->SetValue(false); OSMlod6->SetValue(false); OSMlod7->SetValue(false); OSMlod8->SetValue(false);
	OSNlod1->SetValue(true ); OSNlod2->SetValue(false); OSNlod3->SetValue(false); OSNlod4->SetValue(false);
	OSClod1->SetValue(true ); OSClod2->SetValue(false); OSClod3->SetValue(false); OSClod4->SetValue(false);
	break;
      case '1':
	OSDefaults->FindItem(wxID_DEFT, NULL)->Check(true);
	OSMeshUVs->SetValue(true);
	OSMeshNIF->SetValue(true);
	OSMeshDX->SetValue(false);
	OSMlod1->SetValue(true ); OSMlod2->SetValue(false); OSMlod3->SetValue(false); OSMlod4->SetValue(false);
	OSMlod5->SetValue(false); OSMlod6->SetValue(false); OSMlod7->SetValue(false); OSMlod8->SetValue(false);
	OSNlod1->SetValue(true ); OSNlod2->SetValue(false); OSNlod3->SetValue(false); OSNlod4->SetValue(false);
	OSClod1->SetValue(true ); OSClod2->SetValue(false); OSClod3->SetValue(false); OSClod4->SetValue(false);
	break;
      case '2':
	OSDefaults->FindItem(wxID_DEFL, NULL)->Check(true);
	OSMeshUVs->SetValue(false);
	OSMeshNIF->SetValue(true);
	OSMeshDX->SetValue(true);
	OSMlod1->SetValue(true ); OSMlod2->SetValue(true ); OSMlod3->SetValue(true ); OSMlod4->SetValue(true );
	OSMlod5->SetValue(true ); OSMlod6->SetValue(true ); OSMlod7->SetValue(true ); OSMlod8->SetValue(true );
	OSNlod1->SetValue(true ); OSNlod2->SetValue(false); OSNlod3->SetValue(false); OSNlod4->SetValue(false);
	OSClod1->SetValue(true ); OSClod2->SetValue(false); OSClod3->SetValue(false); OSClod4->SetValue(false);
	break;
    } TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, GetGameKey(), "Texture Sampling", RRF_RT_REG_SZ, NULL, TS, &TSL);
    switch (TS[0]) {
      default:
      case '1': OSTSampling->FindItem(wxID_TS1, NULL)->Check(true); break;
      case '2': OSTSampling->FindItem(wxID_TS2, NULL)->Check(true); break;
      case '4': OSTSampling->FindItem(wxID_TS4, NULL)->Check(true); break;
    } TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, GetGameKey(), "Surface Sampling", RRF_RT_REG_SZ, NULL, TS, &TSL);
    switch (TS[0]) {
      default:
      case '1': OSSSampling->FindItem(wxID_SS1, NULL)->Check(true); break;
      case '2': OSSSampling->FindItem(wxID_SS2, NULL)->Check(true); break;
      case '4': OSSSampling->FindItem(wxID_SS4, NULL)->Check(true); break;
    } TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, GetGameKey(), "Orientation", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSOrientation->SetValue(TS[0] == 'H'); TSL = 1023;
    if (!TS[0]) OSOrientation->SetValue(!OSDefaults->FindItem(wxID_DEFV, NULL)->IsChecked());
    TS[0] = 0; RegGetValue(Settings, GetGameKey(), "Target", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSTarget->SetValue(TS); TSL = 1023;
    if (!TS[0]) OSTarget->SetValue(OSDefaults->FindItem(wxID_DEFV, NULL)->IsChecked() ? "155000" : "450000");
  }

  void OscapeGUI::ChangeDefaults(wxCommandEvent& event) {
    if (OSGame->FindItem(wxID_SKYRIM)->IsChecked())
      return;

    /**/ if (OSDefaults->FindItem(wxID_DEFV, NULL)->IsChecked()) {
      OSMeshUVs->SetValue(true);
      OSMeshNIF->SetValue(true);
      OSMeshDX->SetValue(false);
      OSMlod1->SetValue(true ); OSMlod2->SetValue(false); OSMlod3->SetValue(false); OSMlod4->SetValue(false);
      OSMlod5->SetValue(false); OSMlod6->SetValue(false); OSMlod7->SetValue(false); OSMlod8->SetValue(false);
      OSNlod1->SetValue(true ); OSNlod2->SetValue(false); OSNlod3->SetValue(false); OSNlod4->SetValue(false);
      OSClod1->SetValue(true ); OSClod2->SetValue(false); OSClod3->SetValue(false); OSClod4->SetValue(false);

      RegSetKeyValue(Settings, GetGameKey(), "Defaults", RRF_RT_REG_SZ, "0", 2);
    }
    else if (OSDefaults->FindItem(wxID_DEFT, NULL)->IsChecked()) {
      OSMeshUVs->SetValue(true);
      OSMeshNIF->SetValue(true);
      OSMeshDX->SetValue(false);
      OSMlod1->SetValue(true ); OSMlod2->SetValue(false); OSMlod3->SetValue(false); OSMlod4->SetValue(false);
      OSMlod5->SetValue(false); OSMlod6->SetValue(false); OSMlod7->SetValue(false); OSMlod8->SetValue(false);
      OSNlod1->SetValue(true ); OSNlod2->SetValue(false); OSNlod3->SetValue(false); OSNlod4->SetValue(false);
      OSClod1->SetValue(true ); OSClod2->SetValue(false); OSClod3->SetValue(false); OSClod4->SetValue(false);

      RegSetKeyValue(Settings, GetGameKey(), "Defaults", RRF_RT_REG_SZ, "1", 2);
    }
    else if (OSDefaults->FindItem(wxID_DEFL, NULL)->IsChecked()) {
      OSMeshUVs->SetValue(false);
      OSMeshNIF->SetValue(true);
      OSMeshDX->SetValue(true);
      OSMlod1->SetValue(true ); OSMlod2->SetValue(true ); OSMlod3->SetValue(true ); OSMlod4->SetValue(true );
      OSMlod5->SetValue(true ); OSMlod6->SetValue(true ); OSMlod7->SetValue(true ); OSMlod8->SetValue(true );
      OSNlod1->SetValue(true ); OSNlod2->SetValue(false); OSNlod3->SetValue(false); OSNlod4->SetValue(false);
      OSClod1->SetValue(true ); OSClod2->SetValue(false); OSClod3->SetValue(false); OSClod4->SetValue(false);

      RegSetKeyValue(Settings, GetGameKey(), "Defaults", RRF_RT_REG_SZ, "2", 2);
    }
  }

  void OscapeGUI::ChangeTSampling(wxCommandEvent& event) {
    /**/ if (OSTSampling->FindItem(wxID_TS1, NULL)->IsChecked())
      RegSetKeyValue(Settings, GetGameKey(), "Texture Sampling", RRF_RT_REG_SZ, "1", 2);
    else if (OSTSampling->FindItem(wxID_TS2, NULL)->IsChecked())
      RegSetKeyValue(Settings, GetGameKey(), "Texture Sampling", RRF_RT_REG_SZ, "2", 2);
    else if (OSTSampling->FindItem(wxID_TS4, NULL)->IsChecked())
      RegSetKeyValue(Settings, GetGameKey(), "Texture Sampling", RRF_RT_REG_SZ, "4", 2);
  }

  void OscapeGUI::ChangeSSampling(wxCommandEvent& event) {
    /**/ if (OSSSampling->FindItem(wxID_SS1, NULL)->IsChecked())
      RegSetKeyValue(Settings, GetGameKey(), "Surface Sampling", RRF_RT_REG_SZ, "1", 2);
    else if (OSSSampling->FindItem(wxID_SS2, NULL)->IsChecked())
      RegSetKeyValue(Settings, GetGameKey(), "Surface Sampling", RRF_RT_REG_SZ, "2", 2);
    else if (OSSSampling->FindItem(wxID_SS4, NULL)->IsChecked())
      RegSetKeyValue(Settings, GetGameKey(), "Surface Sampling", RRF_RT_REG_SZ, "4", 2);
  }

  OscapeGUI::OscapeGUI(const wxString& title)
    : wxOscape(NULL, wxID_ANY, title) {
    gui = this;

    OSHeightfieldInfos->Connect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler( OscapeGUI::ChangeHeightfieldInfos ), NULL, this );

    Settings = 0;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Bethesda Softworks\\Oscape", 0, KEY_READ | KEY_WRITE | KEY_WOW64_32KEY, &Settings) == ERROR_SUCCESS) {
    }
    else if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, "Software\\Bethesda Softworks\\Oscape", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE | KEY_WOW64_32KEY, NULL, &Settings, NULL) == ERROR_SUCCESS) {
    }

    char TS[1024]; DWORD TSL = 1023; TS[0] = 0;
    RegGetValue(Settings, NULL, "Game", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if ((TS[0] == '0') || (TS[0] == 0))
      OSGame->FindItem(wxID_OBLIVON, NULL)->Check(true);
    else if (TS[0] == '1')
      OSGame->FindItem(wxID_SKYRIM, NULL)->Check(true);

    ResetHPluginDir(true);
    ResetHPluginList();
    LoadHPluginList();
//  WorldspacesFromPlugins();
//  ResetHButtons();

    ResetLPluginDir(true);
    ResetLPluginList();

    OSFilePoints1->SetPath("");
    OSFilePoints2->SetPath("");

    ReadMemory();

    ChangeMeshes();
    ChangeNormals();
    ChangeColors();
    ChangeHeightmap();
    ChangeLODRes();

    wxString
    ph = OSFileHeightfieldIn1->GetPath();
    if (!ph.IsNull()) {
      ChangeHeightfieldIn1(ph);
      ChangeHeightfieldIn2(ph);
    }

    ph = OSFilePoints1->GetPath();
    if (!ph.IsNull()) {
      ChangePointsIn1(ph);
      ChangePointsIn2(ph);
    }

    ph = OSBaseDirOut1->GetPath();
    if (!ph.IsNull()) {
      ChangeBaseDirOut1(ph);
      ChangeBaseDirOut2(ph);
    }

    ph = OSBaseDirIn->GetPath();
    ChangeBaseDirIn(ph);
  }

  OscapeGUI::~OscapeGUI() {
    if (Settings)
      RegCloseKey(Settings);
  }
