#include <iostream>
#include <array>
#include <random>
#include <vector>
#include <algorithm>
#include <queue>
#include <complex>
#include <fstream>
#include <unordered_map>
#include <stack>

static const int DIMENSIONS_COUNT = 4;
static const int K = 11;
static const int FOLDS = 10;
static const int TEST_SAMPLE_PERCENTAGE = 20;
static std::mt19937 rng = std::mt19937(std::random_device()());

struct Point
{
    std::array<float, DIMENSIONS_COUNT> coordinates;
    std::string name;

    float operator[](int index) const
    {
        return coordinates[index];
    }

    float& operator[](int index)
    {
        return coordinates[index];
    }

    bool operator<(const Point& other) const
    {
        //needed for the knn pq, could be arbitrary
        return name < other.name;
    }

    bool operator==(const Point& other) const
    {
        return coordinates == other.coordinates && name == other.name;
    }
};

bool isLessInDimension(const Point& a, const Point& b, int dimension)
{
    return a[dimension] < b[dimension];
}

float dist(const Point& a, const Point& b)
{
    float sum = 0.0f;
    for (int i = 0; i < DIMENSIONS_COUNT; ++i)
    {
        float diff = a[i] - b[i];
        sum += diff * diff;
    }
    return sum;
}



std::pair<std::vector<Point>, std::vector<Point>> getSample(const std::vector<Point>& points, int percentage)
{
    int sampleSize = points.size() * percentage / 100;
    std::vector<Point> sample;
    std::vector<Point> rest;

    std::unordered_map<std::string, std::vector<Point>> classPoints;
    for (const auto& point : points)
    {
        if (classPoints.find(point.name) == classPoints.end())
        {
            classPoints[point.name] = {};
        }
        classPoints[point.name].push_back(point);
    }

    for (auto& pair : classPoints)
    {
        std::shuffle(pair.second.begin(), pair.second.end(), rng);
        int classSampleSize = pair.second.size() * percentage / 100;
        sample.insert(sample.end(), pair.second.begin(), pair.second.begin() + classSampleSize);
        rest.insert(rest.end(), pair.second.begin() + classSampleSize, pair.second.end());
    }

    return {sample, rest};
}

std::vector<std::vector<Point>> fold(const std::vector<Point>& points, int folds)
{
    std::vector<std::vector<Point>> result(folds);

    std::unordered_map<std::string, std::vector<Point>> classPoints;
    for (const auto& point : points)
    {
        if (classPoints.find(point.name) == classPoints.end())
        {
            classPoints[point.name] = {};
        }
        classPoints[point.name].push_back(point);
    }

    for (auto& pair : classPoints)
    {
        std::shuffle(pair.second.begin(), pair.second.end(), rng);
        int foldIndex = 0;
        for (const auto& point : pair.second)
        {
            result[foldIndex % folds].push_back(point);
            foldIndex++;
        }
    }

    return result;
}

class KDTree
{
private:
    struct Node
    {
        Node* left;
        Node* right;
        Node* parent;
        Point value;
        int depth;
    };

    Node* root;
public:

    KDTree(const std::vector<Point>& points)
    {
        std::vector<Point> pts = points;
        root = buildTree(pts, 0, pts.size(), 0);
    }

    ~KDTree()
    {
        clear(root);
    }

    KDTree(const KDTree& other) = delete;
    KDTree& operator=(const KDTree& other) = delete;

    std::vector<Point> kNearestNeighbours(const Point& point, int k)
    {
        std::stack<Node*> found = find(point, root, 0);
        if (found.empty())
        {
            return {};
        }
        std::priority_queue<std::pair<float, Point>> pq;

        while(!found.empty())
        {
            Node* current = found.top();
            found.pop();

            float currentDist = dist(current->value, point);
            if (pq.size() < k)
            {
                pq.emplace(currentDist, current->value);
            }
            else if (currentDist < pq.top().first)
            {
                pq.pop();
                pq.emplace(currentDist, current->value);
            }

            int dimension = current->depth % DIMENSIONS_COUNT;
            float diff = point[dimension] - current->value[dimension];

            if (diff * diff < pq.top().first)
            {
                Node* farChild = diff > 0 ? current->left : current->right;
                std::stack<Node*> stack = find(point, farChild, current->depth + 1);

                std::vector<Node*> temp;
                while(!stack.empty())
                {
                    temp.push_back(stack.top());
                    stack.pop();
                }
                for (auto it = temp.rbegin(); it != temp.rend(); ++it)
                {
                    found.push(*it);
                }
            }
        }

        std::vector<Point> knn;
        knn.reserve(std::min(k,(int) pq.size()));
        while(!pq.empty())
        {
            knn.push_back(pq.top().second);
            pq.pop();
        }
        return knn;
    }

private:

