/* execute.c
 * Created by Owen Elliott and Daniel Baigel
 * Created on 3/29/2015
 * Summary -- This file's job is to execute the uint32_t instructions in 
 * segment 0. It uses the functions in memory.c to help execute the instructions
 * that are involved with segmented memory. 
 */

#include "execute.h"
#include "bitpack.h"
#include "stack.h"
#include <assert.h>

#define WIDTH 4
#define EOF_WORD -1

/* makes the initial segmented memory. Makes a sequence and sets index 0 to 
 * point to the dyanmic array that gets passed in.
 * INITIAL SIZE OF DYNAMIC ARRAY IS 12
 */
static inline Memstruct make_memory(uint32_t *array_old)
{
        Memstruct memory1 = malloc(sizeof(struct Memstruct));
        assert(memory1 != NULL); 
        memory1->segments = malloc(sizeof(struct Segment) * 10000);
        assert(memory1->segments != NULL);
        memory1->length = 0;
        memory1->size = 10000;
        Segment seg1 = malloc(sizeof(struct Segment));
        assert(seg1 != NULL);

        seg1->array = array_old;
        memory1->segments[0] = seg1;
        memory1->length++;
        
        return memory1;
}

/* expands the dynamic array */
static inline void expand(Memstruct memory)
{
        Segment *segs = malloc(sizeof(struct Segment) * (memory->size * 2));
        assert(segs != NULL);        
     
        int length = memory->length;
        for (int i = 0; i < length; i++) {
                if (memory->segments != NULL){
                        segs[i] = memory->segments[i];
                }
        }

        free(memory->segments);
        memory->segments = segs;
        memory->size *= 2;
}

/* finds the location in memory at [rB][rC]. Returns the value in that location.
 */
static inline uint32_t segmented_load(Memstruct memory, uint32_t rB, uint32_t rC)
{
        Segment seg1 = memory->segments[rB];
        return seg1->array[rC];
} 
 
/* stores the value in [rC] at m[rA][rB] */
static inline void segmented_store(Memstruct memory, uint32_t rA, uint32_t rB, uint32_t rC)
{
        Segment seg1 = memory->segments[rA];
        seg1->array[rB] = rC;
}

/* map segment adds a segment to the end of the sequence, of length specified 
 * in rC. 
 */
static inline void map_segment(Memstruct memory, uint32_t rC)
{
        Segment seg1 = malloc(sizeof(struct Segment));
        seg1->array = calloc(rC, sizeof(uint32_t));
        seg1->length = rC;

        if (memory->length + 1 >= memory->size) {
                expand(memory);
        }

        memory->segments[memory->length]= seg1;
        memory->length++;   
}

/* maps a segment to the index that was popped off the stack. The segment is of
 * length specified in rC
 */
static inline void map_stacked_segment(Memstruct memory, uint32_t index, uint32_t rC)
{
        Segment seg1 = malloc(sizeof(struct Segment));
        assert(seg1 != NULL);
        seg1->array = calloc(rC, sizeof(uint32_t));
        seg1->length = rC;
        memory->segments[index] = seg1;
}

/* deletes the segment at index rC and sets it to point to NULL */
static inline void unmap_segment(Memstruct memory, uint32_t rC)
{
        Segment seg1 = memory->segments[rC];
        free(seg1->array);
        free(seg1);
        memory->segments[rC] = NULL;
}

/* removes the old segment 0. Copies the segment at index rB and sets the
 * duplicate to be the new segment 0.
 */
static inline void program_load(Memstruct memory, uint32_t rB)
{
        Segment seg1 = memory->segments[rB];
        uint32_t length = seg1->length;
        Segment seg2 = malloc(sizeof(struct Segment));
        assert(seg2 != NULL);
        seg2->array = calloc(length, sizeof(uint32_t));
        assert(seg2->array != NULL);
        seg2->length = length;

        for (uint32_t i = 0; i < length; i++) {
                seg2->array[i] = seg1->array[i];
        }

        seg1 = memory->segments[0];
        free(seg1->array);
        free(seg1);
        memory->segments[0] = seg2;
}

/* frees the sequence and the segments stored within */
static inline void free_memory(Memstruct memory)
{
        int length = memory->length;
        for (int i = 0; i < length; i++) {
                if (memory->segments[i] != NULL) {
                        Segment seg = memory->segments[i];
                        free(seg->array);
                        free(seg);
                }
        }
        free(memory->segments);
        free(memory);
}

/* DYNAMIC ARRAY THAT SIMULATES THE STACK */

static inline int stack_is_empty(Stack stack)
{
        return stack->length;
} 

