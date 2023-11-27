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
                            //ȡ������Ҫ������
                            {
                                std::unique_lock<std::mutex> lk(this->cond_mutex_);
                                // �̻߳����������򣩣�1. �̳߳����� 2. ������зǿ�
                                this->cond_var_.wait(lk,
                                                     [this](){return this->stop_ || !this->tasks_.empty();});

                                // �����ǿյ� && �̳߳ؽ�Ҫ����
                                if (this->stop_ && this->tasks_.empty())
                                    return ;
                                task = std::move(this->tasks_.front());
                                this->tasks_.pop();
                            }
                            // ִ��������Ҫ������
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

        // ���첽������װ
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


        this->cond_var_.notify_one(); // ֪ͨһ���߳�ȥ���task

        return p_task->get_future();

    }
//#endif


    ~ThreadPool(){
        // ��ȡ���� ׼��ֹͣ���еĹ���
        {
            std::unique_lock<std::mutex> lk(this->cond_mutex_);
            this->stop_ = true;
        } // ���ͷ����ᵼ����Ч�ͷ�

        this->cond_var_.notify_all(); // ���������������߳�
        for (auto & worker: this->workers_) {
            if (worker.joinable()){
                worker.join();
            }
        }

    }


    // ��ֹ����
    ThreadPool(const ThreadPool &)  = delete;
    ThreadPool& operator=(const ThreadPool &) = delete;

    // ��ֹ�ƶ�
    ThreadPool(ThreadPool&& ) = delete;
    ThreadPool& operator=(ThreadPool &&) = delete;

private:
    bool stop_; // �̳߳عر�״̬
    std::queue<TaskType> tasks_;   // ����
    std::vector<std::thread> workers_; // �̼߳�
    std::mutex cond_mutex_; // ͬ�����ƵĻ�����
    std::condition_variable cond_var_;
};








} // putils



#endif //THREADPOOL_THREADPOOL_HPP
