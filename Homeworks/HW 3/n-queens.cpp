#include <iostream>
#include <vector>
#include <cstdlib>
#include <unordered_set>
#include <chrono>

class Board
{
private:
    int n;
    int collisions;
    std::vector<bool> fields = {};
    std::unordered_set<int> collisionColumns = {};
    std::vector<int> queensPositions = {};
public:
    explicit Board(int n): n(n), fields(std::vector<bool>(n*n,false)), queensPositions(n, -1)
    {
        initializeField();
        updateCollisions();
    }

    std::vector<bool>& getFields()
    {
        return fields;
    }

    int getCollisions()
    {
        return collisions;
    }

    const std::unordered_set<int>& getCollisionColumns()
    {
        return collisionColumns;
    }

    void printBoard()
    {
        std::cout<< std::endl;
        for(int i = 0; i < n; i++)
        {
            for(int j = 0; j < n; j++)
            {
                std::cout << (fields[i * n + j] ? "* " : "_ ");
            }
            std::cout << std::endl;
        }
        std::cout<< std::endl;
    }

    bool setLeastConflictedRowForColumnQueen(int column)
    {
        return setLeastConflictedRowForColumnQueen(column, true);
    }

    const std::vector<int>& getQueensPositions()
    {
        return queensPositions;
    }

    int getSize()
    {
        return n;
    }

private:
    void initializeField()
    {
        for(int i = 0; i < n; i++)
        {
//            setLeastConflictedRowForColumnQueen(i, false);
            int randomRow = rand() % n;
            fields[randomRow * n + i] = true;
            queensPositions[i] = randomRow;
        }
    }
    bool setLeastConflictedRowForColumnQueen(int column, bool isSet)
    {
        if (isSet)
        {
            fields[queensPositions[column] * n + column] = false;
        }

        int leastConflictIndex = getLeastConflictedRow(column);

        fields[leastConflictIndex * n + column] = true;
        queensPositions[column] = leastConflictIndex;


        return leastConflictIndex != queensPositions[column];
    }

    bool isConflicted(int column, int row)
    {
        for(int j = 0; j < n; j++)
        {
            if (fields[row * n + j] && j != column)
            {
                return true;
            }
            int diag1 = row + (column - j);
            int diag2 = row - (column - j);
            if (diag1 >= 0 && diag1 < n && fields[j * n + diag1])
            {
                return true;
            }
            if (diag2 >= 0 && diag2 < n && fields[j * n + diag2])
            {
                return true;
            }
        }
        return false;
    }


    int getLeastConflictedRow(int column)
    {
        std::vector<int> leastConflictRows = {};
        int leastConflicts = n * n;
        for(int j = 0; j < n; j++)
        {
            int conflicts = 0;
            for(int k = 0; k < n; k++)
            {
                if (fields[j * n + k])
                    conflicts++;
                int diag1 = j + (column - k);
                int diag2 = j - (column - k);
                if (diag1 >= 0 && diag1 < n && fields[k * n + diag1])
                    conflicts++;
                if (diag2 >= 0 && diag2 < n && fields[k * n + diag2])
                    conflicts++;
            }
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

    void updateCollisions()
    {
        int currCollisions = 0;
        for(int i = 0; i < n; i++)
        {
            int iRow = queensPositions[i];
            for (int j = i + 1; j < n; j++)
            {
                int jRow = queensPositions[j];
                if (iRow == jRow || abs(iRow - jRow) == abs(i - j))
                {
                    currCollisions++;
                    collisionColumns.insert(i);
                    collisionColumns.insert(j);
                }
            }
        }
        this->collisions = currCollisions;
    }
};

class Solver
{
private:
public:
    static std::vector<int> solve(Board& board, bool shouldPrint)
    {
        const int MAX_ITERATIONS = 10000000;
        int lastMovedIteration = -1;
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

            const std::unordered_set<int>& collisionColumns = board.getCollisionColumns();
            int randomColumn = getRandomColumn(collisionColumns);
            bool moved = board.setLeastConflictedRowForColumnQueen(randomColumn);
            if (moved)
            {
                lastMovedIteration = iteration;
            }
            else if (iteration - lastMovedIteration > 3 * board.getSize())
            {
                // Reinitialize the board if no progress is made
                board = Board(board.getSize());
                lastMovedIteration = iteration;
            }
        }
        return {};
    }


private:

    static int getRandomColumn(const std::unordered_set<int>& collisionColumns)
    {
        int randomIndex = rand() % collisionColumns.size();
        auto it = collisionColumns.begin();
        std::advance(it, randomIndex);
        return *it;
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