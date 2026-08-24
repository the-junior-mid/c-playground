#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>


typedef enum {
    REGULAR,
    IMPOSSIBLE,
} Difficulty;


const char *ROSHAMBO[] = {
    "Rock",
    "Paper",
    "Scissors",
};


int main() {
    srand(time(NULL));

    char buff[32];
    printf("Choose a difficulty: [R]egular or [I]mpossible: ");
    fgets(buff, sizeof(buff), stdin);
    Difficulty diff = (buff[0] == 'R' || buff[0] == 'r') ? REGULAR : IMPOSSIBLE;

    int lost = 0;
    int wins = 0;
    int draws = 0;
    while (true) {
        int choice;
        char buff2[32];
        printf("[R]ock, [P]aper, or [S]cissors? (q/Q to quit): ");
        fgets(buff2, sizeof(buff2), stdin);
        if (buff2[0] == 'q' || buff2[0] == 'Q') {
            break;
        }
        if (diff == REGULAR) {
            choice = rand() % 3;
        } else {
            if (buff2[0] == 'r' || buff2[0] == 'R') {
                choice = 1;
            } else if (buff2[0] == 'p' || buff2[0] == 'P') {
                choice = 2;
            } else {
                choice = 0;
            }
        }
        printf("The all knowing computer picks %s!\n", ROSHAMBO[choice]);
        if (buff2[0] == 'r' || buff2[0] == 'R') {
            if (choice == 0) {
                draws += 1;
            } else if (choice == 1) {
                lost += 1;
            } else {
                wins += 1;
            }
        } else if (buff2[0] == 'p' || buff2[0] == 'P') {
            if (choice == 0) {
                wins += 1;
            } else if (choice == 1) {
                draws += 1;
            } else {
                lost += 1;
            }
        } else if (buff2[0] == 's' || buff2[0] == 'S') {
            if (choice == 0) {
                lost += 1;
            } else if (choice == 1) {
                wins += 1;
            } else {
                draws += 1;
            }
        } else {
            fprintf(stderr, "Error: invalid choice '%s', try again\n", buff2);
        }
        printf("Wins: %d\nLosses: %d\nDraws: %d\n", wins, lost, draws);
    }

    return 0;
}