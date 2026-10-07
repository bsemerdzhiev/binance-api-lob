#pragma once

#include <cstddef>
#include <cstdint>
#include <new>

struct FreeListNode {
  FreeListNode *nxt;
  FreeListNode(FreeListNode *nxt_) : nxt(nxt_) {}
};

class MemoryPool {
public:
  static constexpr int32_t ARENA_SIZE = 4096;

  MemoryPool();

  template <typename T> T *reserve();
  void insert_back(void *ptr);

  ~MemoryPool();

private:
  template <typename T> void create_tickets();
  void destroy_tickets();

  void *pop_ticket();

  FreeListNode *head_;

  std::byte *buffer_;
  std::size_t buffer_size_;
  std::align_val_t align_val_;
};

#include "memory-pool.tpp"
