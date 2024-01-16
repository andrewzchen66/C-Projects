#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <stdarg.h>

#include "../io300.h"



/*
    student.c
    Fill in the following stencils
*/

/*
    When starting, you might want to change this for testing on small files.
*/
#ifndef CACHE_SIZE
#define CACHE_SIZE 4096
#endif

#if(CACHE_SIZE < 4)
#error "internal cache size should not be below 4."
#error "if you changed this during testing, that is fine."
#error "when handing in, make sure it is reset to the provided value"
#error "if this is not done, the autograder will not run"
#endif

/*
   This macro enables/disables the dbg() function. Use it to silence your
   debugging info.
   Use the dbg() function instead of printf debugging if you don't want to
   hunt down 30 printfs when you want to hand in
*/
#define DEBUG_PRINT 0
#define DEBUG_STATISTICS 1

struct io300_file {
    /* read,write,seek all take a file descriptor as a parameter */
    int fd;
    /* this will serve as our cache */
    char *cache;
    // TODO: Your properties go here
    int cache_min;
    int file_offset;
    int cache_offset;
    int unflushed_write_calls;
    int cache_initialized;
    int cache_amt;
    int file_size;

    /* Used for debugging, keep track of which io300_file is which */
    char *description;
    /* To tell if we are getting the performance we are expecting */
    struct io300_statistics {
        int read_calls;
        int write_calls;
        int seeks;
    } stats;
};

/*
    Assert the properties that you would like your file to have at all times.
    Call this function frequently (like at the beginning of each function) to
    catch logical errors early on in development.
*/
static void check_invariants(struct io300_file *f) {
    assert(f != NULL);
    assert(f->cache != NULL);
    assert(f->fd >= 0);

    // TODO: Add more invariants
    assert(f->cache_min >= 0);
    assert(f->file_offset >= 0);
    assert(f->cache_offset >= 0);
    assert(f->unflushed_write_calls >= 0);
    assert((f->cache_initialized == 0) || (f->cache_initialized == 1));
    assert(f->cache_amt >= 0);
}

/*
    Wrapper around printf that provides information about the
    given file. You can silence this function with the DEBUG_PRINT macro.
*/
static void dbg(struct io300_file *f, char *fmt, ...) {
    (void)f; (void)fmt;
#if(DEBUG_PRINT == 1)
    static char buff[300];
    size_t const size = sizeof(buff);
    int n = snprintf(
        buff,
        size,
        // TODO: Add the fields you want to print when debugging
        "{desc:%s, %d, %d, %d, %d, %d} -- ",
        f->description,
        f->file_offset,
        f->cache_min,
        f->io300_statistics->read_calls,
        f->io300_statistics->write_calls,
        f->io300_statistics->seeks,

    );
    int const bytes_left = size - n;
    va_list args;
    va_start(args, fmt);
    vsnprintf(&buff[n], bytes_left, fmt, args);
    va_end(args);
    printf("%s", buff);
#endif
}



struct io300_file *io300_open(const char *const path, char *description) {
    if (path == NULL) {
        fprintf(stderr, "error: null file path\n");
        return NULL;
    }

    int const fd = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        fprintf(stderr, "error: could not open file: `%s`: %s\n", path, strerror(errno));
        return NULL;
    }

    struct io300_file *const ret = malloc(sizeof(*ret));
    if (ret == NULL) {
        fprintf(stderr, "error: could not allocate io300_file\n");
        return NULL;
    }

    ret->fd = fd;
    ret->cache = malloc(CACHE_SIZE);
    if (ret->cache == NULL) {
        fprintf(stderr, "error: could not allocate file cache\n");
        close(ret->fd);
        free(ret);
        return NULL;
    }
    ret->description = description;
    // TODO: Initialize your file
    ret->cache_min = 0;
    ret->file_offset = 0;
    ret->cache_offset = 0;
    ret->unflushed_write_calls = 0;
    ret->cache_initialized = 0;
    ret->cache_amt = 1;
    ret->file_size = io300_filesize(ret);
    check_invariants(ret);
    dbg(ret, "Just finished initializing file from path: %s\n", path);
    return ret;
}

