#include <iostream>
#include <cassert>
#include <filesystem>
#include <boost/lexical_cast.hpp>

#include "utils/fasta_reader.h"

void test_insert(const char &c, const bool &is_ready)
{
    std::cout << c << ' ' << is_ready << '\n';
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

    read_fasta(fasta_path, test_insert, k);
}