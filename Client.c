// Client program

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
    int action; // To store chosen action
} Player;

void attack(Player *attacker, Player *target, int chargeUsed);
void defend(Player *player, int chargeUsed);
void recover(Player *player, int chargeUsed);

void initPlayer(Player *player) {
    player->charge = 0;
    player->hp = 20;
    player->action = 0;
}

void displayMenu(Player *player) {
    printf("\nPlayer HP: %d\n", player->hp);
    printf("Player Charge: %d\n", player->charge);
    printf("\nActions:\n");
    printf("1. Accumulate\n");
    printf("2. Attack\n");
    printf("3. Defend\n");
    printf("4. Recover\n");
}

void chooseAction(Player *player, int choice, Player *target) {
    player->action = choice;
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

int main(int argc, char *argv[]) {
    int client_sock, port_no;
    struct sockaddr_in server_addr;
    struct hostent *server;

    if (argc < 3) {
        printf("Usage: %s hostname port_no\n", argv[0]);
        exit(1);
    }

    printf("Client starting ...\n");

    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock < 0) {
        printf("Error: socket() Failed.\n");
        exit(-1);
    }

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        printf("Error: No such host.\n");
        exit(-1);
    }

    port_no = atoi(argv[2]);
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(port_no);

    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Error: connect() Failed.\n");
        exit(-1);
    }

    printf("Connection successful!\n");

    Player player_1, player_2;
    initPlayer(&player_1);
    initPlayer(&player_2);

    // Game loop
    while (player_1.hp > 0 && player_2.hp > 0) {
        // Receive player_1's state from server
        recv(client_sock, &player_1, sizeof(Player), 0);

        // Display menu for player 2
        displayMenu(&player_2);

        // Choose action for player 2
        int choice;
        printf("Player 2, choose your action (1-4): ");
        scanf("%d", &choice);
        player_2.action = choice;

        // Execute the action chosen by player 2
        chooseAction(&player_2, player_2.action, &player_1);

        // Send player_2's state and action to server
        send(client_sock, &player_2, sizeof(Player), 0);
    }

    printf("Game over!\n");

    close(client_sock);

    return 0;
}
