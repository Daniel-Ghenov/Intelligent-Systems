#include <fstream>
#include <iostream>
#include <utility>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <random>
#include <iomanip>

static std::mt19937 rng = std::mt19937(std::random_device()());

double sigmoid(double x)
{
    return 1.0 / (1.0 + std::exp(-x));
}

double sigmoidDerivative(double x)
{
    return x * (1 - x);
}

double tanhDerivative(double x)
{
    return 1 - x * x;
}


enum class Mode
{
    AND,
    OR,
    XOR,
    ALL
};

Mode getModeFromString(const std::string& modeStr)
{
    if (modeStr == "AND")
        return Mode::AND;
    else if (modeStr == "OR")
        return Mode::OR;
    else if (modeStr == "XOR")
        return Mode::XOR;
    else if (modeStr == "ALL")
        return Mode::ALL;
    else
        throw std::invalid_argument("Invalid mode string");
}

class NeuralNetwork
{
private:
    int activationFunction;
    int hiddenLayersCount;
    int hiddenLayerNeurons;
    Mode mode;
    int epochs;
    double learningRate;

    struct Layer
    {
        std::vector<std::vector<double>> weights;
    };

    std::vector<Layer> layers;

public:

    NeuralNetwork(int activationFunction, int hiddenLayersCount, int hiddenLayerNeurons, Mode mode, int epochs = 50000, double learningRate = 0.1)
        : activationFunction(activationFunction),
          hiddenLayersCount(hiddenLayersCount),
          hiddenLayerNeurons(hiddenLayerNeurons),
          mode(mode),
          epochs(epochs),
          learningRate(learningRate)
    {
        randomizeWeights();
        train();
    }

    double predict(int input1, int input2)
    {
        std::vector<std::pair<int, int>> inputs = {{input1, input2}};
        std::vector<std::vector<double>> activations = getActivations(inputs, 0);
        return activations.back()[0];
    }

private:

    void randomizeWeights()
    {
        std::uniform_real_distribution<double> distribution(-0.05, 0.05);

        int inputSize = 2;
        int outputSize = 1;

        layers.push_back(Layer{
            std::vector<std::vector<double>>(hiddenLayerNeurons, std::vector<double>(inputSize))
        });

        for (int i = 0; i < hiddenLayerNeurons; ++i)
        {
            for (int j = 0; j < inputSize; ++j)
            {
                layers[0].weights[i][j] = distribution(rng);
            }
        }

        for (int l = 1; l < hiddenLayersCount + 1; ++l)
        {
            layers.push_back(Layer{
                std::vector<std::vector<double>>(hiddenLayerNeurons, std::vector<double>(hiddenLayerNeurons))
            });

            for (int i = 0; i < hiddenLayerNeurons; ++i)
            {
                for (int j = 0; j < hiddenLayerNeurons; ++j)
                {
                    layers[l].weights[i][j] = distribution(rng);
                }
            }
        }

        layers.push_back(Layer{
            std::vector<std::vector<double>>(outputSize, std::vector<double>(hiddenLayerNeurons))
        });

        for (int i = 0; i < outputSize; ++i)
        {
            for (int j = 0; j < hiddenLayerNeurons; ++j)
            {
                layers.back().weights[i][j] = distribution(rng);
            }
        }
    }

    void train()
    {
        std::vector<std::pair<int, int>> inputs = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
        std::vector<int> targets = getTargets();

        for (int i = 0; i < epochs; ++i)
        {
            for (size_t j = 0; j < inputs.size(); ++j)
            {
                std::vector<std::vector<double>> activations = getActivations(inputs, j);

                errorBackPropagate(targets, j, activations);
            }
        }
    }

