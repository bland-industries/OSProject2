//  Forking function and branch node and root functions.
//  parent.c
//  prog2
//
//  Created by Thomas Verstraete on 2/8/14.
//  Copyright (c) 2014 Thomas Verstraete. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "leaf.h"
#include "parent.h"
#include "tokenize.h"

//PIDs of both child processes.
int pids[NUMBER_OF_CHILDREN];

//Pipe file descriptors.
//[0] & [1] are for child of this nodes.
//[2] is for parent of this node.
int fd[(NUMBER_OF_CHILDREN + 1)][2];

//Two element array of the two items to compare
word_freq* words[NUMBER_OF_CHILDREN];

//Does the child have any active children.
int childActive[NUMBER_OF_CHILDREN];

//nextRead: -1 = read both; 0-1 read that child in array.
//isMain: used to determine what to do if the root node.
int nextRead, isMain;

//Global Variable for logging purposes.
int treeLevel;

//File to write output into.
FILE *writeFile;

/* This initiates the process forking and acts at the root node.
 *
 * Param int amount: number of leaf nodes and thus files to parse.
 */
void startParent (int amount) {
    
    int val = makeChildren (0, amount, 0);
    //Stops any non-root nodes from continuing on.
    while (val < amount) {
        pause();
    }
    
    //Set up name of outgoing file.
    char fName[15];
    snprintf(fName, 15, "Out%d.txt", amount);
    
    //Open a file to write to.
    writeFile = fopen(fName, "w");
    if (writeFile == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    
    //Only root node reaches this.
    isMain = 1;
    
    //allow processes to propegate.
    sleep(2);
    
    //Collect all the items.
    while (childActive[0] || childActive[1]) {
        sendParentItem (0);
    }
    
    //Clean Up.
    fclose(writeFile);
    closeParentProcesses(0);
}

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
int makeChildren (int offset, int amount, int tLevel) {
    
    //Set global variables.
    treeLevel = tLevel;
    
    //Set ups for iteration through the passed items
    childActive[0] = 1;
    childActive[1] = 1;
    nextRead = -1;
    isMain = 0;
    
    //Variables used to coordinate processes.
    int newAmount;
    int newOffset;
    
    //Create the pipes and the child processes
    for (int createLoop = 0; createLoop < NUMBER_OF_CHILDREN; createLoop++) {
        
        //sets up the perameters for the child process calls
        newAmount = amount/2;
        newOffset = offset + (createLoop * newAmount);
        
        //mallocs the space for the passed items
        words[createLoop] = malloc(sizeof(word_freq));
        
        //create a pipe
        if (pipe (fd[createLoop]) < 0) {
            perror ("plumbing problem");
            exit(1);
        }
        
        //Fork to new process
        pids[createLoop] = fork();
        
        //error checking
        if (pids[createLoop] < 0) {
            //Run time error, exit gracefully
            perror("Fork failed");
            exit(1);
        }
        else if (pids[createLoop] > 0) {
            //Parent Process

            printf("Node at level %d spawned two children\n", treeLevel);
            
            //close write portion of pipe
            //all data transfer passes from child to PARENT
            close (fd[createLoop][WRITE]);
            
            //set up signal handling for the branch nodes
            signal(SIGINT, closeParentProcesses);
            signal(SIGUSR1, sendParentItem);

        } else {
            //This is the child: either a branch or leaf node
            
            if (newAmount == 1) {
                //Create the leaf nodes
                
                //close read portion of pipe
                //all data transfer passes from CHILD to parent
                close (fd[createLoop][READ]);
                
                //turn this process into a leaf node
                startLeaf(newOffset, fd[createLoop]);
                
                return 1;
            } else {
                //this is for the branch nodes who are children
                //this will call back around and run through this again
                
                //close read portion of pipe
                //all data transfer passes from CHILD to parent
                close (fd[createLoop][READ]);
                
                //move this processes "child to parent" pipe so when it creates
                //its own children the pipe is not overwriten.
                memcpy(fd[2], fd[createLoop], (size_t) sizeof(fd[createLoop]));
                
                //this child branch node needs to make child nodes
                return makeChildren(newOffset, newAmount, (treeLevel+1));
                
            }
            
        }//END of child portion of if statement
        
    } //End of for loop creating multiple child processes
    
    //ONLY the parent nodes makes it here.
    
    //Return the number sent
    return amount;
}


/* Function used in IPC to tell the node to read from its children
 * and pass along the comparison result.
 *
 * Param int sigNum: signel type.
 */
void sendParentItem (int sigNum) {
    //If both children are active.
    if (childActive[0] && childActive[1]) {
        sendBothItems();
        
    } else if ((!childActive[0]) && (!childActive[1])){
        //if both children are inactive send back EOF type item.
        word_freq* endStruct = malloc(sizeof(word_freq));
        memcpy(endStruct->word, END_ITEM, sizeof(END_ITEM));
        endStruct->freq = 0;
        write (fd[2][WRITE], (const void *) endStruct, (size_t) sizeof(word_freq));
        
    } else {
        //If only a single child is active.
        sendingSingleChild();
    }
}

/* Function to send the comparison between two items to either
 * the parent through the pipe or if the root to the file.
 */
void sendBothItems () {
    
    //read the next one and call child to send next
    //side effect is filling the global variable
    for (int i = 0; i < NUMBER_OF_CHILDREN && childActive[i]; i++) {
        readNextItem(i);
    }
    
    //check if either of the freq is zero for either process, then that
    //child is empty then convert to the single read path.
    if (checkIfEmpty(0)) { //pipe [0] is empty/
        if (checkIfEmpty(1)) { //and pipe [1] is empty/
            if (isMain) {
                printf("PROGRAM COMPLETE\n");
            } else {
                printf("Node at level %d completed both child nodes\n", treeLevel);
                write (fd[2][WRITE], (const void *) words[0], (size_t) sizeof(word_freq));
            }
            return;
        } else {
            writeToPipe ();
        }
    
    } else if (checkIfEmpty(1)) { //pipe [1] is empty.
        if (checkIfEmpty(0)) { //and pipe [0] is empty.
            if (isMain) {
                printf("PROGRAM COMPLETE\n");
            } else {
                printf("Node at level %d completed both child nodes\n", treeLevel);
                write (fd[2][WRITE], (const void *) words[0], (size_t) sizeof(word_freq));
            }
            return;
        } else {
            writeToPipe ();
        }
        
    } else { //Camparison between two items. Compare the Strings.
        
        int compVal = strcmp(words[0]->word, words[1]->word);
        
        if (compVal < 0) { //[0] is before [1]
            //need to send up [0]
            nextRead = 0;
            
            writeToPipe ();
            
        } else if (compVal > 0) { //[1] is before [0]
            //need to send up [1]
            nextRead = 1;
            
            writeToPipe ();
            
        } else { //they are the same
            //need to send up Combo
            
            words[0]->freq += words[1]->freq;
            nextRead = -1;
            
            if (isMain) {
                printf("Root received: %s (%d)\n", words[0]->word, words[0]->freq);
                fprintf(writeFile, "%s (%d)\n", words[0]->word, words[0]->freq);
            } else {
                //Write the remaining item to pipe up the tree.
                write (fd[2][WRITE], (const void *) words[0], (size_t) sizeof(word_freq));
                
            }
        }
    }
}


/* Function to send a single childs item along the tree.
 * This is when one child is no longer active.
 */
void sendingSingleChild () {
    readNextItem(nextRead);
    
    if (checkIfEmpty(nextRead)) {
        //is not active anymore.
        if (isMain) {
            printf("PROGRAM COMPLETE\n");
        } else {
            printf("Node at level %d completed both child nodes\n", treeLevel);
            write (fd[2][WRITE], (const void *) words[0], (size_t) sizeof(word_freq));
        }
    } else {
        //Not empty.
        writeToPipe ();
    }
    
}


/* Reads the next item from the pipe array at index num.
 *
 * Param int num: index of child pipe to read from.
 */
void readNextItem (int num) {
    
    //Determine if to read next item from child num.
    if ((nextRead == -1 || nextRead == num) && childActive[num]) {
        kill(pids[num], SIGUSR1);

        read(fd[num][READ], (void *) words[num], (size_t) sizeof (word_freq));
    }

}


/* Function used to write to either the pipe or the file.
 */
void writeToPipe () {
    if (isMain) {
        //Root node output.
        printf("Root received: %s (%d)\n", words[nextRead]->word, words[nextRead]->freq);
        fprintf(writeFile, "%s (%d)\n", words[nextRead]->word, words[nextRead]->freq);
    } else {
        //Branch node output.
        //Write the remaining item to pipe up the tree.
        write (fd[2][WRITE], (const void *) words[nextRead], (size_t) sizeof(word_freq));
    }
}


/* Check to see if pipe at index num has any more valid items.
 *
 * Param int num: index of pipe to check.
 *
 * Return: C type boolean of true or false
 */
int checkIfEmpty(int num) {
    if (words[num]->freq == 0 && childActive[num]) { //pipe [num] is empty
        //send only [1] from now on
        childActive[num] = 0;
        nextRead = (num + 1) % 2;
        return 1;
    } else {
        return 0;
    }
}


/*Clean up of spawned processes.
 */
void closeParentProcesses (int sigNum) {
    for (int i = 0; i < NUMBER_OF_CHILDREN; i++) {
        kill(pids[i], SIGINT);
    }
    exit(0);
}