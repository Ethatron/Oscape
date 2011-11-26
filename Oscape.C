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

#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <algorithm>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <vector>

using namespace std;

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <knownfolders.h> // for KnownFolder APIs/datatypes/function headers

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

LPSTR UnicodeToAnsi(LPCWSTR s)
{
  if (s==NULL) return NULL;
  int cw=lstrlenW(s);
  if (cw==0) {CHAR *psz=new CHAR[1];*psz='\0';return psz;}
  int cc=WideCharToMultiByte(CP_ACP,0,s,cw,NULL,0,NULL,NULL);
  if (cc==0) return NULL;
  CHAR *psz=new CHAR[cc+1];
  cc=WideCharToMultiByte(CP_ACP,0,s,cw,psz,cc,NULL,NULL);
  if (cc==0) {delete[] psz;return NULL;}
  psz[cc]='\0';
  return psz;
}

LPCWSTR AnsiToUnicode(LPSTR s)
{
  if (s==NULL) return NULL;
  int cw=lstrlen(s);
  if (cw==0) {WCHAR *psz=new WCHAR[1];*psz='\0';return psz;}
  int cc=MultiByteToWideChar(CP_ACP,0,s,cw,NULL,0);
  if (cc==0) return NULL;
  WCHAR *psz=new WCHAR[cc+1];
  cc=MultiByteToWideChar(CP_ACP,0,s,cw,psz,cc);
  if (cc==0) {delete[] psz;return NULL;}
  psz[cc]='\0';
  return psz;
}

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
//#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <msvc/wx/setup.h>

#include <wx/propgrid/propgrid.h>
#include <wx/filename.h>

#pragma comment(lib,"Comctl32")
#pragma comment(lib,"Rpcrt4")

#include "Oscape_Window.h"

// ----------------------------------------------------------------------------
#include "Oscape_CBash.hpp"

#include "scape.H"

DWORD __stdcall HeightfieldExtract(LPVOID lp);
DWORD __stdcall HeightfieldGenerate(LPVOID lp);
DWORD __stdcall HeightfieldInstall(LPVOID lp);

// ----------------------------------------------------------------------------
struct pluginfile;
typedef	map<string, pluginfile> pluginmap;
typedef	set<string> worldset;

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

class OscapePrg; class OscapePrg *prg;
class OscapePrg : public wxProgress {
private:
  const char *lastpa;
  const char *lastpb;

public:
  void StartProgress(int rng) {
    Wait();

    OSTask1->SetRange(rng);

//  Update();
  }

  void InitProgress(const char *patterna, size_t val, const char *patternb, double err, int dne, int rng) {
    Wait();

    char tmp[256];

    if (patterna) { sprintf(tmp, lastpa = patterna, val); OSSubject1->SetLabel(tmp); }
    if (patternb) { sprintf(tmp, lastpb = patternb, err); OSSubject2->SetLabel(tmp); }

    OSTask1->SetValue(dne);
    OSTask2->SetRange(rng);
    OSTask2->SetValue(0);

//  Update();
  }

  void InitProgress(int rng, Real err) {
    Wait();

    char tmp[256];

    sprintf(tmp, lastpb, err); OSSubject2->SetLabel(tmp);

    OSTask2->SetRange((range1 = rng) * (range2 = 1));
    OSTask2->SetValue(0);

//  Update();
  }

  void SetProgress(int dne, Real err) {
    Wait();

    char tmp[256];

    sprintf(tmp, lastpb, err); OSSubject2->SetLabel(tmp);

    OSTask2->SetValue(dne);
  }

  int range1, value1;
  int range2, value2;

  void InitProgress(int rng) {
    Wait();

    OSTask2->SetRange((range1 = rng) * (range2 = 1));
    OSTask2->SetValue(0);

//  Update();
  }

  void SetProgress(int dne) {
    Wait();

    OSTask2->SetValue((value1 = dne) * (range2) + (value2 = 0));
  }

  void SetTopic(const char *topic) {
    OSSubject2->SetLabel(topic);
  }

  void InitProgress(int rng, int srng) {
    Wait();

    OSTask2->SetRange((range1 = rng) * (range2 = srng));
    OSTask2->SetValue(0);

//  Update();
  }

  void SetProgress(int dne, int sdne) {
    Wait();

    OSTask2->SetValue((value1      ) * (range2) + (value2 = sdne));
  }

private:
  bool paused, quit;
  time_t tinit, tlast, tnow;

public:
  bool RequestFeedback(const char *question) {
    wxMessageDialog d(this, question, "Oscape", wxOK | wxCANCEL | wxCENTRE);
    int ret = d.ShowModal();
    if (ret == wxID_CANCEL)
      return false;
    return true;
  }

  virtual void PauseProgress(wxCommandEvent& event) {
    if (!paused) {
      Block();

      paused = true;
      OSPause->SetLabel("Unpause");
    }
    else {
      Unblock();

      paused = false;
      OSPause->SetLabel("Pause");
    }
  }

  void PollProgress() {
    Wait();
  }

  virtual void AbortProgress(wxCloseEvent& event) {
    if (event.CanVeto())
      event.Veto();

    Abort();
  }

  virtual void AbortProgress(wxCommandEvent& event) {
    Abort();
  }

  virtual void IdleProgress(wxIdleEvent& event) {
    tnow = time(NULL);

    long tpass = (long)(tnow - tinit);
    if (tpass != tlast) {
      tlast = tpass;

      float f1 = (float)(OSTask1->GetValue()) / (OSTask1->GetRange());
      float f2 = (float)(OSTask2->GetValue()) / (OSTask1->GetRange() * OSTask2->GetRange());

      int tfinal = (int)floor((float)tpass / (f1 + f2));
      int trem = tfinal - tlast;

      char buf[256];

      sprintf(buf, "%02d:%02ds", tpass / 60, tpass % 60);
      OSRunning->SetLabel(buf);

      if (tfinal != 0x80000000) {
	sprintf(buf, "%02d:%02ds", trem / 60, trem % 60);
	OSRemaining->SetLabel(buf);
	sprintf(buf, "%02d:%02ds", tfinal / 60, tfinal % 60);
	OSTotal->SetLabel(buf);
      }
      else {
	OSRemaining->SetLabel("00:00s");
	OSTotal->SetLabel(buf);
      }
    }

//  Sleep(500);
//  event.RequestMore();
  }

private:
  HANDLE evt, end;
  HANDLE async;

  /* called from Progress-thread */
  void Abort() {
    quit = true;
    SetEvent(evt);
  }

  /* called from Async-thread */
  void Wait() {
    WaitForSingleObject(evt, INFINITE);

    /* signal abortion (inside Async-thread) */
    if (quit)
      throw runtime_error("ExitThread");
  }

  void Block() {
    ResetEvent(evt);
  }

  void Unblock() {
    SetEvent(evt);
  }

public:
  wxEventType evtLeave;
  int idLeave;
  int retCode;

  class LeaveEvent: public wxCommandEvent {
  public:
    LeaveEvent(int id, const wxEventType& event_type) : wxCommandEvent(event_type, id) {}
    LeaveEvent(const LeaveEvent& event) : wxCommandEvent(event) {}

    wxEvent* Clone() const { return new LeaveEvent(*this); }
  };

  typedef void (wxEvtHandler::*LeaveEventFunction)(LeaveEvent &);

  /* called from outside-thread */
  int Enter(LPTHREAD_START_ROUTINE routine) {
    if ((async = CreateThread(NULL, 0, routine, this, 0, NULL)) == INVALID_HANDLE_VALUE)
      return 0;
    SetThreadPriority(async, THREAD_PRIORITY_BELOW_NORMAL);

    return ShowModal();
  }

  /* called from Async-thread */
  void Leave(int rc) {
    retCode = rc;

    LeaveEvent event(idLeave, evtLeave);
    wxPostEvent(this, event);

    /* wait for the progress-dialog to recognize our message */
    WaitForSingleObject(end, INFINITE);
  }

  /* called from Progress-thread */
  void Leave(LeaveEvent &) {
    /* signal that we recognize the message */
    SetEvent(end);

    WaitForSingleObject(async, INFINITE);
    CloseHandle(async);

    EndModal(retCode);
  }

  OscapePrg::OscapePrg(wxWindow *parent) : wxProgress(parent) {
    evtLeave = wxNewEventType();
    idLeave = wxNewId();

    /* Connect to event handler that will make us close */
    Connect(wxID_ANY, evtLeave,
      (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)wxStaticCastEvent(LeaveEventFunction, &OscapePrg::Leave),
      NULL,
      this);

    tinit = time(NULL);
    paused = false;
    quit = false;
    prg = this;

    evt = CreateEvent(
      NULL,		  // default security attributes
      TRUE,		  // manual reset
      TRUE,		  // initially set
      NULL		  // unnamed mutex
      );

    end = CreateEvent(
      NULL,		  // default security attributes
      TRUE,		  // manual reset
      FALSE,		  // initially not set
      NULL		  // unnamed mutex
      );

    SetSize(600, 265);
  }

  OscapePrg::~OscapePrg() {
    CloseHandle(evt);
    CloseHandle(end);
  }
};

// ----------------------------------------------------------------------------
class OscapeGUI; class OscapeGUI *gui;
class OscapeGUI : public wxOscape
{
public:
  int actives;

  pluginmap plugins;
  vector< pluginmap::iterator > psorted;
  worldset wspaces;
  map<int, worldset::iterator > wspacef;

private:
  // Installed Path
  char IPath[1024];
  HKEY Settings;
  OscapePrg *prog;

  /* ---------------------------------------------------------------------------- */
  void ResetHButtons() {
    /* enable button */
    OSWorldspaceFill->Enable((actives != 0));
    OSPluginExtract->Enable((actives != 0) && (OSFileHeightfieldOut->GetPath() != "") && (OSWorldspace->GetValue() != ""));
    OSHeightfieldInstall->Enable((OSBaseDirIn->GetPath() != "") && (OSPlugoutDir->GetPath() != ""));
  }

  /* ---------------------------------------------------------------------------- */
  void ResetHPluginDir(bool init = false) {
    IPath[0] = 0; DWORD IPL; HKEY IDir;

    if (init)
      RegGetValue(Settings, NULL, "Plugin Directory", RRF_RT_REG_SZ, NULL, &IPath, &IPL);

    if (!IPath[0])
      if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Bethesda Softworks\\Oblivion", 0, KEY_READ | KEY_WOW64_32KEY, &IDir) == ERROR_SUCCESS) {
	RegGetValue(IDir, NULL, "Installed Path", RRF_RT_REG_SZ, NULL, &IPath, &IPL);
	RegCloseKey(IDir);

	strcat(IPath, "Data");
//	strcat(IPath, "\\");
      }

    size_t ILen = strlen(IPath) - 1;
    while (IPath[ILen] == '\\')
      IPath[ILen--] = '\0';

