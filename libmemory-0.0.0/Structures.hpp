/* Version: MPL 1.1
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
 * The Original Code is The Amiga Research OS, short AROS.
 *
 * Copyright (C) 1998-2001 AROS - The Amiga Research OS
 * Copyright (C) 2004-2011 Ethatron <niels@paradice-insight.us>.
 * All Rights Reserved.
 */

#ifndef	MEMORY_STRUCTURES_HPP
#define	MEMORY_STRUCTURES_HPP

#include "libmemory-0.0.0/Defines.hpp"

struct MinNode {

public:
  struct MinNode *ln_Succ,
		 *ln_Pred;

public:
  void Remove() {
    /*
	Unfortunately, there is no (quick) check that the node
	is in a list.
    */

    /*
	Just bend the pointers around the node, ie. we make our
	predecessor point to our successor and vice versa
    */
    this->ln_Pred->ln_Succ = this->ln_Succ;
    this->ln_Succ->ln_Pred = this->ln_Pred;
  } /* Remove */
};

struct SysNode : public MinNode {
    char	 *ln_Name;
    unsigned char ln_Type;
    char	  ln_Pri;
};

struct MinList {

public:
  struct MinNode *lh_Head,
		 *lh_Tail,
		 *lh_TailPred;

public:
  void NewList() {
    this->lh_TailPred = (struct SysNode *)this;
    this->lh_Tail     = 0;
    this->lh_Head     = (struct SysNode *)&this->lh_Tail;
  }

  void AddHead(struct MinNode *node) {
    /*
	Make the node point to the old first node in the list and to the
	head of the list.
    */
    node->ln_Succ	   =                    this->lh_Head;
    node->ln_Pred	   = (struct MinNode *)&this->lh_Head;

    /*
	New we come before the old first node which must now point to us
	and the same applies to the pointer to-the-first-node in the
	head of the list.
    */
    this->lh_Head->ln_Pred = node;
    this->lh_Head	   = node;
  } /* AddHead */

  void AddTail(struct MinNode *node) {
    /*
	Make the node point to the head of the list. Our predecessor is the
	previous last node of the list.
    */
    node->ln_Succ	       = (struct MinNode *)&this->lh_Tail;
    node->ln_Pred	       = this->lh_TailPred;

    /*
	Now we are the last now. Make the old last node point to us
	and the pointer to the last node, too.
    */
    this->lh_TailPred->ln_Succ = node;
    this->lh_TailPred	       = node;
  } /* AddTail */

  struct MinNode *RemHead() {
    struct MinNode * node;

    /*
	Unfortunately, there is no (quick) check that the node
	is in a list
    */

    /* Get the address of the first node or NULL */
    node = lh_Head->ln_Succ;
    if (node) {
      node->ln_Pred = (struct MinNode *)&this->lh_Head;
      node          =                    this->lh_Head;
      this->lh_Head = node->ln_Succ;
    }

    /* Return the address or NULL */
    return node;
  } /* RemHead */

  struct MinNode *RemTail() {
    struct MinNode * node;

    /*
	Unfortunately, there is no (quick) check that the node
	is in a list.
    */

    /* Get the last node of the list */
    if ((node = this->lh_TailPred))
    {
	/* normal code to remove a node if there is one */
	node->ln_Pred->ln_Succ = node->ln_Succ;
	node->ln_Succ->ln_Pred = node->ln_Pred;
    }

    /* return it's address or NULL if there was no node */
    return node;
  } /* RemTail */

  void Insert(struct MinNode *node, struct MinNode *pred) {
    /* If we have a node to insert behind... */
    if (pred) {
      /* Is this the last node in the list ? */
      if (pred->ln_Succ) { /* Normal node ? */
	/*
	    Our successor is the successor of the node we add ourselves
	    behind and our predecessor is just the node itself.
	 */
	node->ln_Succ = pred->ln_Succ;
	node->ln_Pred = pred;

	/*
	    We are the predecessor of the successor of our predecessor
	    (What ? blblblb... ;) and of our predecessor itself.
	    Note that here the sequence is quite important since
	    we need ln_Succ in the first expression and change it in
	    the second.
	  */
	pred->ln_Succ->ln_Pred = node;
	pred->ln_Succ          = node;
      }
      else { /* last node */
	/*
	    Add the node at the end of the list.
	    Make the node point to the head of the list. Our
	    predecessor is the previous last node of the list.
	 */
	node->ln_Succ = (struct SysNode *)&this->lh_Tail;
	node->ln_Pred = this->lh_TailPred;

	/*
	    Now we are the last now. Make the old last node point to us
	    and the pointer to the last node, too.
	 */
	this->lh_TailPred->ln_Succ = node;
	this->lh_TailPred	   = node;
      }
    }
    else {
	/*
	    add at the top of the list. I do not use AddHead() here but
	    write the code twice for two reasons: 1. The code is small and
	    quite prone to errors and 2. If I would call AddHead(), it
	    would take almost as long to call the function as the execution
	    would take yielding 100% overhead.
	*/
      node->ln_Succ = this->lh_Head;
      node->ln_Pred = (struct SysNode *)&this->lh_Head;
      this->lh_Head->ln_Pred = node;
      this->lh_Head = node;
    }
  } /* Insert */
};

struct SysList : public MinList {
    unsigned char lh_Type;
    unsigned char l_pad;
};

#endif	//MEMORY_STRUCTURES_HPP
