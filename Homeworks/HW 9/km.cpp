#include <fstream>
#include <iostream>
#include <utility>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <random>

static const int RANDOM_RESTART_COUNT = 100;
static std::mt19937 rng = std::mt19937(std::random_device()());

struct KMeansResult
{
    std::vector<std::pair<double, double>> centroids;
    std::vector<int> assignments;
};

std::pair<double, double> getCentroid(std::vector<std::pair<double, double>> &vector)
{
    double sumX = 0.0;
    double sumY = 0.0;
    for (const auto& point : vector)
    {
        sumX += point.first;
        sumY += point.second;
    }
    return {sumX / vector.size(), sumY / vector.size()};
}

double dist(const std::pair<double, double>& a, const std::pair<double, double>& b)
{
    return (a.first - b.first) * (a.first - b.first) + (a.second - b.second) * (a.second - b.second);
}

std::vector<std::pair<double, double>> getStartingCentroids(const std::vector<std::pair<double, double>> &points, int k)
{
    std::vector<std::pair<double, double>> pointsCopy = points;

    std::shuffle(pointsCopy.begin(), pointsCopy.end(), rng);
    pointsCopy.resize(k);
    return pointsCopy;
}

double wcss(const std::vector<std::pair<double, double>>& points,
            const std::vector<std::pair<double, double>>& centroids,
            const std::vector<int>& assignments)
{
    double totalCost = 0.0;
    for (size_t i = 0; i < points.size(); ++i)
    {
        totalCost += dist(points[i], centroids[assignments[i]]);
    }
    return totalCost;
}

double silhouetteScore(const std::vector<std::pair<double, double>>& points,
                       const std::vector<std::pair<double, double>>& centroids,
                       const std::vector<int>& assignments)
{
    std::vector<double> alpha(points.size(), 0.0);
    std::vector<double> beta(points.size(), std::numeric_limits<double>::max());

    for (size_t i = 0; i < points.size(); ++i)
    {
        int clusterId = assignments[i];
        alpha[i] = dist(points[i], centroids[clusterId]);
        for (size_t j = 0; j < centroids.size(); ++j)
        {
            if (j != clusterId)
            {
                double d = dist(points[i], centroids[j]);
                if (d < beta[i])
                {
                    beta[i] = d;
                }
            }
        }
    }

    std::vector<double> s(points.size(), 0.0);
    for (size_t i = 0; i < points.size(); ++i)
    {
        s[i] = (beta[i] - alpha[i]) / std::max(alpha[i], beta[i]);
    }
    double totalScore = 0.0;
    for (const auto& score : s)
    {
        totalScore += score;
    }
    return totalScore / points.size();
}

KMeansResult kMeans(const std::vector<std::pair<double, double>>& points, int k, const std::vector<std::pair<double, double>>& initialCentroids)
{

    std::vector<std::pair<double, double>> centroids = initialCentroids;
    std::vector<int> assignments(points.size(), -1);
    bool converged = false;

    while(!converged)
    {
        std::vector<std::vector<std::pair<double, double>>> clusters(k);

        for (int i = 0; i < points.size(); ++i)
        {
            const auto& point = points[i];
            int closestIndex = 0;
            double minDist = dist(point, centroids[0]);
            for (int j = 1; j < k; ++j)
            {
                double newDist = dist(point, centroids[j]);
                if (newDist < minDist)
                {
                    closestIndex = j;
                    minDist = newDist;
                }
            }

            clusters[closestIndex].push_back(point);
            assignments[i] = closestIndex;
        }

        std::vector<std::pair<double, double>> newCentroids;
        for (int i = 0; i < k; ++i)
        {
            std::pair<double, double> newCentroid = getCentroid(clusters[i]);
            newCentroids.push_back(newCentroid);
        }

        converged = true;
        for (int i = 0; i < k; ++i)
        {
            if (dist(centroids[i], newCentroids[i]) > 1e-6)
            {
                converged = false;
                break;
            }
        }
        centroids = newCentroids;
    }
    return {centroids, assignments};
}

bool hasEmptyCluster(const std::vector<int>& assignments, int k)
{
    std::vector<bool> clusterHasPoints(k, false);
    for (const auto& assignment : assignments)
    {
        clusterHasPoints[assignment] = true;
    }
    for (const auto& hasPoints : clusterHasPoints)
    {
        if (!hasPoints)
        {
            return true;
        }
    }
    return false;
}

KMeansResult kMeans(const std::vector<std::pair<double, double>>& points, int k)
{

    std::vector<std::pair<double, double>> centroids = getStartingCentroids(points, k);
    return kMeans(points, k, centroids);
}

