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

#include "../globals.h"
#include "../scape/simplfield.H"
#include "../generation/geometry.hpp"

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
    int x, y;

    t->getSelection(&x, &y);

    out << x << " " << y << " C" << endl;
}

void output_ps(SimplField& ter)
{
    ofstream out("tin.eps",ios::binary);
    out << "%!PS-Adobe-2.0 EPSF-2.0" << endl;
    out << "%%Creator: Scape" << endl;
    out << "%%BoundingBox: 0 0 " << ter.getHField()->getWidth()-1 << " ";
    out << ter.getHField()->getHeight()-1 << endl;
    out << "%%EndComments" << endl;
    out << "/L {moveto lineto stroke} bind def" << endl;
    out << "/C {newpath 2.5 0 360 arc closepath fill} bind def" << endl;
    out << ".2 setlinewidth" << endl;

    ter.OverEdges(ps_edge,(ostream *)&out);
    out << ".3 setgray" << endl;
    ter.OverFaces(ps_face,(ostream *)&out);

    out << "showpage" << endl;
}
