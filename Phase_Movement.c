#include "Phase_Movement.h"
#include "Phase_Init.h"
#include "Define.h"
#include "FileHandler.h"
#include "String.h"
#include "System.h"

void Movement(int player){
#ifdef INTERACTIVE
        int i;
        static int lastFloeScore = 0;
        coordinates penguinToMove;
        coordinates lastPenguinPosition;
        step movesAvailable[6];
        step *pointerMovesAvailable = movesAvailable;
        int numberOfPossibleMoves;
        int chosenMove;
        char *chosenMoveName;
        step move;


        //Print an info
        printf("Turn of P%d\n", player);

        //Choose a penguin to move
        penguinToMove = penguinsChoose(player);
        numberOfPossibleMoves = checkFirstMove(penguinToMove, pointerMovesAvailable);
        while (1) {
        if (numberOfPossibleMoves != 0) {
            printf("Possible moves:\n");
            for (i = 0; i < numberOfPossibleMoves; i++) {
                printf("%d) On floe [%d,%d] (%s)\n", i + 1, movesAvailable[i].coordinates.x,
                       movesAvailable[i].coordinates.y, movesAvailable[i].name);
            }

            //Choose a direction
            chosenMove = moveChoose(numberOfPossibleMoves);
            chosenMoveName = movesAvailable[chosenMove - 1].name; //needed for next moves

            //Move penguin first time
            lastPenguinPosition = penguinToMove;
            board[lastPenguinPosition.x][lastPenguinPosition.y] = lastFloeScore;
            lastFloeScore = board[movesAvailable[chosenMove - 1].coordinates.x][movesAvailable[chosenMove - 1].coordinates.y];
            penguinToMove = makeStep(penguinToMove, movesAvailable[chosenMove - 1].coordinates, player);
            //Print board
            scorePrint();
            boardPrint();

            //Rest of the moves
            move = checkMove(penguinToMove, chosenMoveName);
            while (ifStepForward(makeStepForwardCoordinates(penguinToMove, move))) {
                lastPenguinPosition = penguinToMove;
                board[lastPenguinPosition.x][lastPenguinPosition.y] = lastFloeScore;
                scoreAdd(player, -lastFloeScore);
                lastFloeScore = board[move.coordinates.x][move.coordinates.y];
                penguinToMove = makeStep(lastPenguinPosition, move.coordinates, player);
                move = checkMove(penguinToMove, chosenMoveName);
                scorePrint();
                boardPrint();
            }
            lastFloeScore = 0;
            break;
        } else {
            printf("This penguin has no moves.\nPlease choose another penguin.\n");
            penguinDelete(penguinToMove);
            penguinToMove = penguinsChoose(player);

            if (penguinToMove.x != -1 && penguinToMove.y != -1)
                numberOfPossibleMoves = checkFirstMove(penguinToMove, pointerMovesAvailable);
            else
                printf("You have not penguins.\n");
                break;
          }
        }
#else
    //nothing yet
#endif
};

coordinates penguinsChoose(int player) {
    coordinates penguinsOfPlayer[getPenguins()];
    coordinates error = {-1, -1};

    int x, y, i, penguinsCount = 0;
    int chosenPenguin;
    for (y = 0; y < BoardMY; y++) {
        for (x = 0; x < BoardMX; x++) {
            if (board[x][y] == player + 3) {
                penguinsOfPlayer[penguinsCount].x = x;
                penguinsOfPlayer[penguinsCount].y = y;
                penguinsCount++;
            }
        }
    }

    if (penguinsCount == 0) {
        return error;
    } else {
        printf("Choose coordinates:\n");
        for (i = 0; i < penguinsCount; i++) {
            printf("%d) Penguin on [%d,%d]\n", i + 1, penguinsOfPlayer[i].x, penguinsOfPlayer[i].y);
        }

        do {
            do {
                if (chosenPenguin == ERROR) printf("Invalid data. Please try again.\n");
                printf("Enter a number of penguin:\n");
                chosenPenguin = getIntFromUser();
            } while (chosenPenguin == ERROR);

            if (chosenPenguin > penguinsCount || chosenPenguin <= 0)
                printf("Invalid number, please try again.\n");
        } while (chosenPenguin > penguinsCount || chosenPenguin <= 0);

        printf("You've chosen penguin %d on [%d,%d]\n", chosenPenguin, penguinsOfPlayer[chosenPenguin - 1].x,
               penguinsOfPlayer[chosenPenguin - 1].y);

        return penguinsOfPlayer[chosenPenguin - 1];
    }
}

