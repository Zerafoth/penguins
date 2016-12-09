#include "Movement.h"
#include "Define.h"
#include "FileHandler.h"
#include "Init.h"
#include "String.h"

void Movement(int player){
#ifdef INTERACTIVE
        int i;
        coordinates penguinToMove;
        step movesAvailable[6];
        step *pointerMovesAvailable = movesAvailable;
        int numberOfPossibleMoves;
        int chosenMove;
        char *chosenMoveName;

        //Print an info
        printf("Turn of P%d\n", player);

        //Choose a penguin to move
        penguinToMove = penguinsChoose(player);

        //Find possible moves
        numberOfPossibleMoves = checkFirstMove(penguinToMove, pointerMovesAvailable);
        printf("Possible moves:\n");
        for(i = 0; i < numberOfPossibleMoves; i++) {
            printf("%d) On floe [%d,%d] (%s)\n", i + 1, movesAvailable[i].coordinates.x, movesAvailable[i].coordinates.y, movesAvailable[i].name);
        }

        //Choose a direction
        chosenMove = moveChoose(numberOfPossibleMoves);
        chosenMoveName = movesAvailable[chosenMove - 1].name; //needed for next moves

        //Move penguin first time
        penguinToMove = makeStep(penguinToMove, movesAvailable[chosenMove - 1].coordinates, player);

        //Print board
        scorePrint();
        boardPrint();

        //Rest of the moves
        while(ifStepForward(makeStepForwardCoordinates(penguinToMove, movesAvailable[chosenMove -1]))) {
            numberOfPossibleMoves = checkFirstMove(penguinToMove, pointerMovesAvailable);
            for (i = 0; i < numberOfPossibleMoves; i++)
                if (!strcmp(chosenMoveName, movesAvailable[i].name)) {
                    chosenMove = i + 1;
                    break;
                }

            penguinToMove = makeStep(penguinToMove, movesAvailable[chosenMove - 1].coordinates, player);

            scorePrint();
            boardPrint();
        }
#else
    //nothing yet
#endif
};

coordinates penguinsChoose(int player) {
    coordinates penguinsOfPlayer[getPenguins()];

    int x, y, i, penguinsCount = 0;
    int chosenPenguin;
    for (y = 0; y < BOARD_SIZE_Y; y++) {
        for (x = 0; x < BOARD_SIZE_X; x++) {
            if (board[x][y] == player + 3) {
                penguinsOfPlayer[penguinsCount].x = x;
                penguinsOfPlayer[penguinsCount].y = y;
                penguinsCount++;
            }
        }
    }

    printf("Choose coordinates:\n");
    for(i = 0; i < penguinsCount; i++) {
            printf("%d) Penguin on [%d,%d]\n", i + 1, penguinsOfPlayer[i].x, penguinsOfPlayer[i].y);
    }

    do {
        printf("Enter a number of penguin:\n");
        scanf("%d", &chosenPenguin);

        if (chosenPenguin > penguinsCount || chosenPenguin <= 0)
            printf("Invalid number, please try again.\n");
    } while (chosenPenguin > penguinsCount || chosenPenguin <= 0);

    printf("You've chosen penguin %d on [%d,%d]\n", chosenPenguin, penguinsOfPlayer[chosenPenguin - 1].x, penguinsOfPlayer[chosenPenguin -1].y);

    return penguinsOfPlayer[chosenPenguin - 1];
}

int moveChoose(int numberOfPossibleMoves) {
    int move;

    do {
        printf("Please choose your first move:\n");
        scanf("%d", &move);

        if (move <= 0 || move > numberOfPossibleMoves)
            printf("Invalid number, please try again.\n");
    } while (move <= 0 || move > numberOfPossibleMoves);

    return move;

}

int isFloeValid (coordinates floe) {
    if (floe.x >= 0 && floe.y >= 0 && floe.x < BOARD_SIZE_X && floe.y < BOARD_SIZE_Y) {
        if (board[floe.x][floe.y] == 1 || board[floe.x][floe.y] == 2 || board[floe.x][floe.y] == 3)
            return 1;
        else
            return 0;
    } else
        return 0;
}

int addFloeToList(coordinates penguin, step *movesAvailable, int counter, int shiftX, int shiftY, char* name) {
    coordinates currentFloe;
    currentFloe.x = penguin.x + shiftX;
    currentFloe.y = penguin.y + shiftY;
    if (isFloeValid(currentFloe)) {
        (movesAvailable + counter)->coordinates.x = currentFloe.x;
        (movesAvailable + counter)->coordinates.y = currentFloe.y;
        (movesAvailable + counter)->name = name;
        (movesAvailable + counter)->stepForward.x = shiftX;
        (movesAvailable + counter)->stepForward.y = shiftY;
        counter++;
    }

    return counter;
}

int checkFirstMove(coordinates penguin, step *movesAvailable) {
    int counter = 0;

    switch(penguin.y % 2) {
        case 0:
            //upper left
            counter = addFloeToList(penguin, movesAvailable, counter, -1, -1, "upper-left");
            //upper right
            counter = addFloeToList(penguin, movesAvailable, counter, 0, -1, "upper-right");
            //left
            counter = addFloeToList(penguin, movesAvailable, counter, -1, 0, "left");
            //right
            counter = addFloeToList(penguin, movesAvailable, counter, 1, 0, "right");
            //bottom left
            counter = addFloeToList(penguin, movesAvailable, counter, -1, 1, "bottom-left");
            //bottom right
            counter = addFloeToList(penguin, movesAvailable, counter, 0, 1, "bottom-right");
            break;
        case 1:
            //upper left
            counter = addFloeToList(penguin, movesAvailable, counter, 0, -1, "upper-left");
            //upper right
            counter = addFloeToList(penguin, movesAvailable, counter, 1, -1, "upper-right");
            //left
            counter = addFloeToList(penguin, movesAvailable, counter, -1, 0, "left");
            //right
            counter = addFloeToList(penguin, movesAvailable, counter, 1, 0, "right");
            //bottom left
            counter = addFloeToList(penguin, movesAvailable, counter, 0, 1, "bottom-left");
            //bottom right
            counter = addFloeToList(penguin, movesAvailable, counter, 1, 1, "bottom-right");
            break;
    }

    return counter;
}

int ifStepForward(coordinates step) {
    char choice;

    while(1) {
        if(isFloeValid(step)) {
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
        } else {
            printf("No more moves in this direction available.\n");
            return 0;
        }
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
    board[floe.x][floe.y] = board[penguin.x][penguin.y];
    board[penguin.x][penguin.y] = 0;
    return newPenguinPlace;
}