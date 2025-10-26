#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DIGIT_RANGE 10
#define MAX_DIGITS 5
#define MAX_NAME_LEN 50

typedef struct {
    int hit;
    int nearHit;
} Result;

typedef struct {
    int difficulty;
    int attempts;
    int secret[MAX_DIGITS];
    int lastGuess[MAX_DIGITS];
    Result lastResult;
} GameState;

void clearInput() {
    while (getchar() != '\n');
}

int selectDifficulty() {
    printf("\nChoose your difficulty level:\n");
    printf("1. Easy (3 digits)\n");
    printf("2. Medium (4 digits)\n");
    printf("3. Hard (5 digits)\n\n");

    int level = 0;
    while (level < 1 || level > 3) {
        printf("Select difficulty level (1-3): ");
        scanf_s("%d", &level);
        clearInput();
        if (level < 1 || level > 3) {
            printf("Invalid choice. Please try again.\n");
        }
    }
    return level;
}

void generateSecret(int secret[], int size) {
    int used[DIGIT_RANGE] = { 0 };
    int i = 0;
    while (i < size) {
        int digit = rand() % DIGIT_RANGE;
        if (!used[digit]) {
            secret[i] = digit;
            used[digit] = 1;
            i++;
        }
    }
}

void getUserGuess(int guess[], int size) {
    char input[100];
    int valid = 0;
    while (!valid) {
        printf("Enter your guess (%d different digits): ", size);
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        if (strlen(input) != size) {
            printf("Error: Enter exactly %d digits.\n", size);
            continue;
        }
        int used[DIGIT_RANGE] = { 0 };
        valid = 1;
        for (int i = 0; i < size; i++) {
            if (input[i] < '0' || input[i] > '9') {
                valid = 0;
                break;
            }
            int digit = input[i] - '0';
            if (used[digit]) {
                valid = 0;
                break;
            }
            used[digit] = 1;
            guess[i] = digit;
        }
        if (!valid)
            printf("Error: Digits must be 0-9 and unique.\n");
    }
}

Result checkGuess(int secret[], int guess[], int size) {
    Result result = { 0, 0 };
    int usedSecret[10] = { 0 };
    int usedGuess[10] = { 0 };
    for (int i = 0; i < size; i++) {
        if (secret[i] == guess[i]) {
            result.hit++;
            usedSecret[i] = 1;
            usedGuess[i] = 1;
        }
    }
    for (int i = 0; i < size; i++) {
        if (!usedGuess[i]) {
            for (int j = 0; j < size; j++) {
                if (!usedSecret[j] && guess[i] == secret[j]) {
                    result.nearHit++;
                    usedSecret[j] = 1;
                    break;
                }
            }
        }
    }
    return result;
}

void saveGame(GameState game) {
    FILE* f;
    errno_t err = fopen_s(&f, "save_game.txt", "w");
    if (err != 0 || f == NULL) {
        printf("Error: Could not save game.\n");
        return;
    }

    fprintf(f, "%d %d\n", game.difficulty, game.attempts);

    for (int i = 0; i < game.difficulty; i++) {
        fprintf(f, "%d ", game.secret[i]);
    }
    fprintf(f, "\n");

    for (int i = 0; i < game.difficulty; i++) {
        fprintf(f, "%d ", game.lastGuess[i]);
    }
    fprintf(f, "\n");

    fprintf(f, "%d %d\n", game.lastResult.hit, game.lastResult.nearHit);

    fclose(f);
}

int loadGame(GameState* game) {
    FILE* f;
    errno_t err = fopen_s(&f, "save_game.txt", "r");
    if (err != 0 || f == NULL) {
        printf("No saved game found.\n");
        return 0;
    }

    if (fscanf_s(f, "%d %d", &game->difficulty, &game->attempts) != 2) {
        fclose(f);
        printf("Corrupted save file.\n");
        return 0;
    }

    for (int i = 0; i < game->difficulty; i++) {
        fscanf_s(f, "%d", &game->secret[i]);
    }

    for (int i = 0; i < game->difficulty; i++) {
        fscanf_s(f, "%d", &game->lastGuess[i]);
    }

    fscanf_s(f, "%d %d", &game->lastResult.hit, &game->lastResult.nearHit);

    fclose(f);
    return 1;
}

void saveHighScore(int level, const char* name, int attempts) {
    FILE* f;
    errno_t err = fopen_s(&f, "highscores.txt", "a");
    if (err != 0 || f == NULL) {
        printf("Error: Could not save high score.\n");
        return;
    }

    fprintf(f, "%d %s %d\n", level, name, attempts);
    fclose(f);
}

