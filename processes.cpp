#include <stdio.h>
#include <iostream>
#include <unistd.h>    //for fork, pipe
#include <stdlib.h>    //for exit
#include <sys/wait.h>  //for wait
#include <string.h>
using namespace std;

// main function for processing 3 different commands: ps -A, grep, wc -l
int main(int argc, char* argv[])
{
    // initialize variables
    enum {READ, WRITE}; // READ = 0 | WRITE = 1
    pid_t pid1, pid2, pid3;
    // file descriptor array format:
    // 0 - cin | 1 - cout | 2 - cerr
    int pipe_fd1[2];
    int pipe_fd2[2];

    // check argument from the terminal
    if (argc < 1)
    {
        cout << "Error Syntax!" << endl;
        cout << "./processes <name of the process>" << endl;
        exit(EXIT_FAILURE);
    }

    // create pipe1 for all processes
    if (pipe(pipe_fd1) < 0)
    {
        perror("ERROR! Unable to create pipe 1");
        exit(EXIT_FAILURE);
    }
    // create pipe2 for all processes
    if (pipe(pipe_fd2) < 0)
    {
        perror("ERROR! Unable to create pipe 2");
        exit(EXIT_FAILURE);
    }
    // bash fork() -> 'ps' process
    pid1 = fork();
    if(pid1 < 0)
    {
        perror("Unable to fork!");
        exit(EXIT_FAILURE);
    }
    else if(pid1 == 0)
    {
        // 'wc -l' process fork() -> 'grep' process
        pid2 = fork();
        if (pid2 < 0)
        {
            perror("Unable to fork child!");
            exit(EXIT_FAILURE);
        }
        else if (pid2 == 0)
        {
            // 'grep' process fork() -> 'ps -A' process
            pid3 = fork();
            if (pid3 < 0)
            {
                perror("Unable to fork grand child!");
                exit(EXIT_FAILURE);
            }
            else if (pid3 == 0)
            {
                // - Great-grand child (ps -A) -
                // close unused file descriptors from both pipe
                close(pipe_fd1[READ]);
                close(pipe_fd2[WRITE]);
                close(pipe_fd2[READ]);
                // duplicate data from ps to pipe 1 (write to pipe1)
                dup2(pipe_fd1[WRITE], WRITE);
                // replace process with ps command
                execlp("ps", "ps", "-A", (char*) 0);
                close(pipe_fd1[WRITE]); // close after done writing to pipe1
            }
            else
            {
                // - Grand child (grep argv[1]) -
                // close unused file descriptor from both pipe
                close(pipe_fd1[WRITE]);
                close(pipe_fd2[READ]);
                // duplicate data from pipe1 to grep (read from pipe1)
                dup2(pipe_fd1[READ],READ);
                close(pipe_fd1[READ]); // close after done reading
                // duplicate data from grep to pipe 2 (write to pipe2)
                dup2(pipe_fd2[WRITE], WRITE);
                // replace process with grep command
                execlp("grep", "grep", argv[1], (char*) 0);
                close(pipe_fd2[WRITE]); // close when done writing to pipe 2
            }
        }
        else
        {
            // Child (wc -l)
            // close unused file descriptor from both pipe
            close(pipe_fd1[WRITE]);
            close(pipe_fd1[READ]);
            close(pipe_fd2[WRITE]);
            // duplicate data from pipe 2 to wc (read from pipe2)
            dup2(pipe_fd2[READ], READ);
            execlp("wc", "wc", "-l", (char*)0);
            close(pipe_fd2[READ]); // close after done reading from pipe 2
        }
    }
    else
    {
        // close all pipe when done processing
        close(pipe_fd1[READ]);
        close(pipe_fd1[WRITE]);
        close(pipe_fd2[READ]);
        close(pipe_fd2[WRITE]);
        // Parent (Bash)
        wait(NULL); // waits for process termination
    }
    return 0; // complete program successfully
}

