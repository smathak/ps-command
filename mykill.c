#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "error.h"
#define MAX 1000

int main(){
        int nread;
        char buffer[MAX];

        int res = mkfifo("/system_programming/2017/lab/class_a/spa15/HW2/myfifo5", 0777);
        if(res == 0){
                //printf("myfifo5 created\n");
        }
        else {
            //perror("myfifo5 already exists");
        }

        int myfifo_fd5 = open("myfifo5", O_RDWR|O_NONBLOCK);
        //printf("myfifo_fd5 in myps: %d\n", myfifo_fd5);

        if((nread = read(myfifo_fd5, buffer, MAX))>0){
                //printf("mykill: %s\n", buffer);
                char *ptr = strtok(buffer, " ");
                char **array = (char**)malloc(sizeof(char*)*2);
                int i=0;

                while(ptr!=NULL){
                        array[i++] = ptr;
                        ptr = strtok(NULL, " ");
                }

                int pid = atoi(array[1]);

                printf("Pid that you input: %d\n", pid);
                int result = kill(pid, SIGKILL);
                if(result == 0){
                        printf("kill success\n\n");
                }
        }

        return 0;
}
