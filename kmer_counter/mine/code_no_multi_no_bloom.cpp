// Note: this only works for K <= 32 due to the choice of hash function
#include <iostream>
#include <bitset>
#include <unordered_map>
#include <string>

#include "kmer_utils.h"

#include <boost/lockfree/spsc_queue.hpp>

#define K 16

typedef std::bitset<2 * K> kmer_key_t;
typedef std::unordered_map<kmer_key_t, int> count_map_t;

int main()
{
    // A -> 0(00), C -> 1(01), G -> 2(10), T -> 3(11)
    // Pack ACGT sequence into bitset and use as a key
    std::string input;
    std::cin >> input;

    count_map_t count_map;

    kmer_key_t current_key;
    for (int i = 0; i < input.size(); i++)
    {
        update_key(current_key, input[i]);
        if (i >= K - 1)
            count_map[current_key]++;
    }

    for (const auto &[key, value] : count_map)
        if (value > 1)
            std::cout << bitset_to_ATCG(key) << ' ' << value << '\n';
}