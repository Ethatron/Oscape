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

#include <ctype.h>
#include "scape.H"
#include <stdio.h>

extern "C" {
#include "stmops.h"
}

// finds the area of the oriented 2-D triangle (p, q, r), i.e., the
// area is positive if the triangle is oriented counterclockwise.
// also finds the approximate diameter
void getArea(const Vector3d& p, const Vector3d& q, const Vector3d& r, Real &area, Real &diam) {
    Real ux = q.x - p.x, uy = q.y - p.y;
    Real vx = r.x - p.x, vy = r.y - p.y;

    area = (ux * vy - uy * vx) / 2;
    assert(area >= 0);//??

    Real
    xmin, xmax, ymin, ymax;
    xmin = p.x; ymin = p.y;
    xmax = p.x; ymax = p.y;

    if (q.x < xmin) xmin = q.x;  if (q.y < ymin) ymin = q.y;
    if (q.x > xmax) xmax = q.x;  if (q.y > ymax) ymax = q.y;
    if (r.x < xmin) xmin = r.x;  if (r.y < ymin) ymin = r.y;
    if (r.x > xmax) xmax = r.x;  if (r.y > ymax) ymax = r.y;

    diam = MAX(xmax - xmin, ymax - ymin);
    assert(diam > 0);//??
}

// find the plane z=ax+by+c passing through three points p,q,r
void Plane::init(const Vector3d& p, const Vector3d& q, const Vector3d& r) {
    // We explicitly declare these (rather than putting them in a
    // Vector) so that they can be allocated into registers.
    Real ux = q.x - p.x, uy = q.y - p.y, uz = q.z - p.z;
    Real vx = r.x - p.x, vy = r.y - p.y, vz = r.z - p.z;
    Real den = ux * vy - uy * vx;

    if (den == 0)//??
      cout << "Plane::init p=" << p << " q=" << q << " r=" << r << endl;
    assert(den != 0);

    a = (uz * vy - uy * vz) / den;
    b = (ux * vz - uz * vx) / den;
    c = p.z - a * p.x - b * p.y;
}


ColorTexture::ColorTexture(ifstream& in) {
  int width, height, maxval;
  int x, y;
  char byte;
  char head[3];
  int type = 0;

  if (!in.good()) {
    cerr << "ERROR:  Unable to read texture file." << endl;
    exit(1);
  }

  /* read head */
  in >> head[0] >> head[1] >> head[2];//"STM";

  /* detect file-format */
  if ((head[0] == 'P') &&
      (head[1] == '6') &&
      (head[2] == '\n')) {
    type = 2;

    in >> width >> height >> maxval;
  }
  else {
    type = 3;

    in.seekg(0, ios_base::end);
    maxval = in.tellg();
    in.seekg(0, ios_base::beg);

    maxval /= sizeof(unsigned char) * 3;

    if ((rwsizex <= 0) && (rwsizey <= 0)) {
      /* favor landscape mode */
      for (int rwy = 1; (rwy <= 32) && (rwsizey <= 0); rwy++)
      for (int rwx = 1; (rwx <= 32) && (rwsizex <= 0); rwx++)
	if (maxval == ((rwx * rwy) * (rasterx * rastery))) {
	  rwsizex = rwx;
	  rwsizey = rwy;
	  break;
	}
    }
    else if ((rwsizex > 0) && (rwsizey <= 0)) {
      rwsizey = maxval / rwsizex;
    }
    else if ((rwsizex <= 0) && (rwsizey > 0)) {
      rwsizex = maxval / rwsizey;
    }

    if ((rwsizex > 0) && (rwsizey > 0)) {
      if (maxval != (rwsizex * rwsizey)) {
	cerr << "error: " << maxval << " != " << rwsizex << " * " << rwsizey << endl;
	exit(0);
      }
    }

    width  = rwsizex;
    height = rwsizey;
    maxval = 255;
  }

  if (debug)
    cout << "# Width: " << width << "   Height: " << height;

  data.init(width, height);

//for (y = height - 1; y >= 0; y--)
  for (y = 0; y < height; y++)
  for (x = 0; x < width; x++) {
    Color& c = data.ref(x, y);

    in.get(byte); c.r = (rgb_val)byte / (rgb_val)maxval;
    in.get(byte); c.g = (rgb_val)byte / (rgb_val)maxval;
    in.get(byte); c.b = (rgb_val)byte / (rgb_val)maxval;
  }
}




