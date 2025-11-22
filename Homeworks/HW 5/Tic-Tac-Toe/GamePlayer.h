#pragma once
#include "Solver.h"

class GamePlayer {
private:
    Solver solver = Solver(9);
    Board board;
public:
    explicit GamePlayer(int maxDepth) : solver(maxDepth) {}
    GamePlayer(CellState player, CellState firstPlayer);
    GamePlayer(CellState player, CellState firstPlayer, Board initialBoard);
    GamePlayer(CellState player, Board  initialBoard) : solver(player), board(initialBoard) {}
    GamePlayer(int maxDepth, CellState player) : solver(maxDepth, player) {}

    void initiateGame();
    std::pair<int, int> getNextMove();
private:
    bool initiateGameTurn();
    void printBoard() const;

    bool checkWinner() const;
};
