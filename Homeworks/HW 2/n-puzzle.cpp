#include <cstdlib>
#include <vector>
#include <iostream>
#include <complex>
#include <queue>
#include <algorithm>
#include <chrono>
#include <climits>

enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Board
{
private:
    std::vector<std::vector<int>> matrix;
    long long manhattanDist;
    long long currentEmptyIndex;

public:
    Board(std::vector<std::vector<int>>& mat, int indexOfEmpty):
        matrix(mat),
        manhattanDist(manhattanDistance(mat, indexOfEmpty)),
        currentEmptyIndex(findEmptyIndex(mat))
    {
    }

    Board(std::vector<std::vector<int>>&& mat, long long dist, int currentEmptyIndex): matrix(std::move(mat)), manhattanDist(dist), currentEmptyIndex(currentEmptyIndex)
    {
    }

    long long getManhattanDist() const
    {
        return manhattanDist;
    }

    long long getCurrentEmptyIndex() const
    {
        return currentEmptyIndex;
    }

    std::vector<std::vector<int>>& getMatrix()
    {
        return matrix;
    }

    bool operator==(const Board& other) const
    {
        return (currentEmptyIndex == other.currentEmptyIndex) && (manhattanDist == other.manhattanDist) && (matrix == other.matrix);
    }

    bool operator<(const Board& other) const
    {
        return manhattanDist < other.manhattanDist;
    }

private:

    static long long findEmptyIndex(std::vector<std::vector<int>>& matrix)
    {
        int size = matrix.size();
        for (int i = 0; i < size; ++i)
        {
            for (int j = 0; j < size; ++j)
            {
                if (matrix[i][j] == 0)
                {
                    return i * size + j;
                }
            }
        }
        return -1;
    }

    static long long manhattanDistance(std::vector<std::vector<int>>& matrix, int indexOfEmpty)
    {
        long long distance = 0;
        int size = matrix.size();
        for (int i = 0; i < size; ++i)
        {
            for (int j = 0; j < size; ++j)
            {
                int value = matrix[i][j];
                if (value != 0)
                {
                    if (value > indexOfEmpty)
                    {
                        value++;
                    }
                    int targetX = (value - 1) / size;
                    int targetY = (value - 1) % size;
                    distance += std::abs(i - targetX) + std::abs(j - targetY);
                }
            }
        }
        return distance;
    }

};

bool getIsTimeOnly()
{
    const char* env_p = std::getenv("FMI_TIME_ONLY");
    return (env_p != nullptr && std::string(env_p) == "1");
}

int matrixSizeFromN(int n)
{
    return static_cast<int>(std::sqrt(n + 1));
}


bool isSolvable(std::vector<std::vector<int>> &matrix)
{
    int size = matrix.size();
    std::vector<int> flat;
    int emptyRow = -1;
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            if (matrix[i][j] != 0)
            {
                flat.push_back(matrix[i][j]);
            }
            else
            {
                emptyRow = i;
            }
        }
    }

    int inversions = 0;
    for (size_t i = 0; i < flat.size(); ++i)
    {
        for (size_t j = i + 1; j < flat.size(); ++j)
        {
            if (flat[i] > flat[j])
            {
                inversions++;
            }
        }
    }

    if (size % 2 == 1)
    {
        return inversions % 2 == 0;
    }

    else
    {
        return (inversions + emptyRow) % 2 == 1;
    }
}