void showHighScores() {
    FILE* f;
    errno_t err = fopen_s(&f, "highscores.txt", "r");
    if (err != 0 || f == NULL) {
        printf("No high scores available.\n");
        return;
    }

    printf("\n========================\n");
    printf("     HIGH SCORES MENU\n");
    printf("========================\n");

    typedef struct {
        int level;
        char name[MAX_NAME_LEN];
        int attempts;
    } Entry;

    Entry entries[100];
    int count = 0;
    char line[200];

    while (fgets(line, sizeof(line), f)) {
        int diff, attempts;
        char name[MAX_NAME_LEN];
        if (sscanf_s(line, "%d %s %d", &diff, name, (unsigned)_countof(name), &attempts) == 3) {
            entries[count].level = diff;
            entries[count].attempts = attempts;
            strcpy_s(entries[count].name, MAX_NAME_LEN, name);
            count++;
        }
    }
    fclose(f);

    const char* places[] = { "first", "second", "third", "fourth", "fifth" };

    for (int level = 1; level <= 3; level++) {
        printf("\nLevel %d:\n", level);

        Entry filtered[100];
        int filteredCount = 0;
        for (int i = 0; i < count; i++) {
            if (entries[i].level == level) {
                filtered[filteredCount++] = entries[i];
            }
        }

        for (int i = 0; i < filteredCount - 1; i++) {
            for (int j = i + 1; j < filteredCount; j++) {
                if (filtered[j].attempts < filtered[i].attempts) {
                    Entry temp = filtered[i];
                    filtered[i] = filtered[j];
                    filtered[j] = temp;
                }
            }
        }

        if (filteredCount == 0) {
            printf("No high scores yet for this level.\n");
        }
        else {
            int top = (filteredCount < 5) ? filteredCount : 5;
            for (int i = 0; i < top; i++) {
                printf("%d. The %s high score is: by the player %s with %d attempts\n",
                    i + 1, places[i], filtered[i].name, filtered[i].attempts);
            }
        }
    }

    printf("\n========================\n");
}

void playGame(GameState* game, const char* playerName, int level) {
    int guess[MAX_DIGITS];
    Result result;
    int size = game->difficulty;

    printf("Game started! Good luck, %s.\n", playerName);

    do {
        getUserGuess(guess, size);
        result = checkGuess(game->secret, guess, size);
        game->attempts++;

        for (int i = 0; i < size; i++) {
            game->lastGuess[i] = guess[i];
        }
        game->lastResult = result;

        printf("Attempt #%d: Hit = %d, Near Hit = %d\n", game->attempts, result.hit, result.nearHit);
        saveGame(*game);
    } while (result.hit < size);

    printf("Congratulations %s! You cracked it in %d attempts!\n", playerName, game->attempts);
    remove("save_game.txt");
    saveHighScore(level, playerName, game->attempts);
}

int main() {
    srand((unsigned int)time(NULL));
    printf("====================================\n");
    printf("       WELCOME TO CODE BREAKER      \n");
    printf("====================================\n");
    printf("Press ENTER to start the game...\n");
    getchar();

    int choice;
    GameState game;
    char playerName[MAX_NAME_LEN];

    printf("Enter your name: ");
    fgets(playerName, sizeof(playerName), stdin);
    playerName[strcspn(playerName, "\n")] = 0;

    do {
        printf("\n-- CODE BREAKER MENU --\n");
        printf("1. New Game\n");
        printf("2. Load Saved Game\n");
        printf("3. High Scores\n");
        printf("4. Reset High Scores\n");
        printf("5. Exit\n");
        printf("Choose: ");
        scanf_s("%d", &choice);
        clearInput();

        if (choice == 1) {
            int level = selectDifficulty();
            game.difficulty = level + 2;
            game.attempts = 0;
            generateSecret(game.secret, game.difficulty);
            playGame(&game, playerName, level);
        }
        else if (choice == 2) {
            if (loadGame(&game)) {
                printf("Game loaded successfully.\n");
                printf("Last guess was: ");
                for (int i = 0; i < game.difficulty; i++) {
                    printf("%d", game.lastGuess[i]);
                }
                printf(" -> Hit = %d, Near Hit = %d\n", game.lastResult.hit, game.lastResult.nearHit);
                playGame(&game, playerName, game.difficulty - 2);
            }
            else {
                printf("No saved game found.\n");
            }
        }
        else if (choice == 3) {
            showHighScores();
        }
        else if (choice == 4) {
            if (remove("highscores.txt") == 0)
                printf("High scores have been reset.\n");
            else
                printf("Error: Could not reset high scores.\n");
        }

    } while (choice != 5);

    printf("Goodbye %s!\n", playerName);
    return 0;
}
