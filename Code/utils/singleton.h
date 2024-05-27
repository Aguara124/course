#ifndef SINGLETON_H
#define SINGLETON_H

#include <type_traits>

template <typename T, typename D = T>
class Singleton
{
    friend D;
    //static_assert(is_base_of_v<T, D>, "T should be a base type for D");

public:
    static T& instance();
    static T* ptrInstance();

private:
    Singleton() = default;
    ~Singleton() = default;
    Singleton( const Singleton& ) = delete;
    Singleton& operator=( const Singleton& ) = delete;
};

template <typename T, typename D>
T& Singleton<T, D>::instance()
{
    static D inst;
    return inst;
}

template<typename T, typename D>
T* Singleton<T, D>::ptrInstance()
{
    return &instance();
}

#endif // SINGLETON_H