LumaTexture::LumaTexture(ifstream& in) {
  int width, height, maxval;
  int x, y;
  char byte;
  char head[3];
  int type = 0;

  if (!in.good()) {
    cerr << "ERROR:  Unable to read texture file." << endl;
    exit(1);
  }

  /* read head */
  in >> head[0] >> head[1] >> head[2];//"STM";

  /* detect file-format */
  if ((head[0] == 'P') &&
      (head[1] == '5') &&
      (head[2] == '\n')) {
    type = 2;

    in >> width >> height >> maxval;
  }
  else {
    type = 3;

    in.seekg(0, ios_base::end);
    maxval = in.tellg();
    in.seekg(0, ios_base::beg);

    maxval /= sizeof(unsigned char) * 1;

    if ((rwsizex <= 0) && (rwsizey <= 0)) {
      /* favor landscape mode */
      for (int rwy = 1; (rwy <= 32) && (rwsizey <= 0); rwy++)
      for (int rwx = 1; (rwx <= 32) && (rwsizex <= 0); rwx++)
	if (maxval == ((rwx * rwy) * (rasterx * rastery))) {
	  rwsizex = rwx;
	  rwsizey = rwy;
	  break;
	}
    }
    else if ((rwsizex > 0) && (rwsizey <= 0)) {
      rwsizey = maxval / rwsizex;
    }
    else if ((rwsizex <= 0) && (rwsizey > 0)) {
      rwsizex = maxval / rwsizey;
    }

    if ((rwsizex > 0) && (rwsizey > 0)) {
      if (maxval != (rwsizex * rwsizey)) {
	cerr << "error: " << maxval << " != " << rwsizex << " * " << rwsizey << endl;
	exit(0);
      }
    }

    width  = rwsizex;
    height = rwsizey;
    maxval = 255;
  }

  if (debug)
    cout << "# Width: " << width << "   Height: " << height;

  data.init(width, height);

  //for (y = height - 1; y >= 0; y--)
  for (y = 0; y < height; y++)
  for (x = 0; x < width; x++) {
    Luma& l = data.ref(x, y);

    in.get(byte); l.l = (luma_val)byte / (luma_val)maxval;
  }
}




