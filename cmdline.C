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

#include "scape.H"

int rasterx = 1024;
int rastery = 1024;
int dotilex = -1;
int dotiley = -1;
int nmtilex = -1;
int nmtiley = -1;
int rwsizex = -1;
int rwsizey = -1;
bool writeobjmaps = false;
bool writefcemaps = false;
bool writenoresmaps = false;
bool writehiresmaps = false;
bool writeloresmaps = false;
bool nomodel = false;
bool nobasin = true;
bool noextrd = false;
bool emituvs = false;
bool emitnrm = false;

int limit = 100;
int datadep = 1;

Real alpha=1.0;
int multinsert=0;

Real thresh=0.0;
Real termination=-1.0;
int parallelInsert=0;

Real emphasis = 0;
Real qual_thresh = 0.95f;	// quality threshold
Criterion criterion = SUM2;
//int debug = 0;


const char *texFile = NULL;
const char *greyFile = NULL;
const char *dataFile = NULL;

//-debug <debuglevel>           set debugging level [default=0]\n\

static char option_usage[] = "Options:\n\
 -datadep                      do data dependent triangulation\n\
 -delaunay                     do Delaunay triangulation [default]\n\
 -qthresh <quality_thresh>     set threshold for data-dep tri. [default=.5]\n\
 -termination <stop_thresh>    set threshold for terminating tri. [default=.0]\n\
 -sum                          use sum of max err for data-dep tri. [default]\n\
 -max                          use max of max err for data-dep tri.\n\
 -sqerr			       use sum of squared err for data-dep tri.\n\
 -abn                          use angle between normals for datap-dep tri.\n\
 -frac <area_thresh>	       set threshold for supersampling [default=1e30]\n\
 -npoint <#points>             set number of points to select [default=100]\n\
 -tex <texturefile> <emphasis> set texture and its emphasis [default=0]\n\
 -fracthresh <alpha>           use fractional threshold parallel insertion\n\
 -constthresh <thresh>         use constant threshold parallel insertion\n\
\n\
Options for texture creation:\n\
 -objmaps\n\
 -facemaps\n\
 -nomaps\n\
 -hqmaps\n\
 -lqmaps\n\
\n\
Options for model creation:\n\
 -nomodels\n\
 -withbasin\n\
 -emituvs\n\
 -emitnormals\n\
\n\
Options regarding dimensions:\n\
 -rx <units>, -ry <units>     set the size of a tile in units [default=1024]\n\
 -tx <offset>, -ty <offset>   set the tile-row and -column to produce [default=all]\n\
 -sx <pixels>, -sy <pixels>   set the dimension of the raw input [default=guess]\n\
\n\
 The program can read .raw and .pgm/.stm files. The later need to be flipped vertically.\n\
 ";



static void usage(char *progname)
{
    cerr << "Usage:" << endl;
    cerr << progname << " filename [options]" << endl;
    cerr << option_usage << endl;
    exit(1);
}


void parse_cmdline(int argc, char *argv[])
{
    if( argc<2 ) usage(argv[0]);
    if( argv[1][0] == '-' ) usage(argv[0]);

    int i;

    dataFile = argv[1];

    for(i=2;i<argc;i++) {
	/**/ if (!strcmp(argv[i], "-rx") && i+1<argc)
	  rasterx = atoi(argv[++i]);
	else if (!strcmp(argv[i], "-ry") && i+1<argc)
	  rastery = atoi(argv[++i]);
	else if (!strcmp(argv[i], "-tx") && i+1<argc)
	  dotilex = atoi(argv[++i]);
	else if (!strcmp(argv[i], "-ty") && i+1<argc)
	  dotiley = atoi(argv[++i]);
	else if (!strcmp(argv[i], "-sx") && i+1<argc)
	  rwsizex = atoi(argv[++i]);
	else if (!strcmp(argv[i], "-sy") && i+1<argc)
	  rwsizey = atoi(argv[++i]);

	else if (!strcmp(argv[i], "-nomodels"))
	  nomodel = true;
	else if (!strcmp(argv[i], "-withbasin"))
	  nobasin = false;
	else if (!strcmp(argv[i], "-nonextruded"))
	  noextrd = true;

	else if (!strcmp(argv[i], "-emituvs"))
	  emituvs = true;
	else if (!strcmp(argv[i], "-emitnormals"))
	  emitnrm = true;

	else if (!strcmp(argv[i], "-objmaps"))
	  writeobjmaps = true;
	else if (!strcmp(argv[i], "-facemaps"))
	  writefcemaps = true;
	else if (!strcmp(argv[i], "-nomaps"))
	  writeobjmaps = writefcemaps = writenoresmaps = true;
	else if (!strcmp(argv[i], "-hqmaps"))
	  writeobjmaps = writefcemaps = writehiresmaps = true;
	else if (!strcmp(argv[i], "-lqmaps"))
	  writeobjmaps = writefcemaps = writeloresmaps = true;

	else if (!strcmp(argv[i], "-datadep"))
	    datadep = 1;
	else if (!strcmp(argv[i], "-delaunay"))
	    datadep = 0;
	else if (!strcmp(argv[i], "-npoint") && i+1<argc)
	    limit = atoi(argv[++i]);
	else if (!strcmp(argv[i], "-qthresh") && i+1<argc)
	    qual_thresh = (Real)atof(argv[++i]);
	else if (!strcmp(argv[i], "-termination") && i+1<argc)
	    termination = (Real)atof(argv[++i]);
	else if (!strcmp(argv[i], "-tex") && i+2<argc) {
	    texFile = argv[++i];
	    emphasis = (Real)atof(argv[++i]);
	}
//	else if (!strcmp(argv[i], "-debug") && i+1<argc)
//	    debug = atoi(argv[++i]);
	else if (!strcmp(argv[i],"-constthresh") && i+1<argc) {
	    parallelInsert = 1;
	    thresh = (Real)atof(argv[++i]);
	}
	else if( !strcmp(argv[i],"-fracthresh") && i+1<argc) {
	    multinsert = 1;
	    alpha = (Real)atof(argv[++i]);
	}
	else if (!strcmp(argv[i], "-frac") && i+1<argc)
	    area_thresh = (Real)atof(argv[++i]);
	else if (!strcmp(argv[i], "-sum"))
	    criterion = SUMINF;
	else if (!strcmp(argv[i], "-max"))
	    criterion = MAXINF;
	else if (!strcmp(argv[i], "-sqerr"))
	    criterion = SUM2;
	else if (!strcmp(argv[i], "-abn"))
	    criterion = ABN;
	else {
	    usage(argv[0]);
	}
    }
}
