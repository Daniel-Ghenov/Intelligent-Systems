#include <iostream>
#include "GamePlayer.h"
#include "Board.h"
#include "Solver.h"

void GamePlayer::initiateGame()
{
    while(board.checkWin() == CellState::EMPTY && !board.isFull()) {
        if(initiateGameTurn()) {
            break;
        }
    }
}

bool GamePlayer::initiateGameTurn()
{
    int playerRow, playerCol;
    std::cin >> playerRow >> playerCol;
    playerRow--;
    playerCol--;
    while(!board.makeMove(playerRow, playerCol, otherPlayer(solver.getPlayer()))) {
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
        if (board.checkWin() == CellState::EMPTY)
        {
            std::cout << "DRAW";
            return true;
        }
        std::cout << "WINNER: " << cellStateToChar(board.checkWin()) << std::endl;
        return true;
    }
    return false;
}

void GamePlayer::printBoard() const
{
    board.print();
}

GamePlayer::GamePlayer(CellState player, CellState firstPlayer)
{
    solver = Solver(firstPlayer);
    if(firstPlayer != player)
    {
        Board aiMoveBoard = solver.answerBoard(board);
        board = aiMoveBoard;
    }
}

GamePlayer::GamePlayer(CellState player, CellState firstPlayer, Board initialBoard)
{
    solver = Solver(firstPlayer);
    board = initialBoard;
    if(firstPlayer != player)
    {
        Board aiMoveBoard = solver.answerBoard(board);
        board = aiMoveBoard;
        board.print();
    }
}

std::pair<int, int> GamePlayer::getNextMove()
{
    Board nextBoard = solver.answerBoard(board);
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (board.getBoard()[i][j] != nextBoard.getBoard()[i][j])
            {
                board = nextBoard;
                return {i, j};
            }
        }
    }
    return {-1, -1};
}

