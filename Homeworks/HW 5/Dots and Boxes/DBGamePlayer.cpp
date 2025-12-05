#include "DBGamePlayer.h"

void DBGamePlayer::makeMove()
{
    std::cout << "Your move (isHorizontal[1/2] row col):" << std::endl;
    int isHorizontal;
    int row, col;
    std::cin >> isHorizontal >> row >> col;
    bool horizontal = (isHorizontal - 1) == 0;
    row--;
    col--;
    board.makeMove(horizontal, row, col, otherPlayer(solver.getPlayer()));
}
