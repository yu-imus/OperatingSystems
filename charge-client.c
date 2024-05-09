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

// Function prototypes
void die_with_error(char *error_msg);
void gameStart(int client_sock);
void initPlayer(Player *player);
void displayMenu();
void chooseAction(Player *player, int choice, Player *target);
void attack(Player *attacker, Player *target, int chargeUsed);
void defend(Player *player, int chargeUsed);
void recover(Player *player, int chargeUsed);

int main(int argc,  char *argv[]) {
    int client_sock,  port_no,  n;
    struct sockaddr_in server_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
        printf("Usage: %s hostname port_no",  argv[0]);
        exit(1);
    }

    printf("Client searching ...\n");
    // Create a socket using TCP
    client_sock = socket(AF_INET,  SOCK_STREAM,  0);
    if (client_sock < 0) 
        die_with_error("Error: socket() Failed.");

    printf("Looking for host '%s'...\n", argv[1]);
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        die_with_error("Error: No such host.");
    }
    printf("Host found!\n");

    // Establish a connection to server
    port_no = atoi(argv[2]);
    bzero((char *) &server_addr,  sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,  
         (char *)&server_addr.sin_addr.s_addr, 
         server->h_length);
         
    server_addr.sin_port = htons(port_no);

    printf("Connecting to server at port %d...\n", port_no);
    if (connect(client_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) 
        die_with_error("Error: connect() Failed.");

    printf("Connection successful!\n");

    // Start the game
    gameStart(client_sock);

    close(client_sock); 
    
    return 0;
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
