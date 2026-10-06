#include "free-list-allocator.hpp"

template <typename T> T *FreeListAllocator<T>::allocate(std::size_t n) {}

template <typename T>
template <typename U>
constexpr FreeListAllocator<T>::FreeListAllocator(
    const FreeListAllocator<U> &rhs) noexcept {}

template <typename T>
void FreeListAllocator<T>::deallocate(T *ptr, std::size_t n) {}
