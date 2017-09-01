// BlockingQueue.cpp : Defines the entry point for the console application.
//
#include "BlockingQueue.h"
#include <string>

template<typename T>
class BlockingQueue<T>::sync {
public:
    mutable std::mutex      mutex_;
    std::condition_variable condition_;
};

template<typename T>
BlockingQueue<T>::BlockingQueue()
    : sync_(new sync()) {
}

template<typename T>
void BlockingQueue<T>::push(const T& t)
{
    std::unique_lock<std::mutex> lock(sync_->mutex_);
    queue_.push(t);
    lock.unlock();
    sync_->condition_.notify_one();
}

template<typename T>
bool BlockingQueue<T>::try_pop(T* t)
{
    std::unique_lock<std::mutex> lock(sync_->mutex_);
    if (queue_.empty()) {
        return false;
    }
    *t = queue_.front();
    queue_.pop();
    return true;
}

template<typename T>
T BlockingQueue<T>::pop(const string& log_on_wait)
{
    std::unique_lock<std::mutex> lock(sync_->mutex_);
    while (queue_.empty())
    {
        if (!log_on_wait.empty()) {
            fprintf(stdout, "%s\n", log_on_wait.c_str());
        }
        sync_->condition_.wait(lock);
    }
    T t = queue_.front();
    queue_.pop();
    return t;
}

template<typename T>
bool BlockingQueue<T>::try_peek(T* t)
{
    std::unique_lock<std::mutex> lock(sync_->mutex_);
    if (queue_.empty()) {
        return false;
    }
    *t = queue_.front();
    return true;
}

template<typename T>
T BlockingQueue<T>::peek()
{
    std::unique_lock<std::mutex> lock(sync_->mutex_);
    while (queue_.empty()) {
        sync_->condition_.wait(lock);
    }
    return queue_.front();
}

template<typename T>
size_t BlockingQueue<T>::size() const {
    std::unique_lock<std::mutex> lock(sync_->mutex_);
    return queue_.size();
}

#ifdef BLOCKING_QUEUE_TEST
template class BlockingQueue<int>;
BlockingQueue<int> g_blok_queue;

void provider(int id)
{
    for (int i = 0; i < 10; i++) {
        fprintf(stderr, "producer %d.\n", id);
        g_blok_queue.push(id);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void consumer(int id)
{
    while (1)
    {
        //std::cout << "con:" << id << std::endl;
        string s = string("Consumer ") + std::to_string(id) + string(" waiting...");
        fprintf(stderr, "con(%d): %d.\n", id, g_blok_queue.pop(s));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main()
{
    std::thread p1(provider, 1);
    std::thread p2(provider, 2);
    std::thread p3(provider, 3);
    std::thread s1(consumer, 1);
    std::thread s2(consumer, 2);
    p1.join();
    s1.join();
    p2.join();
    s2.join();
    return 0;
}
#endif
