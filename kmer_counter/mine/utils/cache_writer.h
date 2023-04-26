#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <iostream>

#include "symbols.h"

#define MAX_DICT_CAPACITY 10000

namespace fs = std::filesystem;
template <size_t prefix_length>
class RadixCacheWriter
{
private:
    std::string prev_prefix;
    fs::path basepath;
    bool spilled_to_disk;
    std::unordered_map<std::string, uint16_t> count_map;
    std::unordered_map<std::string, std::ofstream *> prefix_to_writer;

    std::ofstream *get_or_create_writer(const std::string &prefix)
    {
        if (prefix_to_writer.find(prefix) == prefix_to_writer.end())
        {
            std::ofstream *file_writer = new std::ofstream(basepath / (prev_prefix + prefix));
            prefix_to_writer[prefix] = file_writer;
        }
        return prefix_to_writer[prefix];
    }

    void write_to_disk(const std::string &s, uint16_t count)
    {
        spilled_to_disk = true;
        std::string prefix = s.substr(0, prefix_length);
        std::string suffix = s.substr(prefix_length);
        std::ofstream *file_writer = get_or_create_writer(prefix);
        *file_writer << suffix << ' ' << count << '\n';
    }

public:
    RadixCacheWriter(const fs::path &basepath, const std::string &prev_prefix) : basepath(basepath), prev_prefix(prev_prefix), spilled_to_disk(false){};

    void insert(const std::string &s, uint16_t count)
    {
        if ((count_map.size() < MAX_DICT_CAPACITY)    // Dict is not full
            || (count_map.find(s) != count_map.end()) // This string is already contained in dict
            || (s.size() <= prefix_length))           // String size is shorter than prefix_length
            // Cap max count at UINT16_MAX
            count_map[s] = (uint16_t)std::min((uint32_t)count_map[s] + (uint32_t)count, (uint32_t)UINT16_MAX);
        else
            write_to_disk(s, count);
    }

    std::vector<std::string> flush()
    {
        // Close all files and print k-mer that's already counted

        std::vector<std::string> prefixes;
        for (auto &[prefix, file_writer] : prefix_to_writer)
        {
            file_writer->close();
            delete file_writer;
            prefixes.push_back(prev_prefix + prefix);
        }

        for (const auto &[k, v] : count_map)
            if (v > 1)
                std::cout << prev_prefix << k << ' ' << v << '\n';

        return prefixes;
    }
};
