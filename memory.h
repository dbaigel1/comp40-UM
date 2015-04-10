/* memory.h
 * Created by Owen Elliott and Daniel Baigel
 * Created on 3/29/2015
 * Summary -- details the interface of segmented memory
 */

#include <stdlib.h>
#include <stdio.h>
#include "seq.h"
#include "stdint.h"

struct Segment {
        uint32_t *array;
        uint32_t length;
}; 

typedef struct Segment *Segment;

/* creates a sequence that represents segmented memory, where the first index 
 * points to a dynamic array and the rest point to static arrays. 
 */
Seq_T make_seq(uint32_t *array);
 
/* finds the location in memory and returns the value in that location. rB is
 * register B which is the segment number, and rC is the offset 
 */
uint32_t segmented_load(Seq_T sequence, uint32_t rB, uint32_t rC); 
 
/* stores the value in $m[$r[A]][$r[B]] in $r[C] in the sequence */
void segmented_store(Seq_T sequence, uint32_t rA, uint32_t rB, uint32_t rC);

/* map segment adds a segment (index) to the end of the sequence, of length
 * specified in register C 
 */
void map_segment(Seq_T sequence, uint32_t rC);

/* map a segment popped off from the stack */
void map_stacked_segment(Seq_T sequence, uint32_t index, uint32_t rC);

/* deletes the array at index rC and sets it to point to NULL */
void unmap_segment(Seq_T sequence, uint32_t rC);

/* segment $m[$r[B]] is duplicated and replaces $m[0] */
void program_load(Seq_T sequence, uint32_t rB);

/* frees the sequence and all the segments in it */
void free_sequence(Seq_T sequence);


