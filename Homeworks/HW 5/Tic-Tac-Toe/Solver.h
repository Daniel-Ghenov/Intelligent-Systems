#include <vector>
#include "Board.h"
#pragma once

class Solver {
private:
    int maxDepth = -1;
    CellState player = CellState::O;
public:
    explicit Solver(int maxDepth) : maxDepth(maxDepth) {}
    Solver(int maxDepth, CellState player) : maxDepth(maxDepth), player(player) {}
    Board answerBoard(const Board& board);
    CellState getPlayer() const { return player; }
    int getMaxDepth() const { return maxDepth; }

    explicit Solver(CellState state): player(state) {}

private:
    Board answerBoard(const Board& board, CellState currentPlayer, int depth);
    static std::vector<Board> generateNextBoards(const Board &currentBoard, CellState player);
    static int generateCurrentBoardScore(const Board&board, CellState player);

    static int getMaxScoreBoard(CellState player, std::vector<Board> &neighbours) ;

    int generateBoardScore(const Board &board, CellState currentPlayer, int depth);
};