std::vector<Board> generateNeighbors(Board& board, int indexOfEmpty)
{
    std::vector<Board> neighbors;
    long long currentEmptyIndex = board.getCurrentEmptyIndex();
    long long currentEmptyX = currentEmptyIndex / board.getMatrix().size();
    long long currentEmptyY = currentEmptyIndex % board.getMatrix().size();
    if (currentEmptyX > 0)
    {
        std::vector<std::vector<int>> newMatrix = board.getMatrix();
        long long oldDist = board.getManhattanDist();
        int movedValue = newMatrix[currentEmptyX - 1][currentEmptyY];
        if (movedValue > indexOfEmpty)
        {
            movedValue++;
        }
        long long targetX = (movedValue - 1) / newMatrix.size();
        long long targetY = (movedValue - 1) % newMatrix.size();
        long long newValueDist = std::abs(currentEmptyX - targetX) + std::abs(currentEmptyY - targetY);
        long long currentValueDist = std::abs(currentEmptyX - 1 - targetX) + std::abs(currentEmptyY - targetY);
        long long updatedManhattanDist = oldDist - currentValueDist + newValueDist;

        std::swap(newMatrix[currentEmptyX][currentEmptyY], newMatrix[currentEmptyX - 1][currentEmptyY]);
        int newEmptyIndex = currentEmptyIndex - board.getMatrix().size();
        neighbors.emplace_back(std::move(newMatrix), updatedManhattanDist, newEmptyIndex);
    }


    if (currentEmptyX < board.getMatrix().size() - 1)
    {
        std::vector<std::vector<int>> newMatrix = board.getMatrix();
        long long oldDist = board.getManhattanDist();
        int movedValue = newMatrix[currentEmptyX + 1][currentEmptyY];
        if (movedValue > indexOfEmpty)
        {
            movedValue++;
        }
        long long targetX = (movedValue - 1) / newMatrix.size();
        long long targetY = (movedValue - 1) % newMatrix.size();
        long long newValueDist = std::abs(currentEmptyX - targetX) + std::abs(currentEmptyY - targetY);
        long long currentValueDist = std::abs(currentEmptyX + 1 - targetX) + std::abs(currentEmptyY - targetY);
        long long updatedManhattanDist = oldDist - currentValueDist + newValueDist;

        std::swap(newMatrix[currentEmptyX][currentEmptyY], newMatrix[currentEmptyX + 1][currentEmptyY]);
        int newEmptyIndex = currentEmptyIndex + board.getMatrix().size();
        neighbors.emplace_back(std::move(newMatrix), updatedManhattanDist, newEmptyIndex);
    }

    if (currentEmptyY > 0)
    {
        std::vector<std::vector<int>> newMatrix = board.getMatrix();
        long long oldDist = board.getManhattanDist();
        int movedValue = newMatrix[currentEmptyX][currentEmptyY - 1];
        if (movedValue > indexOfEmpty)
        {
            movedValue++;
        }
        long long targetX = (movedValue - 1) / newMatrix.size();
        long long targetY = (movedValue - 1) % newMatrix.size();
        long long newValueDist = std::abs(currentEmptyX - targetX) + std::abs(currentEmptyY - targetY);
        long long currentValueDist = std::abs(currentEmptyX - targetX) + std::abs(currentEmptyY - 1 - targetY);
        long long updatedManhattanDist = oldDist - currentValueDist + newValueDist;

        std::swap(newMatrix[currentEmptyX][currentEmptyY], newMatrix[currentEmptyX][currentEmptyY - 1]);
        int newEmptyIndex = currentEmptyIndex - 1;
        neighbors.emplace_back(std::move(newMatrix), updatedManhattanDist, newEmptyIndex);
    }

    if (currentEmptyY < board.getMatrix().size() - 1)
    {
        std::vector<std::vector<int>> newMatrix = board.getMatrix();
        long long oldDist = board.getManhattanDist();
        int movedValue = newMatrix[currentEmptyX][currentEmptyY + 1];
        if (movedValue > indexOfEmpty)
        {
            movedValue++;
        }
        long long targetX = (movedValue - 1) / newMatrix.size();
        long long targetY = (movedValue - 1) % newMatrix.size();
        long long newValueDist = std::abs(currentEmptyX - targetX) + std::abs(currentEmptyY - targetY);
        long long currentValueDist = std::abs(currentEmptyX - targetX) + std::abs(currentEmptyY + 1 - targetY);
        long long updatedManhattanDist = oldDist - currentValueDist + newValueDist;

        std::swap(newMatrix[currentEmptyX][currentEmptyY], newMatrix[currentEmptyX][currentEmptyY + 1]);
        int newEmptyIndex = currentEmptyIndex + 1;
        neighbors.emplace_back(std::move(newMatrix), updatedManhattanDist, newEmptyIndex);
    }
    std::sort(neighbors.begin(), neighbors.end());
    return neighbors;
}

