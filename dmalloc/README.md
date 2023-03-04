Project 2: DMalloc
===================

<!-- TODO: Fill this out. -->

## Design Overview:
I created a struct called md to contain the metadata for each malloc. The metadata includes the size of the malloc, an upper bound character, and the line and file name of the malloc. I also created a global unordered_map to keep track of all allocations. The key is the void* pointer to the start of the allocated memory, and the value is 0 if the memory is freed and 1 if the memory is currently in use.
## Collaborators:
Matthew Kutam, Ryan Eng
## Extra Credit attempted:

## How long did it take to complete DMalloc?
50 hours
<!-- Enter an approximate number of hours that you spent actively working on the project. -->
