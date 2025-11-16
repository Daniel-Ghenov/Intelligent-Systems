#include <iostream>
#include <cstdlib>
#include <vector>
#include <random>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <iomanip>

bool getIsTimeOnly()
{
    const char* env_p = std::getenv("FMI_TIME_ONLY");
    return (env_p != nullptr && std::string(env_p) == "1");
}

double calculateDistance(double x1, double y1, double x2, double y2)
{
    return std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

struct Path
{
    std::vector<int> nodes;
    double totalWeight;

    Path() : totalWeight(0.0) {}
    Path(std::vector<int>&& nodes, double totalWeight)
            : nodes(std::move(nodes)), totalWeight(totalWeight) {}
    Path(const std::vector<int>& nodes, double totalWeight)
        : nodes(nodes), totalWeight(totalWeight) {}

    bool operator<(const Path& other) const
    {
        return totalWeight < other.totalWeight;
    }
};

class TSPGeneticSolver
{
private:
    std::vector<std::pair<double, double>> points;
    std::vector<std::vector<double>> distanceMatrix;
    std::vector<Path> population;
    std::mt19937 rng{std::random_device{}()};
    int populationSize = 100;
    static const int POPULATION_KEEP = 2;
    static const int POPULATION_KEEP_RESTART_PERCENTAGE = 2;
    static const int MAX_GENERATIONS = 10000;
    static const int STAGNATION_COEF = 2;
    static const int GENERATIONS_TO_PRINT = 10;
    static const int MUTATION_PERCENTAGE = 5;
public:
    explicit TSPGeneticSolver(const std::vector<std::pair<double, double>>& points)
        : points(points)
    {
        distanceMatrix = getDistanceMatrix(points);
        addRandomPopulation(populationSize);
    }

    Path findBestPath(bool shouldPrint = false)
    {
        if (points.size() <= 2)
        {
            return bestPathBaseCase();
        }
        int maxGenerations = MAX_GENERATIONS;
        int printGenerationInterval = maxGenerations / GENERATIONS_TO_PRINT;
        double lastBestWeight = population[0].totalWeight;
        int stagnationCounter = 0;
        int stagnationMax = STAGNATION_COEF * points.size();
        for (int generation = 0; generation < maxGenerations; generation++)
        {
            std::vector<Path> newPopulation;
            std::vector<Path> children = makeChildren(population.size());

            newPopulation.insert(newPopulation.end(), children.begin(), children.end());
            mutate(newPopulation);
            newPopulation.insert(newPopulation.end(), population.begin(), population.begin() + POPULATION_KEEP);
            std::sort(newPopulation.begin(), newPopulation.end());
            newPopulation.resize(populationSize);
            population = std::move(newPopulation);

            if (population[0].totalWeight < lastBestWeight)
            {
                lastBestWeight = population[0].totalWeight;
                stagnationCounter = 0;
            }
            else
            {
                stagnationCounter++;
                if (stagnationCounter >= stagnationMax)
                {
                    addRandomPopulation(populationSize * (100 - POPULATION_KEEP_RESTART_PERCENTAGE) / 100);
                    stagnationCounter = 0;
                }
            }
            if (shouldPrint && (generation % printGenerationInterval == 0 || generation == maxGenerations - 1))
            {
                std::cout << population[0].totalWeight << std::endl;
            }
        }
        return population[0];
    }

private:

    void mutate(std::vector<Path>& populationToMutate)
    {
        for (auto & path : populationToMutate)
        {
            bool shouldMutateSwap = (rand() % 100) < MUTATION_PERCENTAGE;
            if (shouldMutateSwap)
            {
                int size = path.nodes.size();
                int index1 = rand() % size;
                int index2 = rand() % size;
                if (index1 == index2)
                {
                    index2 = (index2 + 1) % size;
                }
                std::swap(path.nodes[index1], path.nodes[index2]);
                path.totalWeight = getPathWeight(path.nodes);
            }
        }
    }

    Path tournamentSelect(const std::vector<Path>& candidates) const
    {
        int tournamentSize = 3;
        int bestIdx = rand() % populationSize;
        for (int j = 1; j < tournamentSize - 1; j++)
        {
            int currIdx = rand() % populationSize;
            if (currIdx < bestIdx)
            {
                bestIdx = currIdx;
            }
        }
        return candidates[bestIdx];
    }

    std::vector<Path> makeChildren(int childrenCount)
    {
        std::vector<Path> children;
        children.resize(childrenCount);

        for(int i = 0; i < childrenCount; i++)
        {
            Path parent1 = tournamentSelect(population);
            Path parent2 = tournamentSelect(population);
            Path child = makeChild(parent1, parent2);
            children[i] = child;
        }
        return children;
    }

    Path makeChild(const Path& parent1, const Path& parent2)
    {
        int size = parent1.nodes.size();
        int i = rand() % parent1.nodes.size();
        int j = rand() % parent1.nodes.size();
        while (i == j)
        {
            j = rand() % parent1.nodes.size();
        }
        if (i > j) std::swap(i, j);
        std::vector<int> child1Nodes(parent1.nodes.size(), -1);

        std::vector<int> parent2Positions(size);
        for (int index = 0; index < size; index++)
        {
            parent2Positions[parent2.nodes[index]] = index;
        }

        std::vector<bool> child1Set(size, false);
        for (int k = i; k <= j; k++)
        {
            child1Nodes[k] = parent1.nodes[k];
            child1Set[parent1.nodes[k]] = true;
        }

        int currentIndex1 = (j + 1) % size;

        for (int k = 0; k < size; k++)
        {
            int parent2Node = parent2.nodes[(j + 1 + k) % size];
            if (!child1Set[parent2Node])
            {
                child1Nodes[currentIndex1] = parent2Node;
                currentIndex1 = (currentIndex1 + 1) % size;
            }
        }
        double child1Weight = getPathWeight(child1Nodes);
        return {child1Nodes, child1Weight};
    }

    void addRandomPopulation(int randomCount)
    {
        if (population.size() == populationSize)
        {
            population.resize(populationSize - randomCount);
        }
        std::vector<int> nodes(points.size());

        for (int j = 0; j < points.size(); j++)
        {
            nodes[j] = j;
        }

        for (int i = 0; i < randomCount; i++)
        {
            std::shuffle(nodes.begin(), nodes.end(), rng);
            population.emplace_back(nodes, getPathWeight(nodes));
        }

        std::sort(population.begin(), population.end());
    }

    double getPathWeight(const std::vector<int> &nodes)
    {
        double totalWeight = 0.0;
        for (size_t j = 0; j < nodes.size() - 1; j++)
        {
            int from = nodes[j];
            int to = nodes[j + 1];

            totalWeight += distanceMatrix[from][to];
        }
        return totalWeight;
    }

    Path bestPathBaseCase()
    {

        if (points.empty())
        {
            return {};
        }
        if (points.size() == 1)
        {
            return Path({0}, 0.0);
        }
        double weight = calculateDistance(points[0].first, points[0].second,
                                          points[1].first, points[1].second);
        return Path({0, 1}, weight);
    }

    std::vector<std::vector<double>> getDistanceMatrix(const std::vector<std::pair<double, double>> &vector)
    {
        int n = vector.size();
        std::vector<std::vector<double>> matrix(n, std::vector<double>(n, 0.0));
        for (int i = 0; i < n; i++)
        {
            for (int j = i + 1; j < n; j++)
            {
                double dist = calculateDistance(vector[i].first, vector[i].second,
                                                vector[j].first, vector[j].second);
                matrix[i][j] = dist;
                matrix[j][i] = dist;
            }
        }
        return matrix;
    }
};

const int TSPGeneticSolver::POPULATION_KEEP;
const int TSPGeneticSolver::POPULATION_KEEP_RESTART_PERCENTAGE;
const int TSPGeneticSolver::MAX_GENERATIONS;
const int TSPGeneticSolver::STAGNATION_COEF;
const int TSPGeneticSolver::GENERATIONS_TO_PRINT;
const int TSPGeneticSolver::MUTATION_PERCENTAGE;

std::vector<std::pair<double, double>> generateRandomPoints(int size)
{
    std::vector<std::pair<double, double>> points (size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 100.0);
    for (int i = 0; i < size; i++)
    {
        points[i] = {dis(gen), dis(gen)};
    }
    return points;
}

int main()
{
    std::cout << std::setprecision(std::numeric_limits<double>::max_digits10);
    bool isTimeOnly = getIsTimeOnly();
    std::string name;
    std::cin >> name;
    std::vector<std::pair<double, double>> points;
    std::vector<std::string> pointNames = {};
    bool namedSolution = false;
    try
    {
        int n = std::stoi(name);
        points = generateRandomPoints(n);
    }
    catch (std::invalid_argument&)
    {
        namedSolution = true;
        int n;
        std::cin >> n;
        points.reserve(n);
        pointNames.reserve(n);
        for (int i = 0; i < n; i++)
        {
            std::string pointName;
            double x, y;
            std::cin >> pointName >> x >> y;
            points.emplace_back(x, y);
            pointNames.push_back(pointName);
        }
    }

    TSPGeneticSolver solver(points);
    Path bestPath = solver.findBestPath(!isTimeOnly);
    std::cout << std::endl;
    if (!isTimeOnly)
    {
        if (namedSolution)
        {
            for (int i = 0; i < bestPath.nodes.size(); i++)
            {
                int node = bestPath.nodes[i];
                std::cout << pointNames[node];
                if (i != bestPath.nodes.size() - 1)
                {
                    std::cout << " -> ";
                }
            }
            std::cout << std::endl;
        }

        std::cout << bestPath.totalWeight << std::endl;
    }


    return 0;
}