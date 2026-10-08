#pragma once

#include "memory-pool.hpp"
#include <cstddef>
template <typename T> class FreeListAllocator {
public:
  using value_type = T;

  explicit FreeListAllocator(MemoryPool &pool) noexcept;

  template <typename U>
  constexpr FreeListAllocator(const FreeListAllocator<U> &rhs) noexcept;

  T *allocate(std::size_t n) noexcept;

  void deallocate(T *ptr, std::size_t n) noexcept;

  template <typename U>
  bool operator==(const FreeListAllocator<U> &rhs) const noexcept;

private:
  MemoryPool *memory_pool_;

  template <typename> friend class FreeListAllocator;
};

#include "free-list-allocator.tpp"
