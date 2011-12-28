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

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>

#include "stmops.h"

#define MAGIC_VALUE 0x04030201

typedef union {
    unsigned int val;
    unsigned char bytes[4];
} bitTest;

int stmMatchOrder(char orderBytes[4])
{
    int match = 1;
    int i;
    bitTest test;

    test.val = MAGIC_VALUE;

    for (i = 0; i < 3; i++)
	match = match & (test.bytes[i] == orderBytes[i]);

    return match;
}

void stmSwapOrder(unsigned short *data, int length)
{
    int i;

    for(i=0;i<length;i++)
	data[i] = (data[i]<<8)&0xff00 | (data[i]>>8)&0xff;
}

void stmReadHeader(FILE *in, int *width, int *height, int *shouldSwap)
{
    char orderBytes[4];

    fscanf(in,"STM %d %d %c%c%c%c\n",
	   width, height,
	   &orderBytes[0],
	   &orderBytes[1],
	   &orderBytes[2],
	   &orderBytes[3]);

    *shouldSwap = !stmMatchOrder(orderBytes);
}

void stmWriteHeader(FILE *out,int width, int height)
{
    bitTest test;

    test.val = MAGIC_VALUE;
    fprintf(out,"STM %d %d %c%c%c%c\n",
	    width, height,
	    test.bytes[0],
	    test.bytes[1],
	    test.bytes[2],
	    test.bytes[3]);
}

void stmWriteData(FILE *out, unsigned short *data, int length)
{
    fwrite(data, sizeof(unsigned short), length, out);
}


/*******************************************************************/

STMdata *stmRead(FILE *in)
{
    STMdata *data;
    int shouldSwap;
    int length;

    data = (STMdata *)malloc( sizeof(STMdata) );
    if( !data ) {
	fprintf(stderr,"stmRead: Unable to allocate memory.\n");
	exit(1);
    }

    stmReadHeader(in, &data->width, &data->height, &shouldSwap);

    length = data->width * data->height;

    data->data = (unsigned short *)malloc( sizeof(unsigned short)*length );
    if( !data->data ) {
	fprintf(stderr,"stmRead: Unable to allocate memory.\n");
	exit(1);
    }
    fread(data->data, sizeof(unsigned short), length, in);

    if( shouldSwap )
	stmSwapOrder(data->data, length);

    return data;
}


void stmWrite(FILE *out, STMdata *stm)
{
    stmWriteHeader(out, stm->width, stm->height);
    stmWriteData(out, stm->data, stm->width * stm->height);
}
