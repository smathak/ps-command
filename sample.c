#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#define MAX 1000


int main(){
        printf("hello system programming\n");

         char buffer[MAX];

        int myfifo_fd2;
        myfifo_fd2 = open("myfifo2", O_RDWR);
        //printf("myfifo_fd2 in sample: %d\n", myfifo_fd2);

        int pid = getpid();
        int ppid = getppid();

        char *cpid = (char*)malloc(sizeof(char)*MAX);
        char *cppid = (char*)malloc(sizeof(char)*MAX);

        sprintf(cpid, "%d", pid);
        sprintf(cppid, "%d", ppid);

        strcat(cppid, " ");
        strcat(cppid, cpid);
        strcat(cppid, " ");

        if(write(myfifo_fd2, cppid, strlen(cppid))<0){
                perror("fail to write to myfifo");
        }

        sleep(120);

        printf("good bye system programming\n");

    exit(7);
}
