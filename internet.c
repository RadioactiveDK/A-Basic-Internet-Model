#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define INT_MAX 123456
// for simplicity, IP addresses are of 1 byte

struct router{
    unsigned char IP;
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
    unsigned char IP;
    FILE* data_unit;    // data storing unit
    struct router *connected_router;
} *myPC , *poem , *math;

// creates a router node
struct router* create_router(struct router* PtrArr[], unsigned char IP){
    struct router *x = (struct router *)malloc(sizeof(struct router));
    struct database *y = (struct database *)malloc(sizeof(struct database));
    x->status = 1;
    x->IP = IP;
    x->adjacent = (struct router**)malloc(2*sizeof(struct router*));
    x->upper_layer = NULL;
    x->lower_layer = NULL;
    x->weight = (int*)malloc(2*sizeof(int));
    x->user=y;
    y->IP=IP;
    y->connected_router=x;

    if(IP%(1<<4) == 0 )x->level = 0;
    else x->level = 1;
    x->routing_table = (int*)malloc(15*sizeof(int));
    PtrArr[(unsigned int)IP] = x;
    return x;
}

// Domain Name Server
unsigned char DNS(char c){
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
    for(int i=0;i<15;i++) {
        r->routing_table[i] = INT_MAX;
        SPT[i] = false;
    }
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
void internet(struct router* PtrArr[], unsigned char PC_IP, unsigned char p_IP , unsigned char m_IP){
    struct router *temp,*prev,*head, *head1;
    int g[15][15];  //adjacency weighted matrix
    for(int i=0;i<15;i++) for(int j=0;j<15;j++) g[i][j] = 0;

    for(int i=1 ; i<16;i++){
        temp = create_router(PtrArr,i<<4);
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
            temp = create_router(PtrArr, (j<<4) + i);
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

    unsigned char r_IP = (unsigned char)(p>>24);
    int r_level;
    if(r_IP % (1<<4) == 0) r_level=0;
    else r_level = 1;

    if( here->level==1 && ((r_level==1 && ((here->IP)>>4) != ((r_IP)>>4)) || (r_level==0)) ) here = here->upper_layer;
    
    if (here->level == 0) {
        if (here->routing_table[(r_IP>>4)-1] == INT_MAX) return here->user;
        while( (r_IP>>4) != (here->IP>>4) ){
            if(here->weight[0] + here->adjacent[0]->routing_table[(r_IP>>4)-1] < here->weight[1] + here->adjacent[1]->routing_table[(r_IP>>4)-1])
                here=here->adjacent[0];
            else here=here->adjacent[1];
        }
        if(here->IP != r_IP ) here=here->lower_layer;
    }

    if (here->level == 1) {
        if (here->routing_table[(r_IP%(1<<4))-1] == INT_MAX) return here->user;
        while( r_IP != here->IP ){
            if(here->weight[0] + here->adjacent[0]->routing_table[(r_IP%(1<<4))-1] < here->weight[1] + here->adjacent[1]->routing_table[(r_IP%(1<<4))-1])
                here=here->adjacent[0];
            else here=here->adjacent[1];
        }
    }
    
    return here->user;
}

// Converting data to packets and delivering them
void send_request( struct database* sender , unsigned char r_IP){
    struct database *temp, *temp1;
    
    unsigned int p = r_IP*(1<<24) + sender->IP*(1<<16);
    temp = route(sender, p);


    if(temp->IP == r_IP) p = sender->IP*(1<<24) + r_IP*(1<<16);
    else p = sender->IP*(1<<24) + r_IP*(1<<16) + (1<<8);

    temp1 = route(temp,p);

    printf("\n");
    if( temp1==sender && (p>>8)%2 == 0) {
        FILE* f = temp->data_unit;
        char c = fgetc(f);
        while(c!=EOF){
            p = sender->IP*(1<<24) + r_IP*(1<<16) + c;
            printf("%c",p % (1<<8));
            c = fgetc(f);
        }
        fclose(f);
        if(temp==poem) poem->data_unit = fopen("TheRoadNotTaken.txt","r");
        else math->data_unit = fopen("PythagorasTheorem.txt","r");
    }
    else printf("\tServer Not Found");
    printf("\n");
}

// Handling crashing of a router
void echo(struct router* PtrArr[], struct router* CR){
    // echo
    for(int i=0;i<15;i++) if(CR->routing_table[i]!=INT_MAX){
        unsigned int p = ((CR->adjacent[0]->IP)<<24) + (CR->IP<<16) + (1<<10);
        route(CR->user, p);
    }

    // updation of routing tables
    int g[15][15];
    if(CR->IP % 16 == 0){
        for(int i=0; i<15; i++) for(int j=i; j<15; j++){
            g[i][j] = 0;
            g[j][i] = 0;
            if( (PtrArr[(i+1)<<4]->status == 0) || (PtrArr[(j+1)<<4]->status == 0) ) ;
            else if(PtrArr[(i+1)<<4]->adjacent[0]->IP == PtrArr[(j+1)<<4]->IP) {
                g[i][j]=PtrArr[(i+1)<<4]->weight[0];
                g[j][i]=PtrArr[(i+1)<<4]->weight[0];
            }
            else if(PtrArr[(i+1)<<4]->adjacent[1]->IP == PtrArr[(j+1)<<4]->IP){
                g[i][j]=PtrArr[(i+1)<<4]->weight[1];
                g[j][i]=PtrArr[(i+1)<<4]->weight[1];
            }
        }

        for(int i=0;i<15;i++) dijkstra(g,i,PtrArr[(i+1)<<4]);

    }
    else{
        unsigned int parent= CR->IP - (CR->IP%16);
        for(int i=0; i<15; i++) for(int j=i; j<15; j++){
            g[i][j] = 0;
            g[j][i] = 0;
            if( (PtrArr[(parent+i+1)]->status == 0) || (PtrArr[(parent+j+1)]->status == 0) ) ;
            else if(PtrArr[(parent+i+1)]->adjacent[0]->IP == PtrArr[(parent+j+1)]->IP) {
                g[i][j]=PtrArr[(parent+i+1)]->weight[0];
                g[j][i]=PtrArr[(parent+i+1)]->weight[0];
            }
            else if(PtrArr[(parent+i+1)]->adjacent[1]->IP == PtrArr[(parent+j+1)]->IP){
                g[i][j]=PtrArr[(parent+i+1)]->weight[1];
                g[j][i]=PtrArr[(parent+i+1)]->weight[1];
            }
        }
        for(int i=0;i<15;i++) dijkstra(g,i,PtrArr[(parent+i+1)]);
    }
}


int main(){

// declaring some variables/attributes
    myPC = (struct database*)malloc(sizeof(struct database));
    poem = (struct database*)malloc(sizeof(struct database));
    math = (struct database*)malloc(sizeof(struct database));
    myPC->data_unit = NULL;
    poem->data_unit = fopen("TheRoadNotTaken.txt","r");
    math->data_unit = fopen("PythagorasTheorem.txt","r");
    int t;
    printf("Enter poem-router's unique IP (16-255): ");
    scanf("%d",&t);
    getchar();
    poem->IP = (unsigned char)t;
    printf("Enter math-router's unique IP (16-255): ");
    scanf("%d",&t);
    getchar();
    math->IP = (unsigned char)t;
    printf("Enter myPC-router's unique IP (16-255): ");
    scanf("%d",&t);
    getchar();
    myPC->IP = (unsigned char)t;

// setting up the internet graph
    struct router* PtrArr[256];
    internet(PtrArr, myPC->IP , poem->IP , math->IP);

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
            printf("\tEnter IP of the router to crash: ");
            scanf("%d",&t);
            getchar();
            PtrArr[(unsigned char)t]->status = 0;
            echo(PtrArr, PtrArr[(unsigned char)t]);
        }
        else printf("Please enter valid choice.\n");
    }

// disconnected
end:printf("\n\tDisconnected!!\n");
    return 0;
}
