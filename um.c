/*
 * um_main.c
 * Created by Owen Elliott and Daniel Baigel
 * Created on 3/28/2015
 * Summary -- This is the home of main, our program manager. It opens the file
 * and passes the needed information to load_program and execute_um which are
 * implemented in other files.  
 */

#include <stdlib.h>
#include <stdio.h>
#include "loader.h"
#include "memory.h"
#include "execute.h"
        
int main(int argc, char *argv[])
{
        (void) argc;
                
        FILE *fp = fopen(argv[1], "r");
        if (fp == NULL) { 
                exit(1);
        }

        uint32_t *wordArray;
        wordArray = load_program(fp);
        fclose(fp);

        execute_um(wordArray);

        return (EXIT_SUCCESS);
}
