#pragma once

#include <cstddef>
template <typename T> class FreeListAllocator {
public:
  using value_type = T;

  FreeListAllocator() noexcept = default;

  template <typename U>
  constexpr FreeListAllocator(const FreeListAllocator<U> &rhs) noexcept;

  T *allocate(std::size_t n);

  void deallocate(T *ptr, std::size_t n);

private:
};
