// -*-c++-*-

#ifndef AF_CONCURRENT_CONCURRENT_QUEUE_H_
#define AF_CONCURRENT_CONCURRENT_QUEUE_H_

namespace af {

template <typename T>
class ConcurrentQueue {

  public:

    virtual bool offer(const T &item) = 0;
    virtual T peek() = 0;
    virtual T poll() = 0;
    virtual size_t size() = 0;
    virtual bool empty() = 0;

    virtual ~ConcurrentQueue() { }
};

template <>
class ConcurrentQueue<void *> {
  public:

    virtual bool offer(void *item) = 0;
    virtual void *peek() = 0;
    virtual void *poll() = 0;
    virtual size_t size() = 0;
    virtual bool empty() = 0;

    virtual ~ConcurrentQueue() { }
};

}


#endif // AF_CONCURRENT_CONCURRENT_QUEUE_H_
