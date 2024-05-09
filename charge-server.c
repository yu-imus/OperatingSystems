#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

typedef struct {
    int charge;
    int hp;
    // Item items[3];
} Player;

// Function prototypes for the game

void initPlayer(Player *player);
void displayMenu();
void chooseAction(Player *player, int choice, Player *target);
void attack(Player *attacker, Player *target, int chargeUsed);
void defend(Player *player, int chargeUsed);
void recover(Player *player, int chargeUsed);

// Function prototypes for game, server, and client

void gameStart();
void serverLoop(int port_no);
void handleClient(int client_sock);
void die_with_error(char *error_msg);
void gameStart(int client_sock);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s port_no", argv[0]);
        exit(1);
    }
    int port_no = atoi(argv[1]);
    printf("Server starting on port %d...\n", port_no);
    serverLoop(port_no);
    return 0;
}

void serverLoop(int port_no) {
    int server_sock, client_sock, client_size;
    struct sockaddr_in server_addr, client_addr;
    
    // Create a socket for incoming connections
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) 
       die_with_error("Error: socket() Failed.");
       
    // Bind socket to a port
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET; // Internet address 
    server_addr.sin_addr.s_addr = INADDR_ANY; // Any incoming interface
    server_addr.sin_port = htons(port_no); // Local port
    
    if (bind(server_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) 
       die_with_error("Error: bind() Failed.");
       
    // Mark the socket so it will listen for incoming connections
    listen(server_sock, 5);
    printf("Server listening to port %d ...\n", port_no);
    
    printf("Waiting for connection(s) ...\n");

    // Accept incoming connections and handle clients
    while (1) {
        client_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr *) &client_addr, &client_size);
        if (client_sock < 0) 
            die_with_error("Error: accept() Failed.");

        printf("Client successfully connected ...\n");
        
        // Handle client in a separate function or thread
        handleClient(client_sock);
    }
}

void handleClient(int client_sock) {
    char buffer[256];
    int n;
    
    while (1) {
        // Receive from client
        bzero(buffer, 256);
        n = recv(client_sock, buffer, 255, 0);
        if (n < 0) {
            printf("Error: recv() Failed.");
            break;
        }
        printf("< %s", buffer);

        // Send to client
        printf("[server] > ");
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        n = send(client_sock, buffer, strlen(buffer), 0);
        if (n < 0) {
            printf("Error: send() Failed.");
            break;
        }
    }

    printf("Closing connection ...\n");
    close(client_sock);
}

void gameStart(int client_sock) {
    Player player_1;
    Player player_2;
    
    initPlayer(&player_1); // Initialize player 1 to same starting point
    initPlayer(&player_2); // Initialize player 2 to same starting point
    
    printf("Welcome to CHAAAAARGE!\n");

    int round = 1;

    while (player_1.hp > 0 && player_2.hp > 0) {
        printf("\nRound %d\n", round);
        printf("Player 1 HP: %d\n", player_1.hp);
        printf("Player 2 HP: %d\n", player_2.hp);
      
        printf("\n");
      
        printf("Player 1 Charge:  %d\n", player_1.charge);
        printf("Player 2 Charge:  %d\n", player_2.charge);

        displayMenu();

        int choice;
        printf("Player 1, enter your choice: ");
        scanf("%d", &choice);
        chooseAction(&player_1, choice, &player_2);

        printf("Player 2, enter your choice: ");
        scanf("%d", &choice);
        chooseAction(&player_2, choice, &player_1);

        round++;
    }
    
    if (player_1.hp <= 0)
        printf("Player 1 loses! Player 2 wins!\n");
    else
        printf("Player 2 loses! Player 1 wins!\n");
}

void initPlayer(Player *player) {
    player->charge = 0;
    player->hp = 20;
}

void displayMenu() {
    printf("\nActions:\n");
    printf("1. Accumulate\n");
    printf("2. Attack\n");
    printf("3. Defend\n");
    printf("4. Recover\n");
    printf("5. Use Item\n");
}

void chooseAction(Player *player, int choice, Player *target) {
    switch(choice) {
        case 1:
            player->charge++;
            if (player->charge >= 5) {
                player->charge = 5;
                printf("You have reached the maximum limit! \n");
            } else {
                printf("Charge accumulated.\n");
            }
            break;
        case 2:
            attack(player, target, player->charge);
            break;
        case 3:
            defend(player, player->charge);
            break;
        case 4:
            recover(player, player->charge);
            break;
        // case USE_ITEM:
        // To be implemented
        // printf("Item usage not implemented yet.\n");
        break;
        default:
            printf("Invalid choice.\n");
    }
}

void attack(Player *attacker, Player *target, int chargeUsed) {
   int damage = 0;
    switch(chargeUsed) {
        case 1:
            damage = 1;
            attacker->charge--;
            break;
        case 2:
            damage = 2;
            attacker->charge = 0;
            break;
        case 3:
            damage = 3;
            attacker->charge = 0;
            break;
        case 4:
            damage = 4;
            attacker->charge = 0;
            break;
        case 5:
            damage = 5;
            attacker->charge = 0;
            break;
        default:
            printf("Invalid charge used.\n");
    }
    printf("Attacker dealt %d damage to target.\n", damage);
    target->hp -= damage;
}

void defend(Player *player, int chargeUsed) {
     double reduction = 0;
    switch(chargeUsed) {
        case 1:
            reduction = 0.3;
            player->charge--;
            break;
        case 2:
            reduction = 0.6;
            player->charge = 0;
            break;
        case 3:
            reduction = 1.0;
            player->charge = 0;
            break;
        default:
            printf("Invalid charge used.\n");
    }
    printf("Defense increased, damage reduced by %.1f.\n", reduction);
}

void recover(Player *player, int chargeUsed) {
    int recovery = 0;
    switch(chargeUsed) {
        case 1:
            recovery = 1;
            player->charge--;
            break;
        case 2:
            recovery = 2;
            player->charge = 0;
            break;
        case 3:
            recovery = 3;
            player->charge = 0;
            break;
        default:
            printf("Invalid charge used.\n");
    }
    if (player->hp + recovery > 20) {
        player->hp = 20;
    } else {
        player->hp += recovery;
    }
    
    printf("Player recovered %d HP.\n", recovery);
}

void die_with_error(char *error_msg){
    printf("%s", error_msg);
    exit(-1);
}
