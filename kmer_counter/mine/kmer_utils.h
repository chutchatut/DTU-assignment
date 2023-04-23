#include <string>

template <typename kmer_key_t>
void update_key(kmer_key_t &current_key, char next_symbol)
{
    current_key = current_key << 2;
    // Switch should be a bit faster than if-else
    switch (next_symbol)
    {
    case 'A':
        current_key |= 0;
        break;
    case 'C':
        current_key |= 1;
        break;
    case 'G':
        current_key |= 2;
        break;
    case 'T':
        current_key |= 3;
        break;
    default:
        throw std::invalid_argument("Invalid input symbol, only allow symbol are A, C, G, T");
    }
}

template <typename kmer_key_t>
std::string bitset_to_ATCG(const kmer_key_t &key)
{
    std::string s;
    for (int i = key.size() - 1; i > 0; i -= 2)
    {
        switch (2 * key[i] + key[i - 1])
        {
        case 0:
            s.push_back('A');
            break;
        case 1:
            s.push_back('C');
            break;
        case 2:
            s.push_back('G');
            break;
        case 3:
            s.push_back('T');
            break;
        }
    }
    return s;
}