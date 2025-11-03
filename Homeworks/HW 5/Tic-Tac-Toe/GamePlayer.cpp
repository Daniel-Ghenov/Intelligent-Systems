#include <iostream>
#include "GamePlayer.h"

void GamePlayer::initiateGame()
{
    board.reset();
    while(board.checkWin() == CellState::EMPTY && !board.isFull()) {
        if(initiateGameTurn()) {
            break;
        }
    }
}

bool GamePlayer::initiateGameTurn()
{
    int playerRow, playerCol;
    std::cout << "Enter your move (row and column): ";
    std::cin >> playerRow >> playerCol;
    while(!board.makeMove(playerRow, playerCol, otherPlayer(solver.getPlayer()))) {
        std::cout << "Invalid move. Enter your move (row and column): ";
        std::cin >> playerRow >> playerCol;
    }
    printBoard();

    if(checkWinner())
    {
        return true;
    }

    Board aiMoveBoard = solver.answerBoard(board);
    board = aiMoveBoard;
    printBoard();
    if(checkWinner())
    {
        return true;
    }

    return false;
}

bool GamePlayer::checkWinner() const
{
    if(board.checkWin() != CellState::EMPTY || board.isFull())
    {
        std::cout << "Game Over!" << std::endl;
        std::cout << "Winner: " << cellStateToString(board.checkWin()) << std::endl;
        return true;
    }
    return false;
}

void GamePlayer::printBoard() const
{
    board.print();
}

