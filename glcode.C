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
// This file contains all the SGI GL dependent code for glscape.
// The file nogl.C contains dummy routines for use in non-GL systems.
//

#include <gl.h>
#include <device.h>

#include "scape.H"



// HField::emit --
//
// This is a simple utility function which is used for emitting a particular
// sample point into the GL rendering pipeline.
//
void HField::emit(Real x,Real y)
{
    Real p[3];
    Real c[3];
    float c2[3];


    p[0] = (Real)x;
    p[1] = (Real)y;
    // p[2] = eval(x,y)/pref->z_scale;
    p[2] = eval(x,y);

    if( render_with_color ) {
	color((int)x,(int)y,c[0],c[1],c[2]);
        c2[0] = c[0];
        c2[1] = c[1];
        c2[2] = c[2];
    } else {
        float c = (eval(x,y) / zmax());

        c2[0] = c;
        c2[1] = c;
        c2[2] = c;
    }

    if( !render_as_surface ) c3f(c2);
    v3d(p);
}


// compute_normal --
//
// Given the three corners of a triangle (p1,p2,p3), this function will
// return the unit normal to the triangle's plane.
//
static void compute_normal(Vector3d& n,const Vector3d& p1,
		    const Vector3d& p2,const Vector3d& p3)
{
    // We explicitly declare these (rather than putting them in a
    // Vector) so that they can be allocated into registers.
    Real v1x = p2.x-p1.x,
	v1y = p2.y-p1.y,
	v1z = p2.z-p1.z;
    Real v2x = p3.x-p2.x,
	 v2y = p3.y-p2.y,
	 v2z = p3.z-p2.z;

    n.x = v1y*v2z - v1z*v2y;
    n.y = v1z*v2x - v1x*v2z;
    n.z = v1x*v2y - v1y*v2x;

    // Normalize
    Real len = n.x*n.x + n.y*n.y + n.z*n.z;
    if( len!=1 ) {
	len = sqrt(len);
	n.x /= len;
	n.y /= len;
	n.z /= len;
    }
}


// emit_normal --
//
// The three given points define a triangle.  This function will set
// the current GL normal the be the normal vector of this triangle.
//
void emit_normal(Vector3d& v1,Vector3d& v2, Vector3d& v3)
{
    Vector3d normal;
    compute_normal(normal,v1,v2,v3);

    float n[3];
    n[0] = normal.x;
    n[1] = normal.y;
    n[2] = normal.z;
    n3f(n);
}


// HField::draw_from_point --
//
// Render the two triangles to the right and down from the given point.
//
void HField::draw_from_point(int x,int y)
{
    RGBcolor(10,200,10);



    Vector3d v1(x,y,eval(x,y)),
	     v2(x+1,y,eval(x+1,y)),
	     v3(x+1,y+1,eval(x+1,y+1));
    bgnpolygon();
    if( render_as_surface )    emit_normal(v1,v2,v3);
    emit(x,y);
    emit(x+1,y);
    emit(x+1,y+1);
    endpolygon();

    Vector3d u1(x,y,eval(x,y)),
	     u2(x,y+1,eval(x,y+1)),
	     u3(x+1,y+1,eval(x+1,y+1));
    bgnpolygon();
    if( render_as_surface )  emit_normal(u1,u2,u3);
    emit(x,y);
    emit(x,y+1);
    emit(x+1,y+1);
    endpolygon();
}


// HField::eval_key --
//
// Returns the internal state associated with the given rendering key.
//
long HField::eval_key(model_key key)
{
    switch( key ) {

    case RENDER_WITH_COLOR:
	return render_as_surface;

    case RENDER_AS_SURFACE:
	return render_as_surface;

    }

    return NULL;
}


// HField::process_key --
//
// Interface function required by Model.  This processes rendering
// keys and sets internal state variables accordingly.
//

void HField::process_key(model_key key,long value)
{

    switch( key ) {

    case RENDER_WITH_COLOR:
	if( has_texture() )
	    render_with_color = (int)value;
	else
	    cerr << "Cannot render color on non-textured surface." << endl;
	break;

    case RENDER_AS_SURFACE:
	render_as_surface = (int)value;
	break;

    }
}


// HField::just_render --
//
// The third interface function required by Model.  This renders the
// height field model.
//
void HField::just_render()
{
    int x,y;


    for(x=0;x<width-1;x++)
        for(y=0;y<height-1;y++)
            draw_from_point(x,y);

/*
 * This code doesn't really work with the render-as-surface style,
 * so it's been replaced.  However, it is *much* faster.  So if you're
 * willing to skip render-as-surface, this is highly recommended
    for(y=0;y<height-1;y++) {
        bgntmesh();
        emit(0,y);
        emit(0,y+1);
        for(x=1;x<width;x++) {
            emit(x,y);
            emit(x,y+1);
        }
        endtmesh();
    }
 */
}



