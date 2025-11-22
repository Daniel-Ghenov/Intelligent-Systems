#include <vector>
#include "Board.h"
#pragma once

class Solver {
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
    explicit Solver(int maxDepth) : maxDepth(maxDepth) {}
    Solver(int maxDepth, CellState player) : maxDepth(maxDepth), player(player) {}
    Board answerBoard(const Board& board);
    CellState getPlayer() const { return player; }
    int getMaxDepth() const { return maxDepth; }

    explicit Solver(CellState state): player(state) {}

private:
    Board answerBoard(const Board &board, CellState currentPlayer, int depth, int alpha, int beta);
    static std::vector<Board> generateNextBoards(const Board &currentBoard, CellState player);
    static int generateCurrentBoardScore(const Board&board, CellState player);
    BoardScore generateBoardScore(const Board &board, CellState currentPlayer, Player player, int depth, int alpha, int beta);

    static Player revertMinMax(Player player)
    {
        return player == Player::MAX ? Player::MIN : Player::MAX;
    }
};

