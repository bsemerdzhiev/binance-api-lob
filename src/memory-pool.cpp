#include "memory-pool.hpp"
#include <algorithm>
#include <cstddef>
#include <memory>
#include <new>

MemoryPool::MemoryPool() { buffer_ = nullptr; }

template <typename T> void MemoryPool::create_tickets() {
  constexpr std::size_t block_size = std::max(sizeof(T), sizeof(FreeListNode));

  for (int32_t i{0}; i < ARENA_SIZE; i++) {
    void *slot = buffer_ + i * block_size;

    head_ = std::construct_at(static_cast<FreeListNode *>(slot), head_);
  }
}

void MemoryPool::destroy_tickets() {
  while (head_ != nullptr) {
    FreeListNode *nxt = head_->nxt;

    std::destroy_at(static_cast<FreeListNode *>(head_));
    head_ = nxt;
  }
}

void *MemoryPool::pop_ticket() {
  if (head_ == nullptr) {
    throw std::bad_alloc();
  }
  void *alloc_location = head_;
  head_ = head_->nxt;

  return alloc_location;
}

template <typename T> void *MemoryPool::reserve() {
  if (buffer_ == nullptr) {
    constexpr std::size_t block_size =
        std::max(sizeof(FreeListNode), sizeof(T));

    constexpr std::size_t alignment =
        std::max(alignof(FreeListNode), alignof(T));

    align_val_ = std::align_val_t{alignment};

    buffer_ = static_cast<std::byte *>(
        ::operator new(block_size * ARENA_SIZE, align_val_));

    head_ = nullptr;

    create_tickets<T>();
  }

  return pop_ticket();
}

MemoryPool::~MemoryPool() {
  if (buffer_ != nullptr) {
    ::operator delete(buffer_, align_val_);
    buffer_ = nullptr;
  }
}
