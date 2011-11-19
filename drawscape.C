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

//
// drawscape.C
//
// This is a quick little program to allow users to look at the
// full resolution height fields (for purposes of comparison).
//

#include <gl.h>
#include <device.h>

#include "scape.H"

int width,height;
float heightscale;

// handle_expose --
//
// This function will be called whenever an expose event is generated
// in the viewing window.
//
int handle_expose(View *v,Event *event)
{
    if( event->dev != REDRAW ) return 0;

    v->redraw();

    return 1;
}

// do_rotation --
//
// A simple little function that allows for a fairly primitive
// spin interaction.
//
void do_rotation(View *v,float angle,Device d)
{
    long statex,statey,newx,newy;

    mmode(MVIEWING);

    statex = getvaluator(MOUSEX);
    statey = getvaluator(MOUSEY);

    while( getbutton(d) ) {
	newx = getvaluator(MOUSEX);
	newy = getvaluator(MOUSEY);

	if( newx-statex || newy-statey ) {
	    v->inc_rot(angle*(float)(statey-newy),
		       angle*(float)(newx-statex),
		       0);

	    v->redraw();

	    statex = newx;
	    statey = newy;
	}

    }
}

// handle_input --
//
// Function to handle input events in the viewing window.
//
int handle_input(View *v,Event *event)
{
    Model *m = v->get_model();

    switch( event->dev ) {
    case LEFTMOUSE:
	do_rotation(v,1.0,LEFTMOUSE);
	break;

    case RIGHTMOUSE:
    case MIDDLEMOUSE:
	cout << "Mouse" << endl;
	break;

    case KEYBD: {
	char c = (char)event->val;
	switch( c ) {
	case 'q':
	    winclose(v->win());
	    exit(0);
	    break;

	case 'v':
	    m->set_key(RENDER_WITH_COLOR,
		       !m->eval_key(RENDER_WITH_COLOR));
	    qenter(REDRAW,(short)v->win());
	    break;

	case 'f':
	    m->set_key(RENDER_AS_SURFACE,
		       !m->eval_key(RENDER_AS_SURFACE));
	    qenter(REDRAW,(short)v->win());
	    break;


	case '+':
	    v->mult_scale(1,1,2);
	    v->redraw();
	    break;
	case '-':
	    v->mult_scale(1,1,.5);
	    v->redraw();
	    break;

	}
	break;
    }
    default:
	return 0;
    }

    return 1;
}


void render(HField& ter)
{
    View view(&ter,"Scape Terrain Viewer");
    EventHandler input(handle_input);
    EventHandler expose(handle_expose);

    view.push_handler(&input);
    view.push_handler(&expose);

    qdevice(LEFTMOUSE);
    qdevice(KEYBD);

    qreset();
    qenter(REDRAW,(short)view.win());

    view.scale(1,1,heightscale);

    for(int done=0;!done;) {
	Event event;

	event.dev = qread(&event.val);
	view.event_dispatch(&event);
    }
}


main(int argc,char **argv)
{
    texFile = NULL;
    parse_cmdline(argc, argv);

    cout << "Opening input file " << stmFile << endl;

    ifstream mntns(stmFile,ios::binary);
    HField ter(mntns,texFile);
    width = ter.get_width();
    height = ter.get_height();

    // Real zrange = ter.zmax() - ter.zmin();
    // heightscale = .3*width/(zrange ? zrange : 1);

    render(ter);
}
