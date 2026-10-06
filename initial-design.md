## Explanation of the solution strategy and key design choices

When handling an update, the first step is to get the order book that corresponds to a symbol. This can be done in average complexity of O(1) using an unordered map. Once the right order book has been retrieved, an update to the right level follows. 

NOTE: If all the symbols were known beforehand, an alternative solution could have been create a mapping string -> uint16_t, and then use that uint16_t as an index in a vector, that stores the order book corresponding to the symbol. Assuming we dont have this info beforehand, this solution is therefore not considered.

Following the assumption that a symbol's book can contain from 1000 to 2000 different levels, different solutions exist for keeping these levels.
Initially, I checked the constraints on the price levels defined from the binance API, including the tickSize, in hopes that the price levels would be dense. 
However, the docs state that the price levels range depends on the symbol, and the example it provides:
`
{
  "filterType": "PRICE_FILTER",
  "minPrice": "0.01000000",
  "maxPrice": "1000000.00000000",
  "tickSize": "0.01000000"
}
`
This led me to the conclusion that the ranges can be quite sparse. Therefore, a container like a vector that has great cache locality seems like a less desirable choice, given the memory size allocation needed for each symbol. 

A map (implemented with a Red Black Tree in the STL), could be a good choice, even though it is implemented with pointers in the STL. The issue steming from the dynamic allocations during execution could be mitigated by using a custom memory pool (implemented with [free lists](https://en.wikipedia.org/wiki/Free_list)). A map would also provide an easy access to the best bid and best ask in complexity O(1). An alternative choice would be to use a Hash Table(unordered_map), but this would require the use of an additional data structure to retrieve the best bid and best ask (for example a Heap(priority_queue)) which then leads to other potential problems. 

## Main data structures, algorithms, and modules
### The main data structures are:
unordered_map(Hash Table) for retrieving the order book for a symbol
Two separate maps(Red-Black Tree) for the bids and the asks for a given symbol
Free List for the memory pool allocations for the unordered_map and the map

The unordered_map provides an O(1) average search, insertion and deletion complexity.
The map provides an O(log(n)) complexity in the worst case for random search, insertion and random deletion. Given an iterator on the other hand, it provides an
amortized O(1) deletion.

The main drawbacks of both of these structures are the dynamic memory allocations that happen during insertions. If the default allocator does not have enough memory at the moment, it will request additional from the kernel via brk or mmap. This leads to an incurred latency penalty. To mitigate this, we implement a custom memory pool allocator with free lists. This is a good choice given the current problem, since the objects we would be instantiating in the Hash Table and the RBT are all of the same size in their corresponding container. 

## Trade-offs and Assumptions made during the design process

To state two assumptions I directly made before implementations:
1. The price levels are sparse and uniform across different symbols.
2. The symbols are not known beforehand.
3. The price level will fit in a 4 byte number.
4. Floats are to be used, since conversion to an integer requires knowing the minPrice, maxPrice and tick size for a symbol.

A trade off I am making is choosing worse locality in favor of being able to handle sparse ranges. A contiguous container such as a vector would have been a 
great choice for storing the different levels in an order book, however the sparse range for the price levels make this much less desirable, given that our program
should be able to have ~1000-2000 DIFFERENT symbols. Following the example provided by the binance API above, the LOB for a symbol might have 10^6/10^{-2} levels, which turns out to be 10^8 indeces. Since each level corresponds to a float (requiring 4 bytes for storage), the whole book needs at least 4*10^8 bytes -> 0.4GB in total for a single symbol. Multiplying this by the total amount of different symbols, we get 2000*0.4GB = 800GB in total, which is physically infeasible for the average computer.

Another alternative solution for storing the prices on a level, could have been to store the price level and corresponding volume as a pair, and insert it into a vector that stores all the <price, volume> pairs for this symbol. This corresponds to a O(n) worst case insertion, look-up, and deletion, but has great cache locality and uses O(P) memory, where P stands for the amount of levels for this symbol. A small improvement is to keep the pairs sorted by key, resulting in O(log(n)) look up, and O(n) insertion and deletion.
