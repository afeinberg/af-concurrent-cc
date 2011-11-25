// -*-c++-*-

#ifndef AF_CONCURRENT_CONCURRENT_LINKED_QUEUE_H_
#define AF_CONCURRENT_CONCURRENT_LINKED_QUEUE_H_

#include <memory>

#include "af/concurrent/concurrent_queue.h"
#include "af/concurrent/concurrent_linked_queue_impl.h"
#include "af/alloc/alloc.h"

namespace af {

using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;

typedef AllocationStrategy<void> NodeAllocStrategy;

template <typename T, class Alloc = std::allocator<unsigned char> >
class ConcurrentLinkedQueue : public ConcurrentQueue<T> {

  public:
    ConcurrentLinkedQueue();
    virtual ~ConcurrentLinkedQueue();


    virtual bool offer(const T & item);
    virtual T peek();
    virtual T poll();
    virtual size_t size();
    virtual bool empty();
};

template <class Alloc>
class ConcurrentLinkedQueue<void *, Alloc> : public ConcurrentQueue<void *> {

  public:
    
    explicit ConcurrentLinkedQueue(const shared_ptr<Alloc> &alloc = make_shared<Alloc>())
        :queue_imp_(new ConcurrentLinkedQueue_(make_shared<AllocatorAdapter<void, Alloc> >(alloc))) {
    }
    
    virtual ~ConcurrentLinkedQueue() { }

    virtual bool offer(void *item) {
        return queue_imp_->offer(item);
    }
    
    virtual void *peek() {
        return queue_imp_->peek();
    }

    virtual void *poll() {
        return queue_imp_->poll();
    }

    virtual size_t size() {
        return queue_imp_->size();
    }

    virtual bool empty() {
        return queue_imp_->empty();
    }

  private:   

    const unique_ptr<ConcurrentLinkedQueue_> queue_imp_;
};

}

#endif // AF_CONCURRENT_CONCURRENT_LINKED_QUEUE_H_
