#include <stdio.h>
#include <stdlib.h>

// for simplicity, IP addresses are char

struct router{
    char IP;
    int status;     // working(1) or not working(0) 
    int packet;
    struct router* *adjacent;
    int connected_users;
    struct database* *user;
    struct router* upper_layer;
};

struct database{  // database or user
    char IP;
    FILE* data_unit;    // data storing unit
    struct router *connected_router;
} *myPC , *poem , *math;

// creates a router node
struct router* create_router(char IP);

// Domain Name Server
char DNS(char);

// Establishes graph with my PC, poem and math at appropriate positions
void internet(char myPC_IP, char poem_IP, char math_IP);

// Dijkstra's Routing algorithm
void route(int packet);

// Converting data to packets and delivering them
void send_packet( struct database* sender , int receiver_IP);

// Handling crashing of a router using Echo algorithm
void handle_crash(struct router* Crashed_Router);

int main(){
// declaring some variables/attributes
    myPC = (struct database*)malloc(sizeof(struct database));
    poem = (struct database*)malloc(sizeof(struct database));
    math = (struct database*)malloc(sizeof(struct database));
    myPC->data_unit = NULL;
    poem->data_unit = fopen("TheRoadNotTaken","r");
    math->data_unit = fopen("PythagorasTheorem","r");
    int t;
    printf("Enter poem-router's IP: ");
    scanf("%d",&t);
    getchar();
    poem->IP = (char)t;
    printf("Enter math-router's IP: ");
    scanf("%d",&t);
    getchar();
    math->IP = (char)t;

connect:
    printf("Enter myPC-router's IP: ");
    scanf("%d",&t);
    getchar();
    myPC->IP = (char)t;
// setting up the internet graph
    internet(myPC->IP , poem->IP , math->IP);

// connected to a router
    printf("\n\tYou are now connected to a wifi.\n");
    char choice;

// some internet send_packeting
    while(myPC->connected_router->status == 1 ){
        printf("\na) 'The Road Not Taken' by Robert Frost\nb) Pythagoras theorem\nc) Crash the router!!\nd) Exit\nWhat would you like to do?: ");
        scanf("%c",&choice);
        getchar();
        if(choice=='d') {
            printf("\n\tPower off\n");
            goto end;
        }
        else if(choice != 'c' ){
            if (DNS(choice) == '\0') printf("ERROR! Page not found.\n");
            else send_packet( myPC , DNS(choice) );
        }
        else myPC->connected_router->status = 0;
    }

// Crash Handling
    handle_crash(myPC->connected_router);
    
// No signal
    printf("\n\tERROR: No signal!\n\tWould you like to connect to another router?(y/n): ");
    scanf("%c",&choice);
    getchar();
    if ( choice == 'y') goto connect;
    else printf("\n\tPower off\n");
end:
    return 0;
}






struct router* create_router(char IP){
    struct router *x = (struct router *)malloc(sizeof(struct router));
    x->status = 1;
    x->IP = IP;
    x->adjacent = (struct router**)malloc(2*sizeof(struct router*));
    x->user = (struct database**)malloc(4*sizeof(struct database*));
    x->connected_users=0;
    x->upper_layer = NULL;
    return x;
}

char DNS(char c){
    if(c=='a') return poem->IP;
    else if(c=='b') return math->IP;
    else return '\0';
}

void connect_routers(struct router* a, struct router* b){
    a->adjacent[1] = b;
    b->adjacent[0] = a;
}

void internet(char PC_IP, char p_IP , char m_IP){
    struct router *temp,*prev,*head, *head1;
    for(int i=1 ; i<16;i++){
        temp = create_router(i<<4);
        if(i == 15){
            connect_routers(temp, head);
            connect_routers(prev,temp);
        }
        else if( i == 1 ) {
            prev = temp;
            head = temp;
        }
        else {
            connect_routers(prev,temp);
            prev = temp;
        }

        if( PC_IP == temp->IP){
            temp->user[temp->connected_users] = myPC;
            myPC->connected_router = temp;
            temp->connected_users++;
        }
        else if( p_IP == temp->IP){
            temp->user[temp->connected_users] = poem;
            poem->connected_router = temp;
            temp->connected_users++;
        }
        else if( m_IP == temp->IP){
            temp->user[temp->connected_users] = math;
            math->connected_router = temp;
            temp->connected_users++;
        }
    }

    for(int j = 1 ; j < 16 ; j++){
        for(int i=1 ; i<16 ; i++){
            temp = create_router((j<<4) + i);
            temp->upper_layer = head;
            if(i == 15){
                connect_routers(temp, head1);
                connect_routers(prev,temp);
            }
            else if( i == 1) {
                head->user[head->connected_users] = temp;
                head->connected_users++;
                prev = temp;
                head1 = temp;
            }
            else {
                connect_routers(prev,temp);
                prev = temp;
            }
            
            if( PC_IP == temp->IP){
                temp->user[temp->connected_users] = myPC;
                myPC->connected_router = temp;
                temp->connected_users++;
            }
            else if( p_IP == temp->IP){
                temp->user[temp->connected_users] = poem;
                poem->connected_router = temp;
                temp->connected_users++;
            }
            else if( m_IP == temp->IP){
                temp->user[temp->connected_users] = math;
                math->connected_router = temp;
                temp->connected_users++;
            }
        }
        head = head->adjacent[1];
    }


}

void route(int packet){
    ;
}

void send_packet( struct database* sender , int r_IP){
    if(sender->data_unit == NULL){
        char c = '\0';
        int p = r_IP*(1<<12) + sender->IP*(1<<8);
        route( p );
    }
    else{
        FILE* f = sender->data_unit;
        char c = getc(f);
        while(c!=EOF){
            int p = r_IP*(1<<12) + sender->IP*(1<<8) + c;
            route(p);
            c = getc(f);
        }
    }
}

void handle_crash(struct router* CR){
    ;
}
