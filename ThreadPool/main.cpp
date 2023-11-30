#include <iostream>
#include <thread>

#include "ThreadPool.hpp"



// case#1
void sayHello(){
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout <<"thread id: " <<std::this_thread::get_id() << "  "<< "say: " <<"Hello!\n";
}
//case#2
int add(int a,int b){
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return a+b;
}

class A{
public:
    void operator()(int a,int b){
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "A::operator() res is: " << a*b << "\n";

    }
    int mul(int a,int b){
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "A::mul res is: " << a*b << "\n";
        return a*b;
    }

};




int main() {
    putils::ThreadPool thread_pool;

    std::cout << "case#1: \n";
    for (int i = 0; i < 200; ++i) {
        thread_pool.submit(sayHello);
    }
    std::cout << "**************************************************************\n";

    std::cout << "case#2: \n";
    auto res = thread_pool.submit(add,100,300);
    int result=res.get();
    std::cout << "res: " << result << "\n";

    std::cout << "**************************************************************\n";

//     case# 3
    std::cout << "case#3: \n";
    auto func = [](){
        std::cout << "It is a lambda func\n";
    };

    thread_pool.submit(func);

    std::cout << "**************************************************************\n";

    // case#4
    std::cout << "case#4: \n";
    A a;
    thread_pool.submit(a,100,100); // A对象是可调用的 也可以是A()

    // 成员函数的调用
    // 成员函数的调用是.* or ->* 需要再套一层才能使用
    auto p_mul = &A::mul;
    auto mem_func = std::bind(p_mul,&a,std::placeholders::_1,std::placeholders::_2);
    auto res_mul=thread_pool.submit(mem_func,100,100);
    std::cout << "final out : "<< res_mul.get()<<"\n";

    std::cout << "**************************************************************\n";


    return 0;
}
