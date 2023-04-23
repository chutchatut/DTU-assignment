#pragma once
#include <iostream>
#include <bitset>
#include <unordered_map>
#include <string>
#include <cmath>
#include <vector>
#include <random>

std::random_device rd;
std::default_random_engine generator(rd());
std::uniform_int_distribution<unsigned long long> distribution(0, 0xFFFFFFFFFFFFFFFF);

unsigned long int murmur3_64(unsigned long int x)
{
    x = (x ^ (x >> 33)) * 0xff51afd7ed558ccdL;
    x = (x ^ (x >> 23)) * 0xc4ceb9fe1a85ec53L;
    x = x ^ (x >> 33);
    return x;
}

template <typename T>
std::hash<T> bitset_hash_fn;

template <typename T>
class BloomFilter
{
public:
    int m; // -n*ln(p) / (ln(2)^2), number of bits
    int k; // m/n * ln(2), number of hash functions
    std::vector<bool> vec;
    std::vector<unsigned long long> seeds;

    BloomFilter(long int n, // number of expected inserts
                double p)   // probability of false negative
    {
        m = -n * log(p) / (log(2) * log(2));
        k = m / n * log(2);
        vec.resize(m);
        for (uint i = 0; i < k; ++i)
            seeds.push_back(distribution(generator));
    }

    void add(T key)
    {
        for (unsigned long long seed : seeds)
        {
            unsigned long long hash = murmur3_64(seed ^ key.to_ullong());
            vec[hash % m] = true;
        }
    }

    bool contains(T key)
    {
        for (unsigned long long seed : seeds)
        {
            unsigned long long hash = murmur3_64(seed ^ key.to_ullong());
            if (!vec[hash % m])
            {
                return false;
            }
        }
        return true;
    }
};