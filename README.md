## LionShell – A Custom Minimalist Linux Shell
LionShell is a simple yet functional command-line shell implemented in C. It provides a basic interface to interact with the Linux operating system, mimicking essential features of a typical Unix shell. Designed as a learning project, LionShell demonstrates key concepts like command parsing, process creation, I/O redirection, and piping.

## Features
* Command Execution: Runs standard Linux commands with support for arguments.
* Prompt: Custom red-colored prompt LionShell >.
* History (!!): Recall and re-execute the last command.

## Built-in Commands:

* exit: Terminates the shell.
* clean: Clears the terminal.
* help: Displays a help message.
* Concurrency (&): Execute commands in the background.
* Pipes (|): One-level piping between two commands.

## I/O Redirection:
* Input (<)
* Output overwrite (>)
* Output append (>>) (not currently implemented)

## Basic Error Handling: 
* Safe recovery from parsing and execution errors.
* Memory used by dynamic parsing is freed after each command.

⚠️ Multiple pipes and combined I/O redirection with pipes are not supported in this version.

## How It Works
1. The shell continuously prompts the user until exit is typed.
2. User input is parsed into commands and arguments.
3. Special symbols like |, <, >, and & are detected and handled accordingly.
4. The shell uses fork() and execvp() to create child processes for command execution.

