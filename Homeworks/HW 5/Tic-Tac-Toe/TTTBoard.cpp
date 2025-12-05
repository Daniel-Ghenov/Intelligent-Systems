#include <iostream>
#include "TTTBoard.h"


TTTBoard::TTTBoard()
{
    reset();
}

void TTTBoard::reset()
{
    for (auto & i : board)
        for (auto & j : i)
            j = CellState::EMPTY;
}

bool TTTBoard::makeMove(int row, int col, CellState player)
{
    if (board[row][col] == CellState::EMPTY) {
        board[row][col] = player;
        return true;
    }
    return false;
}

CellState TTTBoard::checkWin() const
{
    //Rows and Columns
    for (int i = 0; i < 3; ++i) {
        if (board[i][0] != CellState::EMPTY &&
            board[i][0] == board[i][1] &&
            board[i][1] == board[i][2]) {
            return board[i][0];
        }
        if (board[0][i] != CellState::EMPTY &&
            board[0][i] == board[1][i] &&
            board[1][i] == board[2][i]) {
            return board[0][i];
        }
    }

    //Diagonals
    if (board[0][0] != CellState::EMPTY &&
        board[0][0] == board[1][1] &&
        board[1][1] == board[2][2]) {
        return board[0][0];
    }
    if (board[0][2] != CellState::EMPTY &&
        board[0][2] == board[1][1] &&
        board[1][1] == board[2][0]) {
        return board[0][2];
    }

    return CellState::EMPTY;
}

bool TTTBoard::isFull() const
{
    for (const auto & row : board)
        for (const auto & cell : row)
            if (cell == CellState::EMPTY)
                return false;
    return true;
}

void TTTBoard::print() const
{
    std::cout << "+---+---+---+" << std::endl;
    for (const auto & row : board) {
        std::cout << "| ";
        for (int j = 0; j < 3; ++j) {
            std::cout << cellStateToChar(row[j]);
            if (j < 2) {
                std::cout << " | ";
            }
            else
            {
                std::cout << " |" << std::endl;
            }
        }
        std::cout  << "+---+---+---+" << std::endl;
    }
}

TTTBoard::TTTBoard(std::array<std::array<CellState, 3>, 3> initialBoard): board(initialBoard) {}


