# LionShell
Simple Shell that runs on a native Linux shell.
This code is a basic implementation of a shell program that provides a command-line interface to the user. Here is a brief description of the functions and features of this code:

The main function initializes various variables required for the shell program and enters an infinite loop until the user types the "exit" command.
The program prompts the user for input using a red-colored text string: "LionShell >"
The getCommand() function reads the input from the user and stores it in the variable 'command' until the user presses the Enter key. The function returns -1 if the input is empty or exceeds the maximum allowed size.
The program checks if the user has entered any special operator (!!)-> History, and executes the last command from the history if the user has entered '!!'.
The command is stored in the 'history' variable.
The parseCommand() function takes the 'command' string and splits it into an array of arguments. The function returns -1 if there is an error in parsing the command.
The program checks for special commands like 'clear', 'exit', and 'help'. If the user enters any of these commands, the program takes appropriate action, like clearing the console or terminating the shell program.
The program checks for concurrency using the '&' symbol and launches a separate process for the command if concurrency is detected.
The program checks for special operators like '|', '<', '>', and '>>' and takes appropriate action based on the operator found in the command.
If the user enters an invalid command, the program prints an error message and waits for the user to enter a valid command.
Overall, this code provides a basic implementation of a shell program that allows the user to enter commands, executes them, and provides a few additional features like command history and concurrency.
