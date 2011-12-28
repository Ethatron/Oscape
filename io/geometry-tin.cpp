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

ostream *tin_out = NULL;

void output_face(Triangle *t,void *closure)
{
    SimplField *ter = (SimplField *)closure;
    ostream& tin = *tin_out;

    const Point2d& p1 = t->point1();
    const Point2d& p2 = t->point2();
    const Point2d& p3 = t->point3();

    tin << "t ";

    tin << (p1.x * sizescale) << " " << (p1.y * sizescale) << " ";
    tin << (ter->getHField()->getZ(p1) * heightscale - heightshift) << "   ";

    tin << (p2.x * sizescale) << " " << (p2.y * sizescale) << " ";
    tin << (ter->getHField()->getZ(p2) * heightscale - heightshift) << "   ";

    tin << (p3.x * sizescale) << " " << (p3.y * sizescale) << " ";
    tin << (ter->getHField()->getZ(p3) * heightscale - heightshift) << endl;
}

void wrteTIN(SimplField& ter)
{
    ofstream tin("out.tin",ios::binary);
    tin_out = &tin;

    ter.OverFaces(output_face,&ter);
}
