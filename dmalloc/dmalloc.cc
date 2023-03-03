#define DMALLOC_DISABLE 1
#include "dmalloc.hh"
#include <cassert>
#include <cstring>

struct dmalloc_stats g_stats = {0, 0, 0, 0, 0, 0, UINT64_MAX, 0};

void update_invalid(void** ptr, size_t size) {
    g_stats.nfail += 1;
    g_stats.fail_size += (size);
    *ptr = nullptr;
    // return *ptr;
}


/**
 * dmalloc(sz,file,line)
 *      malloc() wrapper. Dynamically allocate the requested amount `sz` of memory and 
 *      return a pointer to it 
 * 
 * @arg size_t sz : the amount of memory requested 
 * @arg const char *file : a string containing the filename from which dmalloc was called 
 * @arg long line : the line number from which dmalloc was called 
 * 
 * @return a pointer to the heap where the memory was reserved
 */
void* dmalloc(size_t sz, const char* file, long line) {
    (void) file, (void) line;   // avoid uninitialized variable warnings
    // Your code here.
    void* void_ptr;
    if ((sizeof(md) + sz) <= sz) {
        update_invalid(&void_ptr, sz);
        // void_ptr = nullptr;
        // g_stats.nfail += 1;
        // g_stats.fail_size += sz;
        return void_ptr;
    }
    else {
        void_ptr = base_malloc(sizeof(md) + sz);
        if (void_ptr) {
            md* md_ptr = (md*) void_ptr;
            md_ptr->size = sz;
            void* payload_ptr = &*(md_ptr + 1);
            
            g_stats.nactive++;
            g_stats.active_size += sz;
            g_stats.total_size += sz;
            g_stats.ntotal++;

            if ((unsigned long)void_ptr < g_stats.heap_min) {
                g_stats.heap_min = (long)void_ptr;
            }
            if ((long)void_ptr + sizeof(md) + sz > g_stats.heap_max) {
                g_stats.heap_max = (long)void_ptr + sizeof(md) + sz;
            }
            return payload_ptr;
        }
        else {
            g_stats.nfail += 1;
            g_stats.fail_size += sz;
            return void_ptr;
        }
    }
}

/**
 * dfree(ptr, file, line)
 *      free() wrapper. Release the block of heap memory pointed to by `ptr`. This should 
 *      be a pointer that was previously allocated on the heap. If `ptr` is a nullptr do nothing. 
 * 
 * @arg void *ptr : a pointer to the heap 
 * @arg const char *file : a string containing the filename from which dfree was called 
 * @arg long line : the line number from which dfree was called 
 */
void dfree(void* ptr, const char* file, long line) {
    (void) file, (void) line;   // avoid uninitialized variable warnings
    // Your code here.
    if (ptr) {
        md* payload_ptr = (md*)ptr;
        md* md_ptr = &*(payload_ptr - 1);
        size_t sz = md_ptr->size;
        g_stats.nactive--;
        g_stats.active_size -= sz;
        base_free(ptr);
        base_free(md_ptr);
    }
}

/**
 * dcalloc(nmemb, sz, file, line)
 *      calloc() wrapper. Dynamically allocate enough memory to store an array of `nmemb` 
 *      number of elements with wach element being `sz` bytes. The memory should be initialized 
 *      to zero  
 * 
 * @arg size_t nmemb : the number of items that space is requested for
 * @arg size_t sz : the size in bytes of the items that space is requested for
 * @arg const char *file : a string containing the filename from which dcalloc was called 
 * @arg long line : the line number from which dcalloc was called 
 * 
 * @return a pointer to the heap where the memory was reserved
 */
void* dcalloc(size_t nmemb, size_t sz, const char* file, long line) {
    // Your code here (to fix test014).
    void* ptr;
    size_t i;
    if ((nmemb * sz) <= sz) {
        update_invalid(&ptr, (nmemb * sz));
        // g_stats.nfail += 1;
        // g_stats.fail_size += (nmemb * sz);
        // ptr = nullptr;
        return ptr;
    }
    for (i = 1; i <= nmemb; i++) {
        if ((sz * i) < sz) {
            update_invalid(&ptr, (nmemb * sz));
            // g_stats.nfail += 1;
            // g_stats.fail_size += (nmemb * sz);
            // ptr = nullptr;
            return ptr;
        }
    }
    ptr = dmalloc(nmemb * sz, file, line);
    if (ptr) {
        memset(ptr, 0, nmemb * sz);
    }
    return ptr;
}

/**
 * get_statistics(stats)
 *      fill a dmalloc_stats pointer with the current memory statistics  
 * 
 * @arg dmalloc_stats *stats : a pointer to the the dmalloc_stats struct we want to fill
 */
void get_statistics(dmalloc_stats* stats) {
    // Stub: set all statistics to enormous numbers
    memset(stats, 255, sizeof(dmalloc_stats));
    stats->nactive = g_stats.nactive;
    stats->active_size = g_stats.active_size;
    stats->ntotal = g_stats.ntotal;
    stats->total_size = g_stats.total_size;
    stats->nfail = g_stats.nfail;
    stats->fail_size = g_stats.fail_size;
    stats->heap_min = g_stats.heap_min;
    stats->heap_max = g_stats.heap_max;

    // Your code here.
}

/**
 * print_statistics()
 *      print the current memory statistics to stdout       
 */
void print_statistics() {
    dmalloc_stats stats;
    get_statistics(&stats);

    printf("alloc count: active %10llu   total %10llu   fail %10llu\n",
           stats.nactive, stats.ntotal, stats.nfail);
    printf("alloc size:  active %10llu   total %10llu   fail %10llu\n",
           stats.active_size, stats.total_size, stats.fail_size);
}

/**  
 * print_leak_report()
 *      Print a report of all currently-active allocated blocks of dynamic
 *      memory.
 */
void print_leak_report() {
    // Your code here.
}
