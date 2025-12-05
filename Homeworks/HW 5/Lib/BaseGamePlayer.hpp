#pragma once

#include <iostream>
#include "CellState.h"

template<typename Solver, typename Board>
class BaseGamePlayer {
protected:
    Solver solver = Solver(10);
    Board board;
public:
    virtual ~BaseGamePlayer() = default;
    explicit BaseGamePlayer(int maxDepth) : solver(maxDepth) {}
    BaseGamePlayer(CellState player, CellState firstPlayer);
    BaseGamePlayer(CellState player, CellState firstPlayer, Board initialBoard);
    BaseGamePlayer(CellState player, CellState firstPlayer, Board initialBoard, int maxDepth);
    BaseGamePlayer(CellState player, Board  initialBoard) : solver(player), board(initialBoard) {}
    BaseGamePlayer(int maxDepth, CellState player) : solver(maxDepth, player) {}

    void initiateGame();
private:
    bool initiateGameTurn();
    void printBoard() const;
    virtual void makeMove() = 0;

    bool checkWinner() const;
};

template<typename Solver, typename Board>
BaseGamePlayer<Solver, Board>::BaseGamePlayer(CellState player, CellState firstPlayer, Board initialBoard, int maxDepth)
{
    solver = Solver(maxDepth, firstPlayer);
    board = initialBoard;
    if(firstPlayer != player)
    {
        Board aiMoveBoard = solver.answerBoard(board);
        board = aiMoveBoard;
        board.print();
    }

}

template<typename Solver, typename Board>
void BaseGamePlayer<Solver, Board>::initiateGame()
{
    while(board.checkWin() == CellState::EMPTY && !board.isFull()) {
        if(initiateGameTurn()) {
            break;
        }
    }
}

template<typename Solver, typename Board>
bool BaseGamePlayer<Solver, Board>::initiateGameTurn()
{
    makeMove();
    printBoard();

    if(checkWinner())
    {
        return true;
    }

    Board aiMoveBoard = solver.answerBoard(board);
    board = aiMoveBoard;
    printBoard();
    if(checkWinner())
    {
        return true;
    }

    return false;
}

template<typename Solver, typename Board>
bool BaseGamePlayer<Solver, Board>::checkWinner() const
{
    if(board.checkWin() != CellState::EMPTY || board.isFull())
    {
        if (board.checkWin() == CellState::EMPTY)
        {
            std::cout << "DRAW";
            return true;
        }
        std::cout << "WINNER: " << cellStateToChar(board.checkWin()) << std::endl;
        return true;
    }
    return false;
}

template<typename Solver, typename Board>
void BaseGamePlayer<Solver, Board>::printBoard() const
{
    board.print();
}

template<typename Solver, typename Board>
BaseGamePlayer<Solver, Board>::BaseGamePlayer(CellState player, CellState firstPlayer)
{
    solver = Solver(firstPlayer);
    if(firstPlayer != player)
    {
        Board aiMoveBoard = solver.answerBoard(board);
        board = aiMoveBoard;
    }
}

template<typename Solver, typename Board>
BaseGamePlayer<Solver, Board>::BaseGamePlayer(CellState player, CellState firstPlayer, Board initialBoard)
{
    solver = Solver(firstPlayer);
    board = initialBoard;
    if(firstPlayer != player)
    {
        Board aiMoveBoard = solver.answerBoard(board);
        board = aiMoveBoard;
        board.print();
    }
}
