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
// glscape.C
//
// This program provides an interactive interface to the approximation
// system.
//
// *****  THIS IS ALL SGI GL SPECIFIC.  *****

#include <gl.h>
#include <device.h>

#include "scape.H"
#include "circle.H"
#include <string.h>

long win;

int width,height;

extern int update_cost;
extern int ndecision; // count of #swap decisions, total
extern int nshape; // count of #swap decisions determined by shape
extern int nchanged; // count of #swap decisions changed by shape

static int ss = 1;	// supersampling resolution for diagnostic error calc.

//
// This section implements the code to draw the simple 2D mesh.
//
// This code is a little ugly.  There is no real unifying approach to
// drawing things.
//


void do_render(SimplField& ter);

// draw --
//
// Just draw the given triangle into the viewing window.
//
// REMEMBER:  This is all just a 2D display.
//
void draw(Triangle *t)
{
    const Point2d& p1 = t->point1();
    const Point2d& p2 = t->point2();
    const Point2d& p3 = t->point3();

    bgnclosedline();
    v2d((double*)&p1);
    v2d((double*)&p2);
    v2d((double*)&p3);
    endclosedline();
}

// draw_special --
//
// Draws a triangle, but highlights it as "special".
//
void draw_special(Triangle *t)
{
    RGBcolor(255,255,255);
    draw(t);
}

// create_display --
//
// This function creates the GL window in which the triangle mesh
// will be displayed.
//
void create_display()
{
#ifdef AUTOPLACE
    int k = 1000/height;
    prefposition(0,width*k-1,0,height*k-1);
#else
    keepaspect(width,height);
#endif

    foreground();
    win = winopen("Scape Terrain Lab");
    RGBmode();
    doublebuffer();

    gconfig();
}

// emit --
//
// Just emit a point into the GL pipeline.  It's only reason for existence
// is to provide a wrapper for 'v2i' which takes two arguments rather than
// a vector.
//
void emit(int x,int y)
{
    long p[2];

    p[0] = x;
    p[1] = y;

    v2i(p);
}

// Draw(Edge*) --
//
// As you might expect, this just draws the given edge.
//
void Draw(Edge *e)
{
    Point2d a = e->Org2d();
    Point2d b = e->Dest2d();

    bgnline();
    v2d((double *)&a);
    v2d((double *)&b);
    endline();
}

// Draw(Region&) --
//
// Draws the specified region (which is always an axis-aligned rectangle.
//
void Draw(Region& r)
{
    int x0,y0,x1,y1;

    r.extent(x0,y0,x1,y1);

    bgnclosedline();
    emit(x0,y0);
    emit(x0,y1);
    emit(x1,y1);
    emit(x1,y0);
    endclosedline();
}


// draw_candidate_dot --
//
// Draws the point which is the candidate for the given triangle.
//
void draw_candidate_dot(Triangle *tri, void *) {
    if (tri->locate()==NOT_IN_HEAP) return;
    int sx, sy;
    tri->get_selection(&sx, &sy);
    circle(sx, sy, 0);
}


// draw_candidate_line --
//
// Draws a line from the given triangle's candidate to the triangle's
// center.  This makes the association between triangles and candidates
// visually clear.
//
void draw_candidate_line(Triangle *tri, void *) {
    if (tri->locate()==NOT_IN_HEAP) return;
    int sx, sy;
    tri->get_selection(&sx, &sy);
    Point2d cand(sx, sy);

    // compute center of triangle
    const Point2d &p = tri->point1();
    const Point2d &q = tri->point2();
    const Point2d &r = tri->point3();
    Point2d cen((p.x+q.x+r.x)/3, (p.y+q.y+r.y)/3);

    bgnline();
    v2d(&cen.x);
    v2d(&cand.x);
    endline();
}

// draw_edge --
//
// Just draws the given edge.
//
void draw_edge(Edge *e,void *)
{
    Point2d a = e->Org2d();
    Point2d b = e->Dest2d();

    bgnline();
    v2d((double*)&a);
    v2d((double*)&b);
    endline();
}

