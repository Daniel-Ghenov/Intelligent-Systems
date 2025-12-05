#pragma once
#include "TTTSolver.h"
#include "BaseGamePlayer.hpp"

class TTTGamePlayer: public BaseGamePlayer<TTTSolver, TTTBoard>{
public:
    explicit TTTGamePlayer(int maxDepth) : BaseGamePlayer<TTTSolver, TTTBoard>(maxDepth) {}
    TTTGamePlayer(CellState player, CellState firstPlayer)
        : BaseGamePlayer<TTTSolver, TTTBoard>(player, firstPlayer) {}

    TTTGamePlayer(CellState player, CellState firstPlayer, TTTBoard initialBoard)
        : BaseGamePlayer<TTTSolver, TTTBoard>(player, firstPlayer, initialBoard) {}

    TTTGamePlayer(CellState player, TTTBoard  initialBoard)
        : BaseGamePlayer<TTTSolver, TTTBoard>(player, initialBoard) {}

    TTTGamePlayer(int maxDepth, CellState player)
        : BaseGamePlayer<TTTSolver, TTTBoard>(maxDepth, player) {}

    std::pair<int, int> getNextMove();
private:
    void makeMove() override;
};
