# Project 4 - WeensyOS
Andrew Chen - azchen

## Design Overview:
For the design overview, I will run through an overall conceptual design of some of the main functions that were implemented inside of kernel.cc.

- kernel(): For the kernel function, the main thing that we had to include was initializing our kernel page table. For this part, there were numerous conditions that were checked when I was initially initializing the kernel page table:
  - CONSOLE_ADDR: Through each iteration, I had to check if the virtual address was equal to the CONSOLE_ADDR. If true, then I set all of the permissions for the console address.
  - PROC_START_ADDR: Through each iteration, I also had to check if the virtual address was inside of the kernel. The range for the kernel would be from 0 to the start of the processor address, which we denoted as PROC_START_ADDR. Therefore, if a virtual address fell in between this range, I made sure to not make the user-accessible permission set because we don't want the users to potentially access the kernel information. The reason for this is because allowing users to access kernel information could lead to negative behavior and potential problems. Therefore, the kernel provides a protected boundary between user space and kernel space to prevent users from accessing kernel information directly. This is why it is so important that the user-accessible flag is not set for the kernel.
  - NULL_POINTER: Then, through each iteration, if the virtual address was not the null pointer, then I knew that we would be in the user-space where I would just set all of the permissions. If the virtual address was equal to the null pointer, then I would just initialize to 0. This concludes the conceptual overview for the kernel function.
- process_setup(): At a high-level, this function is in charge of correctly allocating the space and permissions for the pagetable and memory of a newly created process.
  - First, I used kalloc() in order to allocated a new pagetable. Then, I copied the mappings from my kernel pagetable that I initialized in the previous step into this new pagetable.
  - Then, for each segment of the memory, I used a loader() in order to allocate and map the rest of the memory.
  - Finally, the end of the process_setup() function is left for the stack allocation. I set the stack address to MEMSIZE_VIRTUAL - PAGESIZE where I was able to change each process’s stack to grow down starting at that particular address. Therefore, through this function, we were able to copy the mappings from the kernel pagetable to a pagetable that we were able to allocate while taking into account the stack allocation as well.
- syscall_page_alloc(): This function is in charge of pagetable allocation for the heap. Whenever the requested address is invalid, the function returns -1; otherwise, we would map the physical page that we kalloc()ed to the virtual address that was requested for our heap allocation. There are multiple cases where the requested address would be invalid:
  - The address to be allocated is not divisible by the PAGESIZE.
  - The address is less than PROC_START_ADDR (trying to modify kernel memory, which is not allowed).
  - The address us greater than or equal to MEMSIZE_VIRTUAL, the maximum address inside of our virtual memory.
- syscall_fork(): This function emulates the SYSCALL_FORK system call.
  - First, I looked for a free process slot in the ptable[] array, which stores all active processes. I didn't use slot 0, which is reserved for the kernel. If a free slot exists, I update my pid variable to keep track of the pid.
  - At this specific pid slot, I make a copy of the current pagetable for the child pagetable that we eventually initialize.
  - After copying the mappings, I copy the data from the parent's pagetable into the child's pagetable.
  - Finally, I fill in the fields for the respective proc struct using the pid variable that I initialized at the very beginning of the function. Then, I would return the pid. To ensure that no memory leaks will occur at the conclusion of the program, whenever a mapping or a kalloc() failed, everything is freed and -1 must be returned. This is handled in syscall_exit().
- syscall_exit(): For syscall_exit(), this function is in charge of exiting the program.
  - cleanup(): The main purpose of this function is to serve as a helper inside of syscall_exit() as well as syscall_fork() to reduce the amount of redundant code that I would have to write. First, I used vmiter to free the pages in the page table. Then, I used ptiter to free the pages itself after freeing the pages inside of the pagetable. Then, I would free the rest of the pagetable.
  - The reason why this helper function was useful was because in every scenario inside of syscall_fork() where a mapping failed or a kalloc() failed, I had to free everything that I allocated already, and the main way that I was able to incorporate this was through calling this helper function whenever a failure occured.
  - I was also able to call this inside of my syscall_exit() function where the only line that I added after this function call was just changing the state of our current process to P_FREE.

That concludes the conceptual overview of how I was able to create an Operating System!

## Collaborators:
Matthew Kutam, Ryan Eng