    Node* buildTree(std::vector<Point>& points, int i, int j, int depth)
    {

        if (i == j)
        {
            return nullptr;
        }

        int dimension = depth % DIMENSIONS_COUNT;

        std::sort(points.begin() + i, points.begin() + j, [dimension](Point& a,Point& b) { return isLessInDimension(a, b, dimension);});
        int median = ((j - i) / 2) + i;
        Node* left = buildTree(points, i, median, depth + 1);
        Node* right = buildTree(points, median + 1, j, depth + 1);
        Node* node = new Node({left, right, nullptr, points[median], depth});
        if (left)
        {
            left->parent = node;
        }
        if (right)
        {
            right->parent = node;
        }

        return node;
    }

    std::stack<Node*> find(const Point& point, Node* node, int depth)
    {
        std::stack<Node*> result;

        while(node)
        {
            result.push(node);
            int dimension = depth % DIMENSIONS_COUNT;
            bool isLess = isLessInDimension(point, node->value, dimension);

            if (isLess)
            {
                node = node->left;
            }
            else
            {
                node = node->right;
            }
            depth++;
        }

        return result;
    }


    void clear(Node* node)
    {
        if (!node)
        {
            return;
        }

        if (node->left)
        {
            clear(node->left);
        }

        if (node->right)
        {
            clear(node->right);
        }

        delete node;
    }
};

class KNNClassifier
{
private:
    KDTree* tree = nullptr;
    int k;
    std::array<float, DIMENSIONS_COUNT> means;
    std::array<float, DIMENSIONS_COUNT> stdDevs;
    std::array<float, DIMENSIONS_COUNT> mins;
    std::array<float, DIMENSIONS_COUNT> maxs;
public:
    KNNClassifier(const std::vector<Point>& trainPoints, int k)
    {
        std::vector<Point> normalizedPoints = normalize(trainPoints);
        this->k = k;
        tree = new KDTree(normalizedPoints);
    }

    ~KNNClassifier()
    {
        delete tree;
    }

    std::string classify(const Point& point)
    {
        Point normalizedPoint = normalizeTestPoint(point);
        std::vector<Point> neighbors = tree->kNearestNeighbours(normalizedPoint, k);
        std::unordered_map<std::string, int> classCounts;
        for (const auto& neighbor : neighbors)
        {
            classCounts[neighbor.name]++;
        }

        std::string bestClass;
        int bestCount = 0;
        for (const auto& pair : classCounts)
        {
            if (pair.second > bestCount)
            {
                bestCount = pair.second;
                bestClass = pair.first;
            }
        }
        return bestClass;
    }
private:

    std::vector<Point> minMaxNormalize(const std::vector<Point>& points)
    {

        for (int i = 0; i < DIMENSIONS_COUNT; i++)
        {
            mins[i] = INT_MAX;
            maxs[i] = INT_MIN;
        }


        for (auto point : points)
        {
            for (int i = 0; i < DIMENSIONS_COUNT; i++)
            {
                if (point[i] < mins[i])
                {
                    mins[i] = point[i];
                }
                if (point[i] > maxs[i])
                {
                    maxs[i] = point[i];
                }
            }
        }

        std::vector<Point> normalized;

        for (auto point: points)
        {
            Point newPoint;
            for (int i = 0; i < DIMENSIONS_COUNT; i++)
            {
                newPoint[i] = (point[i] - mins[i]) / (maxs[i] - mins[i]);
            }
            newPoint.name = point.name;
            normalized.push_back(newPoint);
        }
        return normalized;
    }

    std::vector<Point> zScoreNormalize(const std::vector<Point>& points)
    {
        for (int i = 0; i < DIMENSIONS_COUNT; i++)
        {
            means[i] = 0.0f;
            stdDevs[i] = 0.0f;
        }

        for (auto point : points)
        {
            for (int i = 0; i < DIMENSIONS_COUNT; i++)
            {
                means[i] += point[i];
            }
        }

        for (int i = 0; i < DIMENSIONS_COUNT; i++)
        {
            means[i] /= points.size();
        }

        for (auto point : points)
        {
            for (int i = 0; i < DIMENSIONS_COUNT; i++)
            {
                float diff = point[i] - means[i];
                stdDevs[i] += diff * diff;
            }
        }

        for (int i = 0; i < DIMENSIONS_COUNT; i++)
        {
            stdDevs[i] = std::sqrt(stdDevs[i] / points.size());
        }

        std::vector<Point> normalized;

        for (auto point: points)
        {
            Point newPoint;
            for (int i = 0; i < DIMENSIONS_COUNT; i++)
            {
                newPoint[i] = (point[i] - means[i]) / stdDevs[i];
            }
            newPoint.name = point.name;
            normalized.push_back(newPoint);
        }
        return normalized;
    }