//------------------------------ graphics routines

void SimplField::emit(Real x,Real y,Real z)
{
    Real p[3];

    p[0] = x;
    p[1] = y;
    p[2] = z;


    v3d(p);
}


void SimplField::emit_origin(Edge *e)
// emit the point at the origin of edge e
{
    Real p[3];
    Real c[3];
    float c2[3], uv[2];
    int x, y;

    p[0] = e->Org2d().x;
    p[1] = e->Org2d().y;
    x = (int)p[0];
    y = (int)p[1];
    p[2] = H->eval(x,y);

    if( !render_as_surface )
	if( render_with_texture ) {
	    uv[0] = (float)x/H->get_width();
	    uv[1] = (float)y/H->get_height();
	    t2f(uv);
	} else {
	    if( render_with_color ) {
		H->color(x,y,c[0],c[1],c[2]);

		c2[0] = c[0];
		c2[1] = c[1];
		c2[2] = c[2];
	    } else {
		float c = (H->eval(x,y) / H->zmax());

		c2[0] = c;
		c2[1] = c;
		c2[2] = c;
	    }

	    c3f(c2);
	}

    v3d(p);
}

void SimplField::render_face(Triangle *t)
{
    const Point2d& p1 = t->point1();
    const Point2d& p2 = t->point2();
    const Point2d& p3 = t->point3();

    if( render_as_surface ) {
	Vector3d v1(p1,H->eval(p1)),
	    v2(p2,H->eval(p2)),
	    v3(p3,H->eval(p3));
	Vector3d normal;
	compute_normal(normal,v1,v2,v3);

	float n[3];
	n[0] = normal.x;
	n[1] = normal.y;
	n[2] = normal.z;
	n3f(n);
    }

    bgnpolygon();
    emit_origin(t->get_anchor());
    emit_origin(t->get_anchor()->Lnext());
    emit_origin(t->get_anchor()->Lprev());
    endpolygon();

    if( render_with_mesh ) {
	RGBcolor(10,200,10);
	bgnclosedline();
	emit(p1.x,p1.y,H->eval(p1));
	emit(p2.x,p2.y,H->eval(p2));
	emit(p3.x,p3.y,H->eval(p3));
	endclosedline();
    }
}


unsigned long *long_pixels(HField *H) {
    struct ABGR {unsigned char a, b, g, r;};
    ABGR *p = new ABGR[H->get_width()*H->get_height()];
    unsigned long *base = (unsigned long *)p;
    cout << "creating " << H->get_width() << "x" << H->get_height() <<
	" texture" << endl;
    int x, y;
    rgb_val r, g, b;
    for (y=0; y<H->get_height(); y++)
	for (x=0; x<H->get_width(); x++) {
	    H->color(x, y, r, g, b);
	    p->a = 0;
	    p->b = (unsigned char)(b*255);
	    p->g = (unsigned char)(g*255);
	    p->r = (unsigned char)(r*255);
	    p++;
	}
    cout << "  done creating texture" << endl;
    return base;
}

#define MY_TEXTURE 1
#define MY_TEXTURE_ENVIRONMENT 1

void texture_load(HField *H) {
    unsigned long *pixels;

    static float texprops[] = {
	TX_MINFILTER, TX_MIPMAP_TRILINEAR, /* minify with trilinear interp. */
	TX_MAGFILTER, TX_BILINEAR,	  /* magnify with bilinear interp. */
	TX_NULL};
    static float tevprops[] = {TV_MODULATE, TV_NULL};
	/* TV_MODULATE says that texture color modulates polygon color */

    pixels = long_pixels(H);

    texdef2d(/*texno*/ MY_TEXTURE,
	/*nchan*/ 4, /*width*/ H->get_width(), /*height*/ H->get_height(),
	/*image*/ pixels, /*#props*/ 0, /*props*/ texprops);
	/* define a texture (pass texture data to GL, assign it a number) */
    delete[] pixels;

    tevdef(MY_TEXTURE_ENVIRONMENT, 0, tevprops);
	/* define a texture environment: how to use texture data in
	   shading formula */

    tevbind(TV_ENV0, MY_TEXTURE_ENVIRONMENT);
	/* set texture environment */
}

void texture_on(HField *H) {
    static int gotit = 0;
    if (!gotit) {
	texture_load(H);
	gotit = 1;
    }
    texbind(TX_TEXTURE_0, MY_TEXTURE);	// turn on texture mapping
}

void texture_off() {
    texbind(TX_TEXTURE_0, 0);		// turn off texture mapping
}