int io300_seek(struct io300_file *const f, off_t const pos) {
    check_invariants(f);
    f->stats.seeks++;

    // TODO: Implement this
    f->file_offset = pos;
    return lseek(f->fd, f->file_offset, SEEK_SET);
}

int io300_close(struct io300_file *const f) {
    check_invariants(f);

#if(DEBUG_STATISTICS == 1)
    printf("stats: {desc: %s, read_calls: %d, write_calls: %d, seeks: %d}\n",
            f->description, f->stats.read_calls, f->stats.write_calls, f->stats.seeks);
#endif
    // TODO: Implement this
    io300_flush(f);
    close(f->fd);
    free(f->cache);
    free(f);
    return 0;
}

off_t io300_filesize(struct io300_file *const f) {
    check_invariants(f);
    struct stat s;
    int const r = fstat(f->fd, &s);
    if (r >= 0 && S_ISREG(s.st_mode)) {
        return s.st_size;
    } else {
        return -1;
    }
}

int io300_readc(struct io300_file *const f) {
    check_invariants(f);
    // TODO: Implement this
    
    // Naive Implementation
    // unsigned char c;
    // if (read(f->fd, &c, 1) == 1) {
    //     return c;
    // } else {
    //     return -1;
    // }


    unsigned char c;
    //reached or passed the end of file
    if (f->file_offset >= f->file_size) {
        return -1;
    }
    //first ever read on file
    else if (f->cache_initialized == 0) {
        f->cache_amt = read(f->fd, f->cache, CACHE_SIZE);
        f->stats.read_calls++;
        f->cache_initialized = 1;
    }
    else if ((f->file_offset < f->cache_min) || (f->file_offset >= f->cache_min + CACHE_SIZE)) {
        f->cache_offset = 0;
        f->cache_min = f->file_offset;
        f->cache_amt = read(f->fd, f->cache, CACHE_SIZE);
        f->stats.read_calls++;
    }
    //Return the char
    c = f->cache[f->cache_offset];
    f->file_offset++;
    f->cache_offset++;
    return c;
}
int io300_writec(struct io300_file *f, int ch) {
    check_invariants(f);

    // TODO: Implement this
    unsigned char const c = (char)ch;
    //if writing outside the bounds of the filesize
    if (f->file_offset >= f->file_size) {
        f->file_size++;
        //first ever write on file
        if (f->cache_initialized == 0) {
            f->cache_initialized = 1;
            f->cache_amt = 1;
        }
        //current cache range doesn't contain where the write is occuring due to seeks
        else if ((f->file_offset < f->cache_min) || (f->file_offset >= f->cache_min + CACHE_SIZE)) {
            io300_flush(f);
            f->cache_amt = 1;
            f->cache_offset = 0;
            f->cache_min = f->file_offset;
        }
    }
    //if writing inside the bounds of the filesize
    else {
        //first ever write on file
        if (f->cache_initialized == 0) {
            f->cache_amt = read(f->fd, f->cache, CACHE_SIZE);
            f->stats.read_calls++;
            f->cache_initialized = 1;
        }
        //current cache range doesn't contain where the write is occuring due to seeks
        else if ((f->file_offset < f->cache_min) || (f->file_offset >= f->cache_min + CACHE_SIZE)) {
            io300_flush(f);
            f->cache_amt = read(f->fd, f->cache, CACHE_SIZE);
            f->stats.read_calls++;
            f->cache_offset = 0;
            f->cache_min = f->file_offset;
            // lseek(f->fd, f->file_offset, SEEK_SET);
            // io300_seek(f, f->file_offset);
        }
        //current cache range does contain where the write is occuring
        else {
            f->cache_offset = f->file_offset - f->cache_min;
        }
    }

    //Write the char
    f->cache[f->cache_offset] = c;
    f->file_offset++;
    f->cache_offset++;
    if (f->cache_offset >= f->cache_amt) {
        f->cache_amt = f->cache_offset ;
    }
    f->unflushed_write_calls++;
    f->stats.write_calls++;
    return c;


    // naive implementation
    // char const c = (char)ch;
    // return write(f->fd, &c, 1) == 1 ? ch : -1;
}

