#include "./Random.hpp"

RNG::RNG() {
    gen = mt19937(rd());
}

RNG::RNG(int seed) {
    gen = mt19937(seed);
}

int RNG::nextInt() {
    uniform_int_distribution<> dis;
    return dis(gen);
}

int RNG::nextInt(int min, int max) {
    uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

float RNG::nextFloat() {
    uniform_real_distribution<> dis;
    return dis(gen);
}

float RNG::nextFloat(float min, float max) {
    uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

double RNG::nextDouble() {
    uniform_real_distribution<> dis;
    return dis(gen);
}

double RNG::nextDouble(double min, double max) {
    uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

double RNG::nextDoubleRounded(double min, double max) {
        uniform_real_distribution<> dis(min, max);

        double value = dis(gen);
        return round(value * 2.0) / 2.0;
    }

bool RNG::nextBool() {
    bernoulli_distribution dis;
    return dis(gen);
}

bool RNG::nextBool(float probability) {
    bernoulli_distribution dis(probability);
    return dis(gen);
}

void RNG::setSeed(int seed) {
    gen = mt19937(seed);
}

int RNG::getSeed() {
    return gen();
}