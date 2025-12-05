#include "TTTGamePlayer.h"
#include "TTTBoard.h"

std::pair<int, int> TTTGamePlayer::getNextMove()
{
    TTTBoard nextBoard = solver.answerBoard(board);
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (board.getBoard()[i][j] != nextBoard.getBoard()[i][j])
            {
                board = nextBoard;
                return {i, j};
            }
        }
    }
    return {-1, -1};
}

void TTTGamePlayer::makeMove()
{
    int playerRow, playerCol;
    std::cin >> playerRow >> playerCol;
    playerRow--;
    playerCol--;
    while(!board.makeMove(playerRow, playerCol, otherPlayer(solver.getPlayer()))) {
        std::cin >> playerRow >> playerCol;
    }
}

