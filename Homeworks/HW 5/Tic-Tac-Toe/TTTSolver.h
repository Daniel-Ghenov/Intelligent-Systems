#pragma once
#include <vector>
#include "TTTBoard.h"
#include "BaseSolver.hpp"

class TTTSolver: public BaseSolver<TTTBoard> {
private:
public:
    explicit TTTSolver(int maxDepth) : BaseSolver<TTTBoard>(maxDepth) {}
    TTTSolver(int maxDepth, CellState player) : BaseSolver<TTTBoard>(maxDepth, player) {}
    explicit TTTSolver(CellState state): BaseSolver<TTTBoard>(state) {}

private:
    std::vector<TTTBoard> generateNextBoards(const TTTBoard &currentBoard, CellState player) override;
    int generateCurrentBoardScore(const TTTBoard&board, CellState player) override;
};

