// Server program

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
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
    int server_sock, client_sock, port_no, client_size;
    struct sockaddr_in server_addr, client_addr;

    if (argc < 2) {
        printf("Usage: %s port_no\n", argv[0]);
        exit(1);
    }

    printf("Server starting ...\n");

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        printf("Error: socket() Failed.\n");
        exit(-1);
    }

    bzero((char *)&server_addr, sizeof(server_addr));
    port_no = atoi(argv[1]);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_no);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Error: bind() Failed.\n");
        exit(-1);
    }

    listen(server_sock, 5);
    printf("Server listening to port %d ...\n", port_no);
    printf("Waiting for connection(s) ...\n");

    client_size = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_size);
    if (client_sock < 0) {
        printf("Error: accept() Failed.\n");
        exit(-1);
    }

    printf("Client successfully connected ...\n");

    Player player_1, player_2;
    initPlayer(&player_1);
    initPlayer(&player_2);

    // Game loop
    while (player_1.hp > 0 && player_2.hp > 0) {
        // Display menu for player 1
        displayMenu(&player_1);

        // Send player_1's state to client
        send(client_sock, &player_1, sizeof(Player), 0);

        // Receive player_2's action from client
        recv(client_sock, &player_2, sizeof(Player), 0);

        // Perform actions for player 2
        chooseAction(&player_2, player_2.action, &player_1);

        // Perform actions for player 1
        int choice;
        printf("Player 1, choose your action (1-4): ");
        scanf("%d", &choice);
        chooseAction(&player_1, choice, &player_2);
    }

    printf("Game over!\n");

    close(client_sock);
    close(server_sock);

    return 0;
}
