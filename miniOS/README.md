# miniOS — Minimal RISC-V 32-bit educational OS (QEMU virt)

This project is a tiny command-line style OS for a 32-bit RISC-V machine, designed to run on QEMU’s **`virt` Generic Platform** in machine mode.

It’s intentionally small but demonstrates the core OS features listed in the assignment:

- **Loading separate programs**  
  - “Programs” are user tasks (`user_hello`, `user_counter`) with their own stacks and entry points, created via `task_create()`.

- **Running multiple programs simultaneously**  
  - Cooperative multitasking with a round-robin scheduler.  
  - Each task has its own context; `task_yield()` switches between them.

- **Synchronization**  
  - A spinlock abstraction (`spinlock_t`) used to protect the in-memory filesystem.

- **File system**  
  - Tiny RAM filesystem (`fs.c`) with a fixed table of `MAX_FILES`.  
  - Supports `fs_create`, `fs_open`, `fs_read`, `fs_write`, and `fs_list`.

- **How to create/load new programs**  
  - To add a new program, write a C function with signature `void myprog(void)`, then call `task_create(myprog);` from `kmain()`.

---

## Build & Run (QEMU virt, RISC-V 32-bit)

### Dependencies (Debian/Ubuntu lab machine)
  - make
  - qemu-system-riscv32 -machine virt -nographic -bios none -kernel kernel.elf
