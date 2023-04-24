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
class MultilevelBloomFilter
{
private:
    size_t m;  // -n*ln(p) / (ln(2)^2), number of bits
    uint8_t k; // m/n * ln(2), number of hash functions
    size_t num_level;
    std::vector<bool> vec;
    std::vector<unsigned long long> seeds;

    size_t _get_index(size_t index, size_t level)
    {
        size_t idx = level * m + index;
        if (idx > vec.size())
            std::cout << index << ' ' << level << '\n';
        return 0;
    }

    bool _contain_at_level(size_t indices[], size_t level)
    {
        for (int i = 0; i < k; ++i)
            if (!vec[_get_index(indices[i], level)])
                return false;
        return true;
    }

    int _get_best_level(size_t indices[])
    {
        for (int i = 0; i < num_level; ++i)
            if (!_contain_at_level(indices, i))
                return i - 1;

        return num_level - 1;
    }

public:
    MultilevelBloomFilter(size_t n, // number of expected inserts
                          double p, // probability of false negative
                          size_t num_level) : num_level(num_level)
    {
        m = -(long long int)n * log(p) / (log(2) * log(2));
        k = m / n * log(2);
        vec.resize(m * num_level);
        for (uint i = 0; i < k; ++i)
            seeds.push_back(distribution(generator));
    }

    void get_index(T key, size_t results[])
    {
        for (int i = 0; i < k; ++i)
        {
            unsigned long long seed = seeds[i];
            unsigned long long hash = murmur3_64(seed ^ key.to_ullong());
            results[i] = hash % m;
        }
    }

    void add(T key)
    {
        size_t indices[k];
        get_index(key, indices);

        int prev_best_level = _get_best_level(indices);
        if (prev_best_level == num_level - 1)
            return;

        for (int i = 0; i < k; ++i)
            vec[_get_index(indices[i], prev_best_level + 1)] = true;
    }

    bool contains(T key)
    {
        // return true if the key is contains at the LAST level
        size_t indices[k];
        get_index(key, indices);

        return _contain_at_level(indices, num_level - 1);
    }
};