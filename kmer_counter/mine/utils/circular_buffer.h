#include <string>
#include <vector>

class CircularBuffer
{
private:
    uint16_t k;
    std::vector<char> data;
    size_t write_head;

public:
    CircularBuffer(uint16_t k) : write_head(0), k(k)
    {
        data.resize(k);
    };

    void insert(char c)
    {
        data[write_head] = c;
        write_head = (write_head + 1) % k;
    }

    std::string get_string()
    {
        std::string s;
        s.reserve(k);
        for (int16_t i = 0; i < k; ++i)
            s.push_back(data[(write_head + i) % k]);
        return s;
    }
};
