// CPSC 355
// Project Part 1 - C
// Name: Maaz Hassan
// UCID: 30087059

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Generates a random number between -15 and -15, not including 0
// The number will only be negative if the 'neg' parameter is true
float randomNum(int neg) {
    // Bitwise AND instead of modulus
    float num = rand() & 15;  // note: this is essentially an integer at this point
    int temp;

    // If the number is 15, then we return it as it is
    // Otherwise, we add a random decimal between 0 and 1 to it
    if (num != 15) {
        float decimal = rand() / (float) RAND_MAX;  // random decimal
        num += decimal;  // add the decimal to the original number

        num *= 100;  // multiply by 100 to get 2 decimal precision
        temp = (int) num;  // truncate rest of decimal places by casting to int
        num = temp / 100.0;  // convert back to float
    }

    return neg ? -num : num;  // if neg is true, convert num to negative
}

// Initializes the game by filling the board with random positive/negative numbers,
// good surprises (double score), bad surprises (half score) and time packs.
// Counters are kept to ensure that negative numbers don't exceed 20% of the board,
// and all surprises don't exceed 5% each (they can be less though, it's based on chance).
void initializeGame(float *board, int size) {

    // Initialize variables
    int i, rNum, negCount = 0, gSurCount = 0, bSurCount = 0, tPackCount = 0;

    // Loop through every square of the board
    for (i = 0; i < size*size; i++) {

        // A new random number is generated for every cell to determine what should fill it
        rNum = rand() % 100;

        if (rNum < 20 && negCount < ((size*size)/5)) {  // 20% -> negative number
            *(board + i) = randomNum(1);
            negCount++;
        }
        else if (20 <= rNum && rNum < 25 && gSurCount < ((size*size)/20)) {  // 5% -> good surprise
            *(board + i) = 16;  // 16 is used for undiscovered good surprises
            gSurCount++;
        }
        else if (25 <= rNum && rNum < 30 && bSurCount < ((size*size)/20)) {  // 5% -> bad surprise
            *(board + i) = 17;  // 17 is used for undiscovered bad surprises
            bSurCount++;
        }
        else if (30 <= rNum && rNum < 35 && tPackCount < ((size*size)/20)) {  // 5% -> time pack
            *(board + i) = 18;  // 18 is used for undiscovered time packs
            tPackCount++;
        }
        else *(board + i) = randomNum(0);  // otherwise (65%) -> positive number
    }
    
    // Prints some info at the start of the game
    printf("%d negative numbers.\n", negCount);
    printf("%d good surprises.\n", gSurCount);
    printf("%d bad surprises.\n", bSurCount);
    printf("%d time packs.", tPackCount);
}

// Displays the game board, using different symbols depending on the state of the cell
void displayGame(float *board, int size, float score, int time) {

    // Variables
    int i, j = 0;
    float val;

    printf("\n");  // formatting purposes

    // Loops through the board
    for (i = 0; i < size*size; i++) {
        val = *(board + i);  // determines what is in the cell

        // Then prints the appropriate symbol:
        // 19 -> consumed positive cell
        // 20 -> consumed negative cell
        // 21 -> consumed good surprise
        // 22 -> consumed bad surprise
        // 23 -> consumed time pack
        if (val == 19) printf("+ ");
        else if (val == 20) printf("- ");
        else if (val == 21) printf("$ ");
        else if (val == 22) printf("! ");
        else if (val == 23) printf("@ ");
        else printf("X ");  // prints X otherwise (unconsumed cell)

        // Formatting newline
        if (j == size - 1) {
            printf("\n");
            j = 0;
        }
        else {
            j++;
        }
    }
    
    // After the board, print information about the current score and remaining time
    printf("Score: %.2f\n", score);
    printf("Time: %d", time);
}

