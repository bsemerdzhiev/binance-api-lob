#include "memory-pool.hpp"
#include <algorithm>
#include <cstddef>
#include <memory>
#include <new>

template <typename T> void MemoryPool::create_tickets() {
  constexpr std::size_t max_size = std::max(sizeof(T), sizeof(FreeListNode));

  constexpr std::size_t max_align = std::max(alignof(T), alignof(FreeListNode));

  // block_size must be a multiple of max_align, therefore we need to round up
  constexpr std::size_t block_size =
      (max_size + max_align - 1) / max_align * max_align;

  for (int32_t i{0}; i < arena_size_; i++) {
    void *slot = buffer_ + i * block_size;

    head_ = std::construct_at(static_cast<FreeListNode *>(slot), head_);
  }
}

template <typename T> T *MemoryPool::reserve() {
  if (buffer_ == nullptr) {
    constexpr std::size_t max_size = std::max(sizeof(T), sizeof(FreeListNode));

    constexpr std::size_t max_align =
        std::max(alignof(T), alignof(FreeListNode));

    // block_size must be a multiple of max_align, therefore we need to round up
    constexpr std::size_t block_size =
        (max_size + max_align - 1) / max_align * max_align;

    align_val_ = std::align_val_t{max_align};

    buffer_ = static_cast<std::byte *>(
        ::operator new(block_size * arena_size_, align_val_));

    head_ = nullptr;

    create_tickets<T>();
  }

  void *new_address = pop_ticket();

  std::destroy_at(static_cast<FreeListNode *>(new_address));

  return static_cast<T *>(new_address);
}
