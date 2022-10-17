#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// for simplicity, IP addresses are of 1 byte

struct router{
    char IP;
    int status;     // working(1) or not working(0)
    struct router* *adjacent;
    int *weight;
    struct database* user;
    struct router* upper_layer;
    struct router* lower_layer;
    int level;
    int *routing_table;
};

struct database{  // database or user
    char IP;
    FILE* data_unit;    // data storing unit
    struct router *connected_router;
} *myPC , *poem , *math;

// creates a router node
struct router* create_router(char IP){
    struct router *x = (struct router *)malloc(sizeof(struct router));
    x->status = 1;
    x->IP = IP;
    x->adjacent = (struct router**)malloc(2*sizeof(struct router*));
    x->upper_layer = NULL;
    x->weight = (int*)malloc(2*sizeof(int));

    if(IP%(1<<4) == 0 )x->level = 0;
    else x->level = 1;
    x->routing_table = (int*)malloc(15*sizeof(int));
    return x;
}

// Domain Name Server
char DNS(char c){
    if(c=='a') return poem->IP;
    else if(c=='b') return math->IP;
    else return '\0';
}

// Connects two routers
void connect_routers(struct router* a, struct router* b){
    a->adjacent[1] = b;
    a->weight[1] = 5 + rand()%11;
    b->adjacent[0] = a;
    b->weight[0] = a->weight[1];
}

// Evaluates minimum distance between routers
int minDistance(int dist[], bool SPT[]){
    int min = INT_MAX, min_index;
 
    for (int v = 0; v < 15; v++)
        if (SPT[v] == false && dist[v] <= min)
            min = dist[v], min_index = v;

    return min_index;
}

// Making Routing Tables
void dijkstra(int graph[15][15], int src, struct router* r){
    bool SPT[15];
    for(int i=0;i<15;i++) r->routing_table[i] = INT_MAX, SPT[i] = false;
    r->routing_table[src] = 0;
    for (int i = 0; i < 14; i++) {
        int u = minDistance(r->routing_table, SPT);
        SPT[u] = true;

        for (int v = 0; v < 15; v++)
            if (!SPT[v] && graph[u][v] 
                && r->routing_table[u] != INT_MAX 
                && r->routing_table[u] + graph[u][v] < r->routing_table[v])
                r->routing_table[v] = r->routing_table[u] + graph[u][v];
    }
}

// Establishes graph with my PC, poem and math at appropriate positions
void internet(char PC_IP, char p_IP , char m_IP){
    struct router *temp,*prev,*head, *head1;
    int g[15][15];
    for(int i=0;i<15;i++) for(int j=0;j<15;j++) g[i][j] = 0;

    for(int i=1 ; i<16;i++){
        temp = create_router(i<<4);
        if(i == 15){
            connect_routers(temp, head);
            connect_routers(prev,temp);
            g[i-1][i-2]=temp->weight[0];
            g[i-2][i-1]=temp->weight[0];

            g[0][14]=temp->weight[1];
            g[14][0]=temp->weight[1];
        }
        else if( i == 1 ) {
            prev = temp;
            head = temp;
        }
        else {
            connect_routers(prev,temp);
            prev = temp;
            g[i-1][i-2]=temp->weight[0];
            g[i-2][i-1]=temp->weight[0];
        }

        if( PC_IP == temp->IP){
            temp->user = myPC;
            myPC->connected_router = temp;
        }
        else if( p_IP == temp->IP){
            temp->user = poem;
            poem->connected_router = temp;
        }
        else if( m_IP == temp->IP){
            temp->user = math;
            math->connected_router = temp;
        }        
    }

    dijkstra(g,0,head);
    temp=head;
    head=head->adjacent[1];
    for(int i=1;i<15;i++){
        dijkstra(g,i,head);
        head=head->adjacent[1];
    }
    head=temp;
    
    for(int j = 1 ; j < 16 ; j++){
        for(int i=0;i<15;i++) for(int j=0;j<15;j++) g[i][j] = 0;

        for(int i=1 ; i<16 ; i++){
            temp = create_router((j<<4) + i);
            temp->upper_layer = head;
            
            if(i == 15){
                connect_routers(temp, head1);
                connect_routers(prev,temp);
                g[i-1][i-2]=temp->weight[0];
                g[i-2][i-1]=temp->weight[0];

                g[0][14]=temp->weight[1];
                g[14][0]=temp->weight[1];
            }
            else if( i == 1) {
                head->lower_layer = temp;
                prev = temp;
                head1 = temp;
            }
            else {
                connect_routers(prev,temp);
                prev = temp;
                g[i-1][i-2]=temp->weight[0];
                g[i-2][i-1]=temp->weight[0];
            }
            
            if( PC_IP == temp->IP){
                temp->user = myPC;
                myPC->connected_router = temp;
            }
            else if( p_IP == temp->IP){
                temp->user = poem;
                poem->connected_router = temp;
            }
            else if( m_IP == temp->IP){
                temp->user = math;
                math->connected_router = temp;
            }
        }
        
        dijkstra(g,0,head1);
        temp=head1;
        head1=head1->adjacent[1];
        for(int i=1;i<15;i++){
            dijkstra(g,i,head1);
            head1=head1->adjacent[1];
        }
        head1=temp;
        head = head->adjacent[1];
    }
}

