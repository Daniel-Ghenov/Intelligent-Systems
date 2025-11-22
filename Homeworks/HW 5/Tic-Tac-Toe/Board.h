#pragma once

#include <array>
#include <stdexcept>
#include <string_view>

enum class CellState {
    EMPTY,
    X,
    O
};

static const CellState charToCellState(char c)
{
    switch(c) {
        case '_':
            return CellState::EMPTY;
        case 'X':
            return CellState::X;
        case 'O':
            return CellState::O;
        default:
            throw std::invalid_argument("Invalid character for CellState");
    }
}

static char cellStateToChar(CellState state)
{
    switch(state) {
        case CellState::EMPTY:
            return '_';
        case CellState::X:
            return 'X';
        case CellState::O:
            return 'O';
        default:
            throw std::invalid_argument("Invalid CellState");
    }
}

inline static CellState otherPlayer(CellState player)
{
    return player == CellState::O ? CellState::X : CellState::O;
}

class Board {
private:

    std::array<std::array<CellState, 3>, 3> board{};
public:
    Board();
    Board(std::array<std::array<CellState, 3>, 3> initialBoard);
    bool makeMove(int row, int col, CellState player);
    CellState checkWin() const;
    bool isFull() const;
    std::array<std::array<CellState, 3>, 3> getBoard() const { return board; }
    void print() const;
    void reset();

};
