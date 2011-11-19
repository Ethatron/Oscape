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

// To be included from scape.H

typedef long gl_win;
typedef long gl_event;
typedef short gl_event_data;

typedef long model_key;


// Model configuration keys

#define GOURAUD  1
#define TEXTURED 2

#define RENDER_WITH_COLOR 4
#define RENDER_WITH_MESH  5
#define RENDER_WITH_TEXTURE 6
#define RENDER_AS_SURFACE 7
#define RENDER_WITH_DEM 8
#define DEM_STEP 9


#define MY_MATERIAL 1
#define MY_LIGHT    1
#define MY_LMODEL   1

#define DEFAULT_LIGHT_X 1.0
#define DEFAULT_LIGHT_Y 0.0
#define DEFAULT_LIGHT_Z 1.0


enum ShadingStyle { Wire=1, Flat=2, Gouraud=3 };

// A class to hold a bounding volume
class Bounds {
public:
    Vector3d min,max;
};




class Model {
protected:
    Bounds bound_volume;
    Vector3d model_center;

public:
    Bounds& bounds() { return bound_volume; }
    Vector3d& center() { return model_center; }

    void configure(int,...);
    void render(int,...);
    void set_key(model_key key,long value=0) { process_key(key,value); }

    virtual long eval_key(model_key) { return NULL; }
    virtual void process_key(model_key,long) {}
    // virtual void process_key(model_key,double value=0) {}
    virtual void just_render() = 0;
};




class View;

struct Event {
    gl_event dev;
    gl_event_data val;
};

typedef int (*fn_handler)(View *v,Event *);

class EventHandler {
    EventHandler *next;
    fn_handler handler;
public:
    EventHandler(fn_handler f,EventHandler *n=NULL) { next = n; handler=f; }

    void link_to(EventHandler *h) { next = h; }
    EventHandler *get_link() { return next; }

    int operator()(View *p,Event *e) {
        int result = (*handler)(p,e);
        if( !result && next )
            return (*next)(p,e);
        else return 1;
    }
};



class View {
    // View resources
    gl_win window;
    Model *model;

    // View properties
    int will_light;

    float zlookfrom;
    float xrot,yrot,zrot;
    float xscale,yscale,zscale;

    EventHandler *handler;

    gl_win init_viewport(char *);
public:
    View(Model *m,char *name="view") {
        //init_transforms();
	xrot=yrot=zrot=0;
	xscale=yscale=zscale=1;
        model=m;
        window=init_viewport(name);
        handler = NULL;
    }

    void redraw();
    // void dump_to_file(char *);

    void rotate(float x,float y,float z) {xrot=x; yrot=y; zrot=z;}
    void inc_rot(float dx,float dy,float dz) {xrot+=dx; yrot+=dy; zrot+=dz;}

    void scale(float x,float y,float z) {xscale=x; yscale=y; zscale=z;}
    void mult_scale(float x,float y,float z) {xscale*=x; yscale*=y; zscale*=z;}

    gl_win win() { return window; }
    Model *get_model() { return model; }

    void push_handler(EventHandler *h) {
        h->link_to(handler);
        handler=h;
    }
    EventHandler *pop_handler() {
        EventHandler *h=handler; handler=handler->get_link(); return h;
    }
    void event_dispatch(Event *event) {
        if( handler ) (*handler)(this,event);
    }
};
