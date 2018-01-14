[spa15@hpc3 HW2]$ cat hw2.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "error.h"
#define MAX 10000

void psCmd();

typedef struct Record{
        int ppid;
        int pid;
        struct Record *next;
}Node;

// linked list
typedef struct List{
        int count;
        Node *head;
}LinkedList;

void init(LinkedList *list){
        list->count = 0;
        list->head = NULL;
}

int isEmpty(LinkedList *list){
        if(list->count == 0) return 1;
        else return 0;
}

void insert(LinkedList *list, Node *temp){
        if(list->count == 0){
                        temp->next = list->head;
                        list->head = temp;
                        list->count++;
        }else{
                        Node *t = list->head;
                        int i;
                        for(i=0; i<list->count-1; i++){
                                        t = t->next;
                        }
                        temp->next = t->next;
                        t->next = temp;
                        list->count++;
        }
}


void display_jobs(LinkedList *list){
        Node* temp = list->head;

        int myfifo_fd3 = open("myfifo3", O_RDWR); // send it to myjobs

        char *buffer = (char*)malloc(sizeof(char)*MAX);
        memset(buffer, '\0', MAX);

        while(temp!=NULL){
                        char *ppid = (char*)malloc(sizeof(char)*MAX);
                        char *pid = (char*)malloc(sizeof(char)*MAX);

                        if(temp->ppid == 1){
                                sprintf(ppid, "%d", temp->ppid);
                                sprintf(pid, "%d", temp->pid);

                                strcat(buffer, ppid);
                                strcat(buffer, " ");
                                strcat(buffer, pid);
                                strcat(buffer, " ");
                        }

                        // in the case of background, ppid is 1
                        temp = temp->next;
        }

        if(write(myfifo_fd3, buffer, strlen(buffer))<0){
                        perror("fail to write to myfifo_fd3");
        }

        buffer = NULL;
}

void display_ps(LinkedList *list){
        Node* temp = list->head;

        int myfifo_fd4 = open("myfifo4", O_RDWR); // send it to myps

        char *buffer = (char*)malloc(sizeof(char)*MAX);
        memset(buffer, '\0', MAX);

        while(temp!=NULL){
                        char *ppid = (char*)malloc(sizeof(char)*MAX);
                        char *pid = (char*)malloc(sizeof(char)*MAX);

                        if(temp->ppid != 1){
                                sprintf(ppid, "%d", temp->ppid);
                                sprintf(pid, "%d", temp->pid);

                                strcat(buffer, ppid);
                                strcat(buffer, " ");
                                strcat(buffer, pid);
                                strcat(buffer, " ");
                        }

                        // in the case of background, ppid is 1
                        temp = temp->next;
        }
        if(write(myfifo_fd4, buffer, strlen(buffer))<0){
                        perror("fail to write to myfifo_fd4");
        }

        buffer = NULL;
}

int remove_by_pid(LinkedList* list, int pid){
        Node* prev = NULL;
        Node* temp = list->head;

        while(temp!= NULL && (temp->pid != pid)){
                prev = temp;
                temp = temp->next;
        }

        if(prev == NULL){
                if(temp == NULL){
                        printf("No nodes to delete\n");
                }else{
                        Node* p = list->head;
                        list->head = p->next;
                        free(p);
                        list->count -= 1;
                }
        }else{
                if(temp == NULL){
                        printf("No such node\n");
                }else{
                        prev->next = temp->next;
                        free(temp);
                        list->count -= 1;
                }
        }
                return list->count;
//      display(list);
}

int check_dead(LinkedList *list){
        Node *temp = list->head;
        int count = 0;
        while(temp!=NULL){
                int res = kill(temp->pid, 0);
                if(res == -1){
                        count = remove_by_pid(list, temp->pid);
                }
                temp = temp->next;
        }
        return count;
}

LinkedList list;
int getCount(){
        return list.count;
}

/***********************************************************************************/

void psCmd(){
        system("myps");
}

void jobsCmd(){
        system("myjobs");
}

void killCmd(char* command){
        system("mykill");
}

void child_finish(int arg){
        int status;
        printf("SIGCHILD: %d\n", arg);
}

/***********************************************************************************/

