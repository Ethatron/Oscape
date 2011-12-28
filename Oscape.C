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

#define	_CRT_SECURE_NO_WARNINGS
#define	_CRT_NONSTDC_NO_DEPRECATE

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

#ifdef _DEBUG
#include <fcntl.h>
#include <io.h>

// maximum mumber of lines the output console should have
static const WORD MAX_CONSOLE_LINES = 500;

void RedirectIOToConsole() {
  int hConHandle;
  long lStdHandle;
  CONSOLE_SCREEN_BUFFER_INFO coninfo;
  FILE *fp;

  // allocate a console for this app
  AllocConsole();

  // set the screen buffer to be big enough to let us scroll text
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);

  coninfo.dwSize.Y = MAX_CONSOLE_LINES;

  SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

  // redirect unbuffered STDOUT to the console
  lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
  hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

  fp = _fdopen( hConHandle, "w" );
  *stdout = *fp;

  setvbuf( stdout, NULL, _IONBF, 0 );

  // redirect unbuffered STDIN to the console
  lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
  hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

  fp = _fdopen( hConHandle, "r" );
  *stdin = *fp;

  setvbuf( stdin, NULL, _IONBF, 0 );

  // redirect unbuffered STDERR to the console
  lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
  hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

  fp = _fdopen( hConHandle, "w" );
  *stderr = *fp;

  setvbuf( stderr, NULL, _IONBF, 0 );

  // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
  // point to console as well
  ios::sync_with_stdio();
}
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "Oscape.h"

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

#ifdef _DEBUG
  RedirectIOToConsole();
#endif

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
  if (prg)
    prg->PollProgress();
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
