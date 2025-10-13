Caleb Bright - Sloppy counter

The program simulates the sloppy counter by using threads and a shared global counter.
Every thread does however many work iterations and updates the global counter depending
on sloppiness.

Arguments:
    N_Threads = the number of threads needed to run, it defaults to 2
    sloppiness = local increments before the global counter is updated
    work_time = work time per iteration in ms
    work_iterations = the number of iterations per thread
    cpu_bound = true for cpu_bound simulation, false for I/O cpu_bound
    do_logging = true to enable logging, false to disable logging only printing final result

Output:
    Prints log updates if it is enabled, final global counter, and total runtime.
    