// Draw --
//
// This is the top-level drawing function.  It will draw the entire
// triangle mesh along with various embellishments.
//
void Draw(SimplField *mesh, Edge *e)
{

    winset(win);
    reshapeviewport();
    // mmode(MSINGLE);

    // set circle radius and line width (in screen space)
    long wx, wy;
    getsize(&wx, &wy);
    float r = .5*wx/(width+2);	// radius = .5 in object space
    circle_init(r<2.5 ? 2.5 : r);
    linewidth(2);

    RGBcolor(0,0,0);
    clear();

    RGBcolor(80,80,80);
    mesh->OverEdges(draw_edge,NULL);

    if (e) {
	UpdateRegion region(e);
	Triangle *t = region.first();
	do {
	    draw_special(t);
	    t = region.next();
	} while( t );
    }

    RGBcolor(150,255,150);
    mesh->OverFaces(draw_candidate_dot,NULL);

    // draw next point to be selected
    heap_node *next = mesh->get_heap().top();
    if (next) {
	RGBcolor(255,128,128);
	int nx, ny;
	next->tri->get_selection(&nx, &ny);
	circle(nx, ny, 0);
    }

    if (e) {
	// draw point just selected
	RGBcolor(255,0,0);
	circle(e->Org2d().x, e->Org2d().y, 0);
    }

    RGBcolor(255,0,255);
    mesh->OverFaces(draw_candidate_line,NULL);

    swapbuffers();
}





// output_face --
// output_tin --
//
// These two functions can be used to output the current mesh as
// a list of triangles.  output_face is the function iterated over
// every triangle in the mesh.
//
ostream *tin_out = NULL;

void output_face(Triangle *t,void *closure)
{
    SimplField *ter = (SimplField *)closure;
    ostream& tin = *tin_out;

    const Point2d& p1 = t->point1();
    const Point2d& p2 = t->point2();
    const Point2d& p3 = t->point3();

    tin<<"("<<p1.x<<" "<<p1.y<<" "<<ter->original()->eval(p1)<<")"<<" ";
    tin<<"("<<p2.x<<" "<<p2.y<<" "<<ter->original()->eval(p2)<<")"<<" ";
    tin<<"("<<p3.x<<" "<<p3.y<<" "<<ter->original()->eval(p3)<<")"<<endl;

}

void ps_edge(Edge *e,void *closure)
{
    ostream& out = *(ostream *)closure;

    const Point2d& a = e->Org2d();
    const Point2d& b = e->Dest2d();
    out << a.x << " " << a.y << " ";
    out << b.x << " " << b.y << " L" << endl;
}

void ps_face(Triangle *t,void *closure)
{
    ostream& out = *(ostream *)closure;
    int x,y;

    t->get_selection(&x,&y);
    out << x << " " << y << " C" << endl;
}




void output_tin(SimplField& ter)
{
    ofstream tin("out.tin",ios::binary);
    tin_out = &tin;

    ter.OverFaces(output_face,&ter);
}


void output_ps(SimplField& ter)
{
    ofstream out("tin.eps",ios::binary);
    out << "%!PS-Adobe-2.0 EPSF-2.0" << endl;
    out << "%%Creator: Scape" << endl;
    out << "%%BoundingBox: 0 0 " << ter.original()->get_width()-1 << " ";
    out << ter.original()->get_height()-1 << endl;
    out << "%%EndComments" << endl;
    out << "/L {moveto lineto stroke} bind def" << endl;
    out << "/C {newpath 2.5 0 360 arc closepath fill} bind def" << endl;
    out << ".2 setlinewidth" << endl;

    ter.OverEdges(ps_edge,(ostream *)&out);
    out << ".3 setgray" << endl;
    ter.OverFaces(ps_face,(ostream *)&out);

    out << "showpage" << endl;
}



