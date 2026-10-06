## Explanation of the solution strategy and key design choices

When handling an update, the first step is to get the order book that corresponds to a symbol. In this solution, a vector will be used to store the order books for all symbols, and a mapping from a symbol's string to an integer is done to get the corresponding index in it. 

Following the assumption that a symbol's book can contain from 1000 to 2000 different levels, different solutions exist for keeping these levels.
Initially, I checked the constraints on the price levels defined from the Binance API, including the tickSize, in hopes that the price levels would be dense. 
However, the docs state that the price levels range depends on the symbol, and the example it provides shows sparse ranges:
```
{
  "filterType": "PRICE_FILTER",
  "minPrice": "0.01000000",
  "maxPrice": "1000000.00000000",
  "tickSize": "0.01000000"
}
```
This led me to the conclusion that the ranges can be quite sparse. Therefore, a container like a vector that has great cache locality seems like a less desirable choice, given the memory size allocation needed for each symbol. In the Trade-Off section I have performed some computations showing why we cannot afford having sparse indices.

A map (implemented with a Red Black Tree in the STL), could be a good choice, even though it is implemented with pointers in the STL. The issue steming from the dynamic allocations during execution could be mitigated by using a custom memory pool (implemented with [free lists](https://en.wikipedia.org/wiki/Free_list)). A map would also provide an easy access to the best bid and best ask in complexity O(1). An alternative choice would be to use a Hash Table(unordered_map), but this would require the use of an additional data structure to retrieve the best bid and best ask (for example a Heap(priority_queue)) which then leads to other potential problems. 

Careful consideration needs to be done for converting the price level and the quantity which are given from Binance as strings, to integers. Using float/double as the key of a map is not recommended, due to potential representation errors. Therefore, we shall focus on converting the decimal price level to a fixed-point integer. Assuming that the tickSize (for the price level) and stepSize (for the quantity) are known beforehand, the current price and quantity can be converted to integers by moving the decimal point K positions to the right, where K stands for 10^{-K}=tickSize/stepSize respectively in both cases.

## Main data structures, algorithms, and modules
### The main data structures are:
1. Vector(dynamic array) for retrieving the order book for a symbol
2. Two separate maps(Red-Black Tree) for the bids and the asks for a given symbol
3. Free List for the memory pool allocations for the unordered_map and the map
4. An unordered_map that maps symbols to indices for the vector containing the order books

The unordered_map for the indices provides an O(1) average search, insertion and deletion complexity, and accessing the order book corresponding to a symbol with an already known index is O(1).

The map provides an O(log(n)) complexity in the worst case for random search, insertion and random deletion. Given an iterator on the other hand, it provides an
amortized O(1) deletion.

One drawback of the map is the need for dynamic memory allocations that happen during insertions. If the default allocator does not have enough memory at the moment, it will request additional from the kernel via `brk` or `mmap`. This leads to an incurred latency penalty. To mitigate this, we implement a custom memory pool allocator with free lists. This is a good choice given the current problem, since the objects we would be instantiating in the RBT are all of the same size. 

## Trade-offs and Assumptions made during the design process
### The key assumptions I made before implementations:

1. The price levels are sparse.
2. The symbols are known beforehand.
3. The price level and quantity will fit in a 8 byte number (uint64_t).
4. The tickSisze, minPrice, stepSize and minQuantity are provided to us for all symbols at initialization.
5. The tickSize and stepSize are assumed to be in the form 0.{00...00}1 = 10^-K

### Trade-offs

A trade off I am making is choosing worse locality in favor of being able to handle sparse ranges. A contiguous container such as a vector would have been a 
great choice for storing the different levels in an order book, however the sparse range for the price levels make this much less desirable, given that our program
should be able to have ~1000-2000 **different** symbols. Following the example provided by the Binance API above, the LOB for a symbol might have at least $\frac{10^6}{10^{-2}}=10^8$ levels, which turns out to be $10^8$ indices. Since a price level requires 8 bytes for storing the quantity, the whole book for a symbol needs at least $8 \cdot 10^8$ bytes -> 0.8GB in total. Multiplying this by the total amount of different symbols, we get $2000 \cdot 0.8GB = 1.6TB$ in total, which is physically infeasible for a computer having average RAM.

Another alternative solution for storing the prices on a level, could have been to store the price level and corresponding volume as a pair, and insert it into a vector that stores all the <price, volume> pairs for this symbol. This corresponds to a O(n) worst case insertion, look-up, and deletion, but has great cache locality and uses O(P) memory, where P stands for the amount of levels for this symbol. A small improvement is to keep the pairs sorted by key, resulting in O(log(n)) look up, and O(n) insertion and deletion.
