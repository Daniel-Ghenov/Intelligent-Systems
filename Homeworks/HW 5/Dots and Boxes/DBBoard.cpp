#include <iostream>
#include "DBBoard.h"


DBBoard::DBBoard(int n, int m): n(n), m(m)
{
    reset();
}

DBBoard::DBBoard()
{
    reset();
}

void DBBoard::reset()
{
    rows = std::vector<std::vector<bool>>(m + 1, std::vector<bool>(n, false));
    cols = std::vector<std::vector<bool>>(n + 1, std::vector<bool>(m, false));
    board = std::vector<std::vector<CellState>>(n, std::vector<CellState>(m, CellState::EMPTY));
}

bool DBBoard::isFull() const
{
    for (const auto & row : rows)
        for (const auto & cell : row)
            if (!cell)
                return false;
    for (const auto & col : cols)
        for (const auto & cell : col)
            if (!cell)
                return false;
    return true;
}

CellState DBBoard::checkWin() const
{
    if(!isFull()) {
        return CellState::EMPTY;
    }
    int countX = countBoxes(CellState::X);
    int countO = countBoxes(CellState::O);
    return (countX > countO) ? CellState::X : (countO > countX) ? CellState::O : CellState::EMPTY;
}

bool DBBoard::makeMove(bool isHorizontal, int row, int col, CellState player)
{
    bool gotBox = false;
    if (isHorizontal)
    {
        if (row < 0 || row >= n || col < 0 || col > m || rows[row][col])
            return false;
        rows[row][col] = true;
        if (row < n - 1 && col < m - 1 && rows[row + 1][col] && cols[row][col] && cols[row][col + 1])
        {
            board[row][col] = player;
            gotBox = true;
        }
        if (row > 0 && col < m - 1&& rows[row - 1][col] && cols[row - 1][col] && cols[row - 1][col + 1])
        {
            board[row - 1][col] = player;
            gotBox = true;
        }
    }
    else
    {
        if (row < 0 || row > n || col < 0 || col >= m || cols[row][col])
            return false;
        cols[row][col] = true;
        if (col < m - 1 && row < n - 1 && cols[row][col + 1] && rows[row][col] && rows[row + 1][col])
        {
            board[row][col] = player;
            gotBox = true;
        }
        if (col > 0 && row < n - 1 && cols[row][col - 1] && rows[row][col - 1] && rows[row + 1][col - 1])
        {
            board[row][col - 1] = player;
            gotBox = true;
        }
    }
    return gotBox;
}

void DBBoard::print() const
{
    const char dotChar = 'o';
    for (int i = 0; i <= m; ++i)
    {
        for (int j = 0; j <= n; ++j)
        {
            std::cout << dotChar;
            if (rows[i][j])
                std::cout << " - ";
            else
                std::cout << "   ";
        }
        std::cout << dotChar << std::endl;
        for (int j = 0; j < n; ++j)
        {
            if (cols[i][j])
                std::cout << "| ";
            else
                std::cout << "  ";
            std::cout << cellStateToChar(board[i][j]) << " ";
        }
        if (cols[i][m])
            std::cout << "|" << std::endl;
        else
            std::cout << " " << std::endl;
    }
}

DBBoard::DBBoard(int n, int m, const std::vector<std::vector<bool>> &rows, const std::vector<std::vector<bool>> &cols,
                 const std::vector<std::vector<CellState>> &board):
    n(n), m(m), rows(rows), cols(cols), board(board)
{

}

int DBBoard::countBoxes(CellState player) const
{
    int count = 0;
    for (const auto & row : board)
    {
        for (const auto & cell : row)
        {
            if (cell == player)
                count++;
        }
    }
    return count;
}
