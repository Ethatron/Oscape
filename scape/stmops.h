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

/*
 * This code is mean to be a simple interface for programs
 * that want to do simple manipulations of STM terrain files.
 *
 */

int stmMatchOrder(char orderBytes[4]);
void stmSwapOrder(unsigned short *data, int length);
void stmReadHeader(FILE *in, int *width, int *height, int *shouldSwap);

void stmWriteHeader(FILE *out,int width, int height);
void stmWriteData(FILE *out, unsigned short *data, int length);

typedef struct {
    unsigned short *data;
    int width, height;
} STMdata;

#define stmRef(stm,i,j) ((stm)->data[(j)*(stm)->width + (i)])

STMdata *stmRead(FILE *);
void stmWrite(FILE *, STMdata *);
