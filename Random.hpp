#pragma once

#include "Types.hpp"
#include <random>
#include <chrono>

struct Rand{
    std::mt19937_64 gen;
    // Rand(i64 seed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()){
    //     gen.seed(seed);
    // }
    Rand(i64 seed = 213769420){
        gen.seed(seed);
    }
    template<typename T>
    T rand(){
        std::uniform_int_distribution<T> dist;
        return dist(gen);
    }
    template<typename T>
    T rand(T a, T b){
        std::uniform_int_distribution<T> dist(a, b);
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