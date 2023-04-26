#include <filesystem>
#include <fstream>
#include <unordered_set>

std::unordered_set<char> symbols({'A', 'T', 'C', 'G'});

namespace fs = std::filesystem;
void read_fasta(const fs::path &file_path, const std::function<void(const char &, const bool &)> &insert_fn, uint16_t k)
{
    bool is_reading_dna = false;
    char ch;
    int i; // index counter
    std::fstream fin(file_path, std::fstream::in);
    while (fin >> std::noskipws >> ch)
    {
        if (ch == '>')
        {
            is_reading_dna = false;
            i = 0; // reset index counter
        }
        if (ch == '\n')
            is_reading_dna = true;
        ch = toupper(ch);
        if (is_reading_dna && (symbols.find(ch) != symbols.end()))
        {
            insert_fn(ch, i >= k - 1);
            ++i;
        }
    }
}