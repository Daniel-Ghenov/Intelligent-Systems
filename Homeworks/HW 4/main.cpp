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
    std::vector<Path> population;
    int populationSize = 100;
    static const int POPULATION_KEEP = 2;
    static const int POPULATION_KEEP_RESTART_PERCENTAGE = 20;
    static const int MAX_GENERATIONS_COEF = 500;
    static const int STAGNATION_COEF = 2;
    static const int MIN_POPULATION_SIZE = 100;
    static const int MAX_POPULATION_SIZE = 1500;
    static const int GENERATIONS_TO_PRINT = 10;
    static const int MUTATION_PERCENTAGE = 40;
public:
    explicit TSPGeneticSolver(const std::vector<std::pair<double, double>>& points)
        : points(points)
    {
        populationSize = std::max(MIN_POPULATION_SIZE, std::min(MAX_POPULATION_SIZE, static_cast<int>(10 * std::sqrt(points.size()))));
        addRandomPopulation(populationSize);
    }

    Path findBestPath(bool shouldPrint = false)
    {
        if (points.size() <= 2)
        {
            return bestPathBaseCase();
        }
        int maxGenerations = MAX_GENERATIONS_COEF * points.size();
        int printGenerationInterval = maxGenerations / GENERATIONS_TO_PRINT;
        double lastBestWeight = population[0].totalWeight;
        int stagnationCounter = 0;
        int stagnationMax = STAGNATION_COEF * points.size();
        for (int generation = 0; generation < maxGenerations; generation++)
        {
            std::vector<Path> newPopulation;
            std::vector<Path> parents = selectParents();
            std::vector<Path> children = makePairs(parents);

            newPopulation.insert(newPopulation.end(), children.begin(), children.end());
            mutate(newPopulation);
            newPopulation.insert(newPopulation.end(), population.begin(), population.begin() + POPULATION_KEEP);
            std::sort(newPopulation.begin(), newPopulation.end());
            newPopulation.resize(populationSize);
            population = newPopulation;

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
            if (shouldPrint && generation % printGenerationInterval == 0)
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
            bool shouldMutateRev = (rand() % 100) < MUTATION_PERCENTAGE;
            if (shouldMutateRev)
            {
                int size = path.nodes.size();
                int index1 = rand() % size;
                int index2 = rand() % size;
                std::reverse(path.nodes.begin() + std::min(index1, index2),
                             path.nodes.begin() + std::max(index1, index2) + 1);
                path.totalWeight = getPathWeight(path.nodes);
            }
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

    std::vector<Path> selectParents()
    {
//        //rank based selection
//        int totalRank = (populationSize * (populationSize + 1)) / 2;
//        std::vector<Path> selectedParents;
//        std::vector<double> selectionProbabilities(populationSize);
//        for (int i = 0; i < populationSize; i++)
//        {
//            selectionProbabilities[i] = static_cast<double>(populationSize - i) / totalRank;
//        }
//        std::discrete_distribution<int> distribution(selectionProbabilities.begin(), selectionProbabilities.end());
//        std::mt19937 gen(std::random_device{}());
//        for (int i = 0; i < populationSize; i++)
//        {
//            int parentIndex = distribution(gen);
//            selectedParents.push_back(population[parentIndex]);
//        }
//        return selectedParents;

        //tournament based selection
        //k = 3
        int tournamentSize = 3;
        std::vector<Path> selectedParents;
        for (int i = 0; i < populationSize; i++)
        {
            int bestIdx = rand() % populationSize;
            for (int j = 1; j < tournamentSize - 1; j++)
            {
                int currIdx = rand() % populationSize;
                if (currIdx < bestIdx)
                {
                    bestIdx = currIdx;
                }
            }
            selectedParents.push_back(population[bestIdx]);
        }
        return selectedParents;
    }

    std::vector<Path> makePairs(const std::vector<Path> &parents)
    {
        std::vector<Path> children;

        for(int i = 0; i < parents.size(); i+=2)
        {
            std::pair<Path, Path> currChildren = makePair(parents[i], parents[i+1]);
            children.push_back(currChildren.first);
            children.push_back(currChildren.second);
        }
        return children;
    }
    std::pair<Path, Path> makePair(const Path& parent1, const Path& parent2)
    {
        int size = parent1.nodes.size();
        int i = rand() % (parent1.nodes.size() - 2);
        int j = rand() % (parent1.nodes.size() - 2);
        int k;
        if (i > j) std::swap(i, j);
        i += 1;
        j += 1;
        std::vector<int> child1Nodes(parent1.nodes.size(), -1);
        std::vector<int> child2Nodes(parent2.nodes.size(), -1);

        std::unordered_map<int, int> parent1Positions;
        std::unordered_map<int, int> parent2Positions;
        for (int index = 0; index < size; index++)
        {
            parent1Positions[parent1.nodes[index]] = index;
            parent2Positions[parent2.nodes[index]] = index;
        }

//        std::unordered_set<int> child1Set;
//        std::unordered_set<int> child2Set;
        for (int k = i; k <= j; k++)
        {
            child1Nodes[k] = parent1.nodes[k];
            child2Nodes[k] = parent2.nodes[k];
//            child1Set.insert(parent1.nodes[k]);
//            child2Set.insert(parent2.nodes[k]);
        }

        for (int k = i; k <= j; k++)
        {
            int parent2Node = parent2.nodes[k];
            if (parent1Positions[parent2Node] < i || parent1Positions[parent2Node] > j)
            {
                int currentIndex = k;
                while(currentIndex >= i && currentIndex <= j)
                {
                    int currentNodeP1 = parent1.nodes[currentIndex];
                    currentIndex = parent2Positions[currentNodeP1];
                }
                child1Nodes[currentIndex] = parent2Node;
            }
            int parent1Node = parent1.nodes[k];
            if (parent2Positions[parent1Node] < i || parent2Positions[parent1Node] > j)
            {
                int currentIndex = k;
                while(currentIndex >= i && currentIndex <= j)
                {
                    int currentNodeP2 = parent2.nodes[currentIndex];
                    currentIndex = parent1Positions[currentNodeP2];
                }
                child2Nodes[currentIndex] = parent1Node;
            }
        }

        for (k = 0; k < size; k++)
        {
            if (child1Nodes[k] == -1)
            {
                child1Nodes[k] = parent2.nodes[k];
            }
            if (child2Nodes[k] == -1)
            {
                child2Nodes[k] = parent1.nodes[k];
            }
        }

//        int currentIndex1 = (j + 1) % size;
//        int currentIndex2 = (j + 1) % size;
//
//        for (int k = 0; k < size; k++)
//        {
//            int parent2Node = parent2.nodes[(j + 1 + k) % size];
//            if (child1Set.find(parent2Node) == child1Set.end())
//            {
//                child1Nodes[currentIndex1] = parent2Node;
//                currentIndex1 = (currentIndex1 + 1) % size;
//            }
//            int parent1Node = parent1.nodes[(j + 1 + k) % size];
//            if (child2Set.find(parent1Node) == child2Set.end())
//            {
//                child2Nodes[currentIndex2] = parent1Node;
//                currentIndex2 = (currentIndex2 + 1) % size;
//            }
//        }
        double child1Weight = getPathWeight(child1Nodes);
        double child2Weight = getPathWeight(child2Nodes);
        return {Path(child1Nodes, child1Weight), Path(child2Nodes, child2Weight)};
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
            std::shuffle(nodes.begin(), nodes.end(), std::mt19937(std::random_device()()));
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
            totalWeight += calculateDistance(points[from].first, points[from].second,
                                             points[to].first, points[to].second);
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
};

const int TSPGeneticSolver::POPULATION_KEEP;
const int TSPGeneticSolver::POPULATION_KEEP_RESTART_PERCENTAGE;
const int TSPGeneticSolver::MAX_GENERATIONS_COEF;
const int TSPGeneticSolver::STAGNATION_COEF;
const int TSPGeneticSolver::MIN_POPULATION_SIZE;
const int TSPGeneticSolver::MAX_POPULATION_SIZE;
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