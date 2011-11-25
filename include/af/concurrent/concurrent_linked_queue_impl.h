// -*-c++-*-

#ifndef AF_CONCURRENT_CONCURRENT_LINKED_QUEUE_IMPL_H_
#define AF_CONCURRENT_CONCURRENT_LINKED_QUEUE_IMPL_H_

#include <atomic>
#include <memory>

#include <boost/noncopyable.hpp>

#include "af/concurrent/concurrent_queue.h"
#include "af/alloc/alloc.h"

namespace af {

using std::atomic;
using std::shared_ptr;
using std::unique_ptr;
using boost::noncopyable;

typedef AllocationStrategy<void> NodeAllocStrategy;

class ConcurrentLinkedQueue_ : private noncopyable, public ConcurrentQueue<void *>  {
    
  public:
    

    explicit ConcurrentLinkedQueue_(const shared_ptr<NodeAllocStrategy> &alloc);
    virtual ~ConcurrentLinkedQueue_();
    
    virtual bool offer(void *item);
    virtual void * peek();
    virtual void * poll();
    virtual size_t size();
    virtual bool empty();

    class Node {
      public:
        Node(void *value, Node *next = NULL);
        ~Node();

        void *get_item() const;
        void set_item(void *val);        
        bool cas_item(void *cmp, void *cal);
        
        Node *get_next() const;
        void set_next(Node *val);
        bool cas_next(Node *cmp, Node *val);

      private:
        atomic<void *> value_;
        atomic<Node *> next_;        
    };
    
protected:
    Node* first();
    
private:
    bool cas_tail(Node *cmp, Node *val);
    bool cas_head(Node *cmp, Node *val);
    
    const shared_ptr<NodeAllocStrategy> alloc_;
    atomic<Node *> head_;
    atomic<Node *> tail_;
};

}

#endif // AF_CONCURRENT_CONCURRENT_LINKED_QUEUE_IMPL_H_
