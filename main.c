//  The Bigginning.
//  main.c
//  prog2
//
//  Created by Thomas Verstraete on 2/8/14.
//  Copyright (c) 2014 Thomas Verstraete. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "parent.h"

/* Starts the whole process rolling by parsing out the arguments
 * and calling the parent function.
 */
int main (int argc, const char * argv[]) {
//    int num =  atoi(argv[1]);
//    int newNum = 4;
//    
//    
//    while (newNum < num) {
//        newNum = (int) pow((double) newNum,2);
//    }
    
    startParent(atoi(argv[1]));
}