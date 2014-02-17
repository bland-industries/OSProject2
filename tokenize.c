//  Functions used to tokenize a text file and create a struct consisting of a string and the freqency of that string in the file.
//  tokenize.c
//  prog2
//
//  Created by Thomas Verstraete on 2/8/14.
//  Copyright (c) 2014 Thomas Verstraete. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "tokenize.h"

int wordPT;

/* Starts the tokeninzing process
 *
 * Param char* fileName: Name of the File to tokenize.
 * Param int* num: Address for number of items tokenized.
 *
 * Return word_freq* array of all the tokenized struct.
 */
word_freq* start_tokenize(char* fileName, int* num) {
    //Set up variables for tokenizing process.
    wordPT = 0;
	char **wordlist;
	int nbr_words;
    sort_order = ASCEND;
    
    //open the file to load
    FILE * fd = fopen(fileName, "r");
    
	// tokenize the input text read from standard input (or redirected from a file)
	wordlist = tokenize(num, fd);
    
    //close the file
    fclose(fd);
    
	// sort the array of words using qsort() from <stdlib.h>
	qsort(wordlist, *num, sizeof(char *), compare);
    
    //allocate memory for the structure.
    word_freq* all_words = (word_freq*) calloc(*num, sizeof(word_freq));
    
	// print sorted words with frequency count
    nbr_words = build_struct_word_freq(wordlist, *num, all_words);
    
	// free memory space allocated for the array and words
	free_memory(wordlist, *num);
    
    *num = nbr_words;
	return all_words;
}

/* Reads text from a file, tokenizes,
 * and returns an array of strings/words found in input.
 *
 * Param int *nbr_words: Address for number of items tokenized.
 * Param FILE* fd: Open file descriptor to tokenize.
 *
 * Return char**: Array of strings of all the words from the file.
 */
char  **tokenize(int *nbr_words, FILE* fd) {
    //Set up variables.
    int array_size = 80;
    *nbr_words = 0;
    int word_size = 20;
    int word_count = 0;
    char x;
    unsigned char c;
    
    //Allocate space for the list array.
    char **list = (char **) calloc(array_size, sizeof(char *)); //allocates space for the array
    //Allocate space for a single word.
    char *word = malloc(word_size);
    
    //Loop to get a charactor from file and processes.
    while  ( ( x = fgetc( fd ) ) != EOF )
    {
        c = x;
        
        if (isalpha(c)) //the char scanned is an alphabetic charactor
        {
            //printf("153: is alpha\n");
            //resize word to fit more charactors
            if (word_count == word_size) {
                word_size *=2;
                word = realloc(word, word_size);
            }
            
            //add char to word
            word[word_count++] = (char) c;
        }
        
        else  //the char scanned is NOT an alphabetic charactor
        {
            if (word_count != 0) {
                
                //checks to see if there is enough size for the end of string char
                if (word_count == word_size) {
                    word_size +=1;
                    word = realloc(word, word_size);
                    word[word_count++] = 0;
                } else {    //enough room now make smaller
                    word = realloc(word, word_count);
                }
                
                //resize array to fit more words
                if (*nbr_words == array_size) {
                    array_size *= 2;
                    list = (char **) realloc(list, array_size * sizeof(char *));
                }
                
                //add word to array and count it
                list[*nbr_words] = word;
                *nbr_words += 1;
                
                //carve new space for new word and prep for it
                word_size = 20;
                word = malloc(word_size);
                word_count = 0;
            }
        }
    }
    return list;
}


/* Builds the array of word_freq structs from array of strings.
 *
 * Param char** list: Array of strings of all the words from the file.
 * Param int size: number of items in the list array.
 * Param word_freq* all_words: Array of the words.
 *
 * Return int: Number of items in the all_words array.
 */
int build_struct_word_freq(char **list, int size, word_freq* all_words) {
    //Set up variables.
    int word_count=0;
    
    //loop through each of the items in the array.
    for (int i = 0; i < size; i++) {
        //Allocate space for each item.
        word_freq *temp;
        temp = malloc(sizeof(word_freq));
        
        //Copy from the list array to the struct array.
        //Add frequency count of each word.
        strcpy(temp->word, list[i]);
        temp->freq = 1;
        
        if ((i < size-1) && (strcmp(list[i], list[i+1]) == 0)) {
            //
            while ((i < size-1) && (strcmp(list[i], list[i+1]) == 0)) {
                temp->freq++;
                i++;
            }
        }
        //Add item to array and incrament counter.
        all_words[word_count] = *temp;
        word_count++;
    }
    
    //Add an end of list item to indicate the end of the list.
    word_freq* endStruct = malloc(sizeof(word_freq));
    memcpy(endStruct->word, END_ITEM, sizeof(END_ITEM));
    endStruct->freq = 0;
    all_words[word_count] = *endStruct;
    
    return word_count;
}


/*
 * Frees the memory allocated for the words in the array as well as the memory allocated for the array itself.
 *
 * Param char** list: Array of strings of to be freed.
 * Param int size: Number of items in the list array.
 */
void free_memory(char **list, int size) {
    
    free(list);
}

// function used by qsort() of <stdlib.h>
int compare(const void *a, const void *b) {
	char* *s = (char* *) a;
	char* *t = (char* *) b;
	return sort_order * strcmp(*s, *t); // sort_order is -1 or 1
}