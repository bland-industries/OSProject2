//  Forking function and branch node and root functions.
//  parent.h
//  prog2
//
//  Created by Thomas Verstraete on 2/8/14.
//  Copyright (c) 2014 Thomas Verstraete. All rights reserved.
//
#ifndef prog2_parent_h
#define prog2_parent_h

//The number of children each node has.
//to change this the comparisons need to pe change.

#define NUMBER_OF_CHILDREN 2
#include <signal.h>


/* This initiates the process forking and acts at the root node.
 * 
 * Param int amount: number of leaf nodes and thus files to parse.
 */
void startParent (int amount);


/* Function to construct the pipes and fork the processes.
 * This is a recursive function that calls on itself to create
 * more branch nodes.
 *
 * Param int offset: Used by leaf nodes to determine the file
 *      to open.
 * Param int amount: Used by leaf nodes to determine the file
 *      to open.
 *Param int tLevel: Depth level of the node in the tree.
 *
 * Return: amount value of that particular node in the tree.
 */
int makeChildren (int offset, int amount, int tLevel);


/* Function used in IPC to tell the node to read from its children 
 * and pass along the comparison result.
 *
 * Param int sigNum: signel type.
 */
void sendParentItem (int sigNum);


/* Function to send the comparison between two items to either
 * the parent through the pipe or if the root to the file.
 */
void sendBothItems ();


/* Function to send a single childs item along the tree.
 * This is when one child is no longer active.
 */
void sendingSingleChild ();


/* Reads the next item from the pipe array at index num.
 *
 * Param int num: index of child pipe to read from.
 */
void readNextItem (int num);


/* Function used to write to either the pipe or the file. 
 */
void writeToPipe ();


/* Check to see if pipe at index num has any more valid items.
 *
 * Param int num: index of pipe to check.
 *
 * Return: C type boolean of true or false
 */
int checkIfEmpty(int num);


/*Clean up of spawned processes.
 */
void closeParentProcesses (int sigNum);

#endif
