// Caleb Bright
#include <iostream>
#include <thread>
#include <mutex>
#include <random>
#include <chrono>
#include <string>
#include <cstring>
#include <atomic>
#include <cstdlib>

using namespace std;

// All shared data that is used by threads
struct Shared_Data {
    atomic<long long> global_counter{0};
    vector<long long> local_buckets;
    int sloppiness;
    int work_time;
    int work_iterations;
    bool cpu_bound;
    bool do_logging;
    mutex global_mutex;
};

// estimated CPU work time
const long long ms_increment = 550000;


void cpu_work(int work_time_ms) {
    // makes a random duration for requested time
    int time_ms = rand() % static_cast<int>(work_time_ms) + (work_time_ms / 2);
    long long increments = static_cast<long long>(time_ms) * ms_increment;
    // keeps the CPU busy
    volatile long long dummy = 0;
    for (long long i = 0; i < increments; ++i)
        dummy += i;
}

// sleeps the thread for random amount of time
void io_work(int work_time_ms) {
    int time_ms = rand() % static_cast<int>(work_time_ms) + (work_time_ms / 2);
    this_thread::sleep_for(chrono::milliseconds(time_ms));
}

// Each thread does work and increments a local_bucket
// When the local_bucket gets to be > sloppiness, it sends
// the count to the global counter and holds the mutex.
void thread_func(int thread_index, Shared_Data* shared) {
    long long local_bucket = 0;
    for (int i = 0; i < shared->work_iterations; ++i) {
        if(shared->cpu_bound)
            cpu_work(shared->work_time);
        else
            io_work(shared->work_time);
        
        // increment local_bucket after work
        local_bucket++;

        // when the bucket fills up, send to global counter
        if (local_bucket >= shared->sloppiness) {
            lock_guard<mutex> lock(shared->global_mutex);
            shared->global_counter += local_bucket;
            local_bucket = 0;
        }
    }
    // anything leftover also gets sent to global counter
    if (local_bucket > 0)
        lock_guard<mutex> lock(shared->global_mutex);
        shared->global_counter += local_bucket;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: ./sloppySim <N_Threads> <Sloppiness> <work_time> <work_iterations> <cpu_bound> <do_logging>\n";
        return 1;
    }

    // arguments with default values
    int N_Threads = (argc > 1) ? stoi(argv[1]) : 2;
    int sloppiness = (argc > 2) ? stoi(argv[2]) : 10;
    int work_time = (argc > 3) ? stoi(argv[3]) : 10;
    int work_iterations = (argc > 4) ? stoi(argv[4]) : 100;
    bool cpu_bound = (argc > 5) ? (strcmp(argv[5], "true") == 0) : false;
    bool do_logging = (argc > 6) ? (strcmp(argv[6], "true") == 0) : false;

    Shared_Data shared;
    shared.sloppiness = sloppiness;
    shared.work_time = work_time;
    shared.work_iterations = work_iterations;
    shared.cpu_bound = cpu_bound;
    shared.do_logging = do_logging;
    shared.local_buckets.resize(N_Threads, 0);

    // print out each argument if logging is enabled
    if (do_logging) {
        cout << "Threads: " << N_Threads << "\n";
        cout << "Sloppiness: " << sloppiness << "\n";
        cout << "Work time: " << work_time << " ms\n";
        cout << "Work iterations: " << work_iterations << "\n";
        cout << "CPU bound: " << (cpu_bound ? "true" : "false") << "\n";
        cout << "Logging: " << (do_logging ? "true" : "false") << "\n";
    }

    auto start = chrono::high_resolution_clock::now();

    // threads
    vector<thread> threads;
    for (int i = 0; i < N_Threads; ++i) {
        threads.emplace_back(thread_func, i, &shared);
    }
    // display global counters value if logging is enabled
    if (do_logging) {
        int log = max(1, work_time * work_iterations / 10);
        for (int i = 0; i < log; ++i) {
            this_thread::sleep_for(chrono::milliseconds(10));
            cout << "Log time=" << i *10 << "ms, Global time= " << shared.global_counter.load() << endl;
        }
    }
    // when all threads are finished, join them
    for (auto& t : threads)
        t.join();
    
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    cout << "\nFinal Global count: " << shared.global_counter.load() << endl;
    cout << "Elasped time: " << duration.count() << " seconds" << endl;

    return 0;
}