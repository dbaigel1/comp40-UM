/* execute.h
 * Created by Daniel Baigel and Owen Elliott
 * Created on 3/29/2015
 * Summary -- Processes instructions and calls functions to execute them.
 */

#include <stdlib.h>
#include <stdio.h>
#include "stdint.h"
//#include "instructions.h"

struct Segment {
        uint32_t *array;
        uint32_t length;
}; 

typedef struct Segment *Segment;


struct Memstruct {
        int length, size;
        Segment *segments;
};

typedef struct Memstruct *Memstruct;

struct Stack {
        int length, size;
        int *segs;
};

typedef struct Stack *Stack;

void execute_um(uint32_t *wordArray);
