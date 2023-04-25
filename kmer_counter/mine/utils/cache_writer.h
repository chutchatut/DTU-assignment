#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <iostream>

#define MAX_DICT_CAPACITY 3

namespace fs = std::filesystem;
template <size_t prefix_length>
class RadixCacheWriter
{
private:
    fs::path basepath;
    bool spilled_to_disk;
    std::unordered_map<std::string, uint16_t> count_map;
    std::unordered_map<std::string, std::ofstream> prefix_to_writer;

    std::ofstream get_writer(const std::string &prefix)
    {
        if (prefix_to_writer.find(prefix) == prefix_to_writer.end())
        {
            std::ofstream file_writer(prefix);
            prefix_to_writer[prefix] = file_writer;
        }
        return prefix_to_writer[prefix];
    }

    void write_to_disk(const std::string &s, uint16_t count)
    {
        spilled_to_disk = true;
        std::string prefix = s.substr(0, prefix_length);
        std::ofstream file_writer = get_writer(prefix);
        file_writer << s << ' ' << count << '\n';
    }

public:
    RadixCacheWriter(fs::path basepath) : basepath(basepath), spilled_to_disk(false){};

    void insert(const std::string &s, uint16_t count)
    {
        if ((count_map.size() < MAX_DICT_CAPACITY)    // dict is not full
            || (count_map.find(s) != count_map.end()) // this string is already contained in dict
            || (s.size() <= prefix_length))           // string size is shorter than prefix_length
            ++count_map[s];
        else
            write_to_disk(s, count);
    }

    ~RadixCacheWriter()
    {
        if (spilled_to_disk) // flush to disk if already spilled
            for (const auto &[k, v] : count_map)
                write_to_disk(k, v);
        else // if not print to stdout
            for (const auto &[k, v] : count_map)
                std::cout << k << ' ' << v << '\n';
    }
};