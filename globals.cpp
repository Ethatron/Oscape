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

#include "globals.h"

/* ---------------------------------------------------- */

FILE *repfile = NULL;
FILE *logfile = NULL;

/* ---------------------------------------------------- */

int width, height;
int tilesx, tilesy;

__int64 writechk = 0;
int wdspace = 60;
char wdsname[256] = "tamriel";
int wchgame = 0;
char *wpattern = "%02d.%02d.%02d.%02d";

unsigned short basinshift  = (8192 - 512) / (2 * 4);
Real heightscale =    2.0f * 4.0f; /*Annwyn*/
Real heightshift = 8192.0f - 0.0f; /*Annwyn*/
Real heightadjust1 = 4.0f / (4.0f); /*Oblivion*/
Real heightadjust2 = 4.0f / (2.0f * sqrt(2.0f))/*Oblivion (conciously more steep!)*/;
Real sizescale = 131072.0f / 1024.0f;
