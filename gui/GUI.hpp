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

#ifndef OSCAPE_GUI_HPP
#define OSCAPE_GUI_HPP

#include "../Oscape.h"

DWORD __stdcall HeightfieldExtract(LPVOID lp);
DWORD __stdcall HeightfieldGenerate(LPVOID lp);
DWORD __stdcall HeightfieldInstall(LPVOID lp);
DWORD __stdcall HeightfieldRecover(LPVOID lp);

// ----------------------------------------------------------------------------
struct pluginfile;
typedef	map<string, pluginfile> pluginmap;
typedef	set<string> worldset;

struct lodfile;
typedef	map<string, lodfile> lodmap;

struct pluginfile {
  time_t tme;
  bool act;

  static bool compare(
    const pluginmap::iterator d1,
    const pluginmap::iterator d2
  ) {
    return (d1->second.tme < d2->second.tme);
  }
};

struct lodfile {
  time_t tme;
  bool act;

  static bool compare(
    const lodmap::iterator d1,
    const lodmap::iterator d2
  ) {
    return (d1->second.tme < d2->second.tme);
  }
};

struct sset {
  set<unsigned long int>      seed;		// mesh seeds
  map<unsigned long int, int> mesh_nu_x;	// mesh resolutions + tile count
  map<unsigned long int, int> mesh_yu_x;	// mesh resolutions + tile count
  map<unsigned long int, int> mesh_nu_n;	// mesh resolutions + tile count
  map<unsigned long int, int> mesh_yu_n;	// mesh resolutions + tile count
  map<unsigned long int, int> textn_d;		// texture resolutions + tile count
  map<unsigned long int, int> textn_p;		// texture resolutions + tile count
  map<unsigned long int, int> textc_d;		// texture resolutions + tile count
  map<unsigned long int, int> textc_p;		// texture resolutions + tile count
};

// ----------------------------------------------------------------------------
class OscapeGUI; extern class OscapeGUI *gui;
class OscapeGUI : public wxOscape
{
public:
  int actives;

  pluginmap plugins;
  vector< pluginmap::iterator > psorted;
  worldset wspaces;
  map<int, worldset::iterator > wspacef;

  int activel;

  lodmap lodfils;
  vector< lodmap::iterator > lsorted;

private:
  int GetWorldspace(wxString ph, int wsel = 0);
  int GetWorldspacePrefix(wxString ph, int wsel = 0);

private:
  // Installed Path
  char IPath[1024];
  char LPath[1024];
  HKEY Settings;
  OscapePrg *prog;

  /* ---------------------------------------------------------------------------- */
  const char *GetGameKey(const char *subsub = NULL);
  const char *GetGameKey(const char *prefix, const char *subsub);

  /* ---------------------------------------------------------------------------- */
  void ResetHButtons();
  void ResetLButtons();

  /* ---------------------------------------------------------------------------- */
  void LoadWSpaceList();
  void SaveWSpaceList();

  /* ---------------------------------------------------------------------------- */
  void ResetHPluginDir(bool init = false);
  void ResetHPluginList();
  void ClearHPluginList();
  void LoadHPluginList();
  void SaveHPluginList();

  /* ---------------------------------------------------------------------------- */
  void ResetLPluginDir(bool init = false);
  void ResetLPluginList();

  /* ---------------------------------------------------------------------------- */
  void ChangeToOblivion(wxCommandEvent& event);
  void ChangeToSkyrim(wxCommandEvent& event);

  /* ---------------------------------------------------------------------------- */
  void ResetPluginList(wxCommandEvent& event);
  void ClearPluginList(wxCommandEvent& event);
  void LoadPluginList(wxCommandEvent& event);
  void SavePluginList(wxCommandEvent& event);
  void ChangeAutosave(wxCommandEvent& event);

  /* **************************************************************************** */
  void WorldspacesFromPlugins(wxCommandEvent& event);
  void ChangePluginDir(wxFileDirPickerEvent& event);
  void ChangeActivePlugins(wxCommandEvent& event);
  void ChangeWorldspace(wxCommandEvent& event);
  void ChangeHeightfieldOut(wxFileDirPickerEvent& event);

  /* ---------------------------------------------------------------------------- */
  void HeightfieldExtract(wxCommandEvent& event);
public:
  void HeightfieldExtract();
private:

  /* **************************************************************************** */
  void ChangeToHeightfield(wxCommandEvent& event);
  void ChangeToNormalmap(wxCommandEvent& event);
  void ChangeToColormap(wxCommandEvent& event);

  void ChangeLODDir(wxFileDirPickerEvent& event);
  void ChangeLODWorldspace(wxCommandEvent& event);
  void ChangeLODWorldspace();
  void ChangeLODRes(wxCommandEvent& event);
  void ChangeLODRes();
  void ChangeRecoveryOut(wxFileDirPickerEvent& event);

