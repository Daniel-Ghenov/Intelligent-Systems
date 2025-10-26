#include <vector>
#include <algorithm>
#include <iostream>
#include <cstdlib>


void printSolution(std::vector<char> &vector)
{
    for (char c : vector)
    {
        std::cout << c;
    }
    std::cout << std::endl;
}

std::vector<char> initializeField(int n)
{
    std::vector<char> field(2 * n + 1, '_');
    for (int i = 0; i < n; ++i)
    {
        field[i] = '>';
        field[2 * n - i] = '<';
    }
    return field;
}

bool isSolved(std::vector<char>& field, int n)
{
    for (int i = 0; i < n; ++i)
    {
        if (field[i] != '<') return false;
        if (field[2 * n - i] != '>') return false;
    }
    return true;
}

std::vector<std::vector<char>> dfs(std::vector<char>& field, int n)
{
    if (isSolved(field, n))
    {
        return {field};
    }

    int emptyIndex = std::find(field.begin(), field.end(), '_') - field.begin();
    if (emptyIndex + 2 < field.size() && field[emptyIndex + 2] == '<')
    {
        std::swap(field[emptyIndex], field[emptyIndex + 2]);
        auto result = dfs(field, n);
        if (result.size() > 0)
        {
            std::swap(field[emptyIndex], field[emptyIndex + 2]);
            result.push_back(field);
            return result;
        }
        std::swap(field[emptyIndex], field[emptyIndex + 2]);
    }

    if (emptyIndex - 2 >= 0 && field[emptyIndex - 2] == '>')
    {
        std::swap(field[emptyIndex], field[emptyIndex - 2]);
        auto result = dfs(field, n);
        if (result.size() > 0)
        {
            std::swap(field[emptyIndex], field[emptyIndex - 2]);
            result.push_back(field);
            return result;
        }
        std::swap(field[emptyIndex], field[emptyIndex - 2]);
    }

    if (emptyIndex - 1 >= 0 && field[emptyIndex - 1] == '>')
    {
        std::swap(field[emptyIndex], field[emptyIndex - 1]);
        auto result = dfs(field, n);
        if (result.size() > 0)
        {
            std::swap(field[emptyIndex], field[emptyIndex - 1]);
            result.push_back(field);
            return result;
        }
        std::swap(field[emptyIndex], field[emptyIndex - 1]);
    }

    if (emptyIndex + 1 < field.size() && field[emptyIndex + 1] == '<')
    {
        std::swap(field[emptyIndex], field[emptyIndex + 1]);
        auto result = dfs(field, n);
        if (result.size() > 0)
        {
            std::swap(field[emptyIndex], field[emptyIndex + 1]);
            result.push_back(field);
            return result;
        }
        std::swap(field[emptyIndex], field[emptyIndex + 1]);
    }

    return {};
}

void printSolutionDFS(int n, bool isTimeOnly)
{
    std::vector<char> field = initializeField(n);
    auto result = dfs(field, n);
    if (isTimeOnly)
    {
        return;
    }
    if (result.size() == 0)
    {
        std::cout << "No solution found." << std::endl;
        return;
    }

    for(auto it = result.rbegin(); it != result.rend(); ++it)
    {
        printSolution(*it);
    }

}

bool getIsTimeOnly()
{
    const char* env_p = std::getenv("FMI_TIME_ONLY");
    return (env_p != nullptr && std::string(env_p) == "1");
}

int main()
{
    int n = 20;
    std::cin >> n;
    bool isTimeOnly = getIsTimeOnly();
    printSolutionDFS(n, isTimeOnly);
    return 0;
}