add_executable(Tic-Tac-Toe main.cpp
        TTTBoard.cpp
        TTTSolver.cpp
        TTTGamePlayer.cpp
        TTTBoard.h
        TTTSolver.h
        TTTGamePlayer.h)

target_link_libraries(Tic-Tac-Toe PRIVATE Lib)
