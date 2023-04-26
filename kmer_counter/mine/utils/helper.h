#include <vector>
#include <stack>

template <typename T>
void vector_copy_into_stack(const std::vector<T> &vec, std::stack<T> &stack)
{
    for (const T &t : vec)
        stack.push(t);
}