static inline void expand_stack(Stack stack)
{
        int *temp = malloc(sizeof(int) * (stack->size * 2));
        assert(temp != NULL); 

        int length = stack->length;       
        for (int i = 0; i < length; i++) {
                temp[i] = stack->segs[i];
        }

        free(stack->segs);
        stack->segs = temp;
        stack->size *= 2;
}

static inline void stack_push(Stack stack, uint32_t value)
{
        /* check if stack is full */
        if (stack->length + 1 >= stack->size) {
                 expand_stack(stack);
        }

        stack->segs[stack->length] = value;
        stack->length++;
}

static inline uint32_t stack_pop(Stack stack)
{
        stack->length--;
        return stack->segs[stack->length];
}



/* 
 * Purpose: executes the UM. Goes through the instructions and executes the
 *          program.
 * Args: uint32_t *array that represents segment 0. 
 * Returns: void.
 * Notes: calls make_seq in memory.c to set up segmented memory and it makes a
 * stack where the indices on the unmapped segments will be housed. Then it goes
 * through the instructions, parces the opcodes and the registers and executes
 * the instruction. 
 */
void execute_um(uint32_t *wordArray)
{
        Memstruct memArray = make_memory(wordArray);

        /* make stack for unmapped segments */       
        Stack stack = malloc(sizeof(struct Stack));
        stack->segs = malloc(sizeof(int) * 1000);
        stack->size = 1000;
        stack->length = 0;

        uint32_t registers[8];
        for (uint32_t p = 0; p < 8; p++){
                registers[p] = 0;
        }
        int program_counter = 0;
        uint32_t rA, rB, rC, word, opcode, tmp_word;
        while (program_counter != -10) {
                word = segmented_load(memArray, 0, program_counter);
                tmp_word = word;
                opcode = tmp_word >> 28;
                if (opcode == 13) { /* load value */
                        tmp_word = word;
                        tmp_word = tmp_word << 4;
                        rA = tmp_word >> 29;

                        tmp_word = word;
                        tmp_word = tmp_word << 7;
                        registers[rA] = tmp_word >> 7;
                        program_counter++;
                } else { 
                        /* extract registers */
                        tmp_word = word;
                        tmp_word = tmp_word << 23;
                        rA = tmp_word >> 29;
                        tmp_word = word;
                        tmp_word = tmp_word << 26;
                        rB = tmp_word >> 29;
                        tmp_word = word;
                        tmp_word = tmp_word << 29;
                        rC = tmp_word >> 29;

                        if (opcode == 0) { /* conditional move */
                                if (registers[rC] != 0) {
                                        registers[rA] = registers[rB];
                                }
                        } else if (opcode == 1) { /* segmented load */
                                registers[rA] = segmented_load(memArray,
                                                               registers[rB], 
                                                               registers[rC]);
                        } else if (opcode == 2) { /* segmented store */
                                segmented_store(memArray, registers[rA], 
                                                registers[rB], registers[rC]);
                        } else if (opcode == 3) { /* addition */
                                registers[rA] = (registers[rB] + registers[rC])
                                                 % 4294967296;
                        } else if (opcode == 4) { /* multiplication */
                                registers[rA] = (registers[rB] * registers[rC])
                                                 % 4294967296;          
                        } else if (opcode == 5) { /* division */
                                registers[rA] = (registers[rB] / 
                                                 registers[rC]);
                        } else if (opcode == 6) { /* nand */
                                registers[rA] = ~(registers[rB] & 
                                                  registers[rC]);
                        } else if (opcode == 7) { /* halt */
                                free_memory(memArray);
                                free(stack->segs);
                                free(stack);
                                exit(0); 
                        } else if (opcode == 8) { /* map segment */
                                if (stack_is_empty(stack) == 0) { /* stack empty */
                                        map_segment(memArray, registers[rC]);
                                        registers[rB] = memArray->length - 1;
                                } else {
                                        registers[rB] = (uint32_t )(uintptr_t)
                                                        stack_pop(stack);
                                        map_stacked_segment(memArray, 
                                                            registers[rB], 
                                                            registers[rC]);
                                }
                        } else if (opcode == 9) { /* unmap segment */
                                stack_push(stack, registers[rC]);
                                unmap_segment(memArray, registers[rC]);
                                        
                        } else if (opcode == 10) { /* output */
                                        putchar(registers[rC]);            
                        } else if (opcode == 11) { /* input */
                                FILE *wordFile = stdin;
                                uint32_t c = getc(wordFile);
                                if (c == (uint32_t) EOF) {
                                        c = EOF_WORD;
                                }
                                        registers[rC] = c;
                        } else if (opcode == 12) { /* load program */
                                if (registers[rB] != 0) {
                                        program_load(memArray, registers[rB]);
                                }
                                program_counter = registers[rC] - 1; 
                        }
                        program_counter++;
                }
        }
}




