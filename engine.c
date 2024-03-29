#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "engine.h"
#include "board.h"


/**
 * @brief Initializes the board cells' list
 * @param boardCells Linked list with board cells
 */
void initializeCellsList(list *boardCells) {
    boardCells->head = NULL;
    boardCells->tail = NULL;
    boardCells->length = 0;
}

/**
 * @brief Inserts a new board cell into the board cells' linked list.
 * @param boardCells Linked list with board cells
 * @param cell The cell to be inserted
 * @return Returns 0 on 'SUCCESS' and 1 on 'FAILURE'
 */
int insertBoardCell(list *boardCells, node *cell) {
    if (boardCells->head == NULL) {  // List is empty
        boardCells->head = cell;
        boardCells->tail = cell;
        boardCells->length = 1;
    } else {  // List has more than 1 item
        boardCells->tail->next = cell;
        boardCells->tail = cell;
        boardCells->length++;
    }

    return 0;
}

/**
 * @brief Performs board setup. Initializes all board cells 
 * and places home cells as well as safe cells.
 * @param boardCells Linked list with board cells
 * @param safeCells Int array with safe cells position
 * @param totalCells The number of total cells
 * @return Returns 0 on 'SUCCESS' and 1 on 'FAILURE'
 */
int boardSetup(list *boardCells, int *safeCells, int totalCells) {
    // Adds cells to board (Board Setup)
    for (int cellIndex = 0; cellIndex < totalCells; cellIndex++) {
        int letterIdx;
        node *cell = (node*) malloc(sizeof(node));  // Creates a new 'node'

        // Checks if an ERROR ocurred while allocating memory (i.e. out of memory)
        if (cell == NULL) {
            return 1;
        }

        // Adds data to new place
        if (cellIndex == 0) {  // Initializes home cell for player 1
            for (letterIdx = 0; letterIdx <= 3; letterIdx++) {
                cell->item.jogador_peao[0][letterIdx] = TRUE;
                cell->item.jogador_peao[1][letterIdx] = FALSE;
            }

            // Sets home cells as safe cells
            cell->item.casaSegura = 1;

        } else if (cellIndex == (totalCells / 2)) {  // Initializes home cell for player 2
            for (letterIdx = 0; letterIdx <= 3; letterIdx++) {
                cell->item.jogador_peao[1][letterIdx] = TRUE;
                cell->item.jogador_peao[0][letterIdx] = FALSE;
            }

            // Sets home cells as safe cells
            cell->item.casaSegura = TRUE;
        } else {
            // Initializes safe cells given in the config file
            if (safeCells[cellIndex] == TRUE) {
                cell->item.casaSegura = TRUE;
            } else {
                cell->item.casaSegura = FALSE;
            }

            // Cleans all the other player cells
            for (letterIdx = 0; letterIdx <= 3; letterIdx++) {
                cell->item.jogador_peao[0][letterIdx] = FALSE;
                cell->item.jogador_peao[1][letterIdx] = FALSE;
            }
        }

        // Adds data to new cell
        cell->next = NULL;
        
        // Insert new cell onto the board
        if (insertBoardCell(boardCells, cell) == 1) {
            return 1;
        }
    }

    return 0;
}

/**
 * @brief Returns whether the given pawn is a valid pawn or not based on the player of the current play. 
 * @param pawn The given pawn
 * @param player Whether it is player 1 or not
 * @return Returns whether the given pawn is valid or not
 */