  void DefineDimensions(int tsze, int csze, int tmxx, int tmxy);

  /* ---------------------------------------------------------------------------- */
  void HeightfieldRecover(wxCommandEvent& event);
public:
  void HeightfieldRecover();
private:

  /* **************************************************************************** */
  wxImage hgt;
  wxMemoryDC hdc;

  void PaintH(wxPaintEvent& event);
  void RedrawH();

  wxPGProperty *formID;
  wxPGProperty *tSize;
  wxPGProperty *cSize;
  wxPGProperty *rWidth;
  wxPGProperty *rHeight;
  wxPGProperty *tLeft;
  wxPGProperty *tRight;
  wxPGProperty *tTop;
  wxPGProperty *tBottom;
  wxPGProperty *fSealevel;

  void ChangePreview(wxCommandEvent& event);
  void ChangeOrientation(wxCommandEvent& event);
  void ChangeNonSquare(wxCommandEvent& event);
  void ChangeHeightfieldInfos(wxPropertyGridEvent& event);

  /* ---------------------------------------------------------------------------- */
  void HeightfieldAccept(wxCommandEvent& event);

  /* ---------------------------------------------------------------------------- */
  void ChangeHeightfieldIn1(wxFileDirPickerEvent& event);
  void ChangeHeightfieldIn1(wxString ph);

  void ChangePointsIn1(wxFileDirPickerEvent& event);
  void ChangePointsIn1(wxString ph);

  void ClearPoints1(wxCommandEvent& event);

  void ChangeBaseDirOut1(wxFileDirPickerEvent& event);
  void ChangeBaseDirOut1(wxString ph);

  /* ---------------------------------------------------------------------------- */
  void ChangeHeightfieldIn2(wxFileDirPickerEvent& event);
  void ChangeHeightfieldIn2(wxString ph);

  void ChangePointsIn2(wxFileDirPickerEvent& event);
  void ChangePointsIn2(wxString ph);

  void ClearPoints2(wxCommandEvent& event);

  void ChangeBaseDirOut2(wxFileDirPickerEvent& event);
  void ChangeBaseDirOut2(wxString ph);

  /* **************************************************************************** */
  bool verified;

  wxString GetTarget(wxString val);
  wxString GetTarget();

  int MaxSuperSampling(int width, int height);
  void MaxTarget(int &_maxp, int &_maxx, int &_ntls);
  void VerifyTarget(int target);

  void VerifyHeightfieldIn();
  void VerifyHeightfieldOut();

  bool SanitizeGeneration();
  void DefineDimensions(int tsze, int csze);

  void CheckFloat(wxCommandEvent& event);
  void CheckInt(wxCommandEvent& event);

  void ChangeAlgorithm(wxCommandEvent& event);
  void ChangeTarget(wxCommandEvent& event);

  /* ---------------------------------------------------------------------------- */
  void ChangeMeshes(wxCommandEvent& event);
  void ChangeMeshes();

  void ChangeNormals(wxCommandEvent& event);
  void ChangeNormals();

  void ChangeColors(wxCommandEvent& event);
  void ChangeColors();

  void ChangeHeightmap(wxCommandEvent& event);
  void ChangeHeightmap();

  void HeightfieldGenerate(wxCommandEvent& event);
public:
  void HeightfieldGenerate();
private:

  /* **************************************************************************** */
  int installdone;

  bool SynchronizeInstall(int wsv, int res, const char *subdir, int rs, const char *lid);
  bool SynchronizeInstall(int wsv, bool n, int res, const char *subdir, int rs, const char *lid);
  bool SynchronizeInstall(int wsv, int mres, int tres, int cres, const char *subdir, int rs, const char *lid);

  /* ---------------------------------------------------------------------------- */
  void CreateWorldspacePage(int wsv, struct sset *config);

  void ChangeLevel0MeshRes(wxCommandEvent& event);
  void ChangeLevel1MeshRes(wxCommandEvent& event);
  void ChangeLevel2MeshRes(wxCommandEvent& event);
  void ChangeLevel3MeshRes(wxCommandEvent& event);

  void ChangeBaseDirIn(wxFileDirPickerEvent& event);
  void ChangeBaseDirIn(wxString ph);

  void ChangePlugoutDir(wxFileDirPickerEvent& event);
  void ChangePlugoutDir(wxString ph);

  void HeightfieldInstall(wxCommandEvent& event);
public:
  void HeightfieldInstall();
private:

  /* ---------------------------------------------------------------------------- */
  void ReadMemory();

  void ChangeDefaults(wxCommandEvent& event);
  void ChangeTSampling(wxCommandEvent& event);
  void ChangeSSampling(wxCommandEvent& event);

public:
  OscapeGUI::OscapeGUI(const wxString& title);
  OscapeGUI::~OscapeGUI();

  void SetStatus(const char *status);
};

#endif