ssize_t io300_read(struct io300_file *const f, char *const buff, size_t const sz) {
    check_invariants(f);
    // TODO: Implement this
    // // Naive Implementation
    // return read(f->fd, buff, sz);

    //If we are reading more than a whole cache size can hold, then it's more efficient to not use cache
    if (sz >= CACHE_SIZE) {
        f->stats.read_calls++;
        return read(f->fd, buff, sz);
    }
    //If we are reading from outside the file_size, then read is 0
    else if (f->file_offset >= f->file_size) {
        return 0;
    }
    // fill cache if it's the first read call 
    else if (f->cache_initialized == 0) {
        f->cache_amt = read(f->fd, f->cache, CACHE_SIZE);
        f->cache_initialized = 1;
        f->stats.read_calls++;
    }
    //file_offset is outside current cache bounds due to seek call
    else if ((f->file_offset < f->cache_min) || (f->file_offset >= f->cache_min + CACHE_SIZE)) {
        f->cache_amt = read(f->fd, f->cache, CACHE_SIZE);
        f->cache_min = f->file_offset;
        f->cache_offset = 0;
        f->stats.read_calls++;
    }
    //seek changed the file_offset position within the current cache
    else {
        f->cache_offset = f->file_offset - f->cache_min;
    }

    int buff_length;
    //Check cache_amt is valid
    if ((f->cache_amt == -1) || (f->cache_amt == 0)) {
        return f->cache_amt;
    }
    //Can't write all sz bytes because file isn't long enough
    else if ((f->file_offset + (int)sz) > f->file_size) {
        buff_length = f->cache_amt - f->cache_offset;
        memcpy(buff, &(f->cache[f->cache_offset]), buff_length);
        f->file_offset = f->cache_min + f->cache_amt;
        f->cache_offset = f->cache_amt;    
        
        // cache reached end of file
        if (f->file_offset == f->file_size) {
            return buff_length;
        }
        // next cache is needed
        else {
            // read next cache values
            f->cache_amt = read(f->fd, f->cache, CACHE_SIZE);
            char *const buff2 = &(buff[buff_length]);
            f->cache_offset = 0;
            f->cache_min = f->file_offset;
            f->stats.read_calls++;

            // add relevant bytes to buff2
            memcpy(buff2, f->cache, f->cache_amt); 
            buff_length += f->cache_amt;
            f->file_offset = f->cache_min + f->cache_amt;
            f->cache_offset = f->cache_amt;   
            f->file_offset = f->file_size;
            return buff_length;
        }
    }
    // All sz bytes can be written into buffer
    else {
        // current cache is enough
        if ((f->cache_offset + sz) <= CACHE_SIZE) {
            buff_length = sz;
            memcpy(buff, &(f->cache[f->cache_offset]), buff_length);
            f->file_offset += buff_length;
            f->cache_offset += buff_length; 
            return buff_length;
        }
        // need a new cache
        else {
            // copy over rest of current buffer
            buff_length = f->cache_amt - f->cache_offset;
            memcpy(buff, &(f->cache[f->cache_offset]), buff_length);

            // read next cache values
            f->cache_amt = read(f->fd, f->cache, CACHE_SIZE);
            char *const buff2 = &(buff[buff_length]);
            f->cache_min = f->file_offset + buff_length;
            f->stats.read_calls++;

            // add relevant bytes to buff2
            memcpy(buff2, f->cache, sz - buff_length); 
            buff_length = sz;
            f->file_offset += buff_length;
            f->cache_offset = f->file_offset - f->cache_min;
            return buff_length;
        }
    }
}
ssize_t io300_write(struct io300_file *const f, const char *buff, size_t const sz) {
    check_invariants(f);
    // TODO: Implement this
    // Naive Implementation
    // return write(f->fd, buff, sz);
    
    //If we are writing more than a whole cache size can hold, then it's more efficient to not use cache
    if (sz >= CACHE_SIZE) {
        f->stats.write_calls++;
        return write(f->fd, buff, sz);
    }
    //if writing outside the bounds of the file_size
    else if ((f->file_offset + (int)sz) > f->file_size) {
        // Update file_size
        if (f->file_offset >= f->file_size) {
            f->file_size += sz;
        }
        else {
            f->file_size = ((f->file_offset + sz) - f->file_size);
        }
        //fill cache if it's the first read call 
        if (f->cache_initialized == 0) {
            f->cache_initialized = 1;
            f->cache_min = f->file_offset;
            f->cache_offset = 0;
            f->cache_amt = 0;
        }
        //file_offset is outside current cache bounds due to seek call
        else if ((f->file_offset < f->cache_min) || (f->file_offset >= f->cache_min + CACHE_SIZE)) {
            io300_flush(f);
            f->cache_min = f->file_offset;
            f->cache_offset = 0;
            f->cache_amt = 1;
        }
        // seek changed the file_offset position within the current cache
        else if (f->cache_offset != (f->file_offset - f->cache_min)){
            f->cache_offset = f->file_offset - f->cache_min;
            if (f->cache_offset >= f->cache_amt) {
                f->cache_amt = f->cache_offset + 1;
            }
        }
        
        // current cache is enough
        if ((f->cache_offset + sz) <= CACHE_SIZE) {
            memcpy(&(f->cache[f->cache_offset]), buff, sz);
            f->file_offset += sz;
            f->cache_offset += sz;
            if (f->cache_offset >= f->cache_amt) {
                f->cache_amt = f->cache_offset;
            }
            f->unflushed_write_calls++;
            return sz;
        }
        // need a new cache
        else {
            io300_flush(f);
            f->cache_min = f->file_offset;
            memcpy(f->cache, buff, sz);
            f->file_offset += sz;
            f->cache_offset = sz;
            f->cache_amt = sz;
            f->unflushed_write_calls++;
            return sz;
        }
    }
    //if writing inside the bounds of the filesize
    else {
        //fill cache if it's the first read call 
        if (f->cache_initialized == 0) {
            f->cache_amt = read(f->fd, f->cache, CACHE_SIZE);
            f->cache_initialized = 1;
            f->cache_min = f->file_offset;
            f->cache_offset = 0;
            f->stats.read_calls++;
        }
        //file_offset is outside current cache bounds due to seek call
        else if ((f->file_offset < f->cache_min) || (f->file_offset >= f->cache_min + CACHE_SIZE)) {
            io300_flush(f);
            f->cache_amt = read(f->fd, f->cache, CACHE_SIZE);
            f->stats.read_calls++;
            f->cache_min = f->file_offset;
            f->cache_offset = 0;
        }
        // seek changed the file_offset position within the current cache
        else if (f->cache_offset != (f->file_offset - f->cache_min)){
            f->cache_offset = f->file_offset - f->cache_min;
        }
        
        // current cache is enough
        if ((f->cache_offset + sz) <= CACHE_SIZE) {
            memcpy(&(f->cache[f->cache_offset]), buff, sz);
            f->file_offset += sz;
            f->cache_offset += sz;
            f->unflushed_write_calls++;
            return sz;
        }
        // need a new cache
        else {
            io300_flush(f);
            f->cache_amt = read(f->fd, f->cache, CACHE_SIZE);
            f->stats.read_calls++;
            f->cache_min = f->file_offset;
            memcpy(f->cache, buff, sz);
            f->file_offset += sz;
            f->cache_offset = sz;
            f->unflushed_write_calls++;
            return sz;
        }
    }
}

int io300_flush(struct io300_file *const f) {
    check_invariants(f);
    // TODO: Implement this

    //Record the cache info back into the file only if unflushed writes occured
    if (f->unflushed_write_calls != 0) {
        pwrite(f->fd, f->cache, f->cache_amt, f->cache_min);
        f->unflushed_write_calls = 0;
        f->stats.write_calls++;
    }
    return 0;
}