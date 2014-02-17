//  The leaf Node in the tree. This starts the tokenizing and holds the items in an array to pass up the tree.
//  leaf.c
//  prog2
//
//  Created by Thomas Verstraete on 2/8/14.
//  Copyright (c) 2014 Thomas Verstraete. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "leaf.h"
#include "tokenize.h"
#include <signal.h>

word_freq* all_words;
int wordPT;
int fdPipe[2];
int leafNum;
int wordTotal;

/* This begins the child, or leaf node process of loading the contents of the file, storing them and creating the sturucture to pass to the parent.
 *
 * Param int num: the file number to load.
 * Param int fdPipe[2]: the pipe to write the itmes to.
 */
void startLeaf(int num, int fdP[2]){
    //printf("C21: startLeaf (%d)\n", num);
    leafNum = num;
    
    //variable to keep track of the number of words tokenized
    int nbr_words;
    
    //set pointer for passing item array.
    wordPT = 0;
    
    //Set up the signal handeling for the leaf nodes
    signal(SIGINT, closeLeafProcess);
    signal(SIGUSR1, sendLeafItem);
    
    //copy file descriptor into a global variable.
    memcpy(fdPipe, fdP, sizeof(int[2]));
    
    //create string of the file name to open based on the integer value
    char fName[15];
    snprintf(fName, 15, "file%d.txt", (num+1));
    
    //tokenize the items in file fName and return the struct array
    all_words = start_tokenize(fName, &nbr_words);
    
    printf("File %s is tokenized and ready.\n", fName);
}


/* This is used for the parent to call the leaf node to send another packet of information. It is done through IPC.
 *
 * Param int sigNum: Signal type
 */
void sendLeafItem (int sigNum) {

    //Write the next item into the pipe.
    write (fdPipe[1], (const void *) &all_words[wordPT], (size_t) sizeof(word_freq));

    //incrament word array pointer.
    wordPT++;
}

//Function called by SIGINT to end this child process
void closeLeafProcess (int sigNum) {
    exit(0);
}