#ifndef TIMER_H
#define TIMER_H
#include <iostream>
#include <thread>
#include <chrono>

namespace proto::SR{
class Timer
{
    bool clear = false; //timer是否被停止
    bool running = false; //此timer是否已经运行（不能对已经运行的timer再次调用start)

public:
    /**
     * @brief       启动该定时器，如果超时就执行规定的动作
     * @tparam T 
     * @param  delay           延时
     * @param  callback        超时后执行的动作
     */
    template <typename T>
    void start(int delay, T callback);
    //停止计时器计时
    void stop()
    {
        this->clear = true;
        this->running = false;
    }
    
};

template <typename T>
void Timer::start(int delay, T callback)
{
    //一个计时器只能start一次。
    if (this->running == true)
        return;
    this->clear = false;
    this->running = true;

    std::thread([=]() {
        if (this->clear)
            return;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        //途中被stop
        if (this->clear)
            return;
        //超时，执行超时后的响应函数
        callback();
        //超时后重置计时器
        this->stop();
    }).detach();
}
}

#endif
