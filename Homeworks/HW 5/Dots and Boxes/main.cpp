#include <iostream>
#include "DBGamePlayer.h"


int main()
{
    int n, m;
    std::cout << "Enter N, M:" << std::endl;
    std::cin >> n >> m;
    std::cout << "Computer(1) or Player(2) is first?" << std::endl;
    int first;
    std::cin >> first;

    int maxDepth = 1;
    DBGamePlayer gamePlayer(first == 2 ? CellState::X : CellState::O, CellState::X, DBBoard(n, m), maxDepth);
    gamePlayer.initiateGame();
    return 0;
}