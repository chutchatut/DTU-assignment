#include <iostream>
#include <cassert>
#include <filesystem>
#include <boost/lexical_cast.hpp>
#include <stack>

#include "utils/fasta_reader.h"
#include "utils/cache_writer.h"
#include "utils/circular_buffer.h"

#define PREFIX_SIZE 4

bool stage_1(uint16_t k, const fs::path &fasta_path, const fs::path &cache_path)
{
    // return true if spilled to disk
    CircularBuffer buffer(k); // create buffer of length k
    RadixCacheWriter<PREFIX_SIZE> writer(cache_path, cache_path);

    auto read_fn = [&buffer, &writer](const char &c, const bool &is_ready)
    {
        buffer.insert(c);
        if (is_ready)
            writer.insert(buffer.get_string(), 1);
    };

    read_fasta(fasta_path, read_fn, k);

    return writer.flush();
}

void stage_2(uint16_t k, const fs::path &cache_path)
{
    std::stack<fs::path> directories_to_scan;
    auto scan_dir = [&directories_to_scan](const fs::path &path_to_scan)
    {
        for (auto const &dir_entry : std::filesystem::directory_iterator{path_to_scan})
            directories_to_scan.push(dir_entry);
    };

    scan_dir(cache_path);

    while (!directories_to_scan.empty())
    {
        fs::path new_basedir = directories_to_scan.top();
        directories_to_scan.pop();
        std::ifstream input_file(new_basedir / "data");

        RadixCacheWriter<PREFIX_SIZE> writer(new_basedir, cache_path);
        std::string s;
        uint16_t count;

        while (input_file >> s >> count)
            writer.insert(s, count);

        input_file.close();
        fs::remove(new_basedir / "data");

        if (writer.flush())
            scan_dir(new_basedir);
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

    if (stage_1(k, fasta_path, cache_path))
        stage_2(k, cache_path);

    // clean up
    // for (const auto &subfolder : std::filesystem::directory_iterator(cache_path))
    //     std::filesystem::remove_all(subfolder.path());

    return 0;
}