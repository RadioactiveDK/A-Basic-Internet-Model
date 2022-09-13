#include <stdio.h>
#include <stdlib.h>

// for simplicity, assume IP addresses are characters


struct router{
    char IP;
    int status;     // working(1) or not(0)
    int packet;
    int trigger;    // packet being processed(1) or not(0)   
    struct router* adjacent;
} *initial_router;

struct connection{  // database or user
    char IP;
    FILE* A[10];    // data storing unit
    struct router* connected_router;
}myPC,poem,math;

void internet(){
    ;
}

int data2packets(char c){
    ;
}

void route(int packets){
    ;
}

void interact(char IP){
    ;
}

int main(){

// setting simple IPs to avoid use of DNS
    poem.IP = 'a';
    math.IP = 'b';
    myPC.IP = 'c';

// starting the internet
    internet();

// connected to a router
beginning:
    printf("\n\tYou are now connected to a wifi.\n");
    myPC.connected_router = initial_router;
    myPC.connected_router -> status = 1;
    char choice;

    // some internet surfing
    while(myPC.connected_router -> status == 1 ){
        printf("\na) Read a poem\nb) Solve maths problms\nc) Crash the router!!\nWhat would yo like to do?: ");
        scanf("%c",&choice);
        getchar();
        if(choice != 'c') interact(choice);
        else myPC.connected_router -> status = 0;
    }

// No signal
    printf("\n\tERROR: No signal!\n\tWould you like to connect to another wifi?(y/n): ");
    scanf("%c",&choice);
    getchar();
    if ( choice == 'y'){
        myPC.connected_router = myPC.connected_router -> adjacent;
        myPC.connected_router -> status = 1;
        goto beginning;
    }
    else {
        printf("\n\tPower off\n");
    }

    return 0;
}
