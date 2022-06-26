#include "ECS.hpp"

template <typename T>
bool Pool<T>::empty() const
{
    return data.empty();
};

template <typename T>
bool Pool<T>::size() const
{
    return data.size();
};

template <typename T>
void Pool<T>::clear()
{
    data.clear();
};

template <typename T>
void resize(std::uint8_t n)
{
    data.resize(n);
};

template <typename T>
void Pool<T>::add(T element)
{
    data.push_back(element);
};

template <typename T>
void Pool<T>::set(std::uint8_t, T element)
{
    data[index] = element;
};

template <typename T>
T &Pool<T>::get(std::uint8_t index) const
{
    return static_cast<T &>(data[index]);
};

template <typename T>
T &Pool<T>::operator[](std::uint8_t index) const
{
    return data[index];
};
