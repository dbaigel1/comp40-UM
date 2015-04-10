/* loader.h
 * Created by Owen Elliott and Daniel Baigel
 * Created on 3/28/2015
 * Summary: details the interface for loader.c so to make the original segment 0
 */

#include <stdlib.h>
#include "stdint.h"
#include <stdio.h>

uint32_t * load_program(FILE *wordFile);

uint32_t expand_array(uint32_t **wordArray, uint32_t size);