// Calculates the new score given the current score and the value of the uncovered cell
float calculateScore(float current, float new) {
    int converted;

    // If not a special pack, just add the new to the current
    if (new <= 15) return (current + new + 0.000001);  // deal with floating point errors

    // 16 and 17 refer to good surprise and bad surprise
    else if (new == 16 || new == 17) {
        converted = (int) (current * 100);  // need to convert to int for bitwise shift
        converted = new == 16 ? converted << 1 : converted >> 1;  // bitwise shift to double/half score
        return ((converted / 100.0) + 0.000001);  //convert back to float
    }
    else return current;
}

// A function that logs the score once the game ends in a win or loss
// Quitting the game prematurely will not log anything
// The file scores.log must be in the same directory as this file
void logScore(char name[], float score, int time) {
    FILE *outp = fopen("./scores.log", "a");
    fprintf(outp, "Name: %s\n", name);
    fprintf(outp, "Final score: %.2f\n", score);
    fprintf(outp, "Time remaining: %d\n\n", time);
    fclose(outp);
}

// A function that performs a series of checks to determine whether or not the game should end
// This function is not called on the initial turn, so it is impossible to lose on the first move,
// even if the player lands on a negative square (as per the assignment description)
int exitCheck(int maxTime, float score, float *board, int size, char name[]) {
    int i;

    // Player loses if they run out of time
    if (time(NULL) > maxTime) {
        printf("\nGame over. Time's up.");
        printf("\nFinal score: %.2f", score);
        printf("\nTime remaining: 0\n\n");
        logScore(name, score, maxTime - time(NULL));  // log the score
        return 1;  // 1 means the game should exit
    }

    // Player loses if the score falls below/equals 0
    else if (score <= 0) {
        displayGame(board, size, score, maxTime - time(NULL));
        printf("\n\nGame over. You lose.");
        printf("\nFinal score: %.2f", score);
        printf("\nTime remaining: %ld\n\n", maxTime - time(NULL)); 
        logScore(name, score, maxTime - time(NULL));   // log the score
        return 1;
    }

    // Loop through the board
    for (i = 0; i < size*size; i++) {
        if (*(board + i) < 19) {    // if any cell is less than 19 (unconsumed)
            return 0;               // then the game continues
        }
    }

    // If the above loop finishes without returning 0, all cells have been consumed
    // and the player wins.
    displayGame(board, size, score, maxTime - time(NULL));
    printf("\n\nCongratulations. You win.");
    printf("\nFinal score: %.2f", score);
    printf("\nTime remaining: %ld\n\n", maxTime - time(NULL));
    logScore(name, score, maxTime - time(NULL));
    return 1;
}

