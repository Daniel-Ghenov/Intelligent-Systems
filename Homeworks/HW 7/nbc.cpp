#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <random>

static const int NUM_FEATURES = 16;
static const char DELIMITER = ',';
static const int TEST_SAMPLE_PERCENTAGE = 20;
static const int FOLDS = 10;
static std::mt19937 rng = std::mt19937(std::random_device()());

struct Point
{
    std::string label;
    std::vector<char> features;
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

class NBClassifier
{
private:
    int numFeatures;
    bool isQuestionAbstain;
    double laplaceLambda = 1.0;
    std::unordered_map<std::string, double> classProbabilities;
    std::unordered_map<char, int> featureToIndex;
    std::unordered_map<std::string, std::vector<std::vector<double>>> featureGivenClassProbabilities;

public:

    NBClassifier(const std::vector<Point>& datapoints, bool isQuestionAbstain, int numFeatures, double laplaceLambda): isQuestionAbstain(isQuestionAbstain), numFeatures(numFeatures), laplaceLambda(laplaceLambda)
    {
        train(datapoints);
    }

    std::string classify(const Point& point)
    {
        std::string bestClass;
        double bestProb = -INFINITY;

        for (auto& classPair : this->classProbabilities)
        {
            double probability = classPair.second;
            std::vector<std::vector<double>> featureClassProbs = featureGivenClassProbabilities[classPair.first];
            for (int i = 0; i < numFeatures; i++)
            {
                probability += featureClassProbs[i][featureToIndex[point.features[i]]];
            }

            if (probability > bestProb)
            {
                bestProb = probability;
                bestClass = classPair.first;
            }
        };

        return bestClass;
    };

private:

    void extractClassProbabilities(const std::vector<Point> &datapoints)
    {
        std::unordered_map<std::string, int> classCounts;
        for (const auto& point : datapoints)
        {
            classCounts[point.label]++;
        }
        int totalPoints = datapoints.size();
        for (const auto& pair : classCounts)
        {
            classProbabilities[pair.first] = std::log((static_cast<double>(pair.second) + laplaceLambda) / (double)(totalPoints + laplaceLambda * classCounts.size()));
        }
    }


    std::unordered_map<std::string, std::vector<char>>
    getMostFrequentFeaturesPerClass(const std::vector<Point> &datapoints)
    {
        std::unordered_map<std::string, std::vector<char>> mostFrequentFeaturesPerClass;

        std::unordered_map<std::string, std::vector<std::vector<int>>> featureValuesPerClass;
        for (const auto& point : datapoints)
        {
            if (featureValuesPerClass.find(point.label) == featureValuesPerClass.end())
            {
                featureValuesPerClass[point.label] = std::vector<std::vector<int>>(numFeatures, std::vector<int>(featureToIndex.size(), 0));
            }
            for (size_t i = 0; i < point.features.size(); ++i)
            {
                char pointFeature = point.features[i];
                if (pointFeature != '?')
                {
                    featureValuesPerClass[point.label][i][featureToIndex[pointFeature]]++;
                }
            }
        }

        for (const auto& classPair : featureValuesPerClass)
        {
            const std::string& classLabel = classPair.first;
            mostFrequentFeaturesPerClass[classLabel] = std::vector<char>(numFeatures, ' ');
            for (size_t i = 0; i < numFeatures; ++i)
            {
                int maxCount = -1;
                char mostFrequentFeature = ' ';
                for (const auto& featurePair : featureToIndex)
                {
                    if (classPair.second[i][featurePair.second] > maxCount)
                    {
                        maxCount = classPair.second[i][featurePair.second];
                        mostFrequentFeature = featurePair.first;
                    }
                }
                mostFrequentFeaturesPerClass[classLabel][i] = mostFrequentFeature;
            }
        }
        return mostFrequentFeaturesPerClass;
    }

    void extractFeatureGivenClassProbabilities(const std::vector<Point> &datapoints)
    {
        std::unordered_map<std::string, std::vector<std::vector<int>>> featureGivenClassCounts;
        std::unordered_map<std::string, int> classCounts;
        std::unordered_map<std::string, std::vector<char>> mostFrequentFeaturesPerClass;

        if (!isQuestionAbstain)
        {
            mostFrequentFeaturesPerClass = getMostFrequentFeaturesPerClass(datapoints);
        }

        for (const auto& point : datapoints)
        {
            classCounts[point.label]++;
            if (featureGivenClassCounts.find(point.label) == featureGivenClassCounts.end())
            {
                featureGivenClassCounts[point.label] = std::vector<std::vector<int>>(numFeatures, std::vector<int>(featureToIndex.size(), 0));
            }
            for (size_t i = 0; i < point.features.size(); ++i)
            {
                char pointFeature;
                if (isQuestionAbstain || point.features[i] != '?')
                {
                    pointFeature = point.features[i];
                }
                else
                {
                    pointFeature = mostFrequentFeaturesPerClass[point.label][i];
                }
                featureGivenClassCounts[point.label][i][featureToIndex[pointFeature]]++;
            }
        }

        for (const auto& classPair : classCounts)
        {
            const std::string& classLabel = classPair.first;
            int totalClassCount = classPair.second;
            featureGivenClassProbabilities[classLabel] = std::vector<std::vector<double>>(numFeatures, std::vector<double>(featureToIndex.size(), 0.0));

            for (size_t i = 0; i < numFeatures; ++i)
            {
                for (const auto& featurePair : featureToIndex)
                {
                    featureGivenClassProbabilities[classLabel][i][featurePair.second] = std::log(
                            (static_cast<double>(featureGivenClassCounts[classLabel][i][featurePair.second]) + laplaceLambda) /
                            (double)(totalClassCount + laplaceLambda * featureToIndex.size()));
                }
            }
        }

    }

    void train(const std::vector<Point>& datapoints)
    {
        featureToIndex['y'] = 0;
        featureToIndex['n'] = 1;
        if (isQuestionAbstain)
        {
            featureToIndex['?'] = 2;
        }

        extractClassProbabilities(datapoints);
        extractFeatureGivenClassProbabilities(datapoints);

    }
};

int main()
{
    int mode = 0;
    bool isQuestionAbstain;
    std::cin >> mode;
    isQuestionAbstain = (mode == 0);

    double laplaceLambda = 1.0;
    std::cin >> laplaceLambda;
    std::ifstream dataFile("house-votes-84.data");
    if (!dataFile.is_open())
    {
        std::cerr << "Could not open the training data file." << std::endl;
        return 1;
    }

    std::vector<Point> dataPoints;

    std::string line;
    while(std::getline(dataFile, line))
    {
        Point point;
        point.label = line.substr(0, line.find(DELIMITER));
        size_t pos = line.find(',') + 1;
        for (int i = 0; i < NUM_FEATURES; ++i)
        {
            char featureChar = line[pos + i * 2];
            point.features.push_back(featureChar);
        }
        dataPoints.push_back(point);
    }

    dataFile.close();
    auto [testSet, trainSet] = getSample(dataPoints, TEST_SAMPLE_PERCENTAGE);


    std::vector<std::vector<Point>> folds = fold(trainSet, FOLDS);

    NBClassifier classifier(trainSet, isQuestionAbstain, NUM_FEATURES, laplaceLambda);
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


        NBClassifier foldClassifier(trainSet, isQuestionAbstain, NUM_FEATURES, laplaceLambda);
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