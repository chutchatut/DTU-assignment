// Note: this only works for K <= 32 due to the choice of hash function
#include <iostream>
#include <bitset>
#include <unordered_map>
#include <string>
#include <cmath>
#include <vector>
#include <random>

#define K 16

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

std::hash<std::bitset<2 * K>> bitset_hash_fn;

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

    void add(std::bitset<2 * K> key)
    {
        for (unsigned long long seed : seeds)
        {
            unsigned long long hash = murmur3_64(seed ^ key.to_ullong());
            vec[hash % m] = true;
        }
    }

    bool contains(std::bitset<2 * K> key)
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

std::bitset<2 * K>
update_key(const std::bitset<2 * K> &current_key, char next_symbol)
{
    std::bitset<2 *K> new_key = current_key << 2;
    // Switch should be a bit faster than if-else
    switch (next_symbol)
    {
    case 'A':
        new_key |= 0;
        break;
    case 'C':
        new_key |= 1;
        break;
    case 'G':
        new_key |= 2;
        break;
    case 'T':
        new_key |= 3;
        break;
    default:
        throw std::invalid_argument("Invalid input symbol, only allow symbol are A, C, G, T");
    }
    return new_key;
}

BloomFilter get_duplicate_predicate(std::string input)
{
    // Return bloomfilter containing duplicate entries
    BloomFilter first = BloomFilter(input.size(), 0.05);
    BloomFilter dup = BloomFilter(input.size(), 0.05);
    std::bitset<2 * K> current_key;

    for (int i = 0; i < input.size(); i++)
    {
        current_key = update_key(current_key, input[i]);
        if (i >= K - 1)
        {
            if (first.contains(current_key))
                dup.add(current_key);
            first.add(current_key);
        }
    }

    return dup;
}

std::string bitset_to_ATCG(const std::bitset<2 * K> &key)
{
    std::string s;
    for (int i = key.size() - 1; i > 0; i -= 2)
    {
        switch (2 * key[i] + key[i - 1])
        {
        case 0:
            s.push_back('A');
            break;
        case 1:
            s.push_back('C');
            break;
        case 2:
            s.push_back('G');
            break;
        case 3:
            s.push_back('T');
            break;
        }
    }
    return s;
}

int main()
{
    // A -> 0(00), C -> 1(01), G -> 2(10), T -> 3(11)
    // Pack ACGT sequence into bitset and use as a key
    std::string input;
    std::cin >> input;
    // Use bloomfilter to only push duplicate strings into count map (may have some FP)
    BloomFilter duplicate_predicate = get_duplicate_predicate(input);

    std::unordered_map<std::bitset<2 * K>, int> count_map;
    std::bitset<2 * K> current_key;

    for (int i = 0; i < input.size(); i++)
    {
        current_key = update_key(current_key, input[i]);
        if (i >= K - 1)
        {
            if (duplicate_predicate.contains(current_key))
                count_map[current_key]++;
        }
    }

    for (const auto &[key, value] : count_map)
    {
        if (value > 1) // Filter out FP
            std::cout << bitset_to_ATCG(key) << ' ' << value << '\n';
    }
}