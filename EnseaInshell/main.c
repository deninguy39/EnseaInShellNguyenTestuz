#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>



#include "constantHeader.h"
#include "program.c"

int main(){
    char console_input[MAX_INPUT_SIZE];
    int son_status;
    char buf[BUFSIZE];

    write (STDOUT_FILENO,WELCOME_MSG,strlen(WELCOME_MSG));  

   

    while(1){

        struct timespec start, stop;
        int time;
       

        write (STDOUT_FILENO,ENSEASH,strlen(ENSEASH));  
        int nbBytes=read(STDIN_FILENO,console_input,MAX_INPUT_SIZE);

        console_input[nbBytes-1]=0;

        //in the case we want to stop
        if (strcmp(console_input, "exit") == 0) {
            write (STDOUT_FILENO,BYE,strlen(BYE));
            exit(EXIT_SUCCESS);
        }
       
        //start of the clock here
        clock_gettime(CLOCK_REALTIME, &start);
           

        pid_t pid = fork();

        if (pid==-1){
            perror("fork failed");
            exit(EXIT_FAILURE);  
        }

        //in the son process
        if(pid==0){
            //arguments recovery
            char splitter[]=" ";
            int i = 0;
            char **list_arg = NULL;
            char *arg = strtok(console_input,splitter);

             while (arg != NULL){
                // memory allocation
                list_arg = realloc(list_arg,(i+1)*sizeof(char *));
                list_arg[i]= malloc(strlen(arg));
                strcpy(list_arg[i],arg);

                arg = strtok(NULL,splitter);
                i++;
                }
       

            execvp(list_arg[0],list_arg);
            free(list_arg);
            perror("execvp failed");
            exit(EXIT_FAILURE);
            }
           
        //in the parent process
        else{
            wait(&son_status);

            //get the status of the son when exited
            if (WIFEXITED(son_status)){
                clock_gettime( CLOCK_REALTIME, &stop);
                time=(stop.tv_nsec-start.tv_nsec);
                sprintf(buf, "enseash [exit:%d]|%dms %%\n", WEXITSTATUS(son_status),time/1000000); // the time is always not correct (always 0ms)
                write(STDOUT_FILENO,buf,strlen(buf));
            }

            //if the son process exit because of a signal
            else if (WIFSIGNALED(son_status)){
                sprintf(buf, "enseash [sign:%d] %%\n", WTERMSIG(son_status));
                write(STDOUT_FILENO,buf,strlen(buf));
            }
        }
    }
    return EXIT_SUCCESS;
    }