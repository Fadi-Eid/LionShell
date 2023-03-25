/* Main program of the customized UNIX Shell
 * By Fadi EID 19/3/2023
 * Open Source, free to use
 * https://github.com/Fadi-Eid */

#include "shellFunctions.h"


int main(int argc, char* argv[])
{
    char command[MAX_COMMAND_SIZE];
    int command_size;
    int args_size;
    char* args[MAX_ARGS_SIZE];
    char history[MAX_COMMAND_SIZE];
    int isHistory = 0;  // make 1 if there is a command in history
    int last_command_size; // for history feature
    int conc = 0;   // 1 for concurency

    // loop until the user commands "exit"
    while (1)
    {
        printf("\033[1m\033[3m\033[31m\nLionShell >       \033[0m"); // print in red
        fflush(stdout);
        
        if(getCommand(command, MAX_COMMAND_SIZE, &command_size) == -1 )
        {
            continue;
        }

        //check for special operator (!!)-> History
        if(checkHistory(command, command_size) == 1)  // if the user enters !! (execute the last command)
        {
            if(isHistory == 0)
            {
                printf("No commands in history\n");
                continue;
            }
            if(strcpy(command, history) == NULL)
            {
                perror("strcpy");
                continue;
            }
            command_size = last_command_size;
        }
        else
        {
            isHistory = 1;
            last_command_size = command_size;
            if(strcpy(history, command) == NULL)
            {
                perror("strcpy");
                continue;
            }
        }

        

        // parse the command into sub commands (arguments or args)
        if(parseCommand(command, command_size, args, &args_size) == -1)
        {
            // perror("parseCommand");
            continue;
        }

        // check for special commands
        int SC = checkSC(args, args_size);
        int isSC = 0;
        switch(SC)
        {
            case(-1):
                break;
            case(SC_CLEAR):
                system("clear");
                isSC = 1;
                break;
            case(SC_EXIT):
                for (int i = 0; i < args_size; i++)
                    free(args[i]);
                printf("Shell released\n\n");
                exit(0);
            case(SC_HELP):
                printf("HELP COMMAND UNDER CONSTRUCTION\n");
                isSC = 1;
                break;
            default:
                break;
        }

        if(isSC)
        {
            // free dynamically allocated memory in parseCommand function
            for (int i = 0; i < args_size; i++)
                free(args[i]);
            continue;
        }

        
        // check for concurency
        if((conc=checkConc(args,args_size)) == 1)
        {
            args[args_size-1] = NULL;
        }

        // check for all the special operators and act accordingly
        int pipeIndex, outputRedIndex, inputRedIndex;
        // check for pipe
        pipeIndex = checkPipe(args, args_size);




        // Check for the pipe operator
        if(pipeIndex!=-1) // there is the pipe operator
        {
            if(pipeIndex == 0 || pipeIndex == args_size -1)
            {
                printf("Incorrect use of the pipe operator\n");
                // free dynamically allocated memory in parseCommand function
                for (int i = 0; i < args_size; i++) 
                    free(args[i]);
                continue;
            }

            // split the command into 2 commands (command1 and command2)
            char* args1[MAX_ARGS_SIZE/2];
            char* args2[MAX_ARGS_SIZE/2];
            int args1_size=0, args2_size=0;

            // copy elements to args1 or args2 based on their position
            int j = 0;
            for(int i=0; i<args_size; i++)
            {
                if(i<pipeIndex)
                {
                    if((args1[i] = strdup(args[i])) == NULL)
                    {
                        perror("strdup");
                        break;
                    }
                    args1_size++;
                }
                else if(i>pipeIndex)
                {
                    if( (args2[j] = strdup(args[i])) == NULL )
                    {
                        perror("strdup");
                        break;
                    }
                    args2_size++;
                    j++;
                }
            }

            // set the last element of args1 and args2 to NULL
            args1[args1_size] = NULL;
            args2[args2_size] = NULL;


            // run analysis on the first command (check for input and output redirection)
            if(checkPipe(args2, args2_size)!=-1) // if there is another pipe in the second command
            {
                printf("Multiple piping is not supported\n");
                // free dynamically allocated memory in args, args1 and args2
                for (int i = 0; i < args_size; i++)
                    free(args[i]);
                for (int i = 0; i < args1_size; i++)
                    free(args1[i]);
                for (int i = 0; i < args2_size; i++)
                    free(args2[i]);
                continue;
            }

            // check for output redirection in the first command
            if(checkOutputRed(args1, args1_size) != -1 || checkInputRed(args1, args1_size)!=-1 ||
                                checkInputRed(args2, args2_size)!=-1 || checkOutputRed(args2, args2_size)!=-1)
            {
                printf("I/O redirection is not supported with pipes\n");
                // free dynamically allocated memory in args, args1 and args2
                for (int i = 0; i < args_size; i++)
                    free(args[i]);
                for (int i = 0; i < args1_size; i++)
                    free(args1[i]);
                for (int i = 0; i < args2_size; i++)
                    free(args2[i]);
                continue;
            }

            // create a pipe and execute the command
            int fd[2];
            if(pipe(fd) == -1)
            {
                perror("pipe");
                // free dynamically allocated memory in args, args1 and args2
                for (int i = 0; i < args_size; i++)
                    free(args[i]);
                for (int i = 0; i < args1_size; i++)
                    free(args1[i]);
                for (int i = 0; i < args2_size; i++)
                    free(args2[i]);
                continue;
            }

            // create 2 processes for the 2 commands
            int pid1 = fork();
            if(pid1 == -1)
            {
                perror("fork");
                // free dynamically allocated memory in args, args1 and args2
                for (int i = 0; i < args_size; i++)
                    free(args[i]);
                for (int i = 0; i < args1_size; i++)
                    free(args1[i]);
                for (int i = 0; i < args2_size; i++)
                    free(args2[i]);
                continue;
            }
            else if(pid1 == 0) // child process 1
            {
                /*child process for command 1*/

                // redirect the output to the pipe
                close(fd[0]);
                dup2(fd[1], STDOUT_FILENO); // fd[1] is the write end of the pipe
                close(fd[1]);

                if(execvp(args1[0],args1) == -1)
                {
                    perror("execvp_pipe");
                    // free dynamically allocated memory in args, args1 and args2
                    for (int i = 0; i < args_size; i++)
                        free(args[i]);
                    for (int i = 0; i < args1_size; i++)
                        free(args1[i]);
                    for (int i = 0; i < args2_size; i++)
                        free(args2[i]);
                    exit(1);
                }
            }

            // main process

            int pid2 = fork();

            if(pid2 == -1)
            {
                perror("fork");
                // free dynamically allocated memory in args, args1 and args2
                for (int i = 0; i < args_size; i++)
                    free(args[i]);
                for (int i = 0; i < args1_size; i++)
                    free(args1[i]);
                for (int i = 0; i < args2_size; i++)
                    free(args2[i]);
                continue;
            }

            if(pid2 == 0) // child process 2
            {
                // execute commmand 2
                close(fd[1]);
                // redirect the input of the second command
                if(dup2(fd[0], STDIN_FILENO)==-1) // fd[0] reading end of the pipe
                {
                    perror("dup2");
                    // free dynamically allocated memory in args, args1 and args2
                    for (int i = 0; i < args_size; i++)
                        free(args[i]);
                    for (int i = 0; i < args1_size; i++)
                        free(args1[i]);
                    for (int i = 0; i < args2_size; i++)
                        free(args2[i]);
                    exit(1);
                }

                close(fd[0]);

                if(execvp(args2[0], args2) == -1)
                {
                    perror("execvp_pipe_2");
                    // free dynamically allocated memory in args, args1 and args2
                    for (int i = 0; i < args_size; i++)
                        free(args[i]);
                    for (int i = 0; i < args1_size; i++)
                        free(args1[i]);
                    for (int i = 0; i < args2_size; i++)
                        free(args2[i]);
                    exit(1);
                }


            }

            close(fd[0]);
            close(fd[1]);

            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);


            // free dynamically allocated memory in args, args1 and args2
            for (int i = 0; i < args_size; i++)
                free(args[i]);
            for (int i = 0; i < args1_size; i++)
                free(args1[i]);
            for (int i = 0; i < args2_size; i++)
                free(args2[i]);
            continue;


        }

        else // no pipes, check for input and output redirection and act accordingly
        {
            int inputRedIndex = checkInputRed(args, args_size);
            int outputRedIndex = checkOutputRed(args, args_size);

            int pid = fork();
            if(pid == -1)
            {
                perror("fork");
                for (int i = 0; i < args_size; i++) {
                        free(args[i]);
                }
                continue;
            }

            if(pid == 0) // child process
            {
                if(outputRedIndex!=-1)
                {
                    char outputFileName[MAX_ARGS_SIZE];
                    if(strcpy(outputFileName, args[outputRedIndex+1]) == NULL)
                    {
                        perror("strcpy");
                        for (int i = 0; i < args_size; i++)
                            free(args[i]);
                        exit(2);
                    }

                    // open the output file
                    int file = open(outputFileName, O_WRONLY | O_CREAT);
                    if(file == -1)
                    {
                        perror("open_output_file");
                        for (int i = 0; i < args_size; i++)
                            free(args[i]);
                        exit(2);
                    }

                    // redirect output
                    if(dup2(file, STDOUT_FILENO)==-1)
                    {
                        perror("dup2");
                        for (int i = 0; i < args_size; i++)
                            free(args[i]);
                        exit(2);
                    }
                    close(file);
                    args[outputRedIndex] = NULL;
                }

                if(inputRedIndex!=-1) // input and output rediretcion
                {
                    char inputFileName[MAX_ARGS_SIZE];
                    if(strcpy(inputFileName, args[inputRedIndex+1]) == NULL)
                    {
                        perror("strcpy");
                        for (int i = 0; i < args_size; i++)
                            free(args[i]);
                    }

                    // open the output file
                    int file = open(inputFileName, O_RDONLY);
                    if(file == -1)
                    {
                        perror("open_input_file");
                        for (int i = 0; i < args_size; i++)
                            free(args[i]);
                        exit(2);
                    }

                    // redirect input
                    if(dup2(file, STDIN_FILENO)==-1)
                    {
                        perror("dup2");
                        for (int i = 0; i < args_size; i++)
                            free(args[i]);
                        exit(2);
                    }
                    close(file);
                    args[inputRedIndex] = NULL;
                }

                // execute the command
                if(execvp(args[0], args) == -1)
                {
                    perror("execvp");
                    for (int i = 0; i < args_size; i++)
                            free(args[i]);
                        exit(2);
                }

            }

            if(!conc)
            {
                waitpid(pid, NULL, 0);
            }

            for (int i = 0; i < args_size; i++)
                free(args[i]);


        }


    }

    return 0;
}