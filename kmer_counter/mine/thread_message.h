#include <optional>

enum MessageType
{
    populate_filter,
    populate_count,
    terminate
};

template <typename T>
struct ThreadMessage
{
    MessageType message_type;
    std::optional<T> kmer_key;

    ThreadMessage(MessageType message_type,
                  std::optional<T> kmer_key) : message_type(message_type), kmer_key(kmer_key)
    {
    }

    T value()
    {
        return kmer_key.value();
    }
};