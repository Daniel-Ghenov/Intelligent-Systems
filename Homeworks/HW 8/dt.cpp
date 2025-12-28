#include <fstream>
#include <iostream>
#include <utility>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <random>

static const int NUM_FEATURES = 9;
static const char DELIMITER = ',';
static const int TEST_SAMPLE_PERCENTAGE = 20;
static const int FOLDS = 10;
static std::mt19937 rng = std::mt19937(std::random_device()());

struct Point
{
    std::string label;
    std::vector<int> features;
};

std::pair<std::vector<Point>, std::vector<Point>> getSample(const std::vector<Point>& points, int percentage)
{
    int sampleSize = points.size() * percentage / 100;
    std::vector<Point> sample;
    std::vector<Point> rest;

    std::unordered_map<std::string, std::vector<Point>> classPoints;
    for (const auto& point : points)
    {
        if (classPoints.find(point.label) == classPoints.end())
        {
            classPoints[point.label] = {};
        }
        classPoints[point.label].push_back(point);
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
        if (classPoints.find(point.label) == classPoints.end())
        {
            classPoints[point.label] = {};
        }
        classPoints[point.label].push_back(point);
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

std::tuple<int, char, char> getOptions(const std::string& options)
{
    int mode = 0;
    char prePrune = '-';
    char postPrune = '-';

    if (options.size() >= 1)
    {
        mode = options[0] - '0';
    }

    if (options.size() >= 2)
    {
        char secondChar = options[1];
        if (mode == 0 || mode == 2)
        {
            if (secondChar == 'K' || secondChar == 'N' || secondChar == 'G')
            {
                prePrune = secondChar;
            }
        }
        else if (mode == 1)
        {
            if (secondChar == 'E' || secondChar == 'C' || secondChar == 'X')
            {
                postPrune = secondChar;
            }
        }
    }

    if (options.size() >= 5 && mode == 2)
    {
        char fourthChar = options[3];
        if (fourthChar == 'E' || fourthChar == 'C' || fourthChar == 'X')
        {
            postPrune = fourthChar;
        }
    }

    if (mode != 1 && prePrune == '-')
    {
        prePrune = 'A';
    }

    if (mode != 0 && postPrune == '-')
    {
        postPrune = 'A';
    }

    return {mode, prePrune, postPrune};
}

class DecisionTree
{
private:
    struct Node
    {
        bool isLeaf;
        std::string label;
        int examplesCount;
        int featureIndex;
        std::unordered_map<int, Node*> children;

        Node() : isLeaf(false), featureIndex(-1) {}
        Node(bool isLeaf, std::string  label, int examplesCount, int featureIndex, const std::unordered_map<int, Node*>& children) :
            isLeaf(isLeaf), label(std::move(label)), featureIndex(featureIndex), children(children) {}
    };
    char prePrune = '-';
    int maxDepth = 5;
    int minSamplesSplit = 4;
    char postPrune = '-';
    double pseudoErrorConstant = 0.5;
    double chiSquaredCutoff = 0.05;
    int numFeatures = 0;
    Node* root = nullptr;

public:
    DecisionTree(const std::vector<Point>& dataPoints, char prePrune, char postPrune, int numFeatures):
        prePrune(prePrune), postPrune(postPrune), numFeatures(numFeatures)
    {
        root = createTree(dataPoints);
    }

    ~DecisionTree()
    {
        free(root);
    }

    DecisionTree(const DecisionTree& other) = delete;
    DecisionTree& operator=(const DecisionTree& other) = delete;

    std::string classify(const Point& point)
    {
        return classify(point, root);
    }
private:

    std::string classify(const Point& point, Node* node)
    {
        if (node->isLeaf)
        {
            return node->label;
        }

        int featureValue = point.features[node->featureIndex];
        if (!node->children.count(featureValue))
        {
            return node->label;
        }
        return classify(point, node->children[featureValue]);
    }

    Node* createTree(const std::vector<Point>& dataPoints)
    {
        std::vector<bool> isFeatureUsed(numFeatures, false);
        int featuresUsed = 0;
        Node* wantedRoot = createTree(dataPoints, isFeatureUsed, featuresUsed);
        wantedRoot = prune(wantedRoot, dataPoints);
        return wantedRoot;
    }


    Node* createTree(const std::vector<Point>& dataPoints, std::vector<bool>& isFeatureUsed, int featuresUsed)
    {
        int minEntropyFeature = getMinEntropyFeature(dataPoints, isFeatureUsed);
        bool shouldBeLeaf = shouldBeLeafNode(dataPoints, isFeatureUsed, featuresUsed, minEntropyFeature);
        std::string majorityClass = getMajorityClass(dataPoints);
        if (shouldBeLeaf)
        {
            return new Node(true, majorityClass, dataPoints.size(), -1, {});
        }

        isFeatureUsed[minEntropyFeature] = true;
        featuresUsed++;
        std::unordered_map<int, std::vector<Point>> partitions;
        for (const auto& point : dataPoints)
        {
            partitions[point.features[minEntropyFeature]].push_back(point);
        }
        std::unordered_map<int, Node*> children;
        for (const auto& pair : partitions)
        {
            children[pair.first] = createTree(pair.second, isFeatureUsed, featuresUsed);
        }
        isFeatureUsed[minEntropyFeature] = false;
        return new Node(false, majorityClass, dataPoints.size(), minEntropyFeature, children);
    }

    static std::string getMajorityClass(const std::vector<Point> &dataPoints)
    {
        std::unordered_map<std::string, int> classCounts;
        for (const auto& point : dataPoints)
        {
            classCounts[point.label]++;
        }

        std::string majorityClass;
        int maxCount = 0;
        for (const auto& pair : classCounts)
        {
            if (pair.second > maxCount)
            {
                maxCount = pair.second;
                majorityClass = pair.first;
            }
        }
        return majorityClass;
    }

    static int getMinEntropyFeature(const std::vector<Point>& dataPoints, const std::vector<bool>& isFeatureUsed)
    {
        double minEntropy = INFINITY;
        int minEntropyFeature = -1;

        for (int i = 0; i < isFeatureUsed.size(); i++)
        {
            if (isFeatureUsed[i]) continue;

            double entropy = getEntropy(dataPoints, i);

            if (entropy < minEntropy)
            {
                minEntropy = entropy;
                minEntropyFeature = i;
            }
        }

        return minEntropyFeature;
    }

    static double getEntropy(const std::vector<Point> &dataPoints, int featureIndex)
    {
        std::unordered_map<int, std::unordered_map<std::string, int>> featureValueClassCounts;
        std::unordered_map<std::string, int> classCounts;

        for (const auto& point : dataPoints)
        {
            featureValueClassCounts[point.features[featureIndex]][point.label]++;
            classCounts[point.label]++;
        }

        double entropy = 0.0;
        int totalPoints = dataPoints.size();

        for (const auto& valuePair : featureValueClassCounts)
        {
            int valueCount = 0;
            for (const auto& classPair : valuePair.second)
            {
                valueCount += classPair.second;
            }

            double valueEntropy = 0.0;
            for (const auto& classPair : valuePair.second)
            {
                double probability = (double)classPair.second / valueCount;
                valueEntropy -= probability * log2(probability);
            }

            entropy += ((double)valueCount / totalPoints) * valueEntropy;
        }
        return entropy;
    }

    Node* prune(Node *wantedRoot, const std::vector<Point> &dataPoints)
    {
        Node* prunedRoot = wantedRoot;
        if (shouldUseErrorEstimationPruning())
        {
            prunedRoot = errorEstimationPrune(prunedRoot, dataPoints);
        }

        if (shouldUseChiSquaredPruning())
        {
            prunedRoot = chiSquaredPrune(prunedRoot, dataPoints);
        }
        return prunedRoot;
    }


    Node* chiSquaredPrune(Node *wantedRoot, const std::vector<Point> &dataPoints)
    {
        if (!wantedRoot || wantedRoot->isLeaf)
        {
            return wantedRoot;
        }

        std::unordered_map<int, std::vector<Point>> childDataPointsMap;
        for (auto& childPair : wantedRoot->children)
        {
            std::vector<Point> childDataPoints;
            for (const auto& point : dataPoints)
            {
                if (point.features[wantedRoot->featureIndex] == childPair.first)
                {
                    childDataPoints.push_back(point);
                }
            }
            childDataPointsMap[childPair.first] = childDataPoints;
            childPair.second = chiSquaredPrune(childPair.second, childDataPoints);
        }

        double chiSquared = 0.0;

        std::vector<std::vector<int>> examplesPerClassPerBranch = getExamplesPerClassPerBranch(wantedRoot, dataPoints);
        std::vector<int> totalPerClass = getTotalPerClass(examplesPerClassPerBranch);
        std::vector<int> totalPerBranch = getTotalPerBranch(examplesPerClassPerBranch);
        int totalExamples = dataPoints.size();

        for (int branchIndex = 0; branchIndex < examplesPerClassPerBranch.size(); branchIndex++)
        {
            for (int classIndex = 0; classIndex < totalPerClass.size(); classIndex++)
            {
                double expected = (double) totalPerClass[classIndex] * totalPerBranch[branchIndex] / totalExamples;
                double observed = examplesPerClassPerBranch[branchIndex][classIndex];
                if (expected > 0)
                {
                    chiSquared += ((observed - expected) * (observed - expected)) / expected;
                }
            }
        }

        if (chiSquared < chiSquaredCutoff)
        {
            Node* prunedNode = new Node(true, wantedRoot->label, wantedRoot->examplesCount, -1, {});
            free(wantedRoot);
            return prunedNode;
        }

        return wantedRoot;
    }

    std::vector<int> getTotalPerClass(std::vector<std::vector<int>> examplesPerClassPerBranch)
    {
        if (examplesPerClassPerBranch.empty())
        {
            return {};
        }
        int numClasses = examplesPerClassPerBranch[0].size();
        std::vector<int> totalPerClass(numClasses, 0);

        for (const auto& branchCounts : examplesPerClassPerBranch)
        {
            for (int classIndex = 0; classIndex < numClasses; classIndex++)
            {
                totalPerClass[classIndex] += branchCounts[classIndex];
            }
        }
        return totalPerClass;
    }

    std::vector<int> getTotalPerBranch(std::vector<std::vector<int>> examplesPerClassPerBranch)
    {
        std::vector<int> totalPerBranch;
        for (const auto& branchCounts : examplesPerClassPerBranch)
        {
            int branchTotal = 0;
            for (const auto& count : branchCounts)
            {
                branchTotal += count;
            }
            totalPerBranch.push_back(branchTotal);
        }
        return totalPerBranch;
    }

    Node* errorEstimationPrune(Node *wantedRoot, const std::vector<Point> &dataPoints)
    {
        if (!wantedRoot || wantedRoot->isLeaf)
        {
            return wantedRoot;
        }
        std::unordered_map<int, std::vector<Point>> childDataPointsMap;
        for (auto& childPair : wantedRoot->children)
        {
            std::vector<Point> childDataPoints;
            for (const auto& point : dataPoints)
            {
                if (point.features[wantedRoot->featureIndex] == childPair.first)
                {
                    childDataPoints.push_back(point);
                }
            }
            childDataPointsMap[childPair.first] = childDataPoints;
            childPair.second = errorEstimationPrune(childPair.second, childDataPoints);
        }

        double errorsAfterPrune = getErrors(wantedRoot, dataPoints);
        double errorsBeforePrune = 0.0;
        for (const auto& childPair : wantedRoot->children)
        {
            errorsBeforePrune += getErrors(childPair.second, childDataPointsMap[childPair.first]);
        }

        if (errorsAfterPrune <= errorsBeforePrune)
        {
            Node* prunedNode = new Node(true, wantedRoot->label, wantedRoot->examplesCount, -1, {});
            free(wantedRoot);
            return prunedNode;
        }

        return wantedRoot;
    }

    double getErrors(Node* node, const std::vector<Point> &dataPoints) const
    {
        int errors = 0;
        std::string predictedClass = node->label;
        for (const auto& point : dataPoints)
        {
            if (point.label != predictedClass)
            {
                errors++;
            }
        }
        return errors + pseudoErrorConstant;
    }

    std::vector<std::vector<int>> getExamplesPerClassPerBranch(Node* node, const std::vector<Point> &dataPoints)
    {
        std::unordered_map<std::string, int> classToIndex;
        int classIndex = 0;
        for (const auto& point : dataPoints)
        {
            if (classToIndex.find(point.label) == classToIndex.end())
            {
                classToIndex[point.label] = classIndex++;
            }
        }

        int numClasses = classToIndex.size();
        std::vector<std::vector<int>> examplesPerClassPerBranch(node->children.size(),
                                                                 std::vector<int>(numClasses, 0));

        for (const auto& point : dataPoints)
        {
            int branchIndex = 0;
            for (const auto& childPair : node->children)
            {
                if (point.features[node->featureIndex] == childPair.first)
                {
                    int clsIndex = classToIndex[point.label];
                    examplesPerClassPerBranch[branchIndex][clsIndex]++;
                    break;
                }
                branchIndex++;
            }
        }

        return examplesPerClassPerBranch;
    }

    bool shouldBeLeafNode(const std::vector<Point> &dataPoints, const std::vector<bool> &isFeatureUsed, int featuresUsed,
                          int minEntropyFeature)
    {
        return featuresUsed == numFeatures ||
               (shouldUseMaxDepth() && featuresUsed >= maxDepth) ||
                (shouldUseMinSamplesSplit() && dataPoints.size() < minSamplesSplit);
    }

    bool shouldUseMaxDepth() const
    {
        return prePrune == 'A' || prePrune == 'N';
    }

    bool shouldUseMinSamplesSplit() const
    {
        return prePrune == 'A' || prePrune == 'K';
    }

    bool shouldUseChiSquaredPruning() const
    {
        return postPrune == 'A' || postPrune == 'X';
    }

    bool shouldUseErrorEstimationPruning() const
    {
        return postPrune == 'A' || postPrune == 'E';
    }

    void free(Node* node)
    {
        if (node == nullptr) return;
        for (auto& childPair : node->children)
        {
            free(childPair.second);
        }
        delete node;
    }
};

int main()
{
    std::string options;
    std::cin >> options;

    auto [mode, prePrune, postPrune] = getOptions(options);

    std::vector<Point> dataPoints;

    std::ifstream dataFile("../../../Homeworks/HW 8/Test/breast-cancer.data");
    if (!dataFile.is_open())
    {
        std::cerr << "Could not open the training data file." << std::endl;
        return 1;
    }

    std::vector<std::unordered_map<std::string, int>> featuresToIndex(NUM_FEATURES);

    std::string line;
    while(std::getline(dataFile, line))
    {
        Point point;
        point.label = line.substr(0, line.find(DELIMITER));

        size_t start = line.find(DELIMITER) + 1;
        for (int i = 0; i < NUM_FEATURES; i++)
        {
            size_t end = line.find(DELIMITER, start);
            std::string featureStr = line.substr(start, end - start);
            if (featuresToIndex[i].find(featureStr) == featuresToIndex[i].end())
            {
                int index = featuresToIndex[i].size();
                featuresToIndex[i][featureStr] = index;
            }
            point.features.push_back(featuresToIndex[i][featureStr]);
            start = end + 1;
        }

        dataPoints.push_back(point);
    }

    dataFile.close();
    auto [testSet, trainSet] = getSample(dataPoints, TEST_SAMPLE_PERCENTAGE);


    std::vector<std::vector<Point>> folds = fold(trainSet, FOLDS);

    DecisionTree classifier(trainSet, prePrune, postPrune, NUM_FEATURES);
    std::vector<std::string> predictions;
    int trainCorrectCount = 0;
    for (const auto& point : trainSet)
    {
        std::string predictedClass = classifier.classify(point);
        predictions.push_back(predictedClass);
        if (predictedClass == point.label)
        {
            trainCorrectCount++;
        }
    }

    double trainAccuracy = (double)trainCorrectCount / trainSet.size() * 100.0f;

    std::vector<double> foldAccuracies;
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


        DecisionTree foldClassifier(trainingSet, prePrune, postPrune, NUM_FEATURES);
        int correctCount = 0;
        for (const auto& point : validationSet)
        {
            std::string predictedClass = foldClassifier.classify(point);
            if (predictedClass == point.label)
            {
                correctCount++;
            }
        }
        double foldAccuracy = (double)correctCount / validationSet.size() * 100.0f;
        foldAccuracies.push_back(foldAccuracy);
    }
    double sumAccuracies = 0.0f;
    for (const auto& acc : foldAccuracies)
    {
        sumAccuracies += acc;
    }
    double averageAccuracy = sumAccuracies / FOLDS;

    double variance = 0.0f;
    for (const auto& acc : foldAccuracies)
    {
        variance += (acc - averageAccuracy) * (acc - averageAccuracy);
    }
    variance /= FOLDS;
    double stdDeviation = std::sqrt(variance);

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
        if (predictedClass == point.label)
        {
            testCorrectCount++;
        }
    }
    double testAccuracy = (double)testCorrectCount / testSet.size() * 100.0f;
    std::cout << "3. Test Set Accuracy:" << std::endl;
    std::cout << "    Accuracy: " << testAccuracy << "%" << std::endl;

    return 0;
}