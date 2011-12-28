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

#ifndef	OSCAPE_H
#define	OSCAPE_H

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

#include "../Oscape_Window.h"

// ----------------------------------------------------------------------------

#include "globals.h"
#include "io/data.hpp"
#include "io/CBash.hpp"
#include "io/geometry.hpp"
#include "io/texture.hpp"
#include "io/texture-dds.hpp"
#include "generation/geometry.hpp"
#include "generation/texture.hpp"
#include "generation/recovery.hpp"
#include "scape/scape.H"

// ----------------------------------------------------------------------------

#include "gui/Progress.hpp"
#include "gui/GUI.hpp"

#endif