Direction getDirectionDifference(Board &current, const Board &neighbor)
{
    Direction dir;
    long long currentEmptyIndex = current.getCurrentEmptyIndex();
    long long neighborEmptyIndex = neighbor.getCurrentEmptyIndex();
    if (neighborEmptyIndex == currentEmptyIndex - current.getMatrix().size())
    {
        dir = DOWN;
    }
    else if (neighborEmptyIndex == currentEmptyIndex + current.getMatrix().size())
    {
        dir = UP;
    }
    else if (neighborEmptyIndex == currentEmptyIndex - 1)
    {
        dir = RIGHT;
    }
    else
    {
        dir = LEFT;
    }
    return dir;
}

long long search(std::deque<Board>& path, std::vector<Direction>& directions, long long g, long long bound, int indexOfEmpty)
{
    Board current = path.back();
    long long f = g + current.getManhattanDist();
    if (f > bound)
    {
        return f;
    }
    if (current.getManhattanDist() == 0)
    {
        return -1;
    }
    long long min = LLONG_MAX;

    std::vector<Board> neighbors = generateNeighbors(current, indexOfEmpty);
    for (auto& neighbor : neighbors)
    {
        if (std::find(path.begin(), path.end(), neighbor) != path.end())
        {
            continue;
        }

        Direction dir;
        dir = getDirectionDifference(current, neighbor);
        directions.push_back(dir);
        path.push_back(std::move(neighbor));

        long long t = search(path, directions, g + 1, bound, indexOfEmpty);
        if (t == -1)
        {
            return -1;
        }
        if (t < min)
        {
            min = t;
        }
        path.pop_back();
        directions.pop_back();
    }
    return min;
}



std::vector<Direction> getShortestAnswer(std::vector<std::vector<int>>& matrix, int indexOfEmpty)
{
    Board initial = Board(matrix, indexOfEmpty);
    long long bound = initial.getManhattanDist();
    std::deque<Board> path;
    path.push_back(initial);
    std::vector<Direction> result;
    int iterations = 0;
    while(true)
    {
        long long t = search(path, result, 0, bound, indexOfEmpty);
        if (t == -1)
        {
            return result;
        }
        if (t == LLONG_MAX)
        {
            return {};
        }
        iterations++;
        bound = t;
    }
}



int main()
{

    int n = 15;
    std::cin >> n;
    int matrixSize = matrixSizeFromN(n);
    int indexOfEmpty = -1;
    std::cin >> indexOfEmpty;
    if (indexOfEmpty == -1)
    {
        indexOfEmpty = n;
    }
    bool isTimeOnly = getIsTimeOnly();
    std::vector<std::vector<int>> matrix(matrixSize, std::vector<int>(matrixSize));
    for (int i = 0; i < matrixSize; ++i)
    {
        for (int j = 0; j < matrixSize; ++j)
        {
            std::cin >> matrix[i][j];
        }
    }

    if (!isSolvable(matrix))
    {
        if (!isTimeOnly)
        {
            std::cout << -1 << std::endl;
        }
        return 0;
    }
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    auto result = getShortestAnswer(matrix, indexOfEmpty);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::cout << "# TIMES_MS: alg=" << elapsed << std::endl;

    if (isTimeOnly)
    {
        return 0;
    }
    std::cout << result.size() << std::endl;
    for (const auto& direction : result)
    {
        switch (direction)
        {
            case UP:
                std::cout << "up" << std::endl;
                break;
            case DOWN:
                std::cout << "down" << std::endl;
                break;
            case LEFT:
                std::cout << "left" << std::endl;
                break;
            case RIGHT:
                std::cout << "right" << std::endl;
                break;
        }
    }
}