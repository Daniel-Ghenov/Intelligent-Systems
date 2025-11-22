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
    if (board.checkWin() == CellState::X)
        return 1;
    else if (board.checkWin() == CellState::O)
        return -1;
    else
        return 0;
}

Board Solver::answerBoard(const Board &board)
{
    return answerBoard(board, this->player, this->maxDepth, INT_MIN, INT_MAX);
}

Board Solver::answerBoard(const Board &board, CellState currentPlayer, int depth, int alpha, int beta)
{
    std::vector<Board> neighbours = generateNextBoards(board, currentPlayer);
    BoardScore maxScore {};
    if (currentPlayer == CellState::X)
        maxScore = {INT_MIN, 0};
    else
        maxScore = {INT_MAX, 0};
    Board bestBoard;
    Solver::Player minMaxPlayer = currentPlayer == CellState::X ? Solver::Player::MAX : Solver::Player::MIN;
    for (auto& neighbour : neighbours)
    {
        BoardScore boardScore = generateBoardScore(neighbour, otherPlayer(currentPlayer), revertMinMax(minMaxPlayer), depth, alpha, beta);
        if (better(boardScore, maxScore, minMaxPlayer) == boardScore)
        {
            maxScore = boardScore;
            bestBoard = neighbour;
            alpha = std::max(alpha, boardScore.score);
        }
    }
    return bestBoard;
}

Solver::BoardScore Solver::generateBoardScore(const Board &board, CellState currentPlayer, Solver::Player player, int depth, int alpha, int beta)
{
    std::vector<Board> neighbours = generateNextBoards(board, currentPlayer);
    if (board.checkWin() != CellState::EMPTY || board.isFull() || neighbours.empty() || depth == 0)
    {
        return {generateCurrentBoardScore(board, currentPlayer), depth};
    }

    std::vector<int> neighbourSolutions = std::vector<int>(neighbours.size());
    BoardScore maxScore {};
    if (player == Solver::Player::MAX)
        maxScore = {INT_MIN, 0};
    else
        maxScore = {INT_MAX, 0};

    for (int i = 0; i < neighbours.size(); i++)
    {
        BoardScore boardScore = generateBoardScore(neighbours[i], otherPlayer(currentPlayer), revertMinMax(player), depth - 1, alpha, beta);
        maxScore = better(maxScore, boardScore, player);

        if (player == Solver::Player::MAX)
        {
            alpha = std::max(alpha, boardScore.score);
        }
        if (player == Solver::Player::MIN)
        {
            beta = std::min(beta, boardScore.score);
        }
        if (player == Solver::Player::MIN && maxScore.score < alpha)
        {
            break;
        }
        if (player == Solver::Player::MAX && maxScore.score > beta)
        {
            break;
        }

    }
    return maxScore;
}

Solver::BoardScore Solver::better(const Solver::BoardScore &a, const Solver::BoardScore &b, Solver::Player player)
{
    if (player == Player::MAX)
    {
        if (a.score == b.score)
        {
            if (a.score > 0)
                return a.depth > b.depth ? a : b;
            else
                return a.depth < b.depth ? a : b;
        }
        return a.score > b.score ? a : b;
    }
    else
    {
        if (a.score == b.score)
        {
            if (a.score > 0)
                return a.depth < b.depth ? a : b;
            else
                return a.depth > b.depth ? a : b;
        }
        return a.score < b.score ? a : b;
    }
}

bool Solver::BoardScore::operator==(const Solver::BoardScore &other) const
{
    return score == other.score && depth == other.depth;
}
