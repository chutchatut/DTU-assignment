#include <filesystem>
#include <fstream>

#include "symbols.h"

namespace fs = std::filesystem;
void read_fasta(const fs::path &file_path, const std::function<void(const char &, const bool &)> &insert_fn, uint16_t k)
{
    bool is_reading_dna = false;
    char c;
    int i; // index counter
    std::fstream fin(file_path, std::fstream::in);
    while (fin >> std::noskipws >> c)
    {
        if (c == '>')
        {
            is_reading_dna = false;
            i = 0; // reset index counter
        }
        if (c == '\n')
            is_reading_dna = true;
        c = toupper(c);
        if (is_reading_dna && is_symbol_valid(c))
        {
            insert_fn(c, i >= k - 1);
            ++i;
        }
    }
}