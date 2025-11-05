#include <iostream>
#include <random>
#include <vector>
#include <cstdlib>
#include <unordered_set>
#include <chrono>
#include <algorithm>

class Board
{
private:
    int n;
    int collisions;
    std::vector<int> queensPositions = {};
    std::vector<int> queensPerRow = {};
    std::vector<int> queensPerMainDiagonal = {};
    std::vector<int> queensPerSecondaryDiagonal = {};
public:
    explicit Board(int n): n(n), queensPositions(n, -1), collisions(0), queensPerRow(n, 0),
                          queensPerMainDiagonal(2 * n - 1, 0), queensPerSecondaryDiagonal(2 * n - 1, 0)
    {
        initializeField();
        initializeCollisions();
    }

    int getCollisions()
    {
        return collisions;
    }

    void printBoard()
    {
        std::cout<< std::endl;
        for(int row = 0; row < n; row++)
        {
            for(int column = 0; column < n; column++)
            {
                std::cout << (queensPositions[column] == row ? "* " : "_ ");
            }
            std::cout << std::endl;
        }
        std::cout<< std::endl;
    }

    const std::vector<int>& getQueensPositions()
    {
        return queensPositions;
    }

    int getSize()
    {
        return n;
    }

    int getColumnWithMostCollisions()
    {
        int maxConflicts = -1;
        std::vector<int> columnsWithMaxConflicts = {};
        for (int column = 0; column < n; column++)
        {
            int conflicts = numConflicts(column, queensPositions[column]);\

            if (conflicts > maxConflicts)
            {
                maxConflicts = conflicts;
                columnsWithMaxConflicts = {column};
            }
            else if (conflicts == maxConflicts)
            {
                columnsWithMaxConflicts.push_back(column);
            }
        }
        return columnsWithMaxConflicts[rand() % columnsWithMaxConflicts.size()];

    }

    void updateCollisions()
    {
        int currCollisions = 0;
        for(int row = 0; row < n; row++)
        {
            int queensCount = queensPerRow[row];
            if (queensCount > 1)
            {
                currCollisions += (queensCount * (queensCount - 1)) / 2;
            }
        }

        for (int d = 0; d < 2 * n - 1; d++)
        {
            int queensCountMainDiagonal = queensPerMainDiagonal[d];
            int queensCountSecondaryDiagonal = queensPerSecondaryDiagonal[d];

            if (queensCountMainDiagonal > 1)
            {
                currCollisions += (queensCountMainDiagonal * (queensCountMainDiagonal - 1)) / 2;
            }
            if (queensCountSecondaryDiagonal > 1)
            {
                currCollisions += (queensCountSecondaryDiagonal * (queensCountSecondaryDiagonal - 1)) / 2;
            }
        }

        this->collisions = currCollisions;
    }

    void setLeastConflictedRowForColumnQueen(int column)
    {
        int previousRow = queensPositions[column];
        int leastConflictIndex = getLeastConflictedRow(column);
        queensPositions[column] = leastConflictIndex;
        if (leastConflictIndex == previousRow)
        {
            return;
        }
        int conflictDiff = 0;
        queensPerRow[previousRow]--;
        conflictDiff -= queensPerRow[previousRow];
        queensPerMainDiagonal[column + previousRow]--;
        conflictDiff -= queensPerMainDiagonal[column + previousRow];
        queensPerSecondaryDiagonal[column - previousRow + (n - 1)]--;
        conflictDiff -= queensPerSecondaryDiagonal[column - previousRow + (n - 1)];
        queensPerRow[leastConflictIndex]++;
        conflictDiff += queensPerRow[leastConflictIndex] - 1;
        queensPerMainDiagonal[column + leastConflictIndex]++;
        conflictDiff += queensPerMainDiagonal[column + leastConflictIndex] - 1;
        queensPerSecondaryDiagonal[column - leastConflictIndex + (n - 1)]++;
        conflictDiff += queensPerSecondaryDiagonal[column - leastConflictIndex + (n - 1)] - 1;

        collisions += conflictDiff;
    }

private:
    void initializeField()
    {
        std::vector<int> rows(n, 0);
        for (int i = 0; i < n; i++)
        {
            rows[i] = i;
        }
        std::shuffle(rows.begin(), rows.end(), std::mt19937(std::random_device()()));
        for(int column = 0; column < n; column++)
        {
            queensPositions[column] = rows[column];
        }
    }

