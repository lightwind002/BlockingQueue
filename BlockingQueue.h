#ifndef __BLOCKING_QUEUE_H__
#define __BLOCKING_QUEUE_H__
#include <queue>
#include <mutex>
#include <thread>
#include <iostream>
using std::string;

template<typename T>
class BlockingQueue {
public:
    explicit BlockingQueue();

    void push(const T& t);
    // This logs a message if the thread needs to be blocked.
    // Useful for detecting if the data feeding is slow.
    T pop(const string& log_on_wait = "");

    bool try_peek(T* t);

    bool try_pop(T* t);
    // Return element without removing it
    T peek();

    size_t size() const;

protected:
    class                 sync;
    std::queue<T>         queue_;
    std::shared_ptr<sync> sync_;

private:
    // disable copy constructor
    BlockingQueue(const BlockingQueue&);
    // disable assignment
    BlockingQueue& operator=(const BlockingQueue&);

};
#endif // __BLOCKING_QUEUE_H__