int scount = 0;
void readfromsample(){
        int res = mkfifo("/system_programming/2017/lab/class_a/spa15/HW2/myfifo2", 0777);
        if(res == 0) {
                //printf("myfifo2 created\n");
        }
        else {
                //perror("myfifo2 already exists");
        }
        int myfifo_fd2 = open("myfifo2", O_RDWR);
        //printf("myfifo_fd2 in main: %d\n", myfifo_fd2);

        int nread2, count = 0;
        char buffer2[MAX];

        while((nread2 = read(myfifo_fd2, buffer2, MAX))>0){
                buffer2[nread2] = '\0';

                char *ptr = strtok(buffer2, " ");
                int temp, i = 0;
                int array[6];
                while( ptr != NULL ){
                                int temp = atoi(ptr);
                                array[i++] = temp;
                                ptr = strtok(NULL, " ");
                }
                int j;
                for(j=0; j<6;){
                        if(i%2 == 0){
                                Node *temp = (Node*)malloc(sizeof(Node));
                                temp->ppid = array[j++];
                                temp->pid = array[j++];
                                if(temp->pid > 0){
                                        scount++;
                                }
                                insert(&list, temp);
                        }
                }
                if(scount>=3){
                        break;
                }
        }
}

void sendkill(char *buffer){
        int myfifo_fd5 = open("myfifo5", O_RDWR); // send it to mykill

        if(write(myfifo_fd5, buffer, strlen(buffer))<0){
                perror("fail to write to myfifo_fd5");
        }

}


int main(){
        char *command = (char*)malloc(sizeof(char)*MAX);
    char buffer[MAX];
    pid_t pid, pid2, pid3;
    int status, nread;

        int pipe_fd[2];
        pipe(pipe_fd);

        int pipe_fd2[2];
        pipe(pipe_fd2);

        int pipe_fd3[2];
        pipe(pipe_fd3);

//      fcntl(0, F_SETFL, O_NONBLOCK);

    pid = fork();
        if(pid<0){
                        perror("fork fail");
        }else if(pid == 0){
                        //printf("This is Child 1\n");
                        close(pipe_fd[0]);
                        Node *temp = (Node*)malloc(sizeof(Node));
                        temp->ppid = getppid();
                        temp->pid = getpid();
                        write(pipe_fd[1], temp, MAX);
                        system("sample &");
                        exit(1);
        }else{
                        //printf("This is Parent\n");
                        pid2 = fork();
                        if(pid2<0){
                                perror("fork2 fail");
                        }else if(pid2 == 0){
                                //printf("This is Child 2\n");
                                close(pipe_fd2[0]);
                                Node *temp = (Node*)malloc(sizeof(Node));
                                temp->ppid = getppid();
                                temp->pid = getpid();
                                write(pipe_fd2[1], temp, MAX);
                                system("sample &");
                                exit(2);
                        }else{
                                //printf("This is Parent\n");
                                pid3 = fork();
                                if(pid3 < 0){
                                        perror("fork3 fail");
                                }else if(pid3 == 0){
                                        //printf("This is Child 3\n");
                                        close(pipe_fd3[0]);
                                        Node *temp = (Node*)malloc(sizeof(Node));
                                        temp->ppid = getppid();
                                        temp->pid = getpid();
                                        write(pipe_fd3[1], temp, MAX);
                                        system("sample &");
                                        exit(3);
                                }else{
                                        //printf("This is Parent\n");
                                        // In parent process
                                        // do ps, jobs, kill command
                                        close(pipe_fd[1]); //close write
                                        close(pipe_fd2[1]); //close write
                                        close(pipe_fd3[1]); //close write
                                        char a[MAX], b[MAX], c[MAX];
                                        Node *array[3];
                                        read(pipe_fd[0], a, MAX); array[0] = a;
                                        read(pipe_fd2[0], b, MAX); array[1] = b;
                                        read(pipe_fd3[0], c, MAX); array[2] = c;

                                        int k;
                                        for(k=0; k<3; k++){
                                                Node *temp = (Node*)malloc(sizeof(Node));
                                                temp->ppid = array[k]->ppid;
                                                temp->pid = array[k]->pid;
                                                insert(&list, temp);
                                        }

                                        readfromsample();

                                        while(1){
                                                check_dead(&list);
                                                if(getCount() == 9){
                                                        break;
                                                }

                                                nread = read(STDIN_FILENO, buffer, MAX);
                                                buffer[nread-1] = '\0';
                                                printf("You input: %s\n\n", buffer);

                                                if(strcmp(buffer, "ps") == 0){
                                                        check_dead(&list);
                                                        display_ps(&list);
                                                        psCmd();

                                                }else if(strcmp(buffer, "jobs") == 0){
                                                        check_dead(&list);
                                                        display_jobs(&list);
                                                        jobsCmd();

                                                }else{ //kill
                                                        sendkill(buffer);
                                                        killCmd(buffer);
                                                        int cnt = check_dead(&list);
                                                        //printf("cnt: %d\n", cnt);
                                                        if(cnt == 9){
                                                                break;
                                                        }
                                                }

                                        }

                                        printf("Child process are all terminated\n");

                                }
                }
                exit(EXIT_SUCCESS);
        }
        exit(EXIT_SUCCESS);
}
