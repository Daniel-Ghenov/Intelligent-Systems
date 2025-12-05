#pragma once
#include <stdexcept>
#include <vector>
#include "CellState.h"


class DBBoard {
private:
    int n = 3;
    int m = 3;
    std::vector<std::vector<bool>> rows{};
    std::vector<std::vector<bool>> cols{};
    std::vector<std::vector<CellState>> board{};
public:
    DBBoard();
    DBBoard(int n, int m);
    DBBoard(int n, int m, const std::vector<std::vector<bool>>& rows, const std::vector<std::vector<bool>>& cols, const std::vector<std::vector<CellState>>& board);

    int getN() const { return n; }
    int getM() const { return m; }

    int countBoxes(CellState player) const;

    std::vector<std::vector<CellState>> getBoard() const { return board; }
    std::vector<std::vector<bool>> getRows() const { return rows; }
    std::vector<std::vector<bool>> getCols() const { return cols; }

    bool makeMove(bool isHorizontal, int row, int col, CellState player);
    CellState checkWin() const;
    bool isFull() const;
    void print() const;
    void reset();
};
