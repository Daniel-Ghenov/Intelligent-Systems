#include <algorithm>
#include "Solver.h"

std::vector<Board> Solver::generateNextBoards(const Board &currentBoard, CellState player)
{
    std::vector<Board> nextBoards;
    auto boardState = currentBoard.getBoard();

    for (int row = 0; row < boardState.size(); ++row)
    {
        for (int col = 0; col < boardState[row].size(); ++col)
        {
            if (boardState[row][col] == CellState::EMPTY)
            {
                Board newBoard = currentBoard;
                newBoard.makeMove(row, col, player);
                nextBoards.push_back(newBoard);
            }
        }
    }
    return nextBoards;
}

int Solver::generateCurrentBoardScore(const Board &board, CellState player)
{
    if (board.checkWin() == player)
        return 1;
    else if (board.checkWin() != CellState::EMPTY && board.checkWin() != player)
        return -1;
    else
        return 0;
}

Board Solver::answerBoard(const Board &board)
{
    return answerBoard(board, this->player, this->maxDepth);
}

Board Solver::answerBoard(const Board &board, CellState currentPlayer, int depth)
{
    std::vector<Board> neighbours = generateNextBoards(board, currentPlayer);
    int maxScore = INT_MIN;
    Board bestBoard;
    for (auto& neighbour : neighbours)
    {
        int score = -1 * generateBoardScore(neighbour, otherPlayer(currentPlayer), depth);
        if (score > maxScore)
        {
            maxScore = score;
            bestBoard = neighbour;
        }
    }
    return bestBoard;
}

int Solver::getMaxScoreBoard(CellState player, std::vector<Board> &neighbours)
{
    int maxScore = INT_MIN;
    for (auto& board : neighbours)
    {
        int currentScore = generateCurrentBoardScore(board, player);
        if (currentScore > maxScore)
        {
            maxScore = currentScore;
        }
    }
    return maxScore;
}

int Solver::generateBoardScore(const Board &board, CellState currentPlayer, int depth)
{
    std::vector<Board> neighbours = generateNextBoards(board, currentPlayer);
    if (board.checkWin() != CellState::EMPTY || board.isFull())
    {
        return generateCurrentBoardScore(board, currentPlayer);
    }
    if (neighbours.empty())
    {
        return generateCurrentBoardScore(board, currentPlayer);
    }
    if (depth == 1)
    {
        return getMaxScoreBoard(currentPlayer, neighbours);
    }

    std::vector<int> neighbourSolutions = std::vector<int>(neighbours.size());
    std::transform(neighbours.begin(), neighbours.end(), neighbourSolutions.begin(),
                   [this, currentPlayer, depth](Board board) {return -1 * generateBoardScore(board, otherPlayer(currentPlayer), depth - 1);});
    return *std::max_element(neighbourSolutions.begin(), neighbourSolutions.end());
}

