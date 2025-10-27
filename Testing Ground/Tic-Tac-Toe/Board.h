#pragma once

#include <array>
#include <stdexcept>
#include <string_view>

enum class CellState {
    EMPTY,
    X,
    O
};

inline static CellState otherPlayer(CellState player)
{
    return player == CellState::O ? CellState::X : CellState::O;
}

static const char* cellStateToString(CellState state)
{
    switch(state) {
        case CellState::EMPTY:
            return "EMPTY";
        case CellState::X:
            return "X";
        case CellState::O:
            return "O";
        default:
            throw std::invalid_argument("Invalid CellState");
    }
}

static const char cellStateToChar(CellState state)
{
    switch(state) {
        case CellState::EMPTY:
            return '.';
        case CellState::X:
            return 'X';
        case CellState::O:
            return 'O';
        default:
            throw std::invalid_argument("Invalid CellState");
    }
}

class Board {
private:

    std::array<std::array<CellState, 3>, 3> board{};
public:
    Board();
    bool makeMove(int row, int col, CellState player);
    CellState checkWin() const;
    bool isFull() const;
    std::array<std::array<CellState, 3>, 3> getBoard() const { return board; }
    void print() const;
    void reset();

};
