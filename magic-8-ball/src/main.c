#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

const char *RESPONSES[] = {
    "Yes",
    "No",
    "Maybe",
    "Try Again",
    "Sounds like a question for a therapist...",
    "You already know",
    "Leave me alone!",
    "You can count on it",
    "You probably shouldn't",
    "How am I supposed to know that?",
    "How does that make you feel?",
    "Stop asking me questions",
    "I don't know...deez nutz?",
    "It will be so",
    "It will not not be so",
    "Not yes",
    "Not no",
    "Probably",
    "Don't ask me, I'm drunk",
    "Don't ask me, I'm a sycophant",
    "I'm a computer",
    "Go touch grass",
    "git good",
    "A genuine mystery",
    "One may never know",
    "Most definitely",
    "Most definitely not",
    "Idk, just eat more vegatables",
    "The mitochondria is the powerhouse of the cell",
    "Is this 20 responses yet?",
};


int main() {
    // Seed the random number generator
    srand(time(NULL));
    // The length of the response array
    int len = sizeof(RESPONSES) / sizeof(RESPONSES[0]);

    // Flag to determine if loop should keep running
    bool run = true;

    int response;
    char input[2048];
    while (run) {
        printf("What is your question?: ");
        fgets(input, sizeof(input), stdin);
        printf("Thinking...\n");

        // Determine the response and print it
        response = rand() % len;
        printf("%s\n", RESPONSES[response]);

        printf("Ask another question? [Y/n]: \n");
        fgets(input, sizeof(input), stdin);
        if (input[0] == 'N' || input[0] == 'n') {
            run = false;
        } else {
            continue;
        }
    }
   
    return 0;
}