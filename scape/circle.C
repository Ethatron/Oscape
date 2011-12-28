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
circle - draw a filled circle of fixed screen space radius

circf is ridiculously slow, about 100 times slower than this.
Whoever wrote circf should be spanked!

Paul Heckbert	6 July 1994
*/

#include <assert.h>
#include <malloc.h>
#include <gl.h>		/* iris graphics library subroutine prototypes */

#define FONT_INDEX 1


void circle_init(float radius) {
    /* create a font bitmap for a circle */
    /* (this technique for fast dot-drawing suggested by Thad Beier) */

    static float curradius = -1;
    if (radius==curradius) return;
    curradius = radius;

    int x, y, r = (int)radius, d = 2*r+1, nw = (d+15)/16;
    unsigned short *bitmap;
    Fontchar chars[2];
	/* \000 is unused (because it ends string) */
	/* \001 is circle */

    bitmap = (unsigned short *)calloc(nw*d, sizeof bitmap[0]);
	/* calloc zeros the array */
    assert(bitmap);
#   define SETBIT(x, y) bitmap[(y)*nw+(x)/16] |= 1<<(~(x)&15)
    for (y= -r; y<=r; y++)
	for (x= -r; x<=r; x++)
	    if (x*x+y*y<=radius*radius) SETBIT(x+r, y+r);
    chars[1].offset = 0;
    chars[1].w = d;
    chars[1].h = d;
    chars[1].xoff = -r;
    chars[1].yoff = -r;
    chars[1].width = d+1;
    defrasterfont(FONT_INDEX, /*height*/ d,
	sizeof chars/sizeof chars[0], /*character info*/ chars,
	/*length of bitmap array*/ nw*d, bitmap);
    free(bitmap);
    font(FONT_INDEX);
}

void circle(float x, float y, float z) {
    cmov(x, y, z);
    charstr("\001");
}
