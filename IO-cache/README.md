# Project 3: Caching I/O

<!-- TODO: Fill this out. -->

## Design Overview:

I defined the following metadata to implement my cache:

int cache_min; --> index of beginning of cache within the file, starting from index 0 at beginning of file.

int file_offset; --> index of file's read/write head within the file, starting from index 0 at beginning of file.

int cache_offset; --> index of file's read/write head within the cache, starting from index 0 at beginning of cache.

int unflushed_write_calls; --> number of writes that have been performed on the current cache since the last flush. Used for optimizing flush.

int cache_initialized; --> 0 if it's the first time using the cache, 1 if it's not the first time.

int cache_amt; --> number of values currently of importance in the cache. Sometimes the entire cache isn't used, so it's necessary when flushing to only flush what is needed and not leftovers from previous cache uses.

int file_size; --> keeps track of current file size, more optimal than system calling filesize(f) every time it's needed.
