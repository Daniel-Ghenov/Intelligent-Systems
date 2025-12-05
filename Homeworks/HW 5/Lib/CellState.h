#pragma once

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
