/********************************************************************************
* @author: Huang Pisong
* @email: huangpisong@foxmail.com
* @date: 2023/11/27 10:44
* @version: 1.0
* @description: 
********************************************************************************/
#pragma one
#ifndef THREADPOOL_THREADPOOL_HPP
#define THREADPOOL_THREADPOOL_HPP

#include <queue>
#include <mutex>
#include <thread>
#include <future>
#include <functional>
#include <condition_variable>

namespace putils{

class ThreadPool{
public:
    using TaskType = std::function<void()>;
    explicit ThreadPool(std::size_t thread_size=std::thread::hardware_concurrency()):stop_(false) {

        for (std::size_t i = 0; i < thread_size; ++i) {
            workers_.emplace_back(
                    [this](){
                        while (true){
                            TaskType task;
                            //取任务需要保有锁
                            {
                                std::unique_lock<std::mutex> lk(this->cond_mutex_);
                                // 线程唤醒条件（或）：1. 线程池销毁 2. 任务队列非空
                                this->cond_var_.wait(lk,
                                                     [this](){return this->stop_ || !this->tasks_.empty();});

                                // 任务集是空的 && 线程池将要销毁
                                if (this->stop_ && this->tasks_.empty())
                                    return ;
                                task = std::move(this->tasks_.front());
                                this->tasks_.pop();
                            }
                            // 执行任务不需要保有锁
                            task();
                        }
                    }
                    );
        }
    }
//#if __cplusplus == 201103L
    template<typename F,typename ...Args>
    auto submit(F && f,Args&&... args)->
    std::future<decltype(f(args...)) >{
        std::function<decltype(f(args...))()> func =
                std::bind(std::forward<F >(f),std::forward<Args>(args)...);

        // 用异步操作封装
        auto p_task = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

        TaskType task = [p_task](){
            (*p_task)();
        };

        {
            std::unique_lock<std::mutex> lk(this->cond_mutex_);
            if (this->stop_)
                throw std::runtime_error("thread pool is stop! ");
            this->tasks_.emplace(task);
        }


        this->cond_var_.notify_one(); // 通知一个线程去完成task

        return p_task->get_future();

    }
//#endif


    ~ThreadPool(){
        // 获取到锁 准备停止所有的工作
        {
            std::unique_lock<std::mutex> lk(this->cond_mutex_);
            this->stop_ = true;
        } // 不释放锁会导致无效释放

        this->cond_var_.notify_all(); // 唤醒所有阻塞的线程
        for (auto & worker: this->workers_) {
            if (worker.joinable()){
                worker.join();
            }
        }

    }


    // 禁止拷贝
    ThreadPool(const ThreadPool &)  = delete;
    ThreadPool& operator=(const ThreadPool &) = delete;

    // 禁止移动
    ThreadPool(ThreadPool&& ) = delete;
    ThreadPool& operator=(ThreadPool &&) = delete;

private:
    bool stop_; // 线程池关闭状态
    std::queue<TaskType> tasks_;   // 任务集
    std::vector<std::thread> workers_; // 线程集
    std::mutex cond_mutex_; // 同步机制的互斥锁
    std::condition_variable cond_var_;
};








} // putils



#endif //THREADPOOL_THREADPOOL_HPP
