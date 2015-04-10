
/* memory.c
 * Created by Owen Elliott and Daniel Baigel
 * Created on 3/29/2015
 * Summary - the implementation of segmented memory
 */

#include "memory.h"
#include <assert.h>


/* makes the initial segmented memory. Makes a sequence and sets index 0 to 
 * point to the dyanmic array that gets passed in.
 */
Seq_T make_seq(uint32_t *array_old)
{
        Segment seg1 = malloc(sizeof(struct Segment));
        assert(seg1 != NULL);
        seg1->array = array_old;
        Seq_T wordSequence = Seq_new(0);
        Seq_addlo(wordSequence, seg1);
        
        return wordSequence;
}

/* finds the location in memory at [rB][rC]. Returns the value in that location.
 */
uint32_t segmented_load(Seq_T sequence, uint32_t rB, uint32_t rC)
{
        Segment seg1 = Seq_get(sequence, rB);
        return seg1->array[rC];
} 
 
/* stores the value in [rC] at m[rA][rB] */
void segmented_store(Seq_T sequence, uint32_t rA, uint32_t rB, uint32_t rC)
{
        Segment seg1 = Seq_get(sequence, rA);
        seg1->array[rB] = rC;
}

/* map segment adds a segment to the end of the sequence, of length specified 
 * in rC. 
 */
void map_segment(Seq_T sequence, uint32_t rC)
{
        Segment seg1 = malloc(sizeof(struct Segment));
        seg1->array = calloc(rC, sizeof(uint32_t));
        seg1->length = rC;
        Seq_addhi(sequence, seg1);         
}

/* maps a segment to the index that was popped off the stack. The segment is of
 * length specified in rC
 */
void map_stacked_segment(Seq_T sequence, uint32_t index, uint32_t rC)
{
        Segment seg1 = malloc(sizeof(struct Segment));
        assert(seg1 != NULL);
        seg1->array = calloc(rC, sizeof(uint32_t));
        seg1->length = rC;
        Seq_put(sequence, index, seg1);
}

/* deletes the segment at index rC and sets it to point to NULL */
void unmap_segment(Seq_T sequence, uint32_t rC)
{
        Segment seg1 = Seq_get(sequence, rC);
        free(seg1->array);
        free(seg1);
        Seq_put(sequence, rC, NULL);
}

/* removes the old segment 0. Copies the segment at index rB and sets the
 * duplicate to be the new segment 0.
 */
void program_load(Seq_T sequence, uint32_t rB)
{
        Segment seg1 = Seq_get(sequence, rB);
        uint32_t length = seg1->length;
        Segment seg2 = malloc(sizeof(struct Segment));
        assert(seg2 != NULL);
        seg2->array = calloc(length, sizeof(uint32_t));
        assert(seg2->array != NULL);
        seg2->length = length;

        for (uint32_t i = 0; i < length; i++) {
                seg2->array[i] = seg1->array[i];
        }

        seg1 = Seq_get(sequence, 0);
        free(seg1->array);
        free(seg1);
        Seq_remlo(sequence);
        Seq_addlo(sequence, seg2);
}

/* frees the sequence and the segments stored within */
void free_sequence(Seq_T sequence)
{
        int length = Seq_length(sequence);
        for (int i = 0; i < length; i++) {
                if (Seq_get(sequence, i) != NULL) {
                        Segment seg = Seq_get(sequence, i);
                        free(seg->array);
                        free(seg);
                }
        }
        Seq_free(&sequence);
}


