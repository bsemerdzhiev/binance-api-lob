#include "free-list-allocator.hpp"
#include "memory-pool.hpp"
#include <cstdint>
#include <map>

int32_t main() {
  MemoryPool pool;

  std::map<int32_t, int32_t, std::less<int32_t>,
           FreeListAllocator<std::pair<const int32_t, int32_t>>>
      mm{std::less<int32_t>{},
         FreeListAllocator<std::pair<const int32_t, int32_t>>{pool}};

  return 0;
}