    OSPluginDir->SetPath(IPath);
    OSPlugoutDir->SetPath(IPath);
  }

  void ResetHPluginList() {
    actives = 0;
    plugins.clear();
    psorted.clear();
    wspaces.clear();
    wspacef.clear();

    wspacef[60] = wspaces.insert("Tamriel").first;
    wspacef[40728] = wspaces.insert("SEWorld").first;

    char PPath[1024];
    strcpy(PPath, IPath);
    strcat(PPath, "\\*.esp");

    HANDLE IFiles; WIN32_FIND_DATA IFound;
    if ((IFiles = FindFirstFileEx(PPath, FindExInfoBasic, &IFound, FindExSearchNameMatch, NULL, 0)) != INVALID_HANDLE_VALUE) {
      do {
	time_t       tme = IFound.ftLastWriteTime.dwHighDateTime;
	tme <<= 32; tme |= IFound.ftLastWriteTime.dwLowDateTime;
	pluginfile *set = &plugins[string(IFound.cFileName)];

	set->tme = tme;
	set->act = false;
      } while (FindNextFile(IFiles, &IFound));

      FindClose(IFiles);
    }

    strcpy(PPath, IPath);
    strcat(PPath, "\\*.esm");

    if ((IFiles = FindFirstFileEx(PPath, FindExInfoBasic, &IFound, FindExSearchNameMatch, NULL, 0)) != INVALID_HANDLE_VALUE) {
      do {
	time_t       tme = IFound.ftLastWriteTime.dwHighDateTime;
	tme <<= 32; tme |= IFound.ftLastWriteTime.dwLowDateTime;
	pluginfile *set = &plugins[string(IFound.cFileName)];

	set->tme = tme;
	set->act = false;
      } while (FindNextFile(IFiles, &IFound));

      FindClose(IFiles);
    }

    PWSTR Path;
    if (SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &Path) == S_OK) {
      FILE *plugs;
      char *p = UnicodeToAnsi(Path);
      char PBuffer[261];
      strcpy(PBuffer, p);
      strcat(PBuffer, "\\Oblivion\\Plugins.txt");
      if (!fopen_s(&plugs, PBuffer, "rt")) {
	while (!feof(plugs)) {
	  if (fgets(PBuffer, 260, plugs)) {
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
	      if (plugins.count(pb)) {
		pluginfile *set = &plugins[pb];

//	        set->tme = tme;
		set->act = true;
	      }
	    }
	  }
	}

	fclose(plugs);
      }
    }

    map<string, pluginfile>::iterator walk = plugins.begin();
    while (walk != plugins.end())
      psorted.push_back(walk++);

    sort(psorted.begin(), psorted.end(), pluginfile::compare);
    sort(psorted.begin(), psorted.end(), pluginfile::compare);

    OSPluginList->Clear(); int idx = 0;
    vector< map<string, pluginfile>::iterator >::iterator push = psorted.begin();
    while (push != psorted.end()) {
      wxOwnerDrawn *id;

      OSPluginList->Append(wxString((*push)->first));
      OSPluginList->Check(idx, (*push)->second.act);
      id = OSPluginList->GetItem(idx++);
      if ((*push)->second.act) {
	actives++;

	id->SetBackgroundColour(wxColour(240, 255, 240));
      }

      push++;
    }

    /* reset world-space list */
    OSWorldspace->Clear();
    worldset::iterator pushw = wspaces.begin();
    while (pushw != wspaces.end())
      OSWorldspace->Append(*pushw++);

    OSWorldspace->SetValue("Tamriel");

    ResetHButtons();
  }

  void ClearHPluginList() {
    int num = OSPluginList->GetCount();
    for (int n = 0; n < num; n++) {
      OSPluginList->Check(n, FALSE);
    }
  }

  void LoadHPluginList() {
    int num = OSPluginList->GetCount();
    for (int n = 0; n < num; n++) {
      const char *p = OSPluginList->GetItem(n)->GetName().data();
      char s[32] = ""; DWORD sl = 32;

      RegGetValue(Settings, "Plugins", p, RRF_RT_REG_SZ, NULL, s, &sl);

      if (s[0] != '\0')
	OSPluginList->Check(n, s[0] == '1');
    }
  }

  void SaveHPluginList() {
    int num = OSPluginList->GetCount();
    for (int n = 0; n < num; n++) {
      const char *p = OSPluginList->GetItem(n)->GetName().data();
      const char *s = "0";
      if (OSPluginList->IsChecked(n))
	s = "1";

      RegSetKeyValue(Settings, "Plugins", p, RRF_RT_REG_SZ, s, (DWORD)strlen(s) + 1);
    }
  }

  /* ---------------------------------------------------------------------------- */
  void ResetPluginList(wxCommandEvent& event) {
    ResetHPluginList();
  }

  void ClearPluginList(wxCommandEvent& event) {
    ClearHPluginList();
  }

  void LoadPluginList(wxCommandEvent& event) {
    LoadHPluginList();
  }

  void SavePluginList(wxCommandEvent& event) {
    SaveHPluginList();
  }

  void ChangeAutosave(wxCommandEvent& event) {
    RegSetKeyValue(Settings, NULL, "Autosave", RRF_RT_REG_SZ, OSPluginAutosave->GetValue() ? "1" : "0", 2);
  }

  /* ---------------------------------------------------------------------------- */
  void WorldspacesFromPlugins(wxCommandEvent& event) {
    wspaces.clear();
    wspacef.clear();

    wxBusyCursor wait;
    int num = OSPluginList->GetCount();
    for (int n = 0; n < num; n++) {
      if (OSPluginList->IsChecked(n)) {
	const char *p = OSPluginList->GetItem(n)->GetName().data();

	if (strstr(p, ".esm")) {
	  char PPath[260] = "";

	  // WRLD - total size 16
	  // EDID - 2 + string
	  strcpy(PPath, IPath);
	  strcat(PPath, "\\");
	  strcat(PPath, p);

	  OSStatusBar->SetStatusText(wxT(p), 0);

	  OFSTRUCT of;
	  HANDLE oh = (HANDLE)OpenFile(PPath, &of, OF_READ);
	  DWORD len = GetFileSize(oh, NULL);
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

	  const DWORD wrld = MAKEFOURCC('W','R','L','D');
	  const DWORD edid = MAKEFOURCC('E','D','I','D');
	  for (DWORD l = 0; l < len; l++) {
	    if (*((DWORD *)(mem +  0)) == wrld)
	    if (*((DWORD *)(mem + 20)) == edid) {
	      DWORD fid = *((DWORD *)(mem + 12));
	      string wdn(mem + 26);

	      wspacef[fid] = wspaces.insert(wdn).first;
	    }

	    mem++;
	  }

	  UnmapViewOfFile(mem);
	  CloseHandle(mh);
	  CloseHandle(oh);
	}
      }
    }

#if 0
    Collection *col = CreateCollection(IPath, 0);
    class WorldOp : public RecordOp {
    public:
      class OscapeGUI *gui;
      WorldOp(class OscapeGUI *gui) : RecordOp() { this->gui = gui; }

      virtual bool Accept(Record *&curRecord) {
	Ob::WRLDRecord *wrld = (Ob::WRLDRecord *)curRecord;
	if (wrld->EDID.value)
	  gui->wspaces.insert(wrld->EDID.value);
	return false;
      }
    } wo(this);

    int num = OSPluginList->GetCount();
    for (int n = 0; n < num; n++) {
      if (OSPluginList->IsChecked(n)) {
	wxOwnerDrawn *id = OSPluginList->GetItem(n);
	const wxString nam = id->GetName();
	if (strstr(nam.data(), ".esm")) {
	  AddMod(col, (char * const)nam.data(), /*
	    0x00000040 / *fIsLoadMasters* / | ((*push)->second.act ?
	    0x00000008 / *fIsInLoadOrder* / : 0)*/
	    0x00000001 /* fIsFullLoad * /
	    0x00000002 / * fIsMinLoad */);
	  break;
	}
      }
    }

    LoadCollection(col);

    for (int n = 0; n < num; n++) {
      if (OSPluginList->IsChecked(n)) {
	wxOwnerDrawn *id = OSPluginList->GetItem(n);
	const wxString nam = id->GetName();
	if (strstr(nam.data(), ".esm")) {
	  ModFile *mf = GetModIDByName(col, (char * const)nam.data());

	  OSStatusBar->SetStatusText(wxT(nam.data()), 0);
//	  mf->LoadTES4();
	  mf->VisitRecords(REV32(WRLD), wo);
	}
      }
    }

    OSStatusBar->SetStatusText(wxT("Ready"), 0);
    DeleteCollection(col);
#endif

    OSStatusBar->SetStatusText(wxT("Ready"), 0);

    /* reset world-space list */
    OSWorldspace->Clear();
    worldset::iterator pushw = wspaces.begin();
    while (pushw != wspaces.end())
      OSWorldspace->Append(*pushw++);

    OSWorldspace->SetValue("Tamriel");

    ResetHButtons();
  }

  /* ---------------------------------------------------------------------------- */
  void ChangePluginDir(wxFileDirPickerEvent& event) {
    strcpy(IPath, OSPluginDir->GetTextCtrlValue().data());
//  strcat(IPath, "\\");

    ResetHPluginList();
    ResetHButtons();

    RegSetKeyValue(Settings, NULL, "Plugin Directory", RRF_RT_REG_SZ, IPath, (DWORD)strlen(IPath) + 1);
  }

  /* ---------------------------------------------------------------------------- */
  void ChangeActivePlugins(wxCommandEvent& event) {
    actives = 0;

    int num = OSPluginList->GetCount();
    for (int n = 0; n < num; n++) {
      if (OSPluginList->IsChecked(n)) {
	actives++;
      }
    }

    ResetHButtons();
  }

  /* ---------------------------------------------------------------------------- */
  void ChangeWorldspace(wxCommandEvent& event) {
    ResetHButtons();

    wxString ph = event.GetString();
    if (!ph.IsNull()) {
      const char *WS = ph.data();
      RegSetKeyValue(Settings, NULL, "Last worldspace", RRF_RT_REG_SZ, WS, (DWORD)strlen(WS) + 1);
    }

    if (formID) {
      int wsel = 0;

      map<int, worldset::iterator >::iterator srch = wspacef.begin();
      while (srch != wspacef.end()) {
	if (*(srch->second) == ph) {
	  wsel = srch->first;
	  break;
	}

	srch++;
      }

      if (wsel)
	formID->SetValueFromInt(wsel);
    }
  }

  /* ---------------------------------------------------------------------------- */
  void ChangeHeightfieldOut(wxFileDirPickerEvent& event) {
    ResetHButtons();

    wxString ph = event.GetPath();
    if (!ph.IsNull()) {
      const char *HO = ph.data();
      RegSetKeyValue(Settings, NULL, "Heightfield Out", RRF_RT_REG_SZ, HO, (DWORD)strlen(HO) + 1);
    }
  }

  /* ---------------------------------------------------------------------------- */
  void HeightfieldExtract(wxCommandEvent& event) {
#ifdef	CBASH
    OSStatusBar->SetStatusText(wxT("Running extractor ..."), 0);
    wxBusyCursor wait;
    prog = new OscapePrg(this);
    int ret = prog->Enter(::HeightfieldExtract);
    delete prog;
    OSStatusBar->SetStatusText(wxT("Ready"), 0);
    prog = NULL;
    if (ret != 666)
      return;

    /* move the result-file around */
    wxString ph = OSFileHeightfieldOut->GetPath();
#else
    char PPath[260] = "", *PP;
    char POpts[32768] = "";

    GetModuleFileName(0, PPath, sizeof(PPath) - 1);
    if ((PP = strrchr(PPath, '\\')))
      PP[1] = '\0';

    strcat(PPath, "TESAnnwyn.exe");

    string wsn = OSWorldspace->GetValue();
    int fid = 0;
    map<int, worldset::iterator >::iterator srch = wspacef.begin();
    while (srch != wspacef.end()) {
      if (*(srch->second) == wsn) {
	fid = srch->first;
	break;
      }

      srch++;
    }

    strcat(POpts, " -p 1 -b 16 -h 8192 -w ");
    strcat(POpts, wsn.data());
    strcat(POpts, " ");

    int num = OSPluginList->GetCount();
    for (int n = 0; n < num; n++) {
      if (OSPluginList->IsChecked(n)) {
	const char *p = OSPluginList->GetItem(n)->GetName().data();

	if (n)              strcat(POpts, ",");
	if (strchr(p, ' ')) strcat(POpts, "\"");
	                    strcat(POpts, p);
	if (strchr(p, ' ')) strcat(POpts, "\"");
      }
    }

    OSStatusBar->SetStatusText(wxT("Running TESAnnwyn ..."), 0);

    HANDLE jobinfo;
    STARTUPINFO startinfo;
    PROCESS_INFORMATION processinfo;

    memset(&startinfo, 0, sizeof(startinfo));
    startinfo.cb = sizeof(startinfo);

    /* then try to create it */
    CreateProcess(
      PPath,
      POpts,
      NULL,
      NULL,
      TRUE,
      /*CREATE_NO_WINDOW | DEBUG_PROCESS | */
      CREATE_NEW_CONSOLE | INHERIT_PARENT_AFFINITY | CREATE_SUSPENDED,
      NULL,
      IPath,
      &startinfo,
      &processinfo
    );

    if (!processinfo.hProcess) {
      wxMessageDialog d(this, "Failed to start TESAnnwyn, check your installation!", "Oscape error");
      d.ShowModal();
      return;
    }

    // Configure all child processes associated with the job to terminate when the
    // parent process dies
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION
    jeli = { 0 };
    jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;

    jobinfo = CreateJobObject(NULL, NULL);
    AssignProcessToJobObject(jobinfo, processinfo.hProcess);
    SetInformationJobObject(jobinfo, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli));

    {
      wxBusyCursor wait;
      ResumeThread(processinfo.hThread);
      WaitForSingleObject(processinfo.hProcess, INFINITE);
    }

    CloseHandle(processinfo.hProcess);
    CloseHandle(processinfo.hThread);
    CloseHandle(jobinfo);

    OSStatusBar->SetStatusText(wxT("Ready"), 0);

    /* move the result-file around */
    wxString ph = OSFileHeightfieldOut->GetPath();

    strcpy(PPath, IPath);
    strcat(PPath, "tesannwyn.raw");

    /* does it exist? */
    DWORD atr = GetFileAttributes(PPath);
    if ((atr == INVALID_FILE_ATTRIBUTES)) {
      wxMessageDialog d(this, "TESAnnwyn aborted prematurely!", "Oscape error");
      d.ShowModal();
      return;
    }

    /* does it move around? */
    DeleteFile(ph.data());
    if (!MoveFile(PPath, ph.data())) {
      wxMessageDialog d(this, "Failed to access TESAnnwyn results, check your free space and permitions!", "Oscape error");
      d.ShowModal();
      return;
    }
