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

    template <typename T> void shuffle(vector<T> &vec);

  private:
    random_device rd;
    mt19937 gen;
};

template <typename T> void RNG::shuffle(vector<T> &vec) { shuffle(vec.begin(), vec.end(), gen); }
