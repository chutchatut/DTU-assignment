#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <iostream>

#include "symbols.h"

// TODO increase this
#define MAX_DICT_CAPACITY 4

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
            fs::create_directories(basepath / prefix);
            std::ofstream *file_writer = new std::ofstream(basepath / prefix / "data");
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
    RadixCacheWriter(const fs::path &basepath, const fs::path &cache_path) : basepath(basepath), spilled_to_disk(false)
    {
        fs::path relation = fs::relative(basepath, cache_path);
        for (const char &c : relation.u8string())
            if (is_symbol_valid(c))
                prev_prefix.push_back(c);
    };

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

    bool flush()
    {
        // Close all files and print k-mer that's already counted

        for (auto &[_, file_writer] : prefix_to_writer)
        {
            file_writer->close();
            delete file_writer;
        }

        for (const auto &[k, v] : count_map)
            if (v > 1)
                std::cout << prev_prefix << k << ' ' << v << '\n';

        return spilled_to_disk;
    }
};