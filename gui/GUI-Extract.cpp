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

  /* ---------------------------------------------------------------------------- */
  DWORD __stdcall HeightfieldExtract(LPVOID lp) {
    if (gui)
      gui->HeightfieldExtract();
    return 0;
  }

  /* ---------------------------------------------------------------------------- */
  void OscapeGUI::LoadWSpaceList() {
    wspaces.clear();
    wspacef.clear();

    HKEY WSs = 0;
    if (RegOpenKey(Settings, GetGameKey("Worldspaces"), &WSs) == ERROR_SUCCESS) {
      char p[256]; DWORD pl = 64; DWORD pos = 0;
      char s[256]; DWORD sl = 32;

      while (RegEnumValue(WSs, pos, p, &pl, NULL, NULL, (LPBYTE)s, &sl) != ERROR_NO_MORE_ITEMS) {
	int fid = 0;
	if (sscanf(s, "%d", &fid) == 1)
	  wspacef[fid] = wspaces.insert(p).first;

	p[0] = '\0'; pl = 64; pos++;
	s[0] = '\0'; sl = 32;
      }

      RegCloseKey(WSs);
    }
    else if (OSGame->FindItem(wxID_OBLIVON)->IsChecked()) {
      wspacef[60] = wspaces.insert("Tamriel").first;
      wspacef[40728] = wspaces.insert("SEWorld").first;
    }
    else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked()) {
      wspacef[60] = wspaces.insert("Tamriel").first;
    }
  }

  void OscapeGUI::SaveWSpaceList() {
    map<int, worldset::iterator >::iterator srch = wspacef.begin();
    while (srch != wspacef.end()) {
      char s[256];
      const char *p = (srch->second)->data();
      sprintf(s, "%d", srch->first);

      RegSetKeyValue(Settings, GetGameKey("Worldspaces"), p, RRF_RT_REG_SZ, s, (DWORD)strlen(s) + 1);

      srch++;
    }
  }

  /* ---------------------------------------------------------------------------- */
  void OscapeGUI::ResetHPluginDir(bool init) {
    IPath[0] = 0; DWORD IPL = 1023; HKEY IDir;

    if (init)
      RegGetValue(Settings, GetGameKey(), "Plugin Directory", RRF_RT_REG_SZ, NULL, &IPath, &IPL), IPL = 1023;

    if (!IPath[0]) {
      const char *game = NULL;
      if (OSGame->FindItem(wxID_OBLIVON)->IsChecked())
	game = "Software\\Bethesda Softworks\\Oblivion";
      else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked())
	game = "Software\\Bethesda Softworks\\Skyrim";

      if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, game, 0, KEY_READ | KEY_WOW64_32KEY, &IDir) == ERROR_SUCCESS) {
	RegGetValue(IDir, NULL, "Installed Path", RRF_RT_REG_SZ, NULL, &IPath, &IPL);
	RegCloseKey(IDir);

	strcat(IPath, "Data");
//	strcat(IPath, "\\");
      }
    }

    size_t ILen = strlen(IPath) - 1;
    while (IPath[ILen] == '\\')
      IPath[ILen--] = '\0';

    OSPluginDir->SetPath(IPath);
    OSPlugoutDir->SetPath(IPath);
  }

  void OscapeGUI::ResetHPluginList() {
    actives = 0;
    plugins.clear();
    psorted.clear();

    LoadWSpaceList();

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
      /**/ if (OSGame->FindItem(wxID_OBLIVON)->IsChecked())
	strcat(PBuffer, "\\Oblivion\\Plugins.txt");
      else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked())
	strcat(PBuffer, "\\Skyrim\\Plugins.txt");

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

  void OscapeGUI::ClearHPluginList() {
    int num = OSPluginList->GetCount();
    for (int n = 0; n < num; n++) {
      OSPluginList->Check(n, FALSE);
    }
  }

  void OscapeGUI::LoadHPluginList() {
    int num = OSPluginList->GetCount();
    for (int n = 0; n < num; n++) {
      const char *p = OSPluginList->GetItem(n)->GetName().data();
      char s[32] = ""; DWORD sl = 32;

      RegGetValue(Settings, GetGameKey("Plugins"), p, RRF_RT_REG_SZ, NULL, s, &sl);

      if (s[0] != '\0')
	OSPluginList->Check(n, s[0] == '1');
    }
  }

  void OscapeGUI::SaveHPluginList() {
    int num = OSPluginList->GetCount();
    for (int n = 0; n < num; n++) {
      const char *p = OSPluginList->GetItem(n)->GetName().data();
      const char *s = "0";
      if (OSPluginList->IsChecked(n))
	s = "1";

      RegSetKeyValue(Settings, GetGameKey("Plugins"), p, RRF_RT_REG_SZ, s, (DWORD)strlen(s) + 1);
    }
  }

  /* ---------------------------------------------------------------------------- */
  void OscapeGUI::ResetPluginList(wxCommandEvent& event) {
    ResetHPluginList();
  }

  void OscapeGUI::ClearPluginList(wxCommandEvent& event) {
    ClearHPluginList();
  }

  void OscapeGUI::LoadPluginList(wxCommandEvent& event) {
    LoadHPluginList();
  }

  void OscapeGUI::SavePluginList(wxCommandEvent& event) {
    SaveHPluginList();
  }

  void OscapeGUI::ChangeAutosave(wxCommandEvent& event) {
    RegSetKeyValue(Settings, GetGameKey(), "Autosave", RRF_RT_REG_SZ, OSPluginAutosave->GetValue() ? "1" : "0", 2);
  }

  /* ---------------------------------------------------------------------------- */
  void OscapeGUI::WorldspacesFromPlugins(wxCommandEvent& event) {
//  wspaces.clear();
//  wspacef.clear();

    wxBusyCursor wait;
    int num = OSPluginList->GetCount();
    for (int n = 0; n < num; n++) {
      if (OSPluginList->IsChecked(n)) {
	const char *p = OSPluginList->GetItem(n)->GetName().data();

	if (strstr(p, ".esm") || strstr(p, ".esp")) {
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
	    if (*((DWORD *)(mem +  0)) == wrld) {
	      if (*((DWORD *)(mem + 20)) == edid) {
		DWORD fid = *((DWORD *)(mem + 12));
		string wdn(mem + 26);

		wspacef[fid] = wspaces.insert(wdn).first;
	      }

	      if (*((DWORD *)(mem + 24)) == edid) {
		DWORD fid = *((DWORD *)(mem + 12));
		string wdn(mem + 30);

		wspacef[fid] = wspaces.insert(wdn).first;
	      }
	    }

	    mem++;
	  }

	  UnmapViewOfFile(mem);
	  CloseHandle(mh);
	  CloseHandle(oh);
	}
      }
    }

    SaveWSpaceList();

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
  void OscapeGUI::ChangePluginDir(wxFileDirPickerEvent& event) {
    strcpy(IPath, OSPluginDir->GetTextCtrlValue().data());
//  strcat(IPath, "\\");

    ResetHPluginList();
    ResetHButtons();

    RegSetKeyValue(Settings, GetGameKey(), "Plugin Directory", RRF_RT_REG_SZ, IPath, (DWORD)strlen(IPath) + 1);
  }

  /* ---------------------------------------------------------------------------- */
  void OscapeGUI::ChangeActivePlugins(wxCommandEvent& event) {
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
  void OscapeGUI::ChangeWorldspace(wxCommandEvent& event) {
    ResetHButtons();

    wxString ph = event.GetString();
    if (!ph.IsNull()) {
      const char *WS = ph.data();
      RegSetKeyValue(Settings, GetGameKey(), "Last worldspace", RRF_RT_REG_SZ, WS, (DWORD)strlen(WS) + 1);
    }

    if (formID) {
      int wsel = GetWorldspace(ph);
      if (wsel)
	formID->SetValueFromInt(wsel);
    }
  }

  /* ---------------------------------------------------------------------------- */
  void OscapeGUI::ChangeHeightfieldOut(wxFileDirPickerEvent& event) {
    ResetHButtons();

    wxString ph = event.GetPath();
    if (!ph.IsNull()) {
      const char *HO = ph.data();
      RegSetKeyValue(Settings, GetGameKey(), "Heightfield Out", RRF_RT_REG_SZ, HO, (DWORD)strlen(HO) + 1);
    }
  }

  /* ---------------------------------------------------------------------------- */
  void OscapeGUI::HeightfieldExtract(wxCommandEvent& event) {
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
      RegSetKeyValue(Settings, GetGameKey(), "Heightfield In", RRF_RT_REG_SZ, HI, (DWORD)strlen(HI) + 1);
    }

    ChangeHeightfieldIn1(ph);
    ChangeHeightfieldIn2(ph);
    ChangeBaseDirOut1(ph);
    ChangeBaseDirOut2(ph);
  }

  void OscapeGUI::HeightfieldExtract() {
    /* move the result-file around */
    wxString ph = OSFileHeightfieldOut->GetPath();
    wxFileName fnn(ph); fnn.ClearExt(); fnn.SetExt("nrm");
    wxFileName fnh(ph); fnh.ClearExt(); fnh.SetExt("raw");
    wxFileName fnw(ph); fnw.ClearExt(); fnw.SetExt("water");
    wxFileName fnx(ph); fnx.ClearExt(); fnx.SetExt("land");
    wxFileName fnm(ph); fnm.ClearExt(); fnm.SetExt("fmap");

    wedata = OSPluginDir->GetPath(); weoffs = 0;
    weoutn = fnn.GetFullPath(); calcn = false;
    weouth = fnh.GetFullPath(); calch = OSCalcHeight->GetValue();
    weoutw = fnw.GetFullPath(); calcw = OSCalcHeight->GetValue();
    weoutx = fnx.GetFullPath(); calcx = OSCalcColor->GetValue();
    weoutm = fnm.GetFullPath(); calcm = OSCalcImportance->GetValue();
    wename = OSWorldspace->GetValue(); weid = GetWorldspace(wename);

    texsample = 1;
    srfsample = 1;

    /**/ if (OSTSampling->FindItem(wxID_TS1, NULL)->IsChecked())
      texsample = 1;
    else if (OSTSampling->FindItem(wxID_TS2, NULL)->IsChecked())
      texsample = 2;
    else if (OSTSampling->FindItem(wxID_TS4, NULL)->IsChecked())
      texsample = 4;

    /**/ if (OSSSampling->FindItem(wxID_SS1, NULL)->IsChecked())
      srfsample = 1;
    else if (OSSSampling->FindItem(wxID_SS2, NULL)->IsChecked())
      srfsample = 2;
    else if (OSSSampling->FindItem(wxID_SS4, NULL)->IsChecked())
      srfsample = 4;

    Collection *col = NULL;
    /**/ if (OSGame->FindItem(wxID_OBLIVON)->IsChecked())
      col = CreateCollection(IPath, eIsOblivion), weoffs = 0;
    else if (OSGame->FindItem(wxID_SKYRIM)->IsChecked())
      col = CreateCollection(IPath, eIsSkyrim), weoffs = 18040 - 14000 + 1566 - 2000;

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
    col->AddRecordFilter(REV32(WRLD));
    col->AddRecordFilter(REV32(CELL));
    col->AddRecordFilter(REV32(LTEX));
    col->AddRecordFilter(REV32(TXST));
    col->AddRecordFilter(REV32(MATT));
    col->AddRecordFilter(REV32(LAND));
    col->AddWSpaceFilter(weid);

    try {
      prog->InitProgress("Initializing:", 0, "Reading plugins:", 0.0, 0, 1);
      LoadCollection(col, ::SetTopic);

      prog->InitProgress("Extracting:", 0, "Parsing plugins:", 0.0, 1, 1);
      ExtractFromCollection(col);

      prog->InitProgress("Cleanup:", 0, "Freeing plugins:", 0.0, 2, 1);
      DeleteCollection(col);
    }
    catch (exception &e) {
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
