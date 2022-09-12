#include <stdio.h>
#include <stdlib.h>

struct node{
    int status; //working or crashed
    short int packet;
    int IP;
    struct node* adjacent;
} *home;

void data2packets(FILE * f){
  
}

int main(){
    printf("You are now connected to your home wifi!!\v\n");
    struct node* myPC = home;

    while(home->status != 0 ){

    }
    printf("ERROR: No signal!\n");
    return 0;
}
