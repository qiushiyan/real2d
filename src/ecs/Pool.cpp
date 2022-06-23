#include "ECS.hpp"

template <typename T>
class Pool
{
private:
    std::vector<T> data;

public:
    Pool(std::uint8_t n = 100)
    {
        resize(n)
    }
    ~Pool()
    {
        data.clear();
    }

    bool empty() const
    {
        return data.empty();
    };
    bool size() const
    {
        return data.size();
    };
    void clear()
    {
        data.clear();
    };
    void resize(std::uint8_t n)
    {
        data.resize(n);
    };
    void add(T element)
    {
        data.push_back(element);
    };
    void set(std::uint8_t, T element)
    {
        data[index] = element;
    };
    T &get(std::uint8_t index) const
    {
        return static_cast<T &>(data[index]);
    };
    T &operator[](std::uint8_t index) const {
        return data[index]};
};