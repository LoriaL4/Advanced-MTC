#include <thread>
#include <vector>
#include <future>
#include <atomic>
#include <iostream>

#include "threadsafe_queue.h"
#include "function_wrapper.h"
#include "join_threads.h"

template <typename FunctionType>
class thread_pool {
public:
    thread_pool():done(false), joiner(threads) {
        unsigned const thread_count = std::thread::hardware_concurrency();
        
        try {
            for(unsigned i = 0; i < thread_count; ++i) {
                threads.push_back(std::thread(&thread_pool::worker_thread, this));
            }
            
        } catch (...) {
            std::cout << "encountered an error " << std::endl;
            done = true;
            throw;
        }
        
    }
    
    ~thread_pool() {
        done = true;
    }
    
std::future<typename std::invoke_result<FunctionType>::type> submit(FunctionType f) {
        typedef typename std::invoke_result<FunctionType>::type result_type;
        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type> res(task.get_future());
        work_queue.push(std::move(task));
        return res;
    }
    
private:
    // here, order of declaration is important
    std::atomic_bool done;
    
    typedef typename std::invoke_result<FunctionType>::type result_type;
    threadsafe_queue<std::packaged_task<result_type()>> work_queue;
    
    std::vector<std::thread> threads;
    join_threads joiner;
    
    void worker_thread() {
        while(!done) {
            if(auto task = work_queue.try_pop()) {
                (*task.get())();
            } else { std::this_thread::yield(); }
        }
    }

};
