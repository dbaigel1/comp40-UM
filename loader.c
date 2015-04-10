/*
 * loader.c 
 * Created by Owen Elliott and Daniel Baigel
 *            oellio01         dbaige01
 * Created on 3/28/2015
 * summary -- This file's job is to set up the original segment 0. 
 */


#include <loader.h>
#include "bitpack.h"
#include <assert.h>

#define WORD_LENGTH 8

/* load_program takes in a file which has the instructions and it makes the 
 * file into a properly formatted segment 0. It returns the array with all the
 * uint32_t instructions at the end of the function.
 */  
uint32_t * load_program(FILE *wordFile)
{
        uint32_t counter = 0;
        int a, b, c, d;
        uint32_t word = 0;
        uint32_t size = 10;
        uint32_t returner = ~0;
        
        uint32_t *wordArray = calloc(size, sizeof(uint32_t));
        assert(wordArray != NULL);                

        a = fgetc(wordFile);        

        while (a != EOF) {
                
                b = fgetc(wordFile);
                c = fgetc(wordFile);
                d = fgetc(wordFile);

                word = 0;

                /* pack the instruction with a, b, c, d */
                word = Bitpack_newu(word, WORD_LENGTH, 24, (uint32_t)a);
                word = Bitpack_newu(word, WORD_LENGTH, 16, (uint32_t)b);
                word = Bitpack_newu(word, WORD_LENGTH, 8, (uint32_t)c);
                word = Bitpack_newu(word, WORD_LENGTH, 0, (uint32_t)d);
                
                if((counter + 1) >= size) {
                        size = expand_array(&wordArray, size);
                }

                wordArray[counter] = word;
                counter++;
                
                a = fgetc(wordFile);
        }

        /*set the last instruction to returner which is our sentinal for EOF*/
        wordArray[counter] = returner;
        return wordArray;
}

/* expands the dynamic array */
uint32_t expand_array(uint32_t **wordArray, uint32_t size)
{
        uint32_t *newArray = malloc(sizeof(uint32_t)*size*2);   
        assert(newArray != NULL);
     
        for (uint32_t i = 0; i < size; i++) {
                newArray[i] = (*wordArray)[i];
        }

        free(*wordArray);
        *wordArray = newArray;
        size *= 2;
       
        return size;
}