    std::vector<Point> normalize(const std::vector<Point>& points)
    {
        return zScoreNormalize(points);
    }

    Point normalizeMinMaxTestPoint(const Point& point)
    {
        Point newPoint;
        for (int i = 0; i < DIMENSIONS_COUNT; i++)
        {
            newPoint[i] = (point[i] - mins[i]) / (maxs[i] - mins[i]);
        }
        newPoint.name = point.name;
        return newPoint;
    }

    Point normalizeZScoreTestPoint(const Point& point)
    {
        Point newPoint;
        for (int i = 0; i < DIMENSIONS_COUNT; i++)
        {
            newPoint[i] = (point[i] - means[i]) / stdDevs[i];
        }
        newPoint.name = point.name;
        return newPoint;
    }

    Point normalizeTestPoint(const Point& point)
    {
        return normalizeZScoreTestPoint(point);
    }

};

int main()
{
    std::ifstream dataFile("../../../Homeworks/HW 6/Test/iris.data");
    if (!dataFile.is_open())
    {
        std::cerr << "Could not open the training data file." << std::endl;
        return 1;
    }

    std::vector<Point> dataPoints;

    std::string line;
    while (std::getline(dataFile, line))
    {
        Point point;
        size_t pos = 0;
        for (int i = 0; i < DIMENSIONS_COUNT; i++)
        {
            size_t commaPos = line.find(',', pos);
            point[i] = std::stof(line.substr(pos, commaPos - pos));
            pos = commaPos + 1;
        }
        point.name = line.substr(pos, line.size() - pos);
        dataPoints.push_back(point);
    }
    dataFile.close();

    auto [testSet, trainSet] = getSample(dataPoints, TEST_SAMPLE_PERCENTAGE);

    std::vector<std::vector<Point>> folds = fold(trainSet, FOLDS);

    KNNClassifier classifier(trainSet, K);
    std::vector<std::string> predictions;
    int trainCorrectCount = 0;
    for (const auto& point : trainSet)
    {
        std::string predictedClass = classifier.classify(point);
        predictions.push_back(predictedClass);
        if (predictedClass == point.name)
        {
            trainCorrectCount++;
        }
    }
    float trainAccuracy = (float)trainCorrectCount / trainSet.size() * 100.0f;

    std::vector<float> foldAccuracies;
    for (int i = 0; i < FOLDS; i++)
    {
        std::vector<Point> validationSet = folds[i];
        std::vector<Point> trainingSet;
        for (int j = 0; j < FOLDS; j++)
        {
            if (j != i)
            {
                trainingSet.insert(trainingSet.end(), folds[j].begin(), folds[j].end());
            }
        }
        KNNClassifier foldClassifier(trainingSet, K);
        int correctCount = 0;
        for (const auto& point : validationSet)
        {
            std::string predictedClass = foldClassifier.classify(point);
            if (predictedClass == point.name)
            {
                correctCount++;
            }
        }
        float foldAccuracy = (float)correctCount / validationSet.size() * 100.0f;
        foldAccuracies.push_back(foldAccuracy);
    }
    float sumAccuracies = 0.0f;
    for (const auto& acc : foldAccuracies)
    {
        sumAccuracies += acc;
    }
    float averageAccuracy = sumAccuracies / FOLDS;

    float variance = 0.0f;
    for (const auto& acc : foldAccuracies)
    {
        variance += (acc - averageAccuracy) * (acc - averageAccuracy);
    }
    variance /= FOLDS;
    float stdDeviation = std::sqrt(variance);

    std::cout << "1. Train Set Accuracy:" << std::endl;
    std::cout << "    Accuracy: " << trainAccuracy << "%" << std::endl << std::endl;
    std::cout << "2. 10-Fold Cross-Validation Results:" << std::endl << std::endl;
    for (int i = 0; i < FOLDS; i++)
    {
        std::cout << "    Accuracy Fold " << (i + 1) << ": " << foldAccuracies[i] << "%" << std::endl;
    }
    std::cout << std::endl;
    std::cout << "    Average Accuracy: " << averageAccuracy << "%" << std::endl;
    std::cout << "    Standard Deviation: " << stdDeviation << "%" << std::endl << std::endl;
    int testCorrectCount = 0;
    for (const auto& point : testSet)
    {
        std::string predictedClass = classifier.classify(point);
        if (predictedClass == point.name)
        {
            testCorrectCount++;
        }
    }
    float testAccuracy = (float)testCorrectCount / testSet.size() * 100.0f;
    std::cout << "3. Test Set Accuracy:" << std::endl;
    std::cout << "    Accuracy: " << testAccuracy << "%" << std::endl;

    return 0;
}