// A function goes through the scores.log file, and displays the top n scores
// alongside the names and times.
void displayTopScores() {

    // Variables
    int n = 0, i, j, lcount = 0, cMaxIn, digits = 0, numOfScores;
    long int longestName = 0;
    int c;

    // prev_max and current_max set to values that will always satisfy the
    // condition during the first iteration of the loop later on
    float prev_max = 100000.0, current_max = -100000.0, posScore;

    // Open the file in read mode
    FILE *file = fopen("./scores.log", "r");
    if (file == NULL) {
        printf("Could not find scores.log file.\n\n");
        return;
    }

    // Get user input for n -> the number of scores to display
    while (1) {
        printf("How many scores to display?: ");
        if (scanf("%d", &n) != 1) {
            while (getchar() != '\n');
            printf("\nInvalid input.");
            continue;
        }
        printf("\n");
        break;
    }

    // Count the number of lines in the file, by counting the number of
    // newline characters
    fseek(file, 0, SEEK_SET);
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') lcount++;
    }
    // Clear input buffer
    while (getchar() != '\n');

    // If scores.log is empty, return
    if (lcount == 0) {
        printf("There are no scores in the file. Try playing some games first.\n\n");
        return;
    }

    // Every score has a total of 4 newline characters, so we divide by 4
    // to get the total number of scores in the file
    numOfScores = lcount/4;

    // If the user enters a number larger than the total number of scores
    // in the file, all the scores in the file are displayed instead
    if (numOfScores < n) {
        printf("%d scores don't exist. Displaying all scores instead.\n\n", n);
        n = numOfScores;
    }

    // A buffer string for input read from the file
    char buffer[50];

    // Dynamic array declarations with malloc
    // names is an array of strings, to store all the names from the file
    char **names;
    names = malloc(sizeof(char*) * numOfScores);
    for (int i = 0; i < numOfScores; i++) {
        names[i] = malloc(50);
    }
    // scores is an array of all the scores from the file
    float *scores = (float *)malloc(sizeof(float) * numOfScores);
    // times is an array of all the times from the file
    int *times = (int *)malloc(sizeof(int) * numOfScores);

    // Populate the array by going through and parsing the file
    for (i = 0; i < numOfScores; i++) {
        fseek(file, 0, SEEK_SET);
        for (j = 0; j < i*4; j++) {
            for (c = getc(file); c != '\n'; c = getc(file));
        }
        fseek(file, 6, SEEK_CUR);
        fgets(buffer, 50, file);
        buffer[strcspn(buffer, "\n")] = 0;
        strcpy(names[i], buffer);

        fseek(file, 13, SEEK_CUR);
        fgets(buffer, 50, file);
        scores[i] = strtof(buffer, NULL);

        fseek(file, 16, SEEK_CUR);
        fgets(buffer, 50, file);
        times[i] = atoi(buffer);
    }

    // Get length of the longest name for formatting purposes
    for (i = 0; i < numOfScores; i++) {
        if (strlen(names[i]) > longestName) longestName = strlen(names[i]);
    }

    // Uses linear search to find the maximum element that is smaller than the
    // previous max. Then displays the information from all the arrays at that index.
    for (i = 0; i < n; i++) {
        for (j = 0; j < numOfScores; j++) {
            if (scores[j] < prev_max && scores[j] > current_max) {
                current_max = scores[j];
                cMaxIn = j;  // current max index
            }
        }

        // Formatting the output so that columns line up regardless of lengths
        printf("%d. Name: %s", i + 1, names[cMaxIn]);
        // Print the number of spaces based off of the longest name
        for (j = 0; j < (longestName + 4) - strlen(names[cMaxIn]); j++) printf(" ");
        printf("|    Score: %.2f", scores[cMaxIn]);
        // Convert the score to positive to calculate how many digits it has
        posScore = scores[cMaxIn] > 0 ? scores[cMaxIn] : scores[cMaxIn] * -1;
        // Use a while loop to determine the number of digits in the score
        j = (int) posScore;
        digits = 0;
        while (j > 0) {
            digits++;
            j /= 10;
        }
        // Print the number of spaces based off of the number of digits
        for (j = 0; j < 6-digits; j++) printf(" ");
        if (scores[cMaxIn] > 0) printf(" ");  // extra space to compensate for negative sign
        printf("|    Time Remaining: %d\n", times[cMaxIn]);
        
        prev_max = current_max;   // Set the previous max to the current before rerunning the loop
        current_max = -100000.0;  // will satisfy the condition for the first comparison
    }
    printf("\n");
}

// Makes sure that the command line arguments passed by the user are valid
void handleArgs(int argc, char *argv[]) {
    if (argc < 3) {    // too little arguments
        printf("Not enough arguments. Please enter the player name "
        "and dimensions of the game board as arguments.");
        exit(0);
    }
    else if (argc > 3) {    // too many arguments
        printf("Too many arguments. Please enter only the player name "
        "and dimensions of the game board as arguments.");
        exit(0);
    }
    else if (atoi(argv[2]) == 0) {    // invalid second argument
        printf("Invalid second argument. Please enter a number.");
        exit(0);
    }
    else if (atoi(argv[2]) < 5 || atoi(argv[2]) > 20) {    // invalid size
        printf("Invalid size. Minimum: 5 | Maximum: 20.");
        exit(0);
    }
}

