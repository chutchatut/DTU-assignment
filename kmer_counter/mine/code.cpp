// Note: this only works for K <= 32 due to the choice of hash function
#include <iostream>
#include <bitset>
#include <unordered_map>
#include <string>
#include <vector>
#include <thread>

#include "bloom_filter.h"
#include "thread_message.h"
#include "kmer_utils.h"

#include <boost/lockfree/spsc_queue.hpp>

#define K 16
#define NUM_THREAD 16

typedef std::bitset<2 * K> kmer_key_t;
typedef std::unordered_map<kmer_key_t, int> count_map_t;
typedef boost::lockfree::spsc_queue<ThreadMessage<kmer_key_t>, boost::lockfree::capacity<1024>> message_queue_t;

void thread_fn(count_map_t &count_map, message_queue_t &message_queue, size_t expected_size)
{
    BloomFilter first = BloomFilter<kmer_key_t>(expected_size, 0.1);
    BloomFilter dup = BloomFilter<kmer_key_t>(expected_size, 0.1);
    ThreadMessage<kmer_key_t> thread_message{terminate, std::nullopt};
    kmer_key_t kmer_key;
    while (1)
    {
        if (!message_queue.pop(thread_message))
            continue;

        switch (thread_message.message_type)
        {
        case terminate:
            for (const auto &[key, value] : count_map)
                if (value < 2)
                    count_map.erase(key);
            return;
        case populate_filter:
            kmer_key = thread_message.value();
            if (first.contains(kmer_key))
                dup.add(kmer_key);
            first.add(kmer_key);
            break;
        case populate_count:
            kmer_key = thread_message.value();
            if (dup.contains(kmer_key))
                count_map[kmer_key]++;
            break;
        }
    }
}

int main()
{
    // A -> 0(00), C -> 1(01), G -> 2(10), T -> 3(11)
    // Pack ACGT sequence into bitset and use as a key
    std::string input;
    std::cin >> input;

    count_map_t count_maps[NUM_THREAD];
    message_queue_t message_queues[NUM_THREAD];
    std::thread threads[NUM_THREAD];

    for (int i = 0; i < NUM_THREAD; ++i)
        threads[i] = std::thread(thread_fn, std::ref(count_maps[i]), std::ref(message_queues[i]), input.size() / NUM_THREAD);

    kmer_key_t current_key;
    for (int i = 0; i < input.size(); i++)
    {
        update_key(current_key, input[i]);
        if (i >= K - 1)
        {
            int thread = current_key.to_ullong() % NUM_THREAD;
            while (!message_queues[thread].push(ThreadMessage(populate_filter, std::make_optional(current_key))))
                ;
        }
    }

    for (int i = 0; i < input.size(); i++)
    {
        update_key(current_key, input[i]);
        if (i >= K - 1)
        {
            int thread = current_key.to_ullong() % NUM_THREAD;
            while (!message_queues[thread].push(ThreadMessage(populate_count, std::make_optional(current_key))))
                ;
        }
    }

    for (auto &message_queue : message_queues)
        while (!message_queue.push(ThreadMessage<kmer_key_t>(terminate, std::nullopt)))
            ;

    for (auto &thread : threads)
        thread.join();

    for (const auto &count_map : count_maps)
        for (const auto &[key, value] : count_map)
            std::cout << bitset_to_ATCG(key) << ' ' << value << '\n';
}