int moveChoose(int numberOfPossibleMoves) {
    int move;

    do {
        do {
            if (move == ERROR) printf("Invalid data. Please try again.\n");
            printf("Please choose your first move:\n");
            move = getIntFromUser();
        } while (move == ERROR);

        if (move <= 0 || move > numberOfPossibleMoves)
            printf("Invalid number, please try again.\n");
    } while (move <= 0 || move > numberOfPossibleMoves);

    return move;

}

int isFloeValid (coordinates floe) {
    if (floe.x >= 0 && floe.y >= 0 && floe.x < BoardMX && floe.y < BoardMY) {
        if (board[floe.x][floe.y] == 1 || board[floe.x][floe.y] == 2 || board[floe.x][floe.y] == 3)
            return 1;
        else
            return 0;
    } else
        return 0;
}

int checkFirstMove(coordinates penguin, step *movesAvailable) {
    int counter = 0, i;
    char* names[6] = {"upper-left", "upper-right", "left", "right", "bottom-left", "bottom-right"};
    step tempStep;

    for (i = 0; i < 6; i++) {
        tempStep = checkMove(penguin, names[i]);
        if (strcmp(tempStep.name, "error")) {
            *(movesAvailable + counter) = tempStep;
            counter++;
        }
    }

    return counter;
}

step checkMove(coordinates penguin, char* name) {
    if (!strcmp(name, "upper-left")) {
        switch (penguin.y % 2) {
            case 0:
                return createMove(penguin, -1, -1, "upper-left");
            case 1:
                return createMove(penguin, 0, -1, "upper-left");
        }
    }

    if (!strcmp(name, "upper-right")) {
        switch (penguin.y % 2) {
            case 0:
                return createMove(penguin, 0, -1, "upper-right");
            case 1:
                return createMove(penguin, 1, -1, "upper-right");
        }
    }

    if (!strcmp(name, "left")) {
        return createMove(penguin, -1, 0, "left");
    }

    if (!strcmp(name, "right")) {
        return createMove(penguin, 1, 0, "right");
    }

    if (!strcmp(name, "bottom-left")) {
        switch (penguin.y % 2) {
            case 0:
                return createMove(penguin, -1, 1, "bottom-left");
            case 1:
                return createMove(penguin, 0, 1, "bottom-left");
        }
    }

    if (!strcmp(name, "bottom-right")) {
        switch (penguin.y % 2) {
            case 0:
                return createMove(penguin, 0, 1, "bottom-right");
            case 1:
                return createMove(penguin, 1, 1, "bottom-right");
        }
    }

}

step createMove(coordinates penguin, int shiftX, int shiftY, char* directionName) {
    step step;
    coordinates currentFloe;
    currentFloe.x = penguin.x + shiftX;
    currentFloe.y = penguin.y + shiftY;
    if (isFloeValid(currentFloe)) {
        step.coordinates.x = currentFloe.x;
        step.coordinates.y = currentFloe.y;
        step.name = directionName;
        step.stepForward.x = shiftX;
        step.stepForward.y = shiftY;
    } else {
        step.coordinates.x = -1;
        step.coordinates.y = -1;
        step.name = "error";
    }

    return step;
    }

int ifStepForward(coordinates step) {
    char choice;

    if(isFloeValid(step)) {
        while(1) {
                printf("Would you like to do another step? (y/n)\n");
                scanf(" %c", &choice);

                switch (choice) {
                    case 'y':
                        return 1;
                    case 'n':
                        return 0;
                    default:
                        printf("Invalid choice. Please try again\n");
                }
        }
    } else {
        printf("No more moves in this direction available.\n");
        return 0;
    }
}

coordinates makeStepForwardCoordinates(coordinates penguin, step step) {
    coordinates finalCoordinates;

    finalCoordinates.x = penguin.x + step.stepForward.x;
    finalCoordinates.y = penguin.y + step.stepForward.y;

    return finalCoordinates;
}

coordinates makeStep (coordinates penguin, coordinates floe, int player) {
    coordinates newPenguinPlace;
    newPenguinPlace.x = floe.x;
    newPenguinPlace.y = floe.y;

    scoreAdd(player, board[floe.x][floe.y]);
    board[floe.x][floe.y] = player + 3;
    return newPenguinPlace;
}

void penguinDelete(coordinates penguin) {
    board[penguin.x][penguin.y] = 0;
}
