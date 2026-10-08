#include "free-list-allocator.hpp"
#include <cassert>
#include <new>

template <typename T>
FreeListAllocator<T>::FreeListAllocator(MemoryPool &pool) noexcept
    : memory_pool_(&pool) {}

template <typename T>
T *FreeListAllocator<T>::allocate(std::size_t n) noexcept {
  if (n != 1) {
    throw std::bad_alloc();
  }

  return memory_pool_->reserve<T>();
}

template <typename T>
template <typename U>
constexpr FreeListAllocator<T>::FreeListAllocator(
    const FreeListAllocator<U> &rhs) noexcept {
  memory_pool_ = rhs.memory_pool_;
}

template <typename T>
void FreeListAllocator<T>::deallocate(T *ptr, std::size_t n) noexcept {
  if (n != 1) {
    throw std::bad_alloc();
  }

  return memory_pool_->insert_back(ptr);
}

template <typename T>
template <typename U>
bool FreeListAllocator<T>::operator==(
    const FreeListAllocator<U> &rhs) const noexcept {
  return memory_pool_ == rhs.memory_pool_;
}
