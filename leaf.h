//  The leaf Node in the tree. This starts the tokenizing and holds the items in an array to pass up the tree.
//  leaf.h
//  prog2
//
//  Created by Thomas Verstraete on 2/8/14.
//  Copyright (c) 2014 Thomas Verstraete. All rights reserved.
//

#ifndef prog2_leaf_h
#define prog2_leaf_h

#include <signal.h>

#define READ 0
#define WRITE 1



/* This begins the child, or leaf node process of loading the contents of the file, storing them and creating the sturucture to pass to the parent.
 *
 * Param int num: the file number to load.
 * Param int fdPipe[2]: the pipe to write the itmes to.
 */
void startLeaf(int num, int fdPipe[2]);


/* This is used for the parent to call the leaf node to send another packet of information. It is done through IPC.
 *
 * Param int sigNum: Signal type
 */
void sendLeafItem (int sigNum);

//Function called by SIGINT to end this child process
void closeLeafProcess (int sigNum);


#endif