// A function that takes care of the options the user is presented with at the
// start of the game.
void options() {

    // Variables
    char c;
    int extra = 0;

    // Runs until the user gives valid input, then breaks
    while(1) {
        printf("Enter 'p' to play, 't' to view top n scores, or 'q' to quit.\n");
        printf("Choice: ");
        c = getchar();
        while (getchar() != '\n') extra += 1;
        if (extra != 0) c = 'z';
        extra = 0;

        // Deal with the input
        switch (c) {
            case 'p':
                printf("\n");
                return;
            case 't':
                displayTopScores();
                break;
            case 'q': exit(1);
            default:
                printf("\nInvalid choice.\n\n");
                break;
        }
    }
}

// Function that asks the user for a move
int moveInput(int *x, int *y, int size) {

    // Variables
    char input[256];
    char neg;
    int quit;

    // Runs until the user provides valid input
    do {
        printf("\nEnter your move (x y) or '-1' to quit: ");
        // Check validity of input
        if (fgets(input, sizeof(input), stdin)) {
            if (sscanf(input, "%d %d", x, y) == 2) {
                if (*x >= 0 && *y >= 0 && *x < size && *y < size) {
                    break;   // break if input is valid
                }
            }
            // Exit case
            else if (sscanf(input, "%c%d", &neg, &quit) == 2 && neg == '-' && quit == 1) return 1;
        }
        printf("\nInvalid input.\n");
    }
    while(1);
    return 0;
}

// Alters the value inside of a cell, based off of it's current value.
// Used to "consume" a cell.
void alterCell(float *moveCell) {
    float value = *moveCell;    // current value in the cell
    if (value == 16) *moveCell = 21;         // good surprise
    else if (value == 17) *moveCell = 22;    // bad surprise
    else if (value == 18) *moveCell = 23;    // time pack
    else if (value > 0) *moveCell = 19;      // positive
    else *moveCell = 20;                     // negative
}

// Checks the cell the user has inputted, then applies the move
void applyMove(float *board, int x, int y, int size, float *score, int *maxTime) {
    float *moveCell = board + (x * size) + y;    // the inputted cell

    // Check if the cell has already been consumed
    if (*moveCell > 18) {
        printf("\nYou have already gone there.");
        return;
    }
    // Check if the cell is a timepack
    else if (*moveCell == 18) {
        printf("\nTime pack! +%d seconds.", 2 * size);
        alterCell(moveCell);
        *maxTime += (2 * size);
        return;
    }
    // Print the appropriate message otherwise
    else if (*moveCell == 16) printf("\nYour score has been doubled!");
    else if (*moveCell == 17) printf("\nYour score has been halved!");
    else if (*moveCell > 0) printf("\nUncovered a reward of %.2f points.", *moveCell);
    else printf("\nBang! You lost %.2f points.", *moveCell * -1);

    // Then calculate the new score and alter the value of the cell
    *score = calculateScore(*score, *moveCell);
    alterCell(moveCell);
}

// Main method
int main(int argc, char *argv[]) {
    handleArgs(argc, argv);    // handle the arguments

    // Loop runs until the user chooses to exit (happens in the options() function)
    while (1) {
        srand(time(0));    // seed for random function
        options();         // display options

        // Variables
        int size = atoi(argv[2]);
        float scoreArr[size][size];        // the backend board
        float *board = &scoreArr[0][0];    // pointer to the board
        float score = 0;
        int timeOffset = 12 * size;    // calculating the time based on the size of the board
        int x, y;

        // Initialize and display
        initializeGame(board, size);
        displayGame(board, size, score, timeOffset);

        // The initial move (safe from losing, even if negative square chosen)
        if (moveInput(&x, &y, size)) continue;  // initial input
        int maxTime = time(NULL) + timeOffset;  // start timer
        applyMove(board, x, y, size, &score, &maxTime);

        // Move loop, runs until the game ends
        do {
            displayGame(board, size, score, maxTime - time(NULL));
            if (moveInput(&x, &y, size)) break;
            if (maxTime >= time(NULL)) applyMove(board, x, y, size, &score, &maxTime);
        }
        while (!exitCheck(maxTime, score, board, size, argv[1]));  // check exit conditions
    }
    
    return 0;
}