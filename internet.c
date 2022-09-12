#include <stdio.h>
#include <stdlib.h>

struct node{
    int status; // working(1) or crashed(0)
    int packet;
    int IP;
    struct node* adjacent;
} home;


void data2packets( char ){
    ;
}

int main(){
beginning:
    printf("\n\tYou are now connected to a wifi.\n");
    struct node *myPC = &home;
    myPC->status = 1;
    char c;    // choice

    while(myPC->status == 1 ){
        printf("\na) Read a poem\nb) Solve maths problms\nc) Crash the router!!\nWhat would you like to do?: ");
        scanf("%c",&c);
        getchar();
        if(c != 'c') {
            data2packets(c);
        }
        else {
            myPC->status = 0;
        }
    }
    printf("\n\tERROR: No signal!\n\tWould you like to connect to your another wifi?(y/n): ");
    scanf("%c",&c);
    getchar();
    if ( c == 'y'){
        myPC = myPC->adjacent;
        myPC->status = 1;
        goto beginning;
    }
    else {
        printf("\n\tPower off\n");
    }
    return 0;
}
