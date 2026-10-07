#include "memory-pool.hpp"

MemoryPool::MemoryPool(std::size_t arena_size)
    : arena_size_(arena_size), buffer_(nullptr) {}

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

void MemoryPool::insert_back(void *ptr) {
  head_ = std::construct_at(static_cast<FreeListNode *>(ptr), head_);
}

MemoryPool::~MemoryPool() {
  if (buffer_ != nullptr) {
    ::operator delete(buffer_, align_val_);
    buffer_ = nullptr;
  }
}