// Routing algorithm using the routing tables
struct database* route(struct database* s, unsigned int p){
    struct router* here = s->connected_router;

    int r_IP = (p>>24);
    int r_level;
    if(r_IP % (1<<4) == 0) r_level=0;
    else r_level = 1;

    if( here->level==1 && r_level==1 && ((here->IP)>>4) != ((r_IP)>>4) ) here = here->upper_layer;

    if (here->level == 0) {
        while( (r_IP>>4) != (here->IP>>4) ) {
            if(here->weight[0] + here->adjacent[0]->routing_table[(r_IP>>4)-1] < here->weight[1] + here->adjacent[1]->routing_table[(r_IP>>4)-1])
                here=here->adjacent[0];
            else here=here->adjacent[1];
        }
    }

    if(here->IP != r_IP && here->level == 0) here=here->lower_layer;

    if (here->level == 1) {
        while( r_IP != here->IP ) {
            if(here->weight[0] + here->adjacent[0]->routing_table[(r_IP%(1<<4))-1] < here->weight[1] + here->adjacent[1]->routing_table[(r_IP%(1<<4))-1])
                here=here->adjacent[0];
            else here=here->adjacent[1];
        }
    }
    
    return here->user;
}

// Converting data to packets and delivering them
void send_request( struct database* sender , char r_IP){
    struct database *temp, *temp1;
    
    unsigned int p = r_IP*(1<<24) + sender->IP*(1<<16);
    temp = route(sender, p );
    p = sender->IP*(1<<24) + r_IP*(1<<16);
    temp1 = route(temp,p);

    FILE* f = temp->data_unit;
    char c = fgetc(f);
    printf("\n");
    if(temp1==sender)
        while(c!=EOF){
            p = sender->IP*(1<<24) + r_IP*(1<<16) + c;
            printf("%c",p % (1<<8));
            c = fgetc(f);
        }
    printf("\n");
}

// Handling crashing of a router
void handle_crash(struct router* Crashed_Router);////////////////////????????????????????????????????????

int main(){

// declaring some variables/attributes
    {
    myPC = (struct database*)malloc(sizeof(struct database));
    poem = (struct database*)malloc(sizeof(struct database));
    math = (struct database*)malloc(sizeof(struct database));
    myPC->data_unit = NULL;
    poem->data_unit = fopen("TheRoadNotTaken.txt","r");
    math->data_unit = fopen("PythagorasTheorem.txt","r");
    int t;
    printf("Enter poem-router's unique IP (17-255): ");
    scanf("%d",&t);
    getchar();
    poem->IP = (char)t;
    printf("Enter math-router's unique IP (17-255): ");
    scanf("%d",&t);
    getchar();
    math->IP = (char)t;
    printf("Enter myPC-router's unique IP (17-255): ");
    scanf("%d",&t);
    getchar();
    myPC->IP = (char)t;
    }

// setting up the internet graph
    internet(myPC->IP , poem->IP , math->IP);

connect:
// connected to a router
    printf("\n\tYou are now connected to a wifi.\n");
    char choice;

// some internet surfing
    while(myPC->connected_router->status == 1 ){
        printf("\na) 'The Road Not Taken' by Robert Frost\nb) Pythagoras theorem\nc) Crash the router!!\nd) Exit\nWhat would you like to do?: ");
        scanf("%c",&choice);
        getchar();
        if(choice=='d') goto end;
        else if(choice=='a'||choice=='b') send_request(myPC, DNS(choice));
        else if(choice == 'c' ){
        //    handle_crash();////////////////////???????????????????
        }
        else printf("Please enter valid choice.\n");
    }

// No signal
    printf("\n\tERROR: No signal!\n\tWould you like to connect to your neighbour router?(y/n): ");
    scanf("%c",&choice);
    getchar();
    if ( choice == 'y'){
        myPC->connected_router=myPC->connected_router->adjacent[1];
        myPC->connected_router->user=myPC;
        goto connect;
    }
    
end:printf("\n\tDisconnected!!\n");
    return 0;
}
