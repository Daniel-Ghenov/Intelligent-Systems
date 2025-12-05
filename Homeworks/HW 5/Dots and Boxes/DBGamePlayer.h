#pragma once
#include "DBSolver.h"
#include "BaseGamePlayer.hpp"


class DBGamePlayer: public BaseGamePlayer<DBSolver, DBBoard>{
public:
    explicit DBGamePlayer(int maxDepth) : BaseGamePlayer<DBSolver, DBBoard>(maxDepth) {}
    DBGamePlayer(CellState player, CellState firstPlayer)
        : BaseGamePlayer<DBSolver, DBBoard>(player, firstPlayer) {}
    DBGamePlayer(CellState player, CellState firstPlayer, DBBoard initialBoard)
        : BaseGamePlayer<DBSolver, DBBoard>(player, firstPlayer, std::move(initialBoard)) {}
    DBGamePlayer(CellState player, DBBoard  initialBoard)
        : BaseGamePlayer<DBSolver, DBBoard>(player, std::move(initialBoard)) {}
    DBGamePlayer(int maxDepth, CellState player)
        : BaseGamePlayer<DBSolver, DBBoard>(maxDepth, player) {}
    DBGamePlayer(CellState player, CellState firstPlayer, DBBoard initialBoard, int maxDepth)
        : BaseGamePlayer<DBSolver, DBBoard>(player, firstPlayer, std::move(initialBoard), maxDepth) {}

private:

    void makeMove() override;
};