    int numConflicts(int column, int row)
    {
        bool isQueenHere = (queensPositions[column] == row);
        if (!isQueenHere)
        {
            return queensPerRow[row] +
                   queensPerMainDiagonal[column + row] +
                   queensPerSecondaryDiagonal[column - row + (n - 1)];
        }
        return (queensPerRow[row] - 1) +
               (queensPerMainDiagonal[column + row] - 1) +
               (queensPerSecondaryDiagonal[column - row + (n - 1)] - 1);
    }


    int getLeastConflictedRow(int column)
    {
        std::vector<int> leastConflictRows = {};
        int leastConflicts = n * n;
        for(int j = 0; j < n; j++)
        {
            int conflicts = numConflicts(column, j);
            if (conflicts < leastConflicts)
            {
                leastConflicts = conflicts;
                leastConflictRows = {j};
            }
            else if (conflicts == leastConflicts)
            {
                leastConflictRows.push_back(j);
            }
        }
        return leastConflictRows[rand() % leastConflictRows.size()];
    }

    void initializeCollisions()
    {
        for(int row = 0; row < n; row++)
        {
            int queensCount = 0;
            for (int col = 0; col < n; col++)
            {
                if (queensPositions[col] == row)
                {
                    queensCount++;
                }
            }
            queensPerRow[row] = queensCount;
        }

        for (int d = 0; d < 2 * n - 1; d++)
        {
            int queensCountMainDiagonal = 0;
            int queensCountSecondaryDiagonal = 0;
            for (int col = 0; col < n; col++)
            {
                int rowMainDiagonal = d - col;
                if (rowMainDiagonal >= 0 && rowMainDiagonal < n && queensPositions[col] == rowMainDiagonal)
                {
                    queensCountMainDiagonal++;
                }

                int rowSecondaryDiagonal = col - (d - (n - 1));
                if (rowSecondaryDiagonal >= 0 && rowSecondaryDiagonal < n && queensPositions[col] == rowSecondaryDiagonal)
                {
                    queensCountSecondaryDiagonal++;
                }
            }
            queensPerMainDiagonal[d] = queensCountMainDiagonal;
            queensPerSecondaryDiagonal[d] = queensCountSecondaryDiagonal;
        }

        updateCollisions();
    }
};

class Solver
{
public:
    static std::vector<int> solve(Board& board, bool shouldPrint)
    {
        const int MAX_ITERATIONS_MULT = 50000;
        int MAX_ITERATIONS = MAX_ITERATIONS_MULT * board.getSize();
        int lastBetteredIteration = -1;
        int lastCollisions = board.getCollisions();
        for (int iteration = 0; iteration < MAX_ITERATIONS; iteration++)
        {
            if (shouldPrint)
            {
                board.printBoard();
            }

            if (board.getCollisions() == 0)
            {
                return board.getQueensPositions();
            }

            int randomColumn = board.getColumnWithMostCollisions();
            board.setLeastConflictedRowForColumnQueen(randomColumn);
            if (board.getCollisions() < lastCollisions)
            {
                lastBetteredIteration = iteration;
                lastCollisions = board.getCollisions();
            }
            else if (iteration - lastBetteredIteration > 3 * board.getSize())
            {
                // Reinitialize the board if no progress is made
                board = Board(board.getSize());
                lastBetteredIteration = iteration;
            }
        }
        return {};
    }
};

bool getIsTimeOnly()
{
    const char* env_p = std::getenv("FMI_TIME_ONLY");
    return (env_p != nullptr && std::string(env_p) == "1");
}

int main()
{
    srand(time(nullptr));
    int n = 0;
    std::cin >> n;
    bool shouldPrint = false;
    bool isTimeOnly = getIsTimeOnly();
    if (n == 2 || n == 3)
    {
        std::cout << -1;
        return 0;
    }

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::vector<int> solution = Solver::solve(*(new Board(n)), shouldPrint);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::cout << "# TIMES_MS: alg=" << elapsed << std::endl;
    if (isTimeOnly)
    {
        return 0;
    }
    std::cout<<"[";
    for (int i = 0; i < solution.size(); i++)
    {
        std::cout << solution[i];
        if (i != solution.size() - 1)
            std::cout << ", ";
    }
    std::cout<<"]";

    return 0;
}