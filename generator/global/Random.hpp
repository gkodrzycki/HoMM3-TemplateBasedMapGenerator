#pragma once

#include "./Global.hpp"

class RNG {
  public:
    RNG();
    RNG(int seed);
    int nextInt();
    int nextInt(int min, int max);
    float nextFloat();
    float nextFloat(float min, float max);
    double nextDouble();
    double nextDouble(double min, double max);
    double nextDoubleRounded(double min, double max);
    bool nextBool();
    bool nextBool(float probability);
    void setSeed(int seed);
    int getSeed();
    int getOriginalSeed();

    pair<int3, int3> getRandomTriangle(int3 anchorPoint, int perimeter);

    template <typename T> void shuffle(vector<T> &vec);
    template <typename T> T getRandomFromVector(vector<T> &vec);
    template <typename T> int getRandomWeightedIndex(const vector<T> &weights);

  private:
    random_device rd;
    mt19937 gen;
    int originalSeed;
};

template <typename T> void RNG::shuffle(vector<T> &vec) {
    std::shuffle(vec.begin(), vec.end(), gen);
}

template <typename T> T RNG::getRandomFromVector(vector<T> &vec) {
    if (vec.empty()) {
        throw runtime_error("Cannot get random element from an empty vector");
    }
    return vec[nextInt(0, vec.size() - 1)];
}

template <typename T> int RNG::getRandomWeightedIndex(const vector<T> &weights) {
    if (weights.empty()) {
        throw runtime_error("Cannot get random weighted index from an empty vector");
    }

    T totalWeight = accumulate(weights.begin(), weights.end(), static_cast<T>(0));
    if (totalWeight <= 0) {
        throw runtime_error("Total weight must be positive");
    }

    uniform_real_distribution<T> dis(0, totalWeight);
    T randomValue = dis(gen);

    T cumulativeWeight = 0;
    for (size_t i = 0; i < weights.size(); ++i) {
        cumulativeWeight += weights[i];
        if (randomValue < cumulativeWeight) {
            return static_cast<int>(i);
        }
    }

    return static_cast<int>(weights.size() - 1);
}
