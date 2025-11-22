#include <iostream>
#include <limits>
#include "GamePlayer.h"
#include "Board.h"
#include "Solver.h"

enum class SolverMode {
    JUDGE,
    GAME
};

static SolverMode fromString(std::string str)
{
    if (str == "JUDGE")
        return SolverMode::JUDGE;
    else if (str == "GAME")
        return SolverMode::GAME;
    else
        throw std::invalid_argument("Invalid SolverMode string");
}

int main()
{

    std::string strMode;
    std::cin >> strMode;
    SolverMode mode = fromString(strMode);

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
    if (mode == SolverMode::JUDGE)
    {
        char playerChar;
        std::cin >> playerChar;
        CellState player = charToCellState(playerChar);
        std::array<std::array<CellState, 3>, 3> boardState {};
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        for (int i = 0; i < 3; ++i) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '|');
            for (int j = 0; j < 3; ++j) {
                char cellChar;
                std::cin >> cellChar;
                boardState[i][j] = charToCellState(cellChar);
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '|');
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        Board board(boardState);
        if (board.isFull() || board.checkWin() != CellState::EMPTY)
        {
            std::cout << -1;
            return 0;
        }
        GamePlayer judgePlayer(player, boardState);

        std::pair<int, int> nextMove = judgePlayer.getNextMove();
        std::cout << nextMove.first + 1 << " " << nextMove.second + 1;
        return 0;
    }
    char firstPlayerChar;
    std::cin >> firstPlayerChar;
    CellState firstPlayer = charToCellState(firstPlayerChar);
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
    char humanPlayerChar;
    std::cin >> humanPlayerChar;
    CellState humanPlayer = charToCellState(humanPlayerChar);

    std::array<std::array<CellState, 3>, 3> boardState {};
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    for (int i = 0; i < 3; ++i) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '|');
        for (int j = 0; j < 3; ++j) {
            char cellChar;
            std::cin >> cellChar;
            boardState[i][j] = charToCellState(cellChar);
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '|');
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    GamePlayer gamePlayer (humanPlayer, firstPlayer, boardState);
    gamePlayer.initiateGame();

    return 0;
}