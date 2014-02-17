//  Functions used to tokenize a text file and create a struct consisting of a string and the freqency of that string in the file.
//  tokenize.h
//  prog2
//
//  Created by Thomas Verstraete on 2/8/14.
//  Copyright (c) 2014 Thomas Verstraete. All rights reserved.
//

#ifndef prog2_tokenize_h
#define prog2_tokenize_h

//Longest word in a dictionary
#define WORD_SIZE 45

//String after all alphabetic charactors
#define END_ITEM "|||||||||||||||||||||||||||"

//struction to count the individual words
struct word_freq {
    char word[WORD_SIZE];
    int freq;
};

typedef struct word_freq word_freq;

#define ASCEND		1
#define DESCEND		-1


/*Function used for the qsort*/
int sort_order;	// used by compare()


/* Starts the tokeninzing process 
 *
 * Param char* fileName: Name of the File to tokenize.
 * Param int* num: Address for number of items tokenized.
 *
 * Return word_freq* array of all the tokenized struct.
 */
word_freq* start_tokenize(char* fileName, int* num);


/* Reads text from a file, tokenizes,
 * and returns an array of strings/words found in input.
 * 
 * Param int *nbr_words: Address for number of items tokenized.
 * Param FILE* fd: Open file descriptor to tokenize.
 *
 * Return char**: Array of strings of all the words from the file.
 */
char** tokenize(int *nbr_words, FILE* fd);


/* Builds the array of word_freq structs from array of strings.
 *
 * Param char** list: Array of strings of all the words from the file.
 * Param int size: number of items in the list array.
 * Param word_freq* all_words: Array of the words.
 *
 * Return int: Number of items in the all_words array.
 */
int build_struct_word_freq(char **list, int size, word_freq* all_words);

/*
 * Frees the memory allocated for the words in the array as well as the memory allocated for the array itself.
 * 
 * Param char** list: Array of strings of to be freed.
 * Param int size: Number of items in the list array.
 */
void free_memory(char **list, int size);

/* Function used to sort out the list of works*/
int compare(const void *a, const void *b);

#endif