#endif

    /* save list on sucess */
    if (OSPluginAutosave->GetValue())
      SaveHPluginList();

    /* next tab */
    OSToolSwitch->SetSelection(1);

    /* and activate */
    if (!ph.IsNull()) {
      const char *HI = ph.data();
      OSFileHeightfieldIn1->SetPath(HI);
      OSFileHeightfieldIn2->SetPath(HI);
      RegSetKeyValue(Settings, NULL, "Heightfield In", RRF_RT_REG_SZ, HI, (DWORD)strlen(HI) + 1);
    }

    ChangeHeightfieldIn1(ph);
    ChangeHeightfieldIn2(ph);
    ChangeBaseDirOut1(ph);
    ChangeBaseDirOut2(ph);
  }

public:
  void HeightfieldExtract() {
    /* move the result-file around */
    wxString ph = OSFileHeightfieldOut->GetPath();
    wxFileName fnn(ph); fnn.ClearExt(); fnn.SetExt("nrm");
    wxFileName fnh(ph); fnh.ClearExt(); fnh.SetExt("raw");
    wxFileName fnx(ph); fnx.ClearExt(); fnx.SetExt("land");
    wxFileName fnm(ph); fnm.ClearExt(); fnm.SetExt("map");

    wedata = OSPluginDir->GetPath();
    weoutn = fnn.GetFullPath(); calcn = false;
    weouth = fnh.GetFullPath(); calch = OSCalcHeight->GetValue();
    weoutx = fnx.GetFullPath(); calcx = OSCalcColor->GetValue();
    weoutm = fnm.GetFullPath(); calcm = OSCalcImportance->GetValue();
    wename = OSWorldspace->GetValue(); weid = 0;
    
    map<int, worldset::iterator >::iterator srch = wspacef.begin();
    while (srch != wspacef.end()) {
      if (*(srch->second) == wename) {
	weid = srch->first;
	break;
      }

      srch++;
    }

    Collection *col = CreateCollection(IPath, 0);

    int num = OSPluginList->GetCount();
    for (int n = 0; n < num; n++) {
      if (OSPluginList->IsChecked(n)) {
	wxOwnerDrawn *id = OSPluginList->GetItem(n);
	const wxString nam = id->GetName();
	{
	  AddMod(col, (char * const)nam.data(), /*
	    0x00000040 / *fIsLoadMasters* / | ((*push)->second.act ? */
	    0x00000008 /*fIsInLoadOrder* / : 0)*/ |
	    0x00000002 /* fIsFullLoad * /
	    0x00000001 / * fIsMinLoad * / |
	    0x00000200 / * fIsIndexLANDs */);
//	  break;
	}
      }
    }
    
    prog->StartProgress(3);

    /* configure filtering */
    col->SetFilterMode(true);
    col->AddFilter(REV32(WRLD));
    col->AddFilter(REV32(CELL));
    col->AddFilter(REV32(LTEX));
    col->AddFilter(REV32(LAND));

    try {
      prog->InitProgress("Initializing:", 0, "Reading plugins:", 0.0, 0, 1);
      LoadCollection(col, ::SetTopic);

      prog->InitProgress("Extracting:", 0, "Parsing plugins:", 0.0, 1, 1);
      ExtractFromCollection(col);

      prog->InitProgress("Cleanup:", 0, "Freeing plugins:", 0.0, 2, 1);
      DeleteCollection(col);
    }
    catch(exception &e) {
      DeleteCollection(col);

      if (strcmp(e.what(), "ExitThread")) {
	wxMessageDialog d(prog, e.what(), "Oscape error");
	d.ShowModal();
      }

      prog->Leave(0);
      return;
    }
    
    prog->Leave(666);
  }

