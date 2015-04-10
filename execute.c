/* execute.c
 * Created by Owen Elliott and Daniel Baigel
 * Created on 3/29/2015
 * Summary -- This file's job is to execute the uint32_t instructions in 
 * segment 0. It uses the functions in memory.c to help execute the instructions
 * that are involved with segmented memory. 
 */

#include "execute.h"
#include "memory.h"
#include "bitpack.h"
#include "stack.h"

#define WIDTH 4
#define EOF_WORD -1


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
        Seq_T wordSeq = make_seq(wordArray);
        /* make stack for unmapped segments */       
        Stack_T stack = Stack_new();
        uint32_t registers[8];
        for (uint32_t p = 0; p < 8; p++){
                registers[p] = 0;
        }
        int program_counter = 0;
        uint32_t rA, rB, rC, word, opcode;
        while (program_counter != -10) {
                word = segmented_load(wordSeq, 0, program_counter);
                opcode = Bitpack_getu(word, WIDTH, 28);
                if (opcode == 13) { /* load value */
                        rA = Bitpack_getu(word, 3, 25);
                        registers[rA] = Bitpack_getu(word, 25, 0);
                        program_counter++;
                } else { 
                        /* extract registers */
                        rA = Bitpack_getu(word, 3, 6);
                        rB = Bitpack_getu(word, 3, 3);
                        rC = Bitpack_getu(word, 3, 0);

                        if (opcode == 0) { /* conditional move */
                                if (registers[rC] != 0) {
                                        registers[rA] = registers[rB];
                                }
                        } else if (opcode == 1) { /* segmented load */
                                registers[rA] = segmented_load(wordSeq,
                                                               registers[rB], 
                                                               registers[rC]);
                        } else if (opcode == 2) { /* segmented store */
                                segmented_store(wordSeq, registers[rA], 
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
                                free_sequence(wordSeq);
                                Stack_free(&stack);
                                exit(0); 
                        } else if (opcode == 8) { /* map segment */
                                if (Stack_empty(stack) == 1) { /* stack empty */
                                        map_segment(wordSeq, registers[rC]);
                                        registers[rB] = Seq_length(wordSeq)-1;
                                } else {
                                        registers[rB] = (uint32_t )(uintptr_t)
                                                        Stack_pop(stack);
                                        map_stacked_segment(wordSeq, 
                                                            registers[rB], 
                                                            registers[rC]);
                                }
                        } else if (opcode == 9) { /* unmap segment */
                                Stack_push(stack, 
                                           (void *)(uintptr_t)registers[rC]);
                                unmap_segment(wordSeq, registers[rC]);
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
                                        program_load(wordSeq, registers[rB]);
                                }
                                program_counter = registers[rC] - 1; 
                        }
                        program_counter++;
                }
        }
}

