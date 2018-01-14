#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "error.h"
#include <string.h>
#include <dirent.h>
#define MAX 1000

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

static LinkedList list;

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

void display(LinkedList *list){
        Node*temp = list->head;
        printf("---------- ppid, pid display of background process(three samples)----------\n");
        while(temp!=NULL){
                printf("ppid: %d ", temp->ppid);
                printf("pid: %d\n", temp->pid);

                temp = temp->next;
        }
        printf("\n\n");
}


LinkedList *jobslist;
int main(){
        printf("This is myjobs\n");

        int res = mkfifo("/system_programming/2017/lab/class_a/spa15/HW2/myfifo3", 0777);
        if(res == 0){
                //printf("myfifo created\n");
        }
        else {
        //      perror("myfifo already exists");
        }
        int myfifo_fd3 = open("myfifo3", O_RDWR|O_NONBLOCK);
    //printf("myfifo_fd3 in myps: %d\n", myfifo_fd3);

        int nread;
        char buffer[MAX];
        if((nread = read(myfifo_fd3, buffer, MAX))>0){
                buffer[nread] = '\0';
                //printf("buffer in myps: %s\n", buffer);

                char *ptr = strtok(buffer, " ");
                int temp, i = 0;
                int array[6];
                while( ptr != NULL ){
                        //printf("%s\n", ptr);
                        int temp = atoi(ptr);
                        //printf("temp: %d\n", temp);
                        array[i++] = temp;
                        ptr = strtok(NULL, " ");
                }
                //printf("i: %d\n", i);
                int j;
                if(i==6){
                        for(j=0; j<6;){
                                if(i%2 == 0){
                                        Node *temp = (Node*)malloc(sizeof(Node));
                                        temp->ppid = array[j++];
                                        temp->pid = array[j++];
                                        insert(&list, temp);
                                }
                        }
                }

                if(i==4){
                        for(j=0; j<4;){
                                if(i%2 == 0){
                                        Node *temp = (Node*)malloc(sizeof(Node));
                                        temp->ppid = array[j++];
                                        temp->pid = array[j++];
                                        insert(&list, temp);
                                }
                        }
                }

                if(i==2){
                        for(j=0; j<2;){
                                if(i%2 == 0){
                                        Node *temp = (Node*)malloc(sizeof(Node));
                                        temp->ppid = array[j++];
                                        temp->pid = array[j++];
                                        insert(&list, temp);
                                }
                        }
                }
        }

        /*if((nread = read(myfifo_fd3, buffer, MAX))>0){
                printf("buffer in myjobs: %s\n", buffer);
                //jobslist = buffer;
                Node *head = buffer;
                printf("head: %s\n", head);
                while(head!=NULL){
                        printf("head->ppid: %d\n", head->ppid);
                        printf("head->pid: %d\n", head->pid);
                        head = head->next;
                }
        }*/


        display(&list);

        exit(EXIT_SUCCESS);
}