KMeansResult kMeansPlusPlus(const std::vector<std::pair<double, double>>& points, int k)
{

    std::vector<std::pair<double, double>> centroids;
    std::uniform_int_distribution<std::size_t> uni(0, points.size() - 1);
    centroids.push_back(points[uni(rng)]);

    while (centroids.size() < k)
    {
        std::vector<double> distances(points.size(), std::numeric_limits<double>::max());
        for (size_t i = 0; i < points.size(); ++i)
        {
            for (const auto& centroid : centroids)
            {
                double d = dist(points[i], centroid);
                if (d < distances[i])
                {
                    distances[i] = d;
                }
            }
        }

        double sumDistances = 0.0;
        for (const auto& d : distances)
        {
            sumDistances += d;
        }

        std::uniform_real_distribution<double> distUni(0, sumDistances);
        double r = distUni(rng);
        double cumulativeDistance = 0.0;
        for (size_t i = 0; i < points.size(); ++i)
        {
            cumulativeDistance += distances[i];
            if (cumulativeDistance >= r)
            {
                centroids.push_back(points[i]);
                break;
            }
        }
    }

    return kMeans(points, k, centroids);
}


KMeansResult kMeansRandomRestart(const std::vector<std::pair<double, double>>& points, int k, int metric)
{

    std::vector<std::pair<double, double>> bestCentroids;
    std::vector<int> bestAssignments;
    auto [resultCentroids, resultAssignments] = kMeans(points, k);

    while (hasEmptyCluster(resultAssignments, k))
    {
        auto [newCentroids, newAssignments] = kMeans(points, k);
        std::tie(resultCentroids, resultAssignments) = {newCentroids, newAssignments};
    }

    bestCentroids = resultCentroids;
    bestAssignments = resultAssignments;
    double bestMetricValue;
    if (metric == 0)
    {
        bestMetricValue = wcss(points, bestCentroids, bestAssignments);
    }
    else
    {
        bestMetricValue = silhouetteScore(points, bestCentroids, bestAssignments);
    }
    for (int i = 1; i < RANDOM_RESTART_COUNT; i++)
    {
        auto [newCentroids, newAssignments] = kMeans(points, k);

        if (hasEmptyCluster(newAssignments, k))
        {
            continue;
        }

        double newMetricValue;

        if (metric == 0)
        {
            newMetricValue = wcss(points, newCentroids, newAssignments);
            if (newMetricValue < bestMetricValue)
            {
                bestMetricValue = newMetricValue;
                bestCentroids = newCentroids;
            }
        }
        else
        {
            newMetricValue = silhouetteScore(points, newCentroids, newAssignments);
            if (newMetricValue > bestMetricValue)
            {
                bestMetricValue = newMetricValue;
                bestCentroids = newCentroids;
            }
        }
    }

    return {bestCentroids, bestAssignments};
}



int main()
{

    std::string inputFile;
    std::string algorithm;
    int metrics;
    int k;

    std::cin >> inputFile >> algorithm >> metrics >> k;
    std::ifstream in(inputFile);
    if (!in.is_open())
    {
        std::cerr << "Error opening file: " << inputFile << std::endl;
        return 1;
    }

    std::vector<std::pair<double, double>> points;
    double x, y;
    while (in >> x >> y)
    {
        points.emplace_back(x, y);
    }

    std::vector<std::pair<double, double>> centroids;
    std::vector<int> assignments;
    if (algorithm == "kmeans")
    {
        auto [c, a] = kMeansRandomRestart(points, k, metrics);
        centroids = c;
        assignments = a;
    }
    else if (algorithm == "kmeans++")
    {
        auto [c, a] = kMeansPlusPlus(points, k);
        centroids = c;
        assignments = a;
    }
    else
    {
        std::cerr << "Unknown algorithm: " << algorithm << std::endl;
        return 1;
    }

    std::ofstream centroidsFile("centroids.txt");
    for (const auto& centroid : centroids)
    {
        centroidsFile << centroid.first << " " << centroid.second << std::endl;
    }

    centroidsFile.close();

    std::ofstream assignmentsFile("assignments.txt");
    for (const auto& assignment : assignments)
    {
        assignmentsFile << assignment << std::endl;
    }
    assignmentsFile.close();

    //call python3  '.\Homeworks\HW 9\Test\plot_clusters.py' <data_file> <centroids_file> <assignments_file>
    std::string command = "python3 plot_clusters.py " + inputFile + " centroids.txt assignments.txt";
    system(command.c_str());


    return 0;
}