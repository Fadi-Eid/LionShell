#include "shellDefines.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

// fucnction to read input command from the user
int getCommand(char* command, int max_size, int* command_size){
    int c;
    if(fgets(command, max_size, stdin) == NULL)
    {
        return -1;
    }

    // check if the string is > max_size
    if(command[strlen(command) - 1] != '\n')
    {
        printf("Command exceeded maximum size\n");
        // clean the buffered input
        while ((c = getchar()) != '\n' && c != EOF);
        return -1;  // exit the program with an error code
    }

    // check is the string is empty
    if(command[0] == '\0' || command[0] == '\n')
    {
        printf("\n");
        return -1;  // exit the program with an error code
    }


    // remove the newline character from the end
    command[strlen(command) - 1] = '\0';

    // check if the string only contains whitespaces
    int isWhite = 1;
    for(int i=0; i<strlen(command); i++)
    {
        if(command[i] != ' ' && command[i] != '\t')
        {
            isWhite = 0;
            break;
        }
    }
    if(isWhite == 1)
    {
        printf("\n");
        return -1;  // exit the program with an error code
    }

    *command_size = strlen(command); // the length of the command
    return 0; // success
}


// function that parses a command into sub commands or args
int parseCommand(char* command, int command_size, char* args[], int* args_size)
{
    int arg_index = 0;
    int i = 0;
    int start_index = 0;
    int end_index = 0;

    while (i < command_size) {
        if (command[i] == ' ' || command[i] == '\t') {
            if (end_index > start_index) {
                // Allocate memory for the argument and copy the substring
                args[arg_index] = (char*)malloc(end_index - start_index + 1);
                if (args[arg_index] == NULL) {
                    // Error handling: Unable to allocate memory
                    for (int j = 0; j < arg_index; j++) {
                        free(args[j]);
                    }
                    return -1;
                }
                strncpy(args[arg_index], command + start_index, end_index - start_index);
                args[arg_index][end_index - start_index] = '\0';
                arg_index++;
                start_index = end_index + 1;
            } else {
                start_index++;
            }
        }
        end_index++;
        i++;
    }

    // Add the last argument
    if (end_index > start_index) {
        args[arg_index] = (char*)malloc(end_index - start_index + 1);
        if (args[arg_index] == NULL) {
            // Error handling: Unable to allocate memory
            for (int j = 0; j < arg_index; j++) {
                free(args[j]);
            }
            return -1;
        }
        strncpy(args[arg_index], command + start_index, end_index - start_index);
        args[arg_index][end_index - start_index] = '\0';
        if (args[arg_index][strlen(args[arg_index]) - 1] == '\n') {
            args[arg_index][strlen(args[arg_index]) - 1] = '\0';
        }
        arg_index++;
    }

    int j;
    *args_size = 0;
    j = 0;

    for (i = 0; i < command_size; i++) {
        if (command[i] == ' ' || command[i] == '\n' || command[i] == '\r' || command[i] == '\t') {
            if (j != 0) {
                args[*args_size][j] = '\0';
                (*args_size)++;
                j = 0;
            }
        } else {
            args[*args_size][j] = command[i];
            j++;
        }
    }

    if (j != 0) {
        args[*args_size][j] = '\0';
        (*args_size)++;
    }

    args[*args_size] = NULL; // Set the last element to NULL to terminate the array

    return 0;
}


// Check for input redirection operator <
int checkInputRed(char* args[], int args_size)
{
    int found = -1;  // initially not found
    // return index if found
    for(int i=0; i<args_size; i++)
    {
        if(strcmp(args[i], "<")==0)
        {
            found = i;
            break;
        }
    }
    return found; // -1 for not found, index for found
}

// Check for output redirection operator >
int checkOutputRed(char* args[], int args_size)
{
    int found = -1;  // initially not found
    // return index if found
    for(int i=0; i<args_size; i++)
    {
        if(strcmp(args[i], ">")==0)
        {
            found = i;
            break;
        }
    }
    return found; // -1 for not found, index for found
}

// Check for pipe operator |
int checkPipe(char* args[], int args_size)
{
    int found = -1;  // initially not found
    // return index if found
    for(int i=0; i<args_size; i++)
    {
        if(strcmp(args[i], "|")==0)
        {
            found = i;
            break;
        }
    }
    return found; // -1 for not found, index for found
}

// Check for concurency operator &
int checkConc(char* args[], int args_size)
{
    int found = 0;  // initially not found
    // return 1 if found
    if(strcmp(args[args_size-1], "&") == 0) found = 1;
    return found; // 0 for not found, 1 for found
}

// Check for history operator !!
int checkHistory(char* command, int command_size)
{
    int found = 0;  // initially not found
    // return 1 if found
    if((strcmp(command, "!!") == 0)) found = 1;
    return found; // 0 for not found, 1 for found
}

// check for special commands
int checkSC(char* args[], int args_size) // return SC Code if found, -1 if not
{
    int code = -1;
    if(args_size > 1)
    {
        return code;
    }
    if(strcmp(args[0], "help")==0 || strcmp(args[0], "HELP")==0)
    {
        code = SC_HELP;
    }
    if(strcmp(args[0], "clear")==0 || strcmp(args[0], "CLEAR")==0)
    {
        code = SC_CLEAR;
    }
    if(strcmp(args[0], "exit")==0 || strcmp(args[0], "EXIT")==0)
    {
        code = SC_EXIT;
    }

    return code;
}   