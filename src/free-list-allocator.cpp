#include "free-list-allocator.hpp"
#include <cassert>

template <typename T> T *FreeListAllocator<T>::allocate(std::size_t n) {
  assert(n == 1);

  return memory_pool_->reserve<T>();
}

template <typename T>
template <typename U>
constexpr FreeListAllocator<T>::FreeListAllocator(
    const FreeListAllocator<U> &rhs) noexcept {
  memory_pool_ = rhs.memory_pool_;
}

template <typename T>
void FreeListAllocator<T>::deallocate(T *ptr, std::size_t n) {
  assert(n == 1);

  return memory_pool_->insert_back(ptr);
}