    void errorBackPropagate(const std::vector<int> &targets,
                            size_t j,
                            const std::vector<std::vector<double>> &activations)
    {
        std::vector<std::vector<double>> deltas(layers.size());

        for (size_t l = layers.size() - 1; l > 0; --l)
        {
            for (size_t n = 0; n < layers[l].weights.size(); ++n)
            {
                deltas[l].resize(layers[l].weights.size(), 0.0);

                double output = activations[l][n];
                double error = getError(targets, j, deltas, l, n, output);
                double delta = 0.0;

                if (activationFunction == 0)
                {
                    delta = error * sigmoidDerivative(output);
                }
                else if (activationFunction == 1)
                {
                    delta = error * tanhDerivative(output);
                }

                for (size_t k = 0; k < layers[l].weights[n].size(); ++k)
                {
                    double inputActivation = activations[l - 1][k];
                    layers[l].weights[n][k] += learningRate * delta * inputActivation;
                }

                deltas[l][n] = delta;
            }
        }
    }

    double getError(const std::vector<int> &targets,
                    size_t j,
                    const std::vector<std::vector<double>> &deltas,
                    size_t l,
                    size_t n,
                    double output)
    {
        if (l == layers.size() - 1)
        {
            return targets[j] - output;
        }

        double error = 0.0;
        for (size_t m = 0; m < layers[l + 1].weights.size(); ++m)
        {
            error += deltas[l + 1][m] * layers[l + 1].weights[m][n];
        }

        return error;
    }

    std::vector<std::vector<double>> getActivations(const std::vector<std::pair<int, int>> &inputs, size_t j)
    {
        std::vector<std::vector<double>> activations;
        activations.push_back({static_cast<double>(inputs[j].first), static_cast<double>(inputs[j].second)});

        for (size_t l = 1; l < layers.size(); ++l)
        {
            activations.emplace_back(layers[l].weights.size(), 0.0);

            for (size_t n = 0; n < layers[l].weights.size(); ++n)
            {
                double sum = 0.0;
                for (size_t k = 0; k < layers[l - 1].weights[0].size(); ++k)
                {
                    double inputActivation = activations[l - 1][k];
                    sum += layers[l].weights[n][k] * inputActivation;
                }

                if (activationFunction == 0)
                {
                    activations[l][n] = sigmoid(sum);
                }
                else if (activationFunction == 1)
                {
                    activations[l][n] = tanh(sum);
                }

            }
        }
        return activations;
    }

    std::vector<int> getTargets() const
    {
        switch (mode) {
            case Mode::AND:
                return {0, 0, 0, 1};
            case Mode::OR:
                return {0, 1, 1, 1};
            case Mode::XOR:
                return {0, 1, 1, 0};
        }
    }
};

int main()
{

    //make output show up to 5 digits

    std::cout.setf(std::ios::fixed);
    std::cout << std::setprecision(5);

    std::string modeStr;
    std::cin >> modeStr;
    Mode mode = getModeFromString(modeStr);

    int activationFunction, hidenLayersCount, hiddenLayerNeurons;
    std::cin >> activationFunction >> hidenLayersCount >> hiddenLayerNeurons;

    if (mode == Mode::AND || mode == Mode::ALL)
    {
        NeuralNetwork nnAND(activationFunction, hidenLayersCount, hiddenLayerNeurons, Mode::AND);
        std::cout << "AND:" << std::endl;
        for (int i = 0; i <= 1; ++i)
        {
            for (int j = 0; j <= 1; ++j)
            {
                double output = nnAND.predict(i, j);
                std::cout << "(" << i << "," << j << ") -> " << output << std::endl;
            }
        }
    }

    if (mode == Mode::OR || mode == Mode::ALL)
    {
        NeuralNetwork nnOR(activationFunction, hidenLayersCount, hiddenLayerNeurons, Mode::OR);
        std::cout << "OR:" << std::endl;
        for (int i = 0; i <= 1; ++i)
        {
            for (int j = 0; j <= 1; ++j)
            {
                double output = nnOR.predict(i, j);
                std::cout << "(" << i << "," << j << ") -> " << output << std::endl;
            }
        }
    }

    if (mode == Mode::XOR || mode == Mode::ALL)
    {
        NeuralNetwork nnXOR(activationFunction, hidenLayersCount, hiddenLayerNeurons, Mode::XOR);
        std::cout << "XOR:" << std::endl;
        for (int i = 0; i <= 1; ++i)
        {
            for (int j = 0; j <= 1; ++j)
            {
                double output = nnXOR.predict(i, j);
                std::cout << "(" << i << "," << j << ") -> " << output << std::endl;
            }
        }
    }

}