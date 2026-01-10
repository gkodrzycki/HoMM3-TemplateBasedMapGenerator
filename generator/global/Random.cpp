#include "./Random.hpp"

RNG::RNG() { gen = mt19937(rd()); }

RNG::RNG(int seed) { gen = mt19937(seed); }

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

void RNG::setSeed(int seed) { gen = mt19937(seed); }

int RNG::getSeed() { return gen(); }

pair<int3, int3> RNG::getRandomTriangle(int3 anchorPoint, int perimeter) {

    int x,y;
    int a,b,c;
    int maxIterations = 100;
    while (maxIterations--) {
        x = nextInt(5, perimeter);
        y = nextInt(5, perimeter);

        if (x > y) swap(x,y);

        a = x;
        b = y - x ;
        c = perimeter - y;
        if (max(a,max(b,c)) < perimeter / 2) break; 

    }
    
    double randomAngle = nextDouble(0, 2*M_PI);
    int3 B(anchorPoint),C(anchorPoint); 

    B.x += cos(randomAngle) * c;
    B.y += sin(randomAngle) * c;
    
    int dx = (b*b - a*a + c*c) / (2*c);
    int dy = sqrt(b*b - dx*dx);
    if(nextBool()) dy *= -1;
    
    
    int _dx = dx, _dy = dy;
    _dx = cos(randomAngle) * dx - sin(randomAngle) * dy;
    _dy = sin(randomAngle) * dx + cos(randomAngle) * dy;

    

    C.x += _dx;
    C.y += _dy;

    // cerr << "Random Triangle A: " << anchorPoint.toString() << " B: " << B.toString() << " C: " << C.toString() << "\n";  

    return {B,C};
}
