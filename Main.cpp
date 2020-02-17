#include <iostream>
#include <atomic>
#include <functional>
#include <typeinfo>
#include <numeric>

#include "thread_pool.h"

template <typename Iterator, typename T>
struct accumulate_block {
    T operator()(Iterator first, Iterator last) {
        return std::accumulate(first, last, T());
    }
};

template <typename Iterator, typename T>
T parallel_accumulate(Iterator first,Iterator last,T init) {
    unsigned long const length=std::distance(first,last);
    if(!length)
        return init;
    unsigned long const block_size = 2;
    unsigned long const num_blocks = (length+block_size-1)/block_size;
    std::vector<std::future<T> > futures(num_blocks-1);
    typedef typename std::function<T()> task_type;
    thread_pool<task_type> pool;
    Iterator block_start = first;
    for(unsigned long i=0;i<(num_blocks-1);++i) {
        Iterator block_end=block_start;
        std::advance(block_end,block_size);
        futures[i]=pool.submit(std::bind(
        accumulate_block<Iterator,T>(), block_start, block_end));
        block_start=block_end;
    }

    T last_result = accumulate_block<Iterator, T>()(block_start,last);
    T result = init;
    for(unsigned long i = 0; i < (num_blocks-1); ++i) {
        result += futures[i].get();
    }
    result += last_result;
    return result;
}

int main() {
    
    std::vector<int> vec{1,2,3,5,7};
    std::cout << parallel_accumulate(vec.begin(),vec.end(),0) << std::endl;
}

