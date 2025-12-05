#pragma once
#include <vector>
#include "DBBoard.h"
#include "BaseSolver.hpp"

class DBSolver : public BaseSolver<DBBoard> {
private:
    static const int DEFAULT_MAX_DEPTH = 10;
public:
    DBSolver(): DBSolver(DEFAULT_MAX_DEPTH) {};
    explicit DBSolver(int maxDepth) : BaseSolver<DBBoard>(maxDepth) {}
    DBSolver(int maxDepth, CellState player) : BaseSolver<DBBoard>(maxDepth, player) {}
    explicit DBSolver(CellState state): BaseSolver<DBBoard>(state) {}

private:
    int generateCurrentBoardScore(const DBBoard&board, CellState player) override;
    std::vector<DBBoard> generateNextBoards(const DBBoard &currentBoard, CellState player) override;
};

