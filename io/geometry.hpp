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

extern vector<string> skiplist;
extern bool writetin;
extern bool writeobj;
extern bool writedx9;
extern bool writenif;

bool skipTile(int coordx, int coordy, int reso);

/* ---------------------------------------------------- */

void readNIF(const char *pattern);
void readNIF();

void readBTR(const char *pattern);
void readBTR();

/* ---------------------------------------------------- */

void wrteTIN(SimplField& ter);

void wrteWavefront(const char *name);
void wrteWavefront(SimplField& ter, const char *pattern);
void wrteWavefront(SimplField& ter);

void wrteNIF(const char *name);
void wrteNIF(SimplField& ter, const char *pattern);
void wrteNIF(SimplField& ter);

void wrteBTR(const char *name);
void wrteBTR(SimplField& ter, const char *pattern);
void wrteBTR(SimplField& ter);

void wrteDXMesh(const char *name);
void wrteDXMesh(SimplField& ter, const char *pattern);
void wrteDXMesh(SimplField& ter);

bool skipGeometry(const char *pattern, const char *pfx, int coordx, int coordy, int reso, bool uvs);
void wrteGeometry(SimplField& ter, const char *pattern);
void freeGeometry();
