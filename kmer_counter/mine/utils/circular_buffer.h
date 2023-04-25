#include <string>

template <size_t k>
class CircularBuffer
{
private:
    char data[k];
    usize_t write_head;

public:
    CircularBuffer() : write_head(0){};

    void insert(char c)
    {
        data[write_head] = c;
        write_head = (write_head + 1) % k;
    }

    std::string get_string()
    {
        std::string s;
        for (int16_t i = 0; i < k; ++i)
            s.push_back(data[(write_head + i) % k]);
        return s;
    }
};
