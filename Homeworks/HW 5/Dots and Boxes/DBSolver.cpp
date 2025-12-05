#include <algorithm>
#include "DBSolver.h"

int DBSolver::generateCurrentBoardScore(const DBBoard &board, CellState player)
{
    int countPlayer = board.countBoxes(player);
    int countOpponent = board.countBoxes(otherPlayer(player));
    return countPlayer - countOpponent;
}

std::vector<DBBoard> DBSolver::generateNextBoards(const DBBoard &currentBoard, CellState player)
{
    std::vector<DBBoard> nextBoards;
    std::vector<DBBoard> horizontalBoards;
    std::vector<DBBoard> verticalBoards;
    for (int i = 0; i < currentBoard.getN(); ++i)
    {
        for (int j = 0; j < currentBoard.getM() + 1; ++j)
        {
            if (!currentBoard.getRows()[i][j])
            {
                DBBoard newBoard = currentBoard;
                bool gotBox = newBoard.makeMove(true, i, j, player);
                if (gotBox)
                {
                    std::vector<DBBoard> tempBoards = generateNextBoards(newBoard, player);
                    horizontalBoards.insert(horizontalBoards.end(), tempBoards.begin(), tempBoards.end());
                }
                else
                {
                    horizontalBoards.push_back(newBoard);
                }
            }
        }
    }
    for (int i = 0; i < currentBoard.getN() + 1; ++i)
    {
        for (int j = 0; j < currentBoard.getM(); ++j)
        {
            if (!currentBoard.getCols()[i][j])
            {
                DBBoard newBoard = currentBoard;
                bool gotBox = newBoard.makeMove(false, i, j, player);
                if (gotBox)
                {
                    std::vector<DBBoard> tempBoards = generateNextBoards(newBoard, player);
                    verticalBoards.insert(verticalBoards.end(), tempBoards.begin(), tempBoards.end());
                }
                else
                {
                    verticalBoards.push_back(newBoard);
                }
            }
        }
    }
    nextBoards = std::move(horizontalBoards);
    nextBoards.insert(nextBoards.end(), verticalBoards.begin(), verticalBoards.end());

    return nextBoards;
}
