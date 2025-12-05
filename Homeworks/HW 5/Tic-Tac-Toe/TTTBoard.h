#pragma once
#include <array>
#include <stdexcept>
#include "CellState.h"

class TTTBoard {
private:

    std::array<std::array<CellState, 3>, 3> board{};
public:
    TTTBoard();
    explicit TTTBoard(std::array<std::array<CellState, 3>, 3> initialBoard);
    bool makeMove(int row, int col, CellState player);
    CellState checkWin() const;
    bool isFull() const;
    std::array<std::array<CellState, 3>, 3> getBoard() const { return board; }
    void print() const;
    void reset();

};