long SimplField::eval_key(model_key key)
{
    switch( key ) {

    case RENDER_WITH_COLOR:
	return render_with_color;

    case RENDER_WITH_MESH:
	return render_with_mesh;

    case RENDER_WITH_DEM:
	return render_with_dem;

    case DEM_STEP:
	return dem_step;

    case RENDER_WITH_TEXTURE:
	return render_with_texture;

    case RENDER_AS_SURFACE:
	return render_as_surface;

    }

    return NULL;
}



void SimplField::process_key(model_key key,long value)
{

    switch( key ) {

    case RENDER_WITH_COLOR:
	if( H->has_texture() )
	    render_with_color = (int)value;
	else
	    cerr << "Cannot render color on non-textured surface." << endl;
	break;

    case RENDER_WITH_MESH:
	render_with_mesh = (int)value;
	break;

    case RENDER_WITH_DEM:
	render_with_dem = (int)value;
	break;

    case DEM_STEP:
	dem_step = (int)value;
	break;

    case RENDER_AS_SURFACE:
	render_as_surface = (int)value;
	break;

    case RENDER_WITH_TEXTURE:
	if( H->has_texture() ) {
	    render_with_texture = (int)value;
	    if( render_with_texture ) texture_on(H);
	    else                      texture_off();
	} else
	    cerr << "Cannot render color on non-textured surface." << endl;
	break;
    }
}

void face_iterator(Triangle *t,void *closure)
{
    SimplField *F = (SimplField *)closure;

    F->render_face(t);
}

void SimplField::just_render()
{
    OverFaces(face_iterator,this);

    if( dem_step < 0 ) dem_step = 1;

    if( render_with_dem ) {
	RGBcolor(200,10,10);
	int x,y;
	int w = H->get_width();
	int h = H->get_height();
	for(x=0;x<w-dem_step;x+=dem_step) {
	    for(y=0;y<h-dem_step;y+=dem_step) {
		bgnclosedline();
		emit(x,y,H->eval(x,y));
		emit(x+dem_step,y,H->eval(x+dem_step,y));
		emit(x+dem_step,y+dem_step,H->eval(x+dem_step,y+dem_step));
		emit(x,y+dem_step,H->eval(x,y+dem_step));
		endclosedline();
	    }
	}
    }
}



float material[] = {
    AMBIENT,   0.2, 0.2, 0.2,
    DIFFUSE,   0.6, 0.6, 0.6,
    // SPECULAR,  0.5, 0.5, 0.5,
    // SHININESS, 10.0,
    LMNULL,
};

static float light_model[] = {
    AMBIENT, 0.2, 0.2, 0.2,
    LOCALVIEWER, 0.0,
    TWOSIDE, 1.0,
    LMNULL,
};

static float light[] = {
    LCOLOR, 1.0, 1.0, 1.0,
    POSITION, DEFAULT_LIGHT_X, DEFAULT_LIGHT_Y, DEFAULT_LIGHT_Z, 0.0,
    //POSITION, 0.0, -1.0, 1.0, 0.0,
//    POSITION, -1.0, 0.0, 1.0, 0.0,
    LMNULL,
};


gl_win View::init_viewport(char *name)
{
    Bounds& b = model->bounds();
    Real width = b.max.x - b.min.x;
    Real height = b.max.y - b.min.y;
    Real depth = b.max.z - b.min.z;

    keepaspect(width,height);
    gl_win win = winopen(name);
    RGBmode();
    doublebuffer();

    gconfig();

    zbuffer(TRUE);

    mmode(MPROJECTION);

    perspective(450, width/height, 0.1, 5*depth  );

    // zlookfrom = 1.2*depth;
    zlookfrom = 2*height;

    lookat(0,0,zlookfrom,
	   0,0,0,
	   0);

    mmode(MVIEWING);

    lmdef(DEFMATERIAL,MY_MATERIAL,0,material);
    lmbind(MATERIAL,MY_MATERIAL);

    lmdef(DEFLIGHT,MY_LIGHT,0,light);
    lmdef(DEFLMODEL,MY_LMODEL,0,light_model);

    lmbind(LMODEL,MY_LMODEL);
    lmbind(LIGHT0,MY_LIGHT);

    return win;
}

void View::redraw()
{
    winset(window);
    reshapeviewport();

    RGBcolor(80,80,255);
    clear();
    zclear();
    RGBcolor(255,255,255);

    Vector3d& model_center = model->center();

    pushmatrix();
    rot(xrot,'x');
    rot(yrot,'y');
    rot(zrot,'z');
    ::scale(xscale,yscale,zscale);
    translate(-model_center.x,-model_center.y,-model_center.z);
    model->render(0);
    popmatrix();

    swapbuffers();
}
