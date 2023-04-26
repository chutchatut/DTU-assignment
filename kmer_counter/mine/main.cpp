#include <iostream>
#include <cassert>
#include <filesystem>
#include <boost/lexical_cast.hpp>
#include <stack>

#include "utils/fasta_reader.h"
#include "utils/cache_writer.h"
#include "utils/circular_buffer.h"
#include "utils/helper.h"

#define PREFIX_SIZE 3

std::vector<std::string> stage_1(uint16_t k, const fs::path &fasta_path, const fs::path &cache_path)
{
    // return true if spilled to disk
    CircularBuffer buffer(k); // create buffer of length k
    RadixCacheWriter<PREFIX_SIZE> writer(cache_path, "");

    auto read_fn = [&buffer, &writer](const char &c, const bool &is_ready)
    {
        buffer.insert(c);
        if (is_ready)
            writer.insert(buffer.get_string(), 1);
    };

    read_fasta(fasta_path, read_fn, k);

    return writer.flush();
}

void stage_2(uint16_t k, const fs::path &cache_path, std::vector<std::string> &cached_prefixes)
{
    std::stack<std::string> prefixes_stack;
    vector_copy_into_stack(cached_prefixes, prefixes_stack);

    while (!prefixes_stack.empty())
    {
        std::string new_prefix = prefixes_stack.top();
        prefixes_stack.pop();
        std::ifstream input_file(cache_path / new_prefix);

        RadixCacheWriter<PREFIX_SIZE> writer(cache_path, new_prefix);
        std::string s;
        uint16_t count;

        while (input_file >> s >> count)
            writer.insert(s, count);

        input_file.close();
        fs::remove(cache_path / new_prefix);

        vector_copy_into_stack(writer.flush(), prefixes_stack);
    }
}

namespace fs = std::filesystem;
int main(int argc, char *argv[])
{
    // input is k, fasta_path, cache_path
    assert(argc == 4);

    // parse input
    uint16_t k = boost::lexical_cast<uint16_t>(argv[1]);
    fs::path fasta_path(argv[2]);
    fs::path cache_path(argv[3]);

    std::vector<std::string> cached_prefixes = stage_1(k, fasta_path, cache_path);
    if (cached_prefixes.size() > 0)
        stage_2(k, cache_path, cached_prefixes);

    return 0;
}