#include <algorithm>
#include "TTTSolver.h"

std::vector<TTTBoard> TTTSolver::generateNextBoards(const TTTBoard &currentBoard, CellState player)
{
    std::vector<TTTBoard> nextBoards;
    auto boardState = currentBoard.getBoard();

    for (int row = 0; row < boardState.size(); ++row)
    {
        for (int col = 0; col < boardState[row].size(); ++col)
        {
            if (boardState[row][col] == CellState::EMPTY)
            {
                TTTBoard newBoard = currentBoard;
                newBoard.makeMove(row, col, player);
                nextBoards.push_back(newBoard);
            }
        }
    }
    return nextBoards;
}

int TTTSolver::generateCurrentBoardScore(const TTTBoard &board, CellState player)
{
    if (board.checkWin() == CellState::X)
        return 1;
    else if (board.checkWin() == CellState::O)
        return -1;
    else
        return 0;
}