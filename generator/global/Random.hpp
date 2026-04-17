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

  private:
    random_device rd;
    mt19937 gen;
    int originalSeed;
};

template <typename T> void RNG::shuffle(vector<T> &vec) {
    std::shuffle(vec.begin(), vec.end(), gen);
}

template <typename T> T RNG::getRandomFromVector(vector<T> &vec) {
    return vec[nextInt(0, vec.size() - 1)];
}
