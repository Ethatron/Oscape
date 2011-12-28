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
// heap.C
//
// This file implements the basic heap structure used by the simplification
// software.

#include "scape.H"

// This is used for accounting purposes
int heap_cost = 0;



// Heap::swap --
//
// Swaps two nodes in the heap.
//
void Heap::swap(int i, int j) {
  heap_node tmp = node[i];

  node[i] = node[j];
  node[j] = tmp;

  node[i].tri->setLocation(i);
  node[j].tri->setLocation(j);

  heap_cost++;
}


// Heap::upheap --
//
// The given node will be moved up in the heap, if necessary.
//
// NOTE: This function (as well as downheap) performs more swapping
// than is strictly necessary.
//
void Heap::upheap(int i) {
  if (i == 0)
    return;

  if (node[i].val > node[parent(i)].val) {
    swap(i, parent(i));
    upheap(parent(i));
  }
}

// Heap::downheap --
//
// The given node is moved down through the heap, if necessary.
//
void Heap::downheap(int i) {
  // perhaps just extracted the last
  if (i >= extent)
    return;

  int largest = i,
    l = left(i),
    r = right(i);

  if (l < extent && node[l].val > node[largest].val) largest = l;
  if (r < extent && node[r].val > node[largest].val) largest = r;

  if (largest != i) {
    swap(i, largest);
    downheap(largest);
  }
}

// Heap::insert --
//
// Insert the given triangle into the heap using the specified key value.
//
void Heap::insert(Triangle *t, Real v) {
  int i = extent++;

  node[i].tri = t;
  node[i].val = v;

  node[i].tri->setLocation(i);

  upheap(i);
}

// Heap::extract --
//
// Extract the top element from the heap and return it.
//
heap_node *Heap::extract() {
  if (extent < 1)
    return 0;

  swap(0, extent - 1);
  extent--;

  downheap(0);

  node[extent].tri->setLocation(NOT_IN_HEAP);

  return &node[extent];
}

// Heap::kill --
//
// Kill a given node in the heap.
//
heap_node& Heap::kill(int i) {
  if (i >= extent)
    cerr << "ATTEMPT TO DELETE OUTSIDE OF RANGE" << endl;

  swap(i, extent - 1);
  extent--;

  if (node[i].val < node[extent].val)
    downheap(i);
  else
    upheap(i);

  node[extent].tri->setLocation(NOT_IN_HEAP);

  return node[extent];
}


// Heap::update --
//
// This function is called when the key value of the given node has
// changed.  It will record this change and reorder the heap if
// necessary.
//
void Heap::update(int i,Real v)
{
  assert(i < extent);

  Real old = node[i].val;
  node[i].val = v;

  if (v < old)
    downheap(i);
  else
    upheap(i);
}