DEMdata::DEMdata(ifstream& in) {
  int width, height, maxval;
  int x, y;
  char head[3];
  char orderBytes[4];
  int type = 0;

  /* read head */
  in >> head[0] >> head[1] >> head[2];//"STM";

  /* detect file-format */
  if ((head[0] == 'S') &&
      (head[1] == 'T') &&
      (head[2] == 'M')) {
    type = 1;

    in >> width >> height;
    in >> orderBytes[0] >> orderBytes[1] >> orderBytes[2] >> orderBytes[3];

    // Read the EOL byte
    char c; in.get(c);
  }
  else if ((head[0] == 'P') &&
	   (head[1] == '5') &&
	   (head[2] == '\n')) {
    type = 2;

    in >> width >> height >> maxval;

    // big endian
    orderBytes[0] = 0x04;
    orderBytes[1] = 0x03;
    orderBytes[2] = 0x02;
    orderBytes[3] = 0x01;
  }
  else {
    type = 3;

    in.seekg(0, ios_base::end);
    maxval = in.tellg();
    in.seekg(0, ios_base::beg);

    maxval /= sizeof(unsigned short);

    if ((rwsizex <= 0) && (rwsizey <= 0)) {
      /* favor landscape mode */
      for (int rwy = 1; (rwy <= 32) && (rwsizey <= 0); rwy++)
      for (int rwx = 1; (rwx <= 32) && (rwsizex <= 0); rwx++)
	if (maxval == ((rwx * rwy) * (rasterx * rastery))) {
	  rwsizex = rwx;
	  rwsizey = rwy;
	  break;
	}
    }
    else if ((rwsizex > 0) && (rwsizey <= 0)) {
      rwsizey = maxval / rwsizex;
    }
    else if ((rwsizex <= 0) && (rwsizey > 0)) {
      rwsizex = maxval / rwsizey;
    }

    if ((rwsizex > 0) && (rwsizey > 0)) {
      if (maxval != (rwsizex * rwsizey)) {
	cerr << "error: " << maxval << " != " << rwsizex << " * " << rwsizey << endl;
	exit(0);
      }
    }

    width  = rwsizex;
    height = rwsizey;

    // little endian
    orderBytes[0] = 0x01;
    orderBytes[1] = 0x02;
    orderBytes[2] = 0x03;
    orderBytes[3] = 0x04;
  }

  if (debug)
    cout << "# Width: " << width << "   Height: " << height;

  z.init(width, height);
  z.bitread(in);

  if (!stmMatchOrder(orderBytes)) {
    for (x = 0; x < width; x++)
    for (y = 0; y < height; y++) {
      unsigned short v = z.ref(x, y);

      v = ((v << 8) & 0xff00) | ((v >> 8) & 0xff);
      if (nobasin)
	v = max(basinshift, v);

      z.ref(x, y) = v;
    }
  }
  else if (nobasin) {
    for (x = 0; x < width; x++)
    for (y = 0; y < height; y++) {
      unsigned short v = z.ref(x, y);

      v = max(basinshift, v);

      z.ref(x, y) = v;
    }
  }

  //
  // The data is stored with (0,0) in the upper left ala image
  // coordinates.  However, GL will display things with (0,0) in the
  // lower left corner.  So we'll just flip the data.
  // This has no affect on the TIN that is generated.
  // It only changes the display.
  //
  if (type != 3)
    for (y = 0; y < height / 2; y++)
    for (x = 0; x < width; x++) {
      unsigned short tmp = z.ref(x, y);
      z.ref(x, y) = z.ref(x, height - 1 - y);
      z.ref(x, height - 1 - y) = tmp;
    }

  zmax = -MINFLOAT;
  zmin =  MAXFLOAT;

  for (x = 0; x < width; x++)
  for (y = 0; y < height; y++) {
    Real val = (Real)z.val(x, y);

    if (val != DEM_BAD) {
      if (val > zmax) zmax = val;
      if (val < zmin) zmin = val;
    }
  }

  if (debug)
    cerr << "# zmin=" << zmin << ", zmax=" << zmax << endl;
}

RGBAdata::RGBAdata(ifstream& in) {
  int width, height, maxval;
//int x, y;
  char head[3];
//char orderBytes[4];
  int type = 0;

  /* read head */
  in >> head[0] >> head[1] >> head[2];//"STM";

  /* detect file-format */
  {
    type = 3;

    in.seekg(0, ios_base::end);
    maxval = in.tellg();
    in.seekg(0, ios_base::beg);

    maxval /= sizeof(unsigned long);

    if ((rwsizex <= 0) && (rwsizey <= 0)) {
      /* favor landscape mode */
      for (int rwy = 1; (rwy <= 32) && (rwsizey <= 0); rwy++)
      for (int rwx = 1; (rwx <= 32) && (rwsizex <= 0); rwx++)
	if (maxval == ((rwx * rwy) * (rasterx * rastery))) {
	  rwsizex = rwx;
	  rwsizey = rwy;
	  break;
	}
    }
    else if ((rwsizex > 0) && (rwsizey <= 0)) {
      rwsizey = maxval / rwsizex;
    }
    else if ((rwsizex <= 0) && (rwsizey > 0)) {
      rwsizex = maxval / rwsizey;
    }

    if ((rwsizex > 0) && (rwsizey > 0)) {
      if (maxval != (rwsizex * rwsizey)) {
	cerr << "error: " << maxval << " != " << rwsizex << " * " << rwsizey << endl;
	exit(0);
      }
    }

    width  = rwsizex;
    height = rwsizey;
  }

  if (debug)
    cout << "# Width: " << width << "   Height: " << height;

  c.init(width, height);
  c.bitread(in);
}
