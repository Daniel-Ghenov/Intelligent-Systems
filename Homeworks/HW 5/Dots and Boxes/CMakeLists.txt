add_executable(Dots_and_boxes main.cpp
        DBBoard.h
        DBSolver.cpp
        DBBoard.cpp
        DBGamePlayer.h
        DBGamePlayer.cpp
)

target_link_libraries(Dots_and_boxes PRIVATE Lib)