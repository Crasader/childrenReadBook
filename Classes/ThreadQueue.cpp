#include "ThreadQueue.h"

#include<iostream>

using namespace daocode;

threadQueue::threadQueue():max_size_queue(1024),thread_cnt(5),is_start(0)
{
}
threadQueue::~threadQueue()
{
    stop();
}


void threadQueue::push_back(const ThreadfuncPtr ptr)//将一个线程加入队列
{
    std::unique_lock <std::mutex> lck (mtx);
    if (thrPtrs.size() < (size_t)max_size_queue)
    {
        thrPtrs.push_back(ptr);
         cond.notify_one();
    }
}
void threadQueue::Process()
{
    std::unique_lock <std::mutex> lck (mtx);
    while(is_start){
        while(!thrPtrs.empty()){
            ThreadfuncPtr work = thrPtrs.front();
            thrPtrs.pop_front();
            lck.unlock();
            work();
            lck.lock();
        }
        cond.wait(lck);
    }
}

void threadQueue::start()//初始化线程队列
{
    if (!is_start){//如果已初始化，则不会再次进行初始化
        is_start=true;
        for (int i=0;i<thread_cnt;i++){
            std::shared_ptr<std::thread> thrptr (new std::thread(std::bind(&threadQueue::Process,this)));
            work_threads.push_back(thrptr);
        }
    }
}

void threadQueue::stop()
{
    if(!is_start)
        return ;
    is_start=false;
    cond.notify_all();//如果在cond.wait之前发，导致阻塞在cond.wait

    for(int i=0;i<(int)work_threads.size();i++){
        if (work_threads[i]->joinable())
            work_threads[i]->join();//如果任务做完了，阻塞在这里还可，如果没做完，阻塞则导致notify_all，发出后接收不到
    }
}