// display --
//
// The guts of the 2D mesh interface.  It does all the event handling
// and dispatching.
//
void display(SimplField& ter)
{
  Edge *e = NULL;

    create_display();
    ortho2(-1,(Coord)width,-1,(Coord)height);

    qdevice(LEFTMOUSE);
    qdevice(QKEY);
    qdevice(SKEY);
    qdevice(GKEY);
    qdevice(RKEY);
    qdevice(EKEY);
    qdevice(NKEY);
    qdevice(PKEY);
    qdevice(WKEY);

    qreset();
    qenter(REDRAW,(short)win);


    for( int done=FALSE ; !done ; ) {
	short val;

	switch( qread(&val) ) {
	case REDRAW:
	    Draw(&ter,e);
	    break;

	case QKEY:
	    if( val ) done = TRUE;
	    break;

	case SKEY:
	    if( val ) {
		int taken;
		if( parallelInsert ) {
		    taken = ter.select_new_points(thresh);
		    cout << "Points inserted: "<<taken <<endl;
		    Draw(&ter,NULL);
		} else if( multinsert ) {
		    taken = ter.select_new_points(alpha*ter.max_error());
		    cout << "Points inserted: "<<taken<<endl;
		    Draw(&ter,NULL);
		} else
		    if (e=ter.select_new_point())
			Draw(&ter,e);
	    }
	    break;

	case GKEY:
	    if( val ) {
		if( parallelInsert ) {
		    cout << "Will select approx. "<<limit<<" points"<<endl;
		    int taken,i = 0;
		    while( i<limit ) {
			taken = ter.select_new_points(thresh);
			Draw(&ter,NULL);
			if( !taken ) break;

			i+=taken;
		    }
		    cout << "    [actually "<<i<<"]"<<endl;
		} else if( multinsert ){
		    cout << "Will select approx. "<<limit<<" points"<<endl;
		    int taken,i = 0;
		    while( i<limit ) {
			taken = ter.select_new_points(alpha*ter.max_error());
			Draw(&ter,NULL);
			if( !taken ) break;

			i+=taken;
		    }
		    cout << "    [actually "<<i<<"]"<<endl;
		} else {
		    cout << "Will select "<<limit<<" points"<<endl;
		    int i;
		    for(i=0;i<limit && (e=ter.select_new_point());++i)
			Draw(&ter,e);
		    cout << "Total cost so far: " << update_cost << endl;

		}
	    }
	  break;


	case RKEY:
	    if( val ) {
		do_render(ter);
	    }
	  break;

	case NKEY:
	    if (val) {
		do {
		    cout << "supersampling resolution: ";
		    cin >> ss;
		    cout << "ss = " << ss << endl;
		} while (ss<=0 || ss>100);
	    }
	    break;

	case EKEY:
	    if( val ) {
		int nv, ne, nf;
		ter.vef(nv, ne, nf);
		cout << nv << " verts, " << ne << " edges, "
		    << nf << " faces.";
		if (ss==1)
		    cout << "  sqerr=" << ter.rms_error();
		else
		    cout << "  sqerr(" << ss << ")="
			<< ter.rms_error_supersample(ss);
		cout << "(" << ter.rms_error_estimate()
		    << " est), maxerr=" << ter.max_error() << endl;
	    }
	    break;

	case WKEY:
	    if( val ) {
		cout << "Writing output TIN" << endl;
		output_tin(ter);
	    }
	    break;

	case PKEY:
	    if( val ) {
		cout << "Drawing as Postscript..." << endl;
		output_ps(ter);
		cout << "   Done." << endl;
	    }
	    break;
	}
    }
    if (datadep && ndecision)
	cout << 100.*nshape/ndecision << "% of "
	    << ndecision << " swap tests determined by shape, "
	    << 100.*nchanged/ndecision << "% changed" << endl;
    if (datadep && nscan)
	cout << 100.*nsuper/nscan << "% of " << nscan
	    << " triangles scan converted were supersampled" << endl;
}

main(int argc,char **argv)
{
    parse_cmdline(argc, argv);

    if (datadep)
	cout << "doing data-dependent triangulation" << endl
	    << "  with "
	    << (criterion==SUMINF ? "sum" : criterion==MAXINF ?
		"max" : criterion==SUM2 ? "sqerr" : "abn")
	    << " criterion, threshold "
	    << qual_thresh << ", and fraction " << area_thresh
	    << endl;
    else
	cout << "doing Delaunay triangulation" << endl;
    cout << "emphasis=" << emphasis << " npoint=" << limit << endl;
    if( parallelInsert ) {
	cout << "Using constant threshold parallel insert:  thresh=" << thresh;
	cout << endl;
    }
    if( multinsert )
	cout << "Using fractional threshold insert:  thresha="<<alpha << endl;

    ifstream mntns(stmFile,ios::binary);

    HField H(mntns, texFile);
    SimplField ter(&H);

    width = H.get_width();
    height = H.get_height();

    display(ter);

    return 0;
}