private:
  /* ---------------------------------------------------------------------------- */
  wxImage hgt;
  wxMemoryDC hdc;

  void PaintH(wxPaintEvent& event) {
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

  void RedrawH() {
    wxString ph = OSFileHeightfieldIn1->GetPath();
    if (ph.IsNull())
      return;

    int which = OSPreviewSelector->GetSelection();
    wxString pw = OSPreviewSelector->GetString(which);

    wxFileName fnn(ph); fnn.ClearExt(); fnn.SetExt("nrm");
    wxFileName fnh(ph); fnh.ClearExt(); fnh.SetExt("raw");
    wxFileName fnx(ph); fnx.ClearExt(); fnx.SetExt("land");
    wxFileName fnm(ph); fnm.ClearExt(); fnm.SetExt("map");

    /**/ if (pw == "Heightfield") ph = fnh.GetFullPath();
    else if (pw == "Normals"    ) ph = fnn.GetFullPath();
    else if (pw == "Features"   ) ph = fnm.GetFullPath();
    else if (pw == "Surface"    ) ph = fnx.GetFullPath();
    else return;

    /* does it exist? */
    DWORD atr = GetFileAttributes(ph.data());
    if (atr == INVALID_FILE_ATTRIBUTES)
      return;
    
    int rasterx = 32 * 32;
    int rastery = 32 * 32;
    int tilesx = 1;
    int tilesy = 1;
    int offsx = 1;
    int offsy = 1;
    int width = rWidth->GetValueAsVariant().GetInteger();
    int height = rHeight->GetValueAsVariant().GetInteger();
    int wdscape = 0;

    {
      /**/ if (pw == "Heightfield") OSStatusBar->SetStatusText(wxT("Skimming heightfield ..."), 0);
      else if (pw == "Normals"    ) OSStatusBar->SetStatusText(wxT("Skimming normals ..."), 0);
      else if (pw == "Features"   ) OSStatusBar->SetStatusText(wxT("Skimming feature-map ..."), 0);
      else if (pw == "Surface"    ) OSStatusBar->SetStatusText(wxT("Skimming surface-map ..."), 0);

      OFSTRUCT of;
      HANDLE oh = (HANDLE)OpenFile(ph, &of, OF_READ);
      DWORD len = GetFileSize(oh, NULL);
      int pixel = len;
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

      if (pixel) {
	/**/ if (pw == "Heightfield") pixel /= sizeof(unsigned short);
	else if (pw == "Normals"    ) pixel /= sizeof(unsigned char) * 3;
	else if (pw == "Features"   ) pixel /= sizeof(unsigned char) * 1;
	else if (pw == "Surface"    ) pixel /= sizeof(unsigned long);

	tilesx = (width  + (rasterx - 1)) / rasterx;
	tilesy = (height + (rastery - 1)) / rastery;
	offsx = tilesx >> 1;
	offsy = tilesy >> 1;
	int scanx = 64;
	int scany = 64;
	int pwidth  = tilesx * scanx;
	int pheight = tilesy * scany;
	int multx = 1024 / scanx;
	int multy = 1024 / scany;

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

	/**/ if (pw == "Heightfield") {
	  for (int y = 0; y < pheight; y++)
	  for (int x = 0; x < pwidth; x++) {
	    int cy = min(y * multy, height - 1);
	    int cx = min(x * multx, width  - 1);

	    unsigned short el = ((unsigned short *)mem)[(cy * width) + (cx)];

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

	    unsigned char el = ((unsigned char *)mem)[(cy * width) + (cx)];

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

	    unsigned long el = ((unsigned long *)mem)[(cy * width) + (cx)];
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

	hdc.SelectObject(wxBitmap(hgt));

//	this->Refresh();
	OSPreview->Update();
	OSPreview->Refresh();
      }

      UnmapViewOfFile(mem);
      CloseHandle(mh);
      CloseHandle(oh);

      OSStatusBar->SetStatusText(wxT("Ready"), 0);
    }
  }

  void ChangeHeightfieldIn1(wxFileDirPickerEvent& event) {
    wxString ph = event.GetPath();

    ChangeHeightfieldIn1(ph);
    ChangeHeightfieldIn2(ph);
    ChangeBaseDirOut1(ph);
    ChangeBaseDirOut2(ph);
  }

  wxPGProperty *formID;
  wxPGProperty *tSize;
  wxPGProperty *cSize;
  wxPGProperty *rWidth;
  wxPGProperty *rHeight;
  wxPGProperty *tLeft;
  wxPGProperty *tRight;
  wxPGProperty *tTop;
  wxPGProperty *tBottom;

  void ChangeHeightfieldIn1(wxString ph) {
    OSPreviewSelector->Hide();
    OSPreviewSelector->Clear();
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
      RegSetKeyValue(Settings, NULL, "Heightfield In", RRF_RT_REG_SZ, HI, (DWORD)strlen(HI) + 1);
    }

    wxFileName fnn(ph); fnn.ClearExt(); fnn.SetExt("nrm");
    wxFileName fnh(ph); fnh.ClearExt(); fnh.SetExt("raw");
    wxFileName fnx(ph); fnx.ClearExt(); fnx.SetExt("land");
    wxFileName fnm(ph); fnm.ClearExt(); fnm.SetExt("map");

    wxString snn = fnn.GetFullPath();
    wxString snh = fnh.GetFullPath();
    wxString snx = fnx.GetFullPath();
    wxString snm = fnm.GetFullPath();

    atr = GetFileAttributes(snn.data()); bool bnn = (atr != INVALID_FILE_ATTRIBUTES);
    atr = GetFileAttributes(snh.data()); bool bnh = (atr != INVALID_FILE_ATTRIBUTES);
    atr = GetFileAttributes(snx.data()); bool bnx = (atr != INVALID_FILE_ATTRIBUTES);
    atr = GetFileAttributes(snm.data()); bool bnm = (atr != INVALID_FILE_ATTRIBUTES);

    if (bnh) OSPreviewSelector->Append("Heightfield");
    if (bnn) OSPreviewSelector->Append("Normals");
    if (bnm) OSPreviewSelector->Append("Features");
    if (bnx) OSPreviewSelector->Append("Surface");

    OSPreviewSelector->SetSelection(0);
    OSPreviewSelector->Show();
    OSHeightfieldFirst1->Hide();
    OSHeightfieldFirst1->GetParent()->Layout();

    int rasterx = 32 * 32;
    int rastery = 32 * 32;
    int tilesx = 1;
    int tilesy = 1;
    int offsx = 1;
    int offsy = 1;
    int width = 1;
    int height = 1;
    int wdscape = 0;

    /* search from this field first */
    string wsn = OSWorldspace->GetValue();
    map<int, worldset::iterator >::iterator srch = wspacef.begin();
    while (srch != wspacef.end()) {
      if (*(srch->second) == wsn) {
	wdscape = srch->first;
	break;
      }

      srch++;
    }

    /* then check against the file-name */
    wxFileName fn(ph); fn.ClearExt();
    wsn = fn.GetName();
    srch = wspacef.begin();
    while (srch != wspacef.end()) {
      if (*(srch->second) == wsn) {
	wdscape = srch->first;
	break;
      }

      srch++;
    }

    {
      OSStatusBar->SetStatusText(wxT("Analyzing heightfield ..."), 0);

      OFSTRUCT of;
      HANDLE oh = (HANDLE)OpenFile(ph, &of, OF_READ);
      DWORD len = GetFileSize(oh, NULL);
      int pixel = len >> 1;

      if (pixel) {
	bool hor = OSOrientation->GetValue();

	width  = pixel / rasterx;
	height = pixel / width;

	double l = sqrt((double)width * width + (double)height * height);
	for (int h = height; h < pixel; h += rastery) {
	  int w = pixel / h;
	  double m = sqrt((double)w * w + (double)h * h);

	  if (pixel == (w * h))
	  if (( hor && (l >  m)) ||
	      (!hor && (l >= m))) {
	    l = m;

	    width  = w;
	    height = h;
	  }
	}

	tilesx = (width  + (rasterx - 1)) / rasterx;
	tilesy = (height + (rastery - 1)) / rastery;
	offsx = tilesx >> 1;
	offsy = tilesy >> 1;
      }

      CloseHandle(oh);

      OSStatusBar->SetStatusText(wxT("Ready"), 0);
    }

    wxPGProperty *p;

    OSHeightfieldInfos->AppendCategory("Dimensions");
    OSHeightfieldInfos->Append(p = wxIntProperty("Worldspace FormID", wxPG_LABEL, wdscape)); formID = p;
    OSHeightfieldInfos->Append(p = wxIntProperty("Tilesize (in cells)", wxPG_LABEL, 32)); tSize = p;
    OSHeightfieldInfos->Append(p = wxIntProperty("Cellsize (in units)", wxPG_LABEL, 32)); cSize = p;
    wxPGId valProp =
    OSHeightfieldInfos->Append(wxParentProperty("Total", wxPG_LABEL));
    OSHeightfieldInfos->AppendIn(valProp, p = wxIntProperty("Width", wxPG_LABEL, width)); rWidth = p;
    OSHeightfieldInfos->AppendIn(valProp, p = wxIntProperty("Height", wxPG_LABEL, height)); rHeight = p;
    wxPGId tileProp =
    OSHeightfieldInfos->Append(wxParentProperty("Cells", wxPG_LABEL));
    OSHeightfieldInfos->AppendIn(tileProp, p = wxIntProperty("Left", wxPG_LABEL, (-offsx) * 32)); tLeft = p;
    OSHeightfieldInfos->AppendIn(tileProp, p = wxIntProperty("Top", wxPG_LABEL, (-offsy) * 32)); tTop = p;
    OSHeightfieldInfos->AppendIn(tileProp, p = wxIntProperty("Right", wxPG_LABEL, (tilesx - offsx - 1) * 32)); tRight = p;
    OSHeightfieldInfos->AppendIn(tileProp, p = wxIntProperty("Bottom", wxPG_LABEL, (tilesy - offsy - 1) * 32)); tBottom = p;
    OSHeightfieldInfos->AppendCategory("Stats");
    OSHeightfieldInfos->Append(p = wxIntProperty("Minimum", wxPG_LABEL, -8192)); p->SetFlag(wxPG_PROP_READONLY);
    OSHeightfieldInfos->Append(p = wxIntProperty("Seallevel", wxPG_LABEL, 0)); p->SetFlag(wxPG_PROP_READONLY);
    OSHeightfieldInfos->Append(p = wxIntProperty("Maximum", wxPG_LABEL, 55600)); p->SetFlag(wxPG_PROP_READONLY);

//  SetHeightfield(&hgt, _rt);
    RedrawH();

    OSHeightfieldFirst1->GetParent()->Layout();

    VerifyHeightfieldIn();
  }

  void ChangePreview(wxCommandEvent& event) {
    RedrawH();
  }

  void ChangeOrientation(wxCommandEvent& event) {
    ChangeHeightfieldIn1(OSFileHeightfieldIn1->GetPath());

    RegSetKeyValue(Settings, NULL, "Orientation", RRF_RT_REG_DWORD, OSOrientation->GetValue() ? "H" : "V", 2);
  }

  void ChangeHeightfieldInfos(wxPropertyGridEvent& event) {
    wxPGId p = event.GetProperty();
    
    if ((p == rWidth ->GetId()) ||
        (p == rHeight->GetId()))
      RedrawH();
  }

  void HeightfieldAccept(wxCommandEvent& event) {
    /* next tab */
    OSToolSwitch->SetSelection(2);
  }

  /* ---------------------------------------------------------------------------- */
  void ChangePointsIn1(wxFileDirPickerEvent& event) {
    wxString ph = event.GetPath();

    ChangePointsIn1(ph);
    ChangePointsIn2(ph);
  }

  void ChangePointsIn1(wxString ph) {
    OSAdditionalPoints->ClearAll();
    OSAdditionalPoints->Enable(FALSE);
    OSPointsClear1->Enable(FALSE);

    {
      const char *PI = ph.data();
      OSFilePoints1->SetPath(PI);
      OSFilePoints2->SetPath(PI);
      RegSetKeyValue(Settings, NULL, "Points In", RRF_RT_REG_SZ, PI, (DWORD)strlen(PI) + 1);
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

  void ClearPoints1(wxCommandEvent& event) {
    ChangePointsIn1("");
    ChangePointsIn2("");
  }

  void ChangeBaseDirOut1(wxFileDirPickerEvent& event) {
    wxString ph = event.GetPath();

    ChangeBaseDirOut1(ph);
    ChangeBaseDirOut2(ph);
  }

  void ChangeBaseDirOut1(wxString ph) {
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
      RegSetKeyValue(Settings, NULL, "Base directory Out", RRF_RT_REG_SZ, BO, (DWORD)strlen(BO) + 1);
    }

    OSHeightfieldAccept->Enable(TRUE);
  }

  /* ---------------------------------------------------------------------------- */
  void ChangeHeightfieldIn2(wxFileDirPickerEvent& event) {
    wxString ph = event.GetPath();

    ChangeHeightfieldIn1(ph);
    ChangeHeightfieldIn2(ph);
    ChangeBaseDirOut1(ph);
    ChangeBaseDirOut2(ph);
  }

  void ChangeHeightfieldIn2(wxString ph) {
    OSEmphasis->Disable();
    OSSelectGenerator->Hide();
    OSHeightfieldFirst2->Show();
    OSHeightfieldFirst2->GetParent()->Layout();

    if (ph.IsNull())
      return;

    wxFileName fnn(ph); fnn.ClearExt(); fnn.SetExt("nrm");
    wxFileName fnh(ph); fnh.ClearExt(); fnh.SetExt("raw");
    wxFileName fnx(ph); fnx.ClearExt(); fnx.SetExt("land");
    wxFileName fnm(ph); fnm.ClearExt(); fnm.SetExt("map");

    wxString snn = fnn.GetFullPath();
    wxString snh = fnh.GetFullPath();
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
      RegSetKeyValue(Settings, NULL, "Heightfield In", RRF_RT_REG_SZ, HI, (DWORD)strlen(HI) + 1);
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

  void ChangePointsIn2(wxFileDirPickerEvent& event) {
    wxString ph = event.GetPath();

    ChangePointsIn1(ph);
    ChangePointsIn2(ph);
  }

  void ChangePointsIn2(wxString ph) {
    OSPointsClear2->Enable(FALSE);

    {
      const char *PI = ph.data();
      OSFilePoints1->SetPath(PI);
      OSFilePoints2->SetPath(PI);
      RegSetKeyValue(Settings, NULL, "Points In", RRF_RT_REG_SZ, PI, (DWORD)strlen(PI) + 1);
    }

    if (ph.IsNull())
      return;

    OSPointsClear2->Enable(TRUE);
  }

  void ClearPoints2(wxCommandEvent& event) {
    ChangePointsIn1("");
    ChangePointsIn2("");
  }

  void ChangeBaseDirOut2(wxFileDirPickerEvent& event) {
    wxString ph = event.GetPath();

    ChangeBaseDirOut1(ph);
    ChangeBaseDirOut2(ph);
  }

  void ChangeBaseDirOut2(wxString ph) {
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
      RegSetKeyValue(Settings, NULL, "Base directory Out", RRF_RT_REG_SZ, BO, (DWORD)strlen(BO) + 1);
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
  wxString GetTarget(wxString val) {
    size_t ln;
    if ((ln = val.Index(' ')) != wxNOT_FOUND)
      val = val.SubString(0, ln - 1);
    return val;
  }

  wxString GetTarget() {
    return GetTarget(OSTarget->GetValue());
  }

  void MaxTarget(int &_maxp, int &_maxx, int &_ntls) {
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

  void VerifyHeightfieldIn() {
    wxString val = GetTarget();
    long l = 0; val.ToLong(&l);

    OSTarget->Clear();

    OSTarget->Append("220000 (~3/2 of vanilla Cyrodiil)");
    OSTarget->Append("155000 (~3/3 of vanilla Cyrodiil)");
    OSTarget->Append("110000 (~2/3 of vanilla Cyrodiil)");
    OSTarget->Append("55000 (~1/3 of vanilla Cyrodiil)");
    OSTarget->Append("615000 (~4/3 of vanilla TWMP)");
    OSTarget->Append("450000 (~3/3 of vanilla TWMP)");
    OSTarget->Append("290000 (~2/3 of vanilla TWMP)");
    OSTarget->Append("150000 (~1/3 of vanilla TWMP)");

    /* -------------------------------------------------------------------- */
    int maxp, maxx, ntls; MaxTarget(maxp, maxx, ntls);
    int maxs, maxm, it; maxm = maxp;

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

    /* -------------------------------------------------------------------- */
    OSTarget->SetValue(val);
    if (l > maxx) {
      char buf[32]; sprintf(buf, "%d", maxx);

      OSTarget->SetValue(buf);
    }
  }

  void VerifyHeightfieldOut() {
    wxVariant v;

    if (!formID)
      return;
    v = formID->GetValueAsVariant();
    if (v.IsNull())
      return;

    wdspace = v.GetInteger();

    long l = 0; if (GetTarget().ToLong(&l)) limit = l;
    double r = 0.0; if (OSTermination->GetValue().ToDouble(&r)) termination = r;

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
	OSRes1->Enable(true);
      else
	OSRes1->Enable(false), OSRes1->SetValue(true);
    }
  }

  void ChangeAlgorithm(wxCommandEvent& event) {
    OSQThreshold->Enable(event.GetSelection() == 0);
  }

  void ChangeTarget(wxCommandEvent& event) {
    OSRes1->Enable(false);
    OSRes2->Enable(false);
    OSRes3->Enable(false);
    OSRes4->Enable(false);
    OSRes5->Enable(false);
    OSRes6->Enable(false);
    OSRes7->Enable(false);
    OSRes8->Enable(false);

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
      RegSetKeyValue(Settings, NULL, "Target", RRF_RT_REG_DWORD, TG, (DWORD)strlen(TG) + 1);
    }

    int r = 0;
    while (l > 1024) {
      l = (l + 1) >> 1;
      r++;
    }

  /*OSRes1->Enable(r > 0); if (r <= 0) OSRes1->SetValue(false);*/
    OSRes2->Enable(r > 1); if (r <= 1) OSRes2->SetValue(false);
    OSRes3->Enable(r > 2); if (r <= 2) OSRes3->SetValue(false);
    OSRes4->Enable(r > 3); if (r <= 3) OSRes4->SetValue(false);
    OSRes5->Enable(r > 4); if (r <= 4) OSRes5->SetValue(false);
    OSRes6->Enable(r > 5); if (r <= 5) OSRes6->SetValue(false);
    OSRes7->Enable(r > 6); if (r <= 6) OSRes7->SetValue(false);
    OSRes8->Enable(r > 7); if (r <= 7) OSRes8->SetValue(false);

    VerifyHeightfieldOut();
  }

  /* ---------------------------------------------------------------------------- */
  void ChangeMeshes(wxCommandEvent& event) {
    ChangeMeshes();

    RegSetKeyValue(Settings, NULL, "Do Meshes", RRF_RT_REG_DWORD, OSMeshes->GetValue() ? "1" : "0", 2);
  }

  void ChangeNormals(wxCommandEvent& event) {
    ChangeNormals();

    RegSetKeyValue(Settings, NULL, "Do Normals", RRF_RT_REG_DWORD, OSNormals->GetValue() ? "1" : "0", 2);
  }

  void ChangeColors(wxCommandEvent& event) {
    ChangeColors();

    RegSetKeyValue(Settings, NULL, "Do Colors", RRF_RT_REG_DWORD, OSColors->GetValue() ? "1" : "0", 2);
  }

  void ChangeHeightmap(wxCommandEvent& event) {
    ChangeHeightmap();

    RegSetKeyValue(Settings, NULL, "Do Heightmap", RRF_RT_REG_DWORD, OSHeightmap->GetValue() ? "1" : "0", 2);
  }

  void ChangeMeshes() {
    if (OSMeshes->GetValue())
      OSPanelMeshes->Show();
    else
      OSPanelMeshes->Hide();

    OSPanelMeshes->GetParent()->Layout();
    OSSelectGenerator->SetVirtualSize(OSPanelMeshes->GetParent()->GetBestVirtualSize());
  }

  void ChangeNormals() {
    if (OSNormals->GetValue())
      OSPanelNormals->Show();
    else
      OSPanelNormals->Hide();

    OSPanelNormals->GetParent()->Layout();
    OSSelectGenerator->SetVirtualSize(OSPanelNormals->GetParent()->GetBestVirtualSize());
  }

  void ChangeColors() {
    if (OSColors->GetValue())
      OSPanelColors->Show();
    else
      OSPanelColors->Hide();

    OSPanelColors->GetParent()->Layout();
    OSSelectGenerator->SetVirtualSize(OSPanelColors->GetParent()->GetBestVirtualSize());
  }

  void ChangeHeightmap() {
    if (OSHeightmap->GetValue())
      OSPanelHeightmap->Show();
    else
      OSPanelHeightmap->Hide();

    OSPanelHeightmap->GetParent()->Layout();
    OSSelectGenerator->SetVirtualSize(OSPanelHeightmap->GetParent()->GetBestVirtualSize());
  }

  void CheckFloat(wxCommandEvent& event) {
    wxString n = event.GetString();
    double r;
    if (!n.ToDouble(&r))
      event.SetString("0.0");
  }

  void CheckInt(wxCommandEvent& event) {
    wxString n = event.GetString();
    long r;
    if (!n.ToLong(&r))
      event.SetString("0");
  }

  void HeightfieldGenerate(wxCommandEvent& event) {
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

public:
  bool SanitizeGeneration() {
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

  void HeightfieldGenerate() {
    if (!SanitizeGeneration()) {
      prog->Leave(666);
      return;
    }

    /* -------------------------------------------------------------------- */
    rasterx = tSize->GetValueAsVariant().GetInteger() * cSize->GetValueAsVariant().GetInteger();
    rastery = tSize->GetValueAsVariant().GetInteger() * cSize->GetValueAsVariant().GetInteger();

    rwsizex = rWidth ->GetValueAsVariant().GetInteger();
    rwsizey = rHeight->GetValueAsVariant().GetInteger();

    width   = rwsizex;
    height  = rwsizey;

    tilesx  = width  / rasterx;
    tilesy  = height / rastery;

    dotilex = tLeft->GetValueAsVariant().GetInteger() / tSize->GetValueAsVariant().GetInteger();
    dotiley = tTop ->GetValueAsVariant().GetInteger() / tSize->GetValueAsVariant().GetInteger();

    nmtilex = tRight ->GetValueAsVariant().GetInteger() / tSize->GetValueAsVariant().GetInteger() - dotilex + 1;
    nmtiley = tBottom->GetValueAsVariant().GetInteger() / tSize->GetValueAsVariant().GetInteger() - dotiley + 1;

    dotilex = dotilex + (tilesx >> 1);
    dotiley = dotiley + (tilesy >> 1);

    wdspace = formID->GetValueAsVariant().GetInteger();

    /* -------------------------------------------------------------------- */
    long l = 0; double r = 0.0;

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

    if (OSQThreshold->GetValue().ToDouble(&r)) qual_thresh = max(0.001, min(r, 0.999));
    if (OSAThreshold->GetValue().ToDouble(&r)) area_thresh = max(0.0, r);
    if (OSTermination->GetValue().ToDouble(&r)) termination = max(0.0, r);
    if (OSEmphasis->GetValue().ToDouble(&r)) emphasis = max(0.0, min(r, 1.0));

    if (!OSEmphasis->IsEnabled()) emphasis = 0.0;

    /* -------------------------------------------------------------------- */
    wxString dataPth = OSFileHeightfieldIn1->GetPath();
    wxString dataPts = OSFilePoints1->GetPath();
    wxString dataDir = OSBaseDirOut1->GetPath();

    wxFileName fnn(dataPth); fnn.ClearExt(); fnn.SetExt("nrm");
    wxFileName fnh(dataPth); fnh.ClearExt(); fnh.SetExt("raw");
    wxFileName fnx(dataPth); fnx.ClearExt(); fnx.SetExt("land");
    wxFileName fnm(dataPth); fnm.ClearExt(); fnm.SetExt("map");

    wxString greyFPth = fnm.GetFullPath();
    wxString dataFPth = fnh.GetFullPath();
    wxString colrFPth = fnx.GetFullPath();

    texFile = NULL;
    greyFile = greyFPth.data();
    colrFile = colrFPth.data();
    dataFile = dataFPth.data();

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

      if (OSRes1->GetValue()) majortasks += m;
      if (OSRes2->GetValue()) majortasks += m + 1;
      if (OSRes3->GetValue()) majortasks += m + 1;
      if (OSRes4->GetValue()) majortasks += m + 1;
      if (OSRes5->GetValue()) majortasks += m + 1;
      if (OSRes6->GetValue()) majortasks += m + 1;
      if (OSRes7->GetValue()) majortasks += m + 1;
      if (OSRes8->GetValue()) majortasks += m + 1;
    }

    /* generate textures (PPM, DDS, PNG) */
    if (OSNormals->GetValue()) {
      int m = 1;

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
      int m = 1;

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

	vector< pair<int, float> > ress; size_t r = 0;

	if (OSRes1->GetValue()) ress.push_back(pair<int, float>(limit / 1, termination * 1));
	if (OSRes2->GetValue()) ress.push_back(pair<int, float>(limit / 2, termination * 2));
	if (OSRes3->GetValue()) ress.push_back(pair<int, float>(limit / 4, termination * 4));
	if (OSRes4->GetValue()) ress.push_back(pair<int, float>(limit / 8, termination * 8));
	if (OSRes5->GetValue()) ress.push_back(pair<int, float>(limit / 16, termination * 16));
	if (OSRes6->GetValue()) ress.push_back(pair<int, float>(limit / 32, termination * 32));
	if (OSRes7->GetValue()) ress.push_back(pair<int, float>(limit / 64, termination * 64));
	if (OSRes8->GetValue()) ress.push_back(pair<int, float>(limit / 128, termination * 128));

	if ((r = ress.size()) > 0) {
	  char temps[256], base[1024];
	  int target = limit;

	  for (size_t n = 0; n < r; n++) {
	    limit       = ress[n].first;
	    termination = ress[n].second;

	    srandom(limit);

	    _controlfp(_RC_DOWN, _MCW_RC);    // round to -8
//	    _controlfp(_PC_53, _MCW_PC);    // round to -8

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
		strcat(temps, "\\UVoff\\%02d.%02d.%02d.%02d");

		skiplist.push_back(temps);
	      }

	      if (OSMeshUVs->Get3StateValue() != wxCHK_UNCHECKED) {
		strcpy(temps, base);
		strcat(temps, "\\UVon\\%02d.%02d.%02d.%02d");

		skiplist.push_back(temps);
	      }

	      /* initial heightfield-class */
	      SimplField ter(&H);

#ifdef	SPLIT_ON_INJECTION
	      /* damit, the heightfield is [0,width), not inclusive */
	      for (int h = 0; h <= height; h += rastery)
	      for (int w = 0; w <=  width; w += rasterx)
		ter.select_new_point(min(w, width - 1), min(h, height - 1));
#endif

#if 0
	      debug = 0;//(limit == 2025 ? 500 : 0);
#endif

	      /* serial insertion */
	      greedy_insert(ter);

	      /* parallel insertion (consume all above given error) */
//	      greedy_insert_error(ter);

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
		strcat(temps, "\\UVoff\\%02d.%02d.%02d.%02d");
		emituvs = false;

		prog->InitProgress("Resolution %d, saving mesh:", limit, "Saving non-UV tiles:", 0.0, majordone++, 1);

		wrteGeometry(ter, temps);
	      }

	      /* go, write */
	      if (OSMeshUVs->Get3StateValue() != wxCHK_UNCHECKED) {
		sprintf(temps, "%s\\UVon", base);
		CreateDirectory(temps, NULL);

		strcpy(temps, base);
		strcat(temps, "\\UVon\\%02d.%02d.%02d.%02d");
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
	      FreeGeometry();
	    }
	    catch(exception &e) {
	      FreeGeometry();

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
      if (OSNormals->GetValue()) {
	char temps[256], base[1024];

	writeppm = OSNormalPPM->GetValue();
	writepng = OSNormalPNG->GetValue();
	writedds = OSNormalDDS->GetValue();

	try {

	  if (OSNormalLow    ->GetValue()) {
	    /* create the base directory */
	    sprintf(base, "%s\\TEX-%d", dataDir.data(), 512);
	    CreateDirectory(base, NULL);

	    strcpy(temps, base);
	    strcat(temps, "\\%02d.%02d.%02d.%02d");

	    prog->InitProgress("Resolution %d, calculating normal-maps:", 512, "Tiling normals:", 0.0, majordone++, 1);
	    write_nrmhgt0(false, OSNormalLow    ->GetValue(), false, H, temps);
	  }

	  if (OSNormalRegular->GetValue()) {
	    /* create the base directory */
	    sprintf(base, "%s\\TEX-%d", dataDir.data(), 1024);
	    CreateDirectory(base, NULL);

	    strcpy(temps, base);
	    strcat(temps, "\\%02d.%02d.%02d.%02d");

	    prog->InitProgress("Resolution %d, calculating normal-maps:", 1024, "Tiling normals:", 0.0, majordone++, 1);
	    write_nrmhgt1(false, OSNormalRegular->GetValue(), false, H, temps);
	  }

	  if (OSNormalHigh   ->GetValue()) {
	    /* create the base directory */
	    sprintf(base, "%s\\TEX-%d", dataDir.data(), 2048);
	    CreateDirectory(base, NULL);

	    strcpy(temps, base);
	    strcat(temps, "\\%02d.%02d.%02d.%02d");

	    prog->InitProgress("Resolution %d, calculating normal-maps:", 2048, "Tiling normals:", 0.0, majordone++, 1);
	    write_nrmhgt2(false, OSNormalHigh   ->GetValue(), false, H, temps);
	  }

	  FreeTextures();
	}
	catch(exception &e) {
	  FreeTextures();

	  if (strcmp(e.what(), "ExitThread")) {
	    wxMessageDialog d(prog, e.what(), "Oscape error");
	    d.ShowModal();
	  }

	  prog->Leave(0);
	  return;
	}
      }
    }

    /* -------------------------------------------------------------------- */
    if (OSColors->GetValue()) {
      prog->InitProgress("Initializing:", 0, "Reading surface-map:", 0.0, majordone++, 1);

      ifstream mntns(colrFile, ios::binary);
      CField C(mntns);

      /* -------------------------------------------------------------------- */
      if (OSColors->GetValue()) {
	char temps[256], base[1024];

	writeppm = OSColorPPM->GetValue();
	writepng = OSColorPNG->GetValue();
	writedds = OSColorDDS->GetValue();

	try {

	  if (OSColorLow    ->GetValue()) {
	    /* create the base directory */
	    sprintf(base, "%s\\TEX-%d", dataDir.data(), 512);
	    CreateDirectory(base, NULL);

	    strcpy(temps, base);
	    strcat(temps, "\\%02d.%02d.%02d.%02d");

	    prog->InitProgress("Resolution %d, calculating color-maps:", 512, "Tiling colors:", 0.0, majordone++, 1);
	    write_col0(OSColorLow    ->GetValue(), C, temps);
	  }

	  if (OSColorRegular->GetValue()) {
	    /* create the base directory */
	    sprintf(base, "%s\\TEX-%d", dataDir.data(), 1024);
	    CreateDirectory(base, NULL);

	    strcpy(temps, base);
	    strcat(temps, "\\%02d.%02d.%02d.%02d");

	    prog->InitProgress("Resolution %d, calculating color-maps:", 1024, "Tiling colors:", 0.0, majordone++, 1);
	    write_col1(OSColorRegular->GetValue(), C, temps);
	  }

	  if (OSColorHigh   ->GetValue()) {
	    /* create the base directory */
	    sprintf(base, "%s\\TEX-%d", dataDir.data(), 2048);
	    CreateDirectory(base, NULL);

	    strcpy(temps, base);
	    strcat(temps, "\\%02d.%02d.%02d.%02d");

	    prog->InitProgress("Resolution %d, calculating color-maps:", 2048, "Tiling colors:", 0.0, majordone++, 1);
	    write_col2(OSColorHigh   ->GetValue(), C, temps);
	  }

	  if (OSColorUltra  ->GetValue()) {
	    /* create the base directory */
	    sprintf(base, "%s\\TEX-%d", dataDir.data(), 4096);
	    CreateDirectory(base, NULL);

	    strcpy(temps, base);
	    strcat(temps, "\\%02d.%02d.%02d.%02d");

	    prog->InitProgress("Resolution %d, calculating color-maps:", 4096, "Tiling colors:", 0.0, majordone++, 1);
	    write_col3(OSColorUltra  ->GetValue(), C, temps);
	  }

	  FreeTextures();
	}
	catch(exception &e) {
	  FreeTextures();

	  if (strcmp(e.what(), "ExitThread")) {
	    wxMessageDialog d(prog, e.what(), "Oscape error");
	    d.ShowModal();
	  }

	  prog->Leave(0);
	  return;
	}
      }
    }

    prog->Leave(666);
  }
private:

  /* ---------------------------------------------------------------------------- */

  void CreateWorldspacePage(int wsv, struct sset *config) {
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

    wxGridSizer* gSizer4;
    gSizer4 = new wxGridSizer( 2, 2, 0, 0 );

    m_staticText40 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxT("Mesh-resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText40->Wrap( -1 );
    gSizer4->Add( m_staticText40, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxArrayString OSInstallLevel0MeshResChoices;
    OSInstallLevel0MeshRes = new wxChoice( m_scrolledWindow4, wID(LEVEL0_MRES), wxDefaultPosition, wxDefaultSize, OSInstallLevel0MeshResChoices, 0 );
    OSInstallLevel0MeshRes->SetSelection( 0 );
    gSizer4->Add( OSInstallLevel0MeshRes, 0, wxALL|wxEXPAND, 5 );

    m_staticText401 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxT("Texture-resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText401->Wrap( -1 );
    gSizer4->Add( m_staticText401, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxArrayString OSInstallLevel0TextResNChoices;
    OSInstallLevel0TextResN = new wxChoice( m_scrolledWindow4, wID(LEVEL0N_TRES), wxDefaultPosition, wxDefaultSize, OSInstallLevel0TextResNChoices, 0 );
    OSInstallLevel0TextResN->SetSelection( 0 );
    gSizer4->Add( OSInstallLevel0TextResN, 0, wxALL|wxEXPAND, 5 );

    m_staticText4014 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText4014->Wrap( -1 );
    gSizer4->Add( m_staticText4014, 0, wxALL, 5 );

    wxArrayString OSInstallLevel0TextResCChoices;
    OSInstallLevel0TextResC = new wxChoice( m_scrolledWindow4, wID(LEVEL0C_TRES), wxDefaultPosition, wxDefaultSize, OSInstallLevel0TextResCChoices, 0 );
    OSInstallLevel0TextResC->SetSelection( 0 );
    gSizer4->Add( OSInstallLevel0TextResC, 0, wxALL|wxEXPAND, 5 );

    OSLevel0->Add( gSizer4, 0, wxEXPAND, 5 );

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

    wxGridSizer* gSizer41;
    gSizer41 = new wxGridSizer( 2, 2, 0, 0 );

    m_staticText402 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxT("Mesh-resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText402->Wrap( -1 );
    gSizer41->Add( m_staticText402, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxArrayString OSInstallLevel1MeshResChoices;
    OSInstallLevel1MeshRes = new wxChoice( m_scrolledWindow4, wID(LEVEL1_MRES), wxDefaultPosition, wxDefaultSize, OSInstallLevel1MeshResChoices, 0 );
    OSInstallLevel1MeshRes->SetSelection( 0 );
    gSizer41->Add( OSInstallLevel1MeshRes, 0, wxALL|wxEXPAND, 5 );

    m_staticText4011 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxT("Texture-resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText4011->Wrap( -1 );
    gSizer41->Add( m_staticText4011, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxArrayString OSInstallLevel1TextResNChoices;
    OSInstallLevel1TextResN = new wxChoice( m_scrolledWindow4, wID(LEVEL1N_TRES), wxDefaultPosition, wxDefaultSize, OSInstallLevel1TextResNChoices, 0 );
    OSInstallLevel1TextResN->SetSelection( 0 );
    gSizer41->Add( OSInstallLevel1TextResN, 0, wxALL|wxEXPAND, 5 );

    m_staticText40111 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText40111->Wrap( -1 );
    gSizer41->Add( m_staticText40111, 0, wxALL, 5 );

    wxArrayString OSInstallLevel1TextResCChoices;
    OSInstallLevel1TextResC = new wxChoice( m_scrolledWindow4, wID(LEVEL1C_TRES), wxDefaultPosition, wxDefaultSize, OSInstallLevel1TextResCChoices, 0 );
    OSInstallLevel1TextResC->SetSelection( 0 );
    gSizer41->Add( OSInstallLevel1TextResC, 0, wxALL|wxEXPAND, 5 );

    OSLevel1->Add( gSizer41, 0, wxEXPAND, 5 );

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

    wxGridSizer* gSizer42;
    gSizer42 = new wxGridSizer( 2, 2, 0, 0 );

    m_staticText403 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxT("Mesh-resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText403->Wrap( -1 );
    gSizer42->Add( m_staticText403, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxArrayString OSInstallLevel2MeshResChoices;
    OSInstallLevel2MeshRes = new wxChoice( m_scrolledWindow4, wID(LEVEL2_MRES), wxDefaultPosition, wxDefaultSize, OSInstallLevel2MeshResChoices, 0 );
    OSInstallLevel2MeshRes->SetSelection( 0 );
    gSizer42->Add( OSInstallLevel2MeshRes, 0, wxALL|wxEXPAND, 5 );

    m_staticText4012 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxT("Texture-resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText4012->Wrap( -1 );
    gSizer42->Add( m_staticText4012, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxArrayString OSInstallLevel2TextResNChoices;
    OSInstallLevel2TextResN = new wxChoice( m_scrolledWindow4, wID(LEVEL2N_TRES), wxDefaultPosition, wxDefaultSize, OSInstallLevel2TextResNChoices, 0 );
    OSInstallLevel2TextResN->SetSelection( 0 );
    gSizer42->Add( OSInstallLevel2TextResN, 0, wxALL|wxEXPAND, 5 );

    m_staticText40121 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxT("Texture-resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText40121->Wrap( -1 );
    gSizer42->Add( m_staticText40121, 0, wxALL, 5 );

    wxArrayString OSInstallLevel2TextResCChoices;
    OSInstallLevel2TextResC = new wxChoice( m_scrolledWindow4, wID(LEVEL2C_TRES), wxDefaultPosition, wxDefaultSize, OSInstallLevel2TextResCChoices, 0 );
    OSInstallLevel2TextResC->SetSelection( 0 );
    gSizer42->Add( OSInstallLevel2TextResC, 0, wxALL|wxEXPAND, 5 );

    OSLevel2->Add( gSizer42, 0, wxEXPAND, 5 );

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

    wxGridSizer* gSizer43;
    gSizer43 = new wxGridSizer( 2, 2, 0, 0 );

    m_staticText404 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxT("Mesh-resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText404->Wrap( -1 );
    gSizer43->Add( m_staticText404, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxArrayString OSInstallLevel3MeshResChoices;
    OSInstallLevel3MeshRes = new wxChoice( m_scrolledWindow4, wID(LEVEL3_MRES), wxDefaultPosition, wxDefaultSize, OSInstallLevel3MeshResChoices, 0 );
    OSInstallLevel3MeshRes->SetSelection( 0 );
    gSizer43->Add( OSInstallLevel3MeshRes, 0, wxALL|wxEXPAND, 5 );

    m_staticText4013 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxT("Texture-resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText4013->Wrap( -1 );
    gSizer43->Add( m_staticText4013, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxArrayString OSInstallLevel3TextResNChoices;
    OSInstallLevel3TextResN = new wxChoice( m_scrolledWindow4, wID(LEVEL3N_TRES), wxDefaultPosition, wxDefaultSize, OSInstallLevel3TextResNChoices, 0 );
    OSInstallLevel3TextResN->SetSelection( 0 );
    gSizer43->Add( OSInstallLevel3TextResN, 0, wxALL|wxEXPAND, 5 );

    m_staticText40131 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText40131->Wrap( -1 );
    gSizer43->Add( m_staticText40131, 0, wxALL, 5 );

    wxArrayString OSInstallLevel3TextResCChoices;
    OSInstallLevel3TextResC = new wxChoice( m_scrolledWindow4, wID(LEVEL3C_TRES), wxDefaultPosition, wxDefaultSize, OSInstallLevel3TextResCChoices, 0 );
    OSInstallLevel3TextResC->SetSelection( 0 );
    gSizer43->Add( OSInstallLevel3TextResC, 0, wxALL|wxEXPAND, 5 );

    OSLevel3->Add( gSizer43, 0, wxEXPAND, 5 );

    bSizer52->Add( OSLevel3, 0, wxEXPAND, 5 );

    /* ----------------------------------------------------------------------- */
    m_scrolledWindow4->SetSizer( bSizer52 );
    m_scrolledWindow4->Layout();
    bSizer52->Fit( m_scrolledWindow4 );
    OSInstallWS->AddPage( m_scrolledWindow4, wxT(wsn), false );

    /* ####################################################################### */
    char buf[256]; DWORD bufl = 256;

    buf[0] = 0; RegGetValue(Settings, wsn.data(), "Install0", RRF_RT_REG_SZ, NULL, buf, &bufl);
    if (buf[0]) OSInstallLevel0->SetValue(buf[0] == '1'); bufl = 1023;
    buf[0] = 0; RegGetValue(Settings, wsn.data(), "Install1", RRF_RT_REG_SZ, NULL, buf, &bufl);
    if (buf[0]) OSInstallLevel1->SetValue(buf[0] == '1'); bufl = 1023;
    buf[0] = 0; RegGetValue(Settings, wsn.data(), "Install2", RRF_RT_REG_SZ, NULL, buf, &bufl);
    if (buf[0]) OSInstallLevel2->SetValue(buf[0] == '1'); bufl = 1023;
    buf[0] = 0; RegGetValue(Settings, wsn.data(), "Install3", RRF_RT_REG_SZ, NULL, buf, &bufl);
    if (buf[0]) OSInstallLevel3->SetValue(buf[0] == '1'); bufl = 1023;

    /* --------------------------------------------------------------------- */
    unsigned long int res_;
    int numN, numY;
    int addm = 0, fndn = 0, fndc = 0, divm = 1, texn = 0, texc = 0,
	ackm = 0, tckn = 0, tckc = 0, resm = 0, resn = 0, resc = 0;

    set<unsigned long int>::const_iterator walk = config->seed.begin();
    while (walk != config->seed.end()) {
      res_ = *walk;
      while (res_ > 1024) {
	if ((numN = config->mesh_nu_n[res_]) > 0) {
	  sprintf(buf, "1/%d: %d points, no UVs, %d tiles", divm, res_, numN); resm = max(resm, res_);

	  OSInstallLevel0MeshRes->Append(buf, (void *)-((int)res_));
	  if ((ackm == 0))
	    OSInstallLevel0MeshRes->SetSelection(OSInstallLevel0MeshRes->GetCount() - 1), ackm = 1;
	}

	if ((numY = config->mesh_yu_n[res_]) > 0) {
	  sprintf(buf, "1/%d: %d points, UVs, %d tiles", divm, res_, numY); resm = max(resm, res_);

	  OSInstallLevel0MeshRes->Append(buf, (void *) ((int)res_));
	  if ((ackm == 0))
	    OSInstallLevel0MeshRes->SetSelection(OSInstallLevel0MeshRes->GetCount() - 1), ackm = 1;
	}

	addm += (numN || numY) ? 1 : 0;
	res_ /= 2;
	divm *= 2;
      }

      walk++;
    }

    {
      res_ = 2048, fndn = 0;
      while (res_ > 64) {
	numN = config->textn_d[res_];
	numY = config->textn_p[res_];

	/* if there are resolution, all lower ones are available as well */
	if ((numN + numY + fndn) > 0) {
	  sprintf(buf, "%dx%d normals, %d tiles", res_, res_, max(numN, numY)); resn = max(resn, res_);

	  OSInstallLevel0TextResN->Append(buf, (void *) ((int)res_));
	  if ((tckn == 0))
	    OSInstallLevel0TextResN->SetSelection(OSInstallLevel0TextResN->GetCount() - 1), tckn = 1;
	}

	texn += (numN || numY || fndn) ? 1 : 0;
	fndn += (numN || numY) ? 1 : 0;
	res_ /= 2;
      }

      res_ = 4096, fndc = 0;
      while (res_ > 64) {
	numN = config->textc_d[res_];
	numY = config->textc_p[res_];

	/* if there are resolution, all lower ones are available as well */
	if ((numN + numY + fndc) > 0) {
	  sprintf(buf, "%dx%d colors, %d tiles", res_, res_, max(numN, numY)); resc = max(resc, res_);

	  OSInstallLevel0TextResC->Append(buf, (void *) ((int)res_));
	  if ((tckc == 0))
	    OSInstallLevel0TextResC->SetSelection(OSInstallLevel0TextResC->GetCount() - 1), tckc = 1;
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
      res_ = *walk;
      while (res_ > 1024) {
	if ((numN = config->mesh_nu_x[res_]) > 0) {
	  sprintf(buf, "1/%d: %d points, no UVs, %d tiles", divm, res_, numN);

	  OSInstallLevel1MeshRes->Append(buf, (void *)-((int)res_));
	  OSInstallLevel2MeshRes->Append(buf, (void *)-((int)res_));
	  OSInstallLevel3MeshRes->Append(buf, (void *)-((int)res_));

	  if ((ackm == 3) && (res_ < resm))
	    OSInstallLevel3MeshRes->SetSelection(OSInstallLevel3MeshRes->GetCount() - 1), ackm = 4;
	  if ((ackm == 2) && (res_ < resm))
	    OSInstallLevel2MeshRes->SetSelection(OSInstallLevel2MeshRes->GetCount() - 1), ackm = 3;
	  if ((ackm == 1) && (res_ < resm))
	    OSInstallLevel1MeshRes->SetSelection(OSInstallLevel1MeshRes->GetCount() - 1), ackm = 2;
	}

	if ((numY = config->mesh_yu_x[res_]) > 0) {
	  sprintf(buf, "1/%d: %d points, UVs, %d tiles", divm, res_, numY);

	  OSInstallLevel1MeshRes->Append(buf, (void *) ((int)res_));
	  OSInstallLevel2MeshRes->Append(buf, (void *) ((int)res_));
	  OSInstallLevel3MeshRes->Append(buf, (void *) ((int)res_));

	  if ((ackm == 3) && (res_ < resm))
	    OSInstallLevel3MeshRes->SetSelection(OSInstallLevel3MeshRes->GetCount() - 1), ackm = 4;
	  if ((ackm == 2) && (res_ < resm))
	    OSInstallLevel2MeshRes->SetSelection(OSInstallLevel2MeshRes->GetCount() - 1), ackm = 3;
	  if ((ackm == 1) && (res_ < resm))
	    OSInstallLevel1MeshRes->SetSelection(OSInstallLevel1MeshRes->GetCount() - 1), ackm = 2;
	}

	addm += (numN || numY) ? 1 : 0;
	res_ /= 2;
	divm *= 2;
      }

      walk++;
    }

    {
      res_ = 2048, fndn = 0;
      while (res_ > 64) {
	numN = config->textn_d[res_];
	numY = config->textn_p[res_];

	/* if there are resolution, all lower ones are available as well */
	if ((numN + numY + fndn) > 0) {
	  /* if there is a higher resolution and this one got none, we can calculate them */
	  if (resn && !(numN + numY))
	    sprintf(buf, "%dx%d normals, automatic", res_, res_);
	  else
	    sprintf(buf, "%dx%d normals, %d tiles", res_, res_, max(numN, numY));

	  OSInstallLevel1TextResN->Append(buf, (void *) ((int)res_));
	  OSInstallLevel2TextResN->Append(buf, (void *) ((int)res_));
	  OSInstallLevel3TextResN->Append(buf, (void *) ((int)res_));

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

      res_ = 4096, fndc = 0;
      while (res_ > 64) {
	numN = config->textc_d[res_];
	numY = config->textc_p[res_];

	/* if there are resolution, all lower ones are available as well */
	if ((numN + numY + fndc) > 0) {
	  /* if there is a higher resolution and this one got none, we can calculate them */
	  if (resc && !(numN + numY))
	    sprintf(buf, "%dx%d colors, automatic", res_, res_);
	  else
	    sprintf(buf, "%dx%d colors, %d tiles", res_, res_, max(numN, numY));

	  OSInstallLevel1TextResC->Append(buf, (void *) ((int)res_));
	  OSInstallLevel2TextResC->Append(buf, (void *) ((int)res_));
	  OSInstallLevel3TextResC->Append(buf, (void *) ((int)res_));

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

    /* --------------------------------------------------------------------- */
    {
      if (addm <= 1) {
	if (!texn && !texc)
	           OSLevel1->ShowItems(FALSE),
		   OSInstallLevel1->SetValue(FALSE);
	else       OSInstallLevel1MeshRes ->Enable(FALSE); }
      if (!texn)   OSInstallLevel1TextResN->Enable(FALSE);
      if (!texc)   OSInstallLevel1TextResC->Enable(FALSE);

      if (addm <= 2) {
	if (!texn && !texc)
		   OSLevel2->ShowItems(FALSE),
		   OSInstallLevel2->SetValue(FALSE);
	else       OSInstallLevel2MeshRes ->Enable(FALSE); }
      if (!texn)   OSInstallLevel2TextResN->Enable(FALSE);
      if (!texc)   OSInstallLevel2TextResC->Enable(FALSE);

      if (addm <= 3) {
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

  void ChangeBaseDirIn(wxFileDirPickerEvent& event) {
    ChangeBaseDirIn(event.GetPath());
  }

  void ChangeBaseDirIn(wxString ph) {
    OSInstallWS->Hide();
    OSHeightfieldFirst3->SetLabel("Select a directory first");
    OSHeightfieldFirst3->Show();
    OSHeightfieldFirst3->GetParent()->Layout();

    if (ph.IsNull())
      return;
    {
      const char *BO = ph.data();
      OSBaseDirIn->SetPath(BO);
      RegSetKeyValue(Settings, NULL, "Base directory In", RRF_RT_REG_SZ, BO, (DWORD)strlen(BO) + 1);
    }

    OSStatusBar->SetStatusText(wxT("Skimming directory ..."), 0);

    map<long, struct sset> wsset;

    // worldspace, mesh-res, texture-res

    /* look for all the directories */
    char temp[1024], base[1024];
    sprintf(base, "%s\\*", ph.data(), 1024);
    long res; int ws, cx, xy, rs;

    HANDLE IFiles; WIN32_FIND_DATA IFound;
    HANDLE RFiles; WIN32_FIND_DATA RFound;
    if ((IFiles = FindFirstFileEx(base, FindExInfoBasic, &IFound, FindExSearchNameMatch, NULL, 0)) != INVALID_HANDLE_VALUE) {
      do {
	const char *file = IFound.cFileName;

	if ((file == stristr(file, "LOD-"))) {
	  if (sscanf(file + 4, "%d", &res) == 1) {
	    sprintf(temp, "%s\\LOD-%d\\*.pts", ph.data(), res);
	    if ((RFiles = FindFirstFileEx(temp, FindExInfoBasic, &RFound, FindExSearchNameMatch, NULL, 0)) != INVALID_HANDLE_VALUE) {
	      do {
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
		if (sscanf(subfile, "%d.%d.%d.%d", &ws, &cx, &xy, &rs) == 4) {
		  struct sset *match = &wsset[ws];
		  if (stristr(subfile, ".x"  )) match->mesh_yu_x[res]++;
		  if (stristr(subfile, ".nif")) match->mesh_yu_n[res]++;
		}
	      } while (FindNextFile(RFiles, &RFound));

	      FindClose(RFiles);
	    }

	    sprintf(temp, "%s\\LOD-%d\\UVoff\\*", ph.data(), res);
	    if ((RFiles = FindFirstFileEx(temp, FindExInfoBasic, &RFound, FindExSearchNameMatch, NULL, 0)) != INVALID_HANDLE_VALUE) {
	      do {
		const char *subfile = RFound.cFileName;
		if (sscanf(subfile, "%d.%d.%d.%d", &ws, &cx, &xy, &rs) == 4) {
		  struct sset *match = &wsset[ws];
		  if (stristr(subfile, ".x"  )) match->mesh_nu_x[res]++;
		  if (stristr(subfile, ".nif")) match->mesh_nu_n[res]++;
		}
	      } while (FindNextFile(RFiles, &RFound));

	      FindClose(RFiles);
	    }

//	    wsset.push_back(res);
	  }
	}

	if ((file == stristr(file, "TEX-"))) {
	  if (sscanf(file + 4, "%d", &res) == 1) {
	    sprintf(temp, "%s\\TEX-%d\\*", ph.data(), res);
	    if ((RFiles = FindFirstFileEx(temp, FindExInfoBasic, &RFound, FindExSearchNameMatch, NULL, 0)) != INVALID_HANDLE_VALUE) {
	      do {
		const char *subfile = RFound.cFileName;
		if (sscanf(subfile, "%d.%d.%d.%d", &ws, &cx, &xy, &rs) == 4) {
		  struct sset *match = &wsset[ws];

		  /* a bit weak, but works */
		  if (stristr(subfile, ".dds")) {
		    if (stristr(subfile, "_fn"))
		      match->textn_d[res]++;
		    else
		      match->textc_d[res]++;
		  }

		  /* a bit weak, but works */
		  if (stristr(subfile, ".png")) {
		    if (stristr(subfile, "_fn"))
		      match->textn_p[res]++;
		    else
		      match->textc_p[res]++;
		  }
		}
	      } while (FindNextFile(RFiles, &RFound));

	      FindClose(RFiles);
	    }

//	    wsset.push_back(res);
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

  void ChangePlugoutDir(wxFileDirPickerEvent& event) {
    ChangePlugoutDir(event.GetPath());
  }

  void ChangePlugoutDir(wxString ph) {
    OSHeightfieldInstall->Enable(FALSE);

    if (ph.IsNull())
      return;

    OSHeightfieldInstall->Enable(TRUE);
  }

  void HeightfieldInstall(wxCommandEvent& event) {
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

public:
  int installdone;

  bool SynchronizeInstall(int wsv, int res, const char *subdir, const char *lid) {
    wxString ph = OSBaseDirIn->GetPath();
    HANDLE RFiles; WIN32_FIND_DATA RFound;
    vector< pair<string, string> > copy;
    vector< pair<string, string> >::const_iterator walk;
    int dne = 0; 
    char tpc[256], trgm[261];
    char base[1024], temp[1024];

    /* --------------------------------------------------------------------- */
    sprintf(tpc, "Level %s meshes:", lid);
    prog->InitProgress(NULL, 0, tpc, 0.0, installdone++, 1);

    strcpy(trgm, OSPlugoutDir->GetPath().data());
    strcat(trgm, "\\Meshes\\Landscape\\LOD");
    if (subdir)
      strcat(trgm, "\\"),
      strcat(trgm, subdir);
    CreateDirectoryRecursive(trgm);
    strcat(trgm, "\\");

    /* look for all the directories */
    sprintf(temp, "%s\\LOD-%d\\%s\\"          , ph.data(), abs(res), res > 0 ? "UVon" : "UVoff"     );
    sprintf(base, "%s\\LOD-%d\\%s\\%02d.*.nif", ph.data(), abs(res), res > 0 ? "UVon" : "UVoff", wsv);

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

  bool SynchronizeInstall(int wsv, bool n, int res, const char *subdir, const char *lid) {
    wxString ph = OSBaseDirIn->GetPath();
    HANDLE RFiles; WIN32_FIND_DATA RFound;
    vector< pair<string, string> > copy;
    vector< pair<string, string> >::const_iterator walk;
    int dne = 0; 
    char tpc[256], trgt[261];
    char base[1024], temp[1024];

    /* --------------------------------------------------------------------- */
    sprintf(tpc, "Level %s %s textures:", lid, n ? "normal" : "color");
    prog->InitProgress(NULL, 0, tpc, 0.0, installdone++, 1);

    strcpy(trgt, OSPlugoutDir->GetPath().data());
    strcat(trgt, "\\Textures\\LandscapeLOD\\Generated");
    if (subdir)
      strcat(trgt, "\\"),
      strcat(trgt, subdir);
    CreateDirectoryRecursive(trgt);
    strcat(trgt, "\\");

    /* look for all the directories */
    sprintf(temp, "%s\\TEX-%d\\"          , ph.data(), abs(res));
    sprintf(base, "%s\\TEX-%d\\%02d.*.dds", ph.data(), abs(res), wsv);

    copy.clear();
    if ((RFiles = FindFirstFileEx(base, FindExInfoBasic, &RFound, FindExSearchNameMatch, NULL, 0)) != INVALID_HANDLE_VALUE) {
      string in = temp;
      string ou = trgt;

      do {
	if (( n &&  stristr(RFound.cFileName, "_fn")) ||
	    (!n && !stristr(RFound.cFileName, "_fn")))
	  copy.push_back(pair<string, string>(
	    in + RFound.cFileName,
	    ou + RFound.cFileName
	  ));
      } while (FindNextFile(RFiles, &RFound));

      FindClose(RFiles);
    }

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
      int levels = 1; {
	int ww = res;
	int hh = res;
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
	sprintf(temp, "%s\\TEX-%d\\", ph.data(), abs(tcnv));
	sprintf(base, "%s\\TEX-%d\\%02d.*.ppm", ph.data(), abs(tcnv), wsv);

	copy.clear();
	if ((RFiles = FindFirstFileEx(base, FindExInfoBasic, &RFound, FindExSearchNameMatch, NULL, 0)) != INVALID_HANDLE_VALUE) {
	  string in = temp;
	  string ou = trgt;

	  do {
	    if (( n &&  stristr(RFound.cFileName, "_fn")) ||
		(!n && !stristr(RFound.cFileName, "_fn")))
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
	sprintf(temp, "%s\\TEX-%d\\", ph.data(), abs(tcnv));
	sprintf(base, "%s\\TEX-%d\\%02d.*.png", ph.data(), abs(tcnv), wsv);

	copy.clear();
	if ((RFiles = FindFirstFileEx(base, FindExInfoBasic, &RFound, FindExSearchNameMatch, NULL, 0)) != INVALID_HANDLE_VALUE) {
	  string in = temp;
	  string ou = trgt;

	  do {
	    if (( n &&  stristr(RFound.cFileName, "_fn")) ||
		(!n && !stristr(RFound.cFileName, "_fn")))
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
	sprintf(temp, "%s\\TEX-%d\\", ph.data(), abs(tcnv));
	sprintf(base, "%s\\TEX-%d\\%02d.*.dds", ph.data(), abs(tcnv), wsv);

	copy.clear();
	if ((RFiles = FindFirstFileEx(base, FindExInfoBasic, &RFound, FindExSearchNameMatch, NULL, 0)) != INVALID_HANDLE_VALUE) {
	  string in = temp;
	  string ou = trgt;

	  do {
	    if (( n &&  stristr(RFound.cFileName, "_fn")) ||
	        (!n && !stristr(RFound.cFileName, "_fn")))
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

	    if ((ret = D3DXCreateTexture(
	      pD3DDevice,
	      res, res, 0,
	      0, based.Format, D3DPOOL_SYSTEMMEM, &trns
	    )) != D3D_OK)
	      goto terminal_error;

	    int baselvl = 0;
	    int trnslvl = 0;
	    while (based.Width > (UINT)res) {
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
	  /* do the recalculation */
	  else if (stristr(walk->first.data(), ".ppm") ||
		   stristr(walk->first.data(), ".png")) {
	    D3DSURFACE_DESC based;
	    base->GetLevelDesc(0, &based);

	    if ((ret = D3DXCreateTexture(
		pD3DDevice,
		res, res, 0,
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
	      if (!TextureCompressXYZ(&base, 0))
		goto terminal_error;
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

  bool SynchronizeInstall(int wsv, int mres, int tres, int cres, const char *subdir, const char *lid) {
    installdone -= 2;

    if (!SynchronizeInstall(wsv, mres, subdir, lid))
      return false;
    if (!SynchronizeInstall(wsv, true, tres, subdir, lid))
      return false;
    if (!SynchronizeInstall(wsv, false, cres, subdir, lid))
      return false;

    return true;
  }

  void HeightfieldInstall() {
    size_t pages = OSInstallWS->GetPageCount();
    installdone = 0;

    prog->StartProgress((int)(pages * 12));
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

	  prog->InitProgress(tpc, 0, "Level zero:", 0.0, installdone, 1), installdone += 3;
	  if (inst0)
	    if (!SynchronizeInstall(wsv, mres0, tres0, cres0, NULL, "zero"))
	      break;

	  prog->InitProgress(tpc, 0, "Level one:", 0.0, installdone, 1), installdone += 3;
	  if (inst1)
	    if (!SynchronizeInstall(wsv, mres1, tres1, cres1, "farnear", "one"))
	      break;

	  prog->InitProgress(tpc, 0, "Level two:", 0.0, installdone, 1), installdone += 3;
	  if (inst2)
	    if (!SynchronizeInstall(wsv, mres2, tres2, cres2, "farfar", "two"))
	      break;

	  prog->InitProgress(tpc, 0, "Level three:", 0.0, installdone, 1), installdone += 3;
	  if (inst3)
	    if (!SynchronizeInstall(wsv, mres3, tres3, cres3, "farinf", "three"))
	      break;

	  FreeTextures();
	}
	catch(exception &e) {
	  FreeTextures();

	  if (strcmp(e.what(), "ExitThread")) {
	    wxMessageDialog d(prog, e.what(), "Oscape error");
	    d.ShowModal();
	  }

	  prog->Leave(0);
	  return;
	}

	RegSetKeyValue(Settings, wsn.data(), "Install0", RRF_RT_REG_SZ, inst0 ? "1" : "0", 2);
	RegSetKeyValue(Settings, wsn.data(), "Install1", RRF_RT_REG_SZ, inst1 ? "1" : "0", 2);
	RegSetKeyValue(Settings, wsn.data(), "Install2", RRF_RT_REG_SZ, inst2 ? "1" : "0", 2);
	RegSetKeyValue(Settings, wsn.data(), "Install3", RRF_RT_REG_SZ, inst3 ? "1" : "0", 2);
      }
      else
	prog->InitProgress(tpc, 0, "Skipping:", 0.0, installdone += 8, 1);
    }

    prog->Leave(666);
  }

  void SetStatus(const char *status) {
    OSStatusBar->SetStatusText(status);
  }
private:

  void ChangeDefaults(wxCommandEvent& event) {
    if (OSDefaults->FindChildItem(wxID_DEFV, NULL)->IsChecked()) {
      OSMeshUVs->SetValue(true);
      OSMeshNIF->SetValue(true);
      OSMeshDX->SetValue(false);
      OSRes1->SetValue(true ); OSRes2->SetValue(false); OSRes3->SetValue(false); OSRes4->SetValue(false);
      OSRes5->SetValue(false); OSRes6->SetValue(false); OSRes7->SetValue(false); OSRes8->SetValue(false);

      RegSetKeyValue(Settings, NULL, "Defaults", RRF_RT_REG_SZ, "0", 2);
    }
    else if (OSDefaults->FindChildItem(wxID_DEFT, NULL)->IsChecked()) {
      OSDefaults->FindChildItem(wxID_DEFT, NULL)->Check(true);
      OSMeshUVs->SetValue(true);
      OSMeshNIF->SetValue(true);
      OSMeshDX->SetValue(false);
      OSRes1->SetValue(true ); OSRes2->SetValue(false); OSRes3->SetValue(false); OSRes4->SetValue(false);
      OSRes5->SetValue(false); OSRes6->SetValue(false); OSRes7->SetValue(false); OSRes8->SetValue(false);

      RegSetKeyValue(Settings, NULL, "Defaults", RRF_RT_REG_SZ, "1", 2);
    }
    else if (OSDefaults->FindChildItem(wxID_DEFL, NULL)->IsChecked()) {
      OSDefaults->FindChildItem(wxID_DEFL, NULL)->Check(true);
      OSMeshUVs->SetValue(false);
      OSMeshNIF->SetValue(true);
      OSMeshDX->SetValue(true);
      OSRes1->SetValue(true); OSRes2->SetValue(true); OSRes3->SetValue(true); OSRes4->SetValue(true);
      OSRes5->SetValue(true); OSRes6->SetValue(true); OSRes7->SetValue(true); OSRes8->SetValue(true);

      RegSetKeyValue(Settings, NULL, "Defaults", RRF_RT_REG_SZ, "2", 2);
    }
  }

public:
  OscapeGUI::OscapeGUI(const wxString& title)
    : wxOscape(NULL, wxID_ANY, title) {
    gui = this;

    OSHeightfieldInfos->Connect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler( OscapeGUI::ChangeHeightfieldInfos ), NULL, this );

    Settings = 0;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Bethesda Softworks\\Oscape", 0, KEY_READ | KEY_WRITE | KEY_WOW64_32KEY, &Settings) == ERROR_SUCCESS) {
    }
    else if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, "Software\\Bethesda Softworks\\Oscape", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE | KEY_WOW64_32KEY, NULL, &Settings, NULL) == ERROR_SUCCESS) {
    }

    ResetHPluginDir(true);
    ResetHPluginList();
    LoadHPluginList();
//  WorldspacesFromPlugins();
//  ResetHButtons();

    OSFilePoints1->SetPath("");
    OSFilePoints2->SetPath("");

    char TS[1024]; DWORD TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, NULL, "Last worldspace", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSWorldspace->SetValue(TS); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, NULL, "Heightfield Out", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSFileHeightfieldOut->SetPath(TS); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, NULL, "Heightfield In", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSFileHeightfieldIn1->SetPath(TS); TSL = 1023;
    if (TS[0]) OSFileHeightfieldIn2->SetPath(TS); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, NULL, "Points In", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSFilePoints1->SetPath(TS); TSL = 1023;
    if (TS[0]) OSFilePoints2->SetPath(TS); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, NULL, "Base directory Out", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSBaseDirOut1->SetPath(TS); TSL = 1023;
    if (TS[0]) OSBaseDirOut2->SetPath(TS); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, NULL, "Base directory In", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSBaseDirIn->SetPath(TS); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, NULL, "Autosave", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSPluginAutosave->SetValue(TS[0] == '1'); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, NULL, "Do Meshes", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSMeshes->SetValue(TS[0] == '1'); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, NULL, "Do Normals", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSNormals->SetValue(TS[0] == '1'); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, NULL, "Do Colors", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSColors->SetValue(TS[0] == '1'); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, NULL, "Do Heightmap", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSHeightmap->SetValue(TS[0] == '1'); TSL = 1023;
    TS[0] = 0; RegGetValue(Settings, NULL, "Defaults", RRF_RT_REG_SZ, NULL, TS, &TSL);
    switch (TS[0]) {
      default:
      case '0':
	OSDefaults->FindChildItem(wxID_DEFV, NULL)->Check(true);
	OSMeshUVs->SetValue(true);
	OSMeshNIF->SetValue(true);
	OSMeshDX->SetValue(false);
	OSRes1->SetValue(true ); OSRes2->SetValue(false); OSRes3->SetValue(false); OSRes4->SetValue(false);
	OSRes5->SetValue(false); OSRes6->SetValue(false); OSRes7->SetValue(false); OSRes8->SetValue(false);
	break;
      case '1':
	OSDefaults->FindChildItem(wxID_DEFT, NULL)->Check(true);
	OSMeshUVs->SetValue(true);
	OSMeshNIF->SetValue(true);
	OSMeshDX->SetValue(false);
	OSRes1->SetValue(true ); OSRes2->SetValue(false); OSRes3->SetValue(false); OSRes4->SetValue(false);
	OSRes5->SetValue(false); OSRes6->SetValue(false); OSRes7->SetValue(false); OSRes8->SetValue(false);
	break;
      case '2':
	OSDefaults->FindChildItem(wxID_DEFL, NULL)->Check(true);
	OSMeshUVs->SetValue(false);
	OSMeshNIF->SetValue(true);
	OSMeshDX->SetValue(true);
	OSRes1->SetValue(true); OSRes2->SetValue(true); OSRes3->SetValue(true); OSRes4->SetValue(true);
	OSRes5->SetValue(true); OSRes6->SetValue(true); OSRes7->SetValue(true); OSRes8->SetValue(true);
	break;
    }
    TS[0] = 0; RegGetValue(Settings, NULL, "Orientation", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSOrientation->SetValue(TS[0] == 'H'); TSL = 1023;
    if (!TS[0]) OSOrientation->SetValue(!OSDefaults->FindChildItem(wxID_DEFV, NULL)->IsChecked());
    TS[0] = 0; RegGetValue(Settings, NULL, "Target", RRF_RT_REG_SZ, NULL, TS, &TSL);
    if (TS[0]) OSTarget->SetValue(TS); TSL = 1023;
    if (!TS[0]) OSTarget->SetValue(OSDefaults->FindChildItem(wxID_DEFV, NULL)->IsChecked() ? "155000" : "450000");

    ChangeMeshes();
    ChangeNormals();
    ChangeColors();
    ChangeHeightmap();

    wxString
    ph = OSFileHeightfieldIn1->GetPath();
    if (!ph.IsNull()) {
      ChangeHeightfieldIn1(ph);
      ChangeHeightfieldIn2(ph);
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
};

// ----------------------------------------------------------------------------
#include "../AMDTootle/include/tootlelib.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class OscapeApp : public wxApp
{
  OscapeGUI *frame;

public:
  // override base class virtuals
  // ----------------------------

  // this one is called on application startup and is a good place for the app
  // initialization (doing it here and not in the ctor allows to have an error
  // return: if OnInit() returns false, the application terminates)
  virtual bool OnInit();
  virtual int OnExit();
};

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. OscapeApp and
// not wxApp)
IMPLEMENT_APP_NO_MAIN(OscapeApp)

extern "C" int WINAPI WinMain(HINSTANCE hInstance,
			      HINSTANCE hPrevInstance,
			      wxCmdLineArgType lpCmdLine,
			      int nCmdShow)
{
  // initialize Tootle
  if (TootleInit() != TOOTLE_OK)
    return 0;
  if (!TextureInit())
    return 0;

  int ret = wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

  // clean up tootle
  TootleCleanup();
  TextureCleanup();

  return ret;
}

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool OscapeApp::OnInit()
{
  // call the base class initialization method, currently it only parses a
  // few common command-line options but it could be do more in the future
  if ( !wxApp::OnInit() )
    return false;

  // create the main application window
  frame = new OscapeGUI(_T("Oscape"));

  // and show it (the frames, unlike simple controls, are not shown when
  // created initially)
  frame->Show(true);

  // success: wxApp::OnRun() will be called which will enter the main message
  // loop and the application will run. If we returned false here, the
  // application would exit immediately.
  return true;
}

int OscapeApp::OnExit()
{
  return wxApp::OnExit();
}

/* --------------------------------------------------------------
 */

void InitProgress(int rng, Real err) {
  if (prg)
    prg->InitProgress(rng, err);
}

void SetProgress(int dne, Real err) {
  if (prg)
    prg->SetProgress(dne, err);
}

void InitProgress(int rng) {
  if (prg)
    prg->InitProgress(rng);
}

void SetProgress(int dne) {
  if (prg)
    prg->SetProgress(dne);
}

void SetStatus(const char *status) {
  if (gui)
    gui->SetStatus(status);
}

void SetTopic(const char *topic) {
  if (prg)
    prg->SetTopic(topic);
}

void SetTopic(const char *topic, int a, int b) {
  if (prg) {
    char btopic[256]; sprintf(btopic, topic, a, b);

    prg->SetTopic(btopic);
  }
}

bool SetTopic(const unsigned long p, const unsigned long t, const char *topic) {
  if (prg) {
    char btopic[256]; sprintf(btopic, "Reading plugin \"%s\":", topic);
    static unsigned long lastt = 0;

    prg->SetTopic(btopic);
    if (lastt != t)
      prg->InitProgress(lastt = t);
    prg->SetProgress(p);
  }

  return true;
}

void InitProgress(int rng, int srng) {
  if (prg)
    prg->InitProgress(rng, srng);
}

void SetProgress(int dne, int sdne) {
  if (prg)
    prg->SetProgress(dne, sdne);
}

void PollProgress() {
  if (prg)
    prg->PollProgress();
}

bool RequestFeedback(const char *question) {
  if (prg)
    return prg->RequestFeedback(question);
  return true;
}

DWORD __stdcall HeightfieldExtract(LPVOID lp) {
  if (gui)
    gui->HeightfieldExtract();
  return 0;
}

DWORD __stdcall HeightfieldGenerate(LPVOID lp) {
  if (gui)
    gui->HeightfieldGenerate();
  return 0;
}

DWORD __stdcall HeightfieldInstall(LPVOID lp) {
  if (gui)
    gui->HeightfieldInstall();
  return 0;
}

bool IsOlder(const char *name, __int64 tschk) {
  if (!tschk)
    return true;

  WIN32_FILE_ATTRIBUTE_DATA infow; BOOL wex =
  GetFileAttributesEx(name, GetFileExInfoStandard, &infow);

  /* newer seed-resolution */
  if (wex && (*((__int64 *)&infow.ftLastWriteTime) > tschk))
    return false;

  return true;
}
