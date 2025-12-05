#pragma once
#include "CellState.h"


template<typename Board>
class BaseSolver {
private:

    enum class Player
    {
        MIN,
        MAX
    };

    struct BoardScore
    {
        int score;
        int depth;

        bool operator==(const BoardScore& other) const;
    };

    static BoardScore better(const BoardScore& a, const BoardScore& b, Player player);

    int maxDepth = 9;
    CellState player = CellState::O;
public:
    explicit BaseSolver(int maxDepth) : maxDepth(maxDepth) {}
    BaseSolver(int maxDepth, CellState player) : maxDepth(maxDepth), player(player) {}

    virtual ~BaseSolver() = default;
    virtual Board answerBoard(const Board& board);
    virtual std::vector<Board> generateNextBoards(const Board &currentBoard, CellState player) = 0;
    CellState getPlayer() const { return player; }
    int getMaxDepth() const { return maxDepth; }
    explicit BaseSolver(CellState state): player(state) {}

private:
    Board answerBoard(const Board &board, CellState currentPlayer, int depth, int alpha, int beta);
    virtual int generateCurrentBoardScore(const Board& board, CellState player) = 0;
    BoardScore generateBoardScore(const Board &board, CellState currentPlayer, Player player, int depth, int alpha, int beta);

    static Player revertMinMax(Player player)
    {
        return player == Player::MAX ? Player::MIN : Player::MAX;
    }
};

template<typename Board>
typename BaseSolver<Board>::BoardScore BaseSolver<Board>::better(const BaseSolver<Board>::BoardScore &a, const BaseSolver<Board>::BoardScore &b, BaseSolver<Board>::Player player)
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

template<typename Board>
bool BaseSolver<Board>::BoardScore::operator==(const BaseSolver<Board>::BoardScore &other) const
{
    return score == other.score && depth == other.depth;
}

template<typename Board>
Board BaseSolver<Board>::answerBoard(const Board &board)
{
    return answerBoard(board, this->player, this->maxDepth, INT_MIN, INT_MAX);
}

template<typename Board>
Board BaseSolver<Board>::answerBoard(const Board &board, CellState currentPlayer, int depth, int alpha, int beta)
{
    std::vector<Board> neighbours = generateNextBoards(board, currentPlayer);
    BoardScore maxScore {};
    if (currentPlayer == CellState::X)
        maxScore = {INT_MIN, 0};
    else
        maxScore = {INT_MAX, 0};
    Board bestBoard;
    BaseSolver<Board>::Player minMaxPlayer = currentPlayer == CellState::X ? BaseSolver<Board>::Player::MAX : BaseSolver<Board>::Player::MIN;
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

template<typename Board>
typename BaseSolver<Board>::BoardScore BaseSolver<Board>::generateBoardScore(const Board &board, CellState currentPlayer, BaseSolver<Board>::Player player, int depth, int alpha, int beta)
{
    std::vector<Board> neighbours = generateNextBoards(board, currentPlayer);
    if (board.checkWin() != CellState::EMPTY || board.isFull() || neighbours.empty() || depth == 0)
    {
        return {generateCurrentBoardScore(board, currentPlayer), depth};
    }

    std::vector<int> neighbourSolutions = std::vector<int>(neighbours.size());
    BoardScore maxScore {};
    if (player == BaseSolver<Board>::Player::MAX)
        maxScore = {INT_MIN, 0};
    else
        maxScore = {INT_MAX, 0};

    for (int i = 0; i < neighbours.size(); i++)
    {
        BoardScore boardScore = generateBoardScore(neighbours[i], otherPlayer(currentPlayer), revertMinMax(player), depth - 1, alpha, beta);
        maxScore = better(maxScore, boardScore, player);

        if (player == BaseSolver<Board>::Player::MAX)
        {
            alpha = std::max(alpha, boardScore.score);
        }
        if (player == BaseSolver<Board>::Player::MIN)
        {
            beta = std::min(beta, boardScore.score);
        }
        if (player == BaseSolver<Board>::Player::MIN && maxScore.score < alpha)
        {
            break;
        }
        if (player == BaseSolver<Board>::Player::MAX && maxScore.score > beta)
        {
            break;
        }

    }
    return maxScore;
}