// do_rotation --
//
// A little hack to implement model rotation using the mouse.  Nothing
// fancy and nothing maximally intuitive.  Just simple.
//
void do_rotation(View *v,float angle,Device d)
{
    long statex,statey,newx,newy;

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

void do_light_movement(View *v,float delta,Device d)
{
    static float ldef[] = {POSITION, DEFAULT_LIGHT_X,
			   DEFAULT_LIGHT_Y, DEFAULT_LIGHT_Z, 0.0,
			   LMNULL};
    long statex,statey,newx,newy;

    statex = getvaluator(MOUSEX);
    statey = getvaluator(MOUSEY);

    cout << "Moving light..."<<endl;

    while( getbutton(d) ) {
	newx = getvaluator(MOUSEX);
	newy = getvaluator(MOUSEY);

	if( newx-statex || newy-statey ) {
	    ldef[1] += delta*(float)(newx-statex);
	    ldef[2] += delta*(float)(statey-newy);
	    lmdef(DEFLIGHT,MY_LIGHT,sizeof(ldef),ldef);

	    v->redraw();

	    statex = newx;
	    statey = newy;
	}

    }
}





// handle_expose --
// handle_input --
//
// The event handlers for the 3D terrain view.  These are based on the
// views model defined in views.H.
//
int handle_expose(View *v,Event *event)
{
    if( event->dev != REDRAW ) return 0;

    v->redraw();

    return 1;
}


static int render_done;

int handle_input(View *v,Event *event)
{
    SimplField *m = (SimplField *)v->get_model();

    switch( event->dev ) {
    case LEFTMOUSE:
	do_rotation(v,1.0,LEFTMOUSE);
	break;

    case MIDDLEMOUSE:
	do_light_movement(v,0.1,MIDDLEMOUSE);
	break;


    case KEYBD: {
	char c = (char)event->val;
	switch( c ) {
	case 'q':
	    render_done = 1;
	    winclose(v->win());
	    break;

	case '+':
	    v->mult_scale(1,1,2);
	    v->redraw();
	    break;
	case '-':
	    v->mult_scale(1,1,.5);
	    v->redraw();
	    break;

	case 'v':
	    m->set_key(RENDER_WITH_COLOR,
		       !m->eval_key(RENDER_WITH_COLOR));
	    qenter(REDRAW,(short)v->win());
	    break;

	case 'd':
	    m->set_key(RENDER_WITH_DEM,
		       !m->eval_key(RENDER_WITH_DEM));
	    qenter(REDRAW,(short)v->win());
	    break;
	case ',':
	    m->set_key(DEM_STEP,
		       m->eval_key(DEM_STEP)-10);
	    qenter(REDRAW,(short)v->win());
	    break;
	case '.':
	    m->set_key(DEM_STEP,
		       m->eval_key(DEM_STEP)+10);
	    qenter(REDRAW,(short)v->win());
	    break;

	case 'f':
	    m->set_key(RENDER_AS_SURFACE,
		       !m->eval_key(RENDER_AS_SURFACE));
	    qenter(REDRAW,(short)v->win());
	    break;

	case 'x':
	    {
		m->set_key(RENDER_WITH_TEXTURE,
			   !m->eval_key(RENDER_WITH_TEXTURE));
		qenter(REDRAW,(short)v->win());
	    }
	    break;

	case 't':
	    m->set_key(RENDER_WITH_MESH,
		       !m->eval_key(RENDER_WITH_MESH));
	    qenter(REDRAW,(short)v->win());
	    break;
	case 's':
	    if (m->select_new_point())
	      qenter(REDRAW,(short)v->win());
	    break;

	case 'g':
	    {
		cout << "Will select "<<limit<<" points"<<endl;
		int i;
		for(i=0;i<limit && m->select_new_point();++i)
		    v->redraw();
	    }
	    break;
	}
    }
    default:
	return 0;
    }

    return 1;
}


// do_render --
//
// The main function which creates and runs the rendering view for
// looking at the 3D terrain.
//
void do_render(SimplField& ter)
{
    View view(&ter);
    EventHandler input(handle_input);
    EventHandler expose(handle_expose);

    view.push_handler(&input);
    view.push_handler(&expose);

    qdevice(LEFTMOUSE);
    qdevice(MIDDLEMOUSE);
    qdevice(KEYBD);

    qreset();
    qenter(REDRAW,(short)view.win());

    linewidth(2);

    Real zrange = ter.original()->zmax() - ter.original()->zmin();

    float heightscale = .3*width/(zrange ? zrange : 1);
    //float heightscale = 0.03;
    view.scale(1,1,heightscale);

    for(render_done=0;!render_done;) {
	Event event;
	event.dev = qread(&event.val);
	view.event_dispatch(&event);
    }

    qenter(REDRAW,(short)win);
}
