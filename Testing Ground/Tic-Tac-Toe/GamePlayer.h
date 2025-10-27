#pragma once
#include "Solver.h"

class GamePlayer {
private:
    Solver solver;
    Board board;
public:
    explicit GamePlayer(int maxDepth) : solver(maxDepth) {}
    explicit GamePlayer(CellState player) : solver(player) {}
    GamePlayer(int maxDepth, CellState player) : solver(maxDepth, player) {}

    void initiateGame();
private:
    bool initiateGameTurn();
    void printBoard() const;

    bool checkWinner() const;
};
