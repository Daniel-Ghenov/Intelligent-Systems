add_library(Lib INTERFACE
        BaseGamePlayer.hpp
        BaseSolver.hpp
        CellState.h)

target_include_directories(Lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})