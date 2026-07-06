#pragma once

#include "Types.hpp"
#include <random>

class Rand{
    std::mt19937_64 gen;
    public:
    Rand(int seed = 1){
        gen.seed(seed);
    }
    u64 randll(){
        static std::uniform_int_distribution<u64> dist;
        return dist(gen);
    }
    u64 randll(u64 a, u64 b){
        std::uniform_int_distribution<u64> dist(a, b);
        return dist(gen);
    }
    u32 rand(){
        static std::uniform_int_distribution<u32> dist;
        return dist(gen);
    }
    u32 rand(u32 a, u32 b){
        std::uniform_int_distribution<u32> dist(a, b);
        return dist(gen);
    }
    u32 randb(){
        static std::uniform_int_distribution<u32> dist;
        return dist(gen);
    }
    u32 randb(u32 a, u32 b){
        std::uniform_int_distribution<u32> dist(a, b);
        return dist(gen);
    }
    float randf(){
        static std::uniform_real_distribution<float> dist(0.0, 1.0);
        return dist(gen);
    }
    float randf(float a, float b){
        static std::uniform_real_distribution<float> dist(0.0, 1.0);
        return a + (b - a) * dist(gen);
    }
};