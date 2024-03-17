#ifndef THREAD_POOL_CLASS_H
#define THREAD_POOL_CLASS_H
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
class threadPool
{
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>>tasks;
    // 同步
    std::mutex queue_mutex;
    std::condition_variable condition;
public:
    threadPool(size_t);
    template<class F,class... Args>
    auto enqueue(F&& f,Args&& ... args)
        -> std::future<typename std::result_of<F(Args...)>::type>{
        using return_type = typename std::result_of<F(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f),std::forward<Args>(args)...)
        );
        // auto task = std::packaged_task<return_type()>(
        //     std::bind(std::forward<F>(f),std::forward<Args>(args)...)
        // );
        
        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> u_lock(queue_mutex);
            if(stop) throw std::runtime_error("enqueue failed cause stopped threadpool");
            tasks.emplace([task]{(*task)();});
        }
        if(tasks.size()>4){ // one 还是 all 取决于负载的动态变化 
            condition.notify_one();
        }else{
            condition.notify_all();
        } 
        return res;
    }

    ~threadPool();
    bool stop;
};





#endif