bool validPawn(char pawn, bool player1) {
    for (int i = 1; i < 5; i++) {
        if (player1 == true && SYMBOLS_J1[i] == pawn) {
            return true;
        }

        if (player1 == false && SYMBOLS_J2[i] == pawn) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Checks if any of the two player has already won the game.
 * @param boardCells Linked list with board cells
 * @param totalCells The number of total cells
 * @return Returns 1 if 'Player 1 WON the game', 2 if 'Player 2 WON the game' and 0 if the 'Game still in progress'
 */
int checkGameWin(list *boardCells, int totalCells) {
    int homeP1 = 0;  // Player 1 home
    int homeP2 = totalCells / 2;  // Player 2 home
    bool p1Won = true;  // Player 1 win case
    bool p2Won = true;  // Player 2 win case

    node currentNode = *boardCells->head;  // Holds current node being checked

    for (int home = homeP1; home <= homeP2; home++) {
        for (int pawnIdx = 0; pawnIdx < 4; pawnIdx++) {
            if (home == homeP1 && currentNode.item.jogador_peao[0][pawnIdx] != WIN) {
                p1Won = false;
            }

            if (home == homeP2 && currentNode.item.jogador_peao[1][pawnIdx] != WIN) {
                p2Won = false;
            }
        }
        
        // Gets next node
        currentNode = *currentNode.next;
    }

    // Checks which player won, if any
    if (p1Won) {
        return 1;
    } else if (p2Won) {
        return 2;
    } else {
        return 0;
    }
}

/**
 * @brief Gets the index of the given pawn in the cell.
 * @param pawn The given pawn 
 * @return Returns the index of the given pawn. If the pawn is not valid returns -1.
 */
int getPawnIndex(char pawn) {
    int index;
    if (pawn == 'a' || pawn == 'w') {
        index = 0;
    } else if (pawn == 'b' || pawn == 'x') {
        index = 1;
    } else if (pawn == 'c' || pawn == 'y') {
        index = 2;
    } else if (pawn == 'd' || pawn == 'z') {
        index = 3;
    } else {
        index = -1;
    }

    return index;
}

/**
 * @brief Get the Node Index for the given pawn.
 * @param boardCells Linked list with board cells
 * @param pawn The given pawn
 * @return Returns the node index for the given pawn
 */
int getPawnNodeIndex(list *boardCells, char pawn) {
    int playerPos;  // Gets the player position to which the pawn belongs (0 - P1, 1 - P2)
    int pawnPos;  // Gets the pawn position
    int nodeIndex;  // Holds the current node index being checked
    node currentNode = *boardCells->head; // Holds the current node being checked 
    
    // Sets 'playerPos' based on the given 'pawn'
    if (pawn >= 97 && pawn <= 100) {
        playerPos = 0;
    } else {
        playerPos = 1;
    }

    // Sets 'pawnPos' based on the given 'pawn'
    pawnPos = getPawnIndex(pawn);
    
    // Iterates over the board cells until it finds the pawn position
    for (nodeIndex = 0; nodeIndex < boardCells->length; nodeIndex++) {
        if ((char) currentNode.item.jogador_peao[playerPos][pawnPos] == TRUE) {
            return nodeIndex;
        }
        currentNode = *currentNode.next;
    }

    return -1;
}

/**
 * @brief Moves a pawn from a given source to a given destination.
 * @param boardCells Linked list with board cells
 * @param pawn The given pawn
 * @param pawnIndex The index in 'jogador_peao' array inside 'node' for the current pawn
 * @param srcIndex The index for the node where the pawn currently resides
 * @param destIndex The index for the node to which the pawn should reside from now on
 */
void movePawn(list *boardCells, char pawn, int pawnIndex, int srcIndex, int destIndex) {
    int playerIndex;
    int currentIndex;
    node *currentNode = boardCells->head;
    int homeP1 = 0;
    int homeP2 = boardCells->length / 2;
    int totalCells = boardCells->length;
    int finalDestIndex = destIndex;
    bool completesLap;

    // Gets player index to access based on the given pawn
    if (pawn == 'a' || pawn == 'b' || pawn == 'c' || pawn == 'd') {
        playerIndex = 0;
    } else {
        playerIndex = 1;
    }

    // Calculates destination index for P2
    if (playerIndex == 1) {
        if (srcIndex >= 0 && srcIndex < homeP2) {
            finalDestIndex = destIndex > homeP2 ? homeP2 : destIndex;
        } else {
            finalDestIndex = destIndex >= totalCells ? destIndex - totalCells : destIndex;
        }
    }

    // Checks if pawn completes lap in current play
    completesLap = pawnCompletesLapInCurrentPlay(playerIndex, boardCells->length, srcIndex, destIndex, finalDestIndex);

    for (currentIndex = 0; currentIndex < boardCells->length; currentIndex++) {
        if (currentIndex == srcIndex) {
            // Removes the pawn from its current position in the board
            currentNode->item.jogador_peao[playerIndex][pawnIndex] = FALSE;
        }

        /* 
            Checks if pawn has gone around the whole board, if that's the case, 
            the pawn must be moved to its home cell and converted to uppercase
        */
        // Win case for P1
        if (completesLap && playerIndex == 0 && currentIndex == homeP1) {
            currentNode->item.jogador_peao[playerIndex][pawnIndex] = WIN;
        }
        
        // Win case for P2
        if (completesLap && playerIndex == 1 && currentIndex == homeP2) {
            currentNode->item.jogador_peao[playerIndex][pawnIndex] = WIN;
        }
        
        if (!completesLap && currentIndex == finalDestIndex) {  // Places the pawn in its new destination
            currentNode->item.jogador_peao[playerIndex][pawnIndex] = TRUE;
        }

        // Gets next node
        currentNode = currentNode->next;
    }
}

/**
 * @brief Resets the adversary pawn by removing it from its current place
 * and moving it back to its home cell.
 * @param boardCells Linked list with board cells
 * @param pawn The given pawn
 * @param player The player who owns the pawn ('0' - P1, '1' - P2)
 * @param pawnSrcIndex The current node index in which the pawn currently is
 */
void resetAdversaryPawn(list *boardCells, char pawn, int player, int pawnSrcIndex) {
    int playerHome;  // Stores player home node index
    int totalCells = boardCells->length;  // Total amount of board cells
    char playerSymbols[10];  // Stores the symbols for the current player
    int pawnIndex;  // Stores the index of the pawn in 'playerSymbols'
    node *currentNode;  // Store the current node being checked

    if (player == 0) {
        playerHome = 0;
        strcpy(playerSymbols, SYMBOLS_J1);
    } else {
        playerHome = totalCells / 2;
        strcpy(playerSymbols, SYMBOLS_J2);
    }

    // Gets pawn index
    pawnIndex = getPawnIndex(pawn);

    currentNode = boardCells->head;

    for (int nodeIndex = 0; nodeIndex < totalCells; nodeIndex++) {
        // Removes pawn from its current place
        if (nodeIndex == pawnSrcIndex) {
            currentNode->item.jogador_peao[player][pawnIndex] = FALSE;
        }

        // Adds pawn to its home cell
        if (nodeIndex == playerHome) {
            currentNode->item.jogador_peao[player][pawnIndex] = TRUE;
        }

        // Gets next node
        currentNode = currentNode->next;
    }
}

/**
 * @brief Makes the play based on the given pawn and the amount of cells it should advance.
 * @param boardCells Linked list with board cells
 * @param pawn The given pawn
 * @param amount The amount of cells the pawn should advance (based on dices value)
 */
void makePlay(list *boardCells, char pawn, int amount) {
    int playerIndex;
    int adversaryPlayerIndex;
    node *currentNode;  // Stores the pointer of the current node being checked
    int currentIndex;  // Stores current node index
    int pawnIndex;  // Stores the index of the pawn in the cell
    char playerSymbols[10];  // Stores string with player symbols
    char adversarySymbols[10];  // Stores string with adversary symbols
    int placesMoved;  // Stores the number of places the current pawn will be moved
    int totalCells = boardCells->length;  // Stores the number of total board cells

    // Gets current pawn node index
    int pawnCurrentPos = getPawnNodeIndex(boardCells, pawn);

    // Gets player index based on pawn, sets 'playerSymbols' based on it
    if (pawn == 'a' || pawn == 'b' || pawn == 'c' || pawn == 'd') {
        playerIndex = 0;
        strcpy(playerSymbols, SYMBOLS_J1);
        strcpy(adversarySymbols, SYMBOLS_J2);
    } else {
        playerIndex = 1;
        strcpy(playerSymbols, SYMBOLS_J2);
        strcpy(adversarySymbols, SYMBOLS_J1);
    }

    // Sets 'adversaryPlayerIndex'
    adversaryPlayerIndex = playerIndex == 0 ? 1 : 0;

    // Gets pawn index in the cell
    pawnIndex = getPawnIndex(pawn);

    // Moves the chosen 'pawn' to its destination based on 'amount' (dices value)
    movePawn(boardCells, pawn, pawnIndex, pawnCurrentPos, pawnCurrentPos + amount);

    // Sets number of placed the pawn moved
    if (playerIndex == 0) {  // P1
        placesMoved = pawnCurrentPos + amount >= totalCells ? totalCells - pawnCurrentPos : amount;
    } else {  // P2
        // Case: current pawn index is between 0 (inclusive) and home (exclusive)
        if (pawnCurrentPos >= 0 && pawnCurrentPos < totalCells / 2) {
            // If the new pawn position is beyond board length
            if (pawnCurrentPos + amount >= totalCells / 2) {
                // 'placesMoved' is the index of P2 home less the current position index
                placesMoved = totalCells / 2 - pawnCurrentPos;
            } else {  // New pawn position is within board length
                placesMoved = amount;
            }
        } else {  // Case: current pawn index is between home (inclusive) and board length
            // If the new pawn index is beyond the board length
            if (pawnCurrentPos + amount >= totalCells) {
                placesMoved = totalCells - pawnCurrentPos;

                // Adds the left over
                // First we check if what's left is enough to complete a lap with the current pawn
                if (amount - (totalCells - pawnCurrentPos) >= totalCells / 2) {
                    // If it is, we add the amount needed to complete the lap to 'placedMoved'
                    placesMoved += totalCells / 2;
                } else {
                    // If not, we just add whatever amount was left over
                    placesMoved += amount - (totalCells - pawnCurrentPos);
                }
            } else {
                placesMoved = amount;
            }
        }
    }

    /* 
        Checks every board cell that the current pawn will go through.
        If the cell is not a safe cell, moves all the other player
        cells to his home cell.
    */

   // Sets 'currentNode' to the first node of the board

   currentNode = boardCells->head;
   
    for (currentIndex = 0; currentIndex < totalCells && placesMoved > 0; currentIndex++) {
        if (currentIndex > pawnCurrentPos) {
            // Checks if the current node has any of the opponnent players pawns
            for (int symbol = 0; symbol < 4; symbol++) {
                if (currentNode->item.jogador_peao[adversaryPlayerIndex][symbol] == TRUE) {
                    resetAdversaryPawn(boardCells, adversarySymbols[symbol+1], adversaryPlayerIndex, currentIndex);
                }
            }
            
            placesMoved--;
        }
        
        // Gets next node
        currentNode = currentNode->next;
    }

    // Sets the node to the beginnig of the linked list again
    currentNode = boardCells->head;

    // If its P2 and there's still cells to 'clear' we need to iterate starting at index 0 again
    if (placesMoved > 0 && playerIndex == 1) {
        for (currentIndex = 0; currentIndex <= totalCells / 2 && placesMoved > 0; currentIndex++) {
            // Checks if the current node has any of the opponnent players pawns
            for (int symbol = 0; symbol < 4; symbol++) {
                if (currentNode->item.jogador_peao[adversaryPlayerIndex][symbol] == TRUE) {
                    resetAdversaryPawn(boardCells, adversarySymbols[symbol+1], adversaryPlayerIndex, currentIndex);
                }
            }
            
            placesMoved--;
            
            // Gets next node
            currentNode = currentNode->next;
        }
    }
}

/**
 * @brief Returns whether the pawn will complete a lap in the current play.
 * @param player The current player ('0' - P1, '1' - P2)
 * @param srcIndex The current pawn node index
 * @param destIndex The pawn destination node index
 * @return Returns whether the pawn completes a lap in the current play 
 */
bool pawnCompletesLapInCurrentPlay(int player, int totalCells, int srcIndex, int destIndex, int finalDestIndex) {
    // P1
    // If 'destIndex' exceeds board length, then pawn completes a lap
    if (player == 0 && destIndex >= totalCells) {
        return true;
    }

    // P2
    if (player == 1) {
        /* 
            If pawn 'srcIndex' is between 0 (inclusive) and P2 home (exclusive) 
            and 'finalDestIndex' exceeds P2 home, then pawn completes lap
        */
        if ((srcIndex < totalCells / 2 && srcIndex >= 0) && finalDestIndex >= totalCells / 2) {
            return true;
        }

        /*
            If pawn 'srcIndex' is different from the above, then we just need to check if
            the pawn completes a lap by checking if 'finalDestIndex' is greater than P2 home
        */
        if (destIndex >= totalCells && finalDestIndex >= totalCells / 2) {
            return true;
        }
    }

    return false;
}

/**
 * @brief Returns whether a pawn is moveable or not.
 * @param pawn The given pawn
 * @param boardCells Linked list with board cells
 * @param player The current player (P1 - 'true', P2 - 'false')
 * @return Returns whether pawn is moveable
 */
bool isPawnMovable(char pawn, list *boardCells, bool player1) {
    int pawnIndex = getPawnIndex(pawn);
    int homeP1 = 0;
    int homeP2 = boardCells->length / 2;
    node currentNode = *boardCells->head;

    if (player1) {
        if (currentNode.item.jogador_peao[0][pawnIndex] != WIN) {
            return true;
        }
    } else {
        for (int pos = homeP1; pos <= homeP2; pos++) {
            // If the pawn is not uppercase then its moveable
            if (pos == homeP2 && currentNode.item.jogador_peao[1][pawnIndex] != WIN) {
                return true;
            }

            // Gets next node
            currentNode = *currentNode.next;
        }
    }
    return false;
}
