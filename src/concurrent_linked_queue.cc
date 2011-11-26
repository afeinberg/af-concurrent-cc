#include <cassert>
#include <cstdlib>

#include "af/concurrent/concurrent_linked_queue_impl.h"
#include "af/concurrent/hazard_ptr_guard.h"

namespace af {

typedef ConcurrentLinkedQueue_::Node Node;

thread_specific_ptr<vector<Node *> > ConcurrentLinkedQueue_::rlist_;

Node::Node(void *value, Node *next)
        :value_({ value }),
         next_({ next }) { }

Node::~Node() { }

void *
Node::get_item() const {
    return value_.load();
}

void
Node::set_item(void *val) {
    value_.store(val);
}

bool
Node::cas_item(void *cmp, void *val) {
    return value_.compare_exchange_strong(cmp, val);
}

Node *
Node::get_next() const {
    return next_.load();
}

void
Node::set_next(Node *val) {
    next_.store(val);
}

bool
Node::cas_next(Node *cmp, Node *val) {
    return next_.compare_exchange_strong(cmp, val);
}

ConcurrentLinkedQueue_::ConcurrentLinkedQueue_(const shared_ptr<NodeAllocStrategy> &alloc)
        :alloc_(alloc) {
    Node *h = new (alloc_->allocate(sizeof(Node))) Node(NULL, NULL);
    head_ = { h };
    tail_ = { h };
}

ConcurrentLinkedQueue_::~ConcurrentLinkedQueue_() {
    Node *node = head_.load();
    while (node != NULL) {
        Node *prev = node;
        node = node->get_next();
        alloc_->deallocate(prev, sizeof(Node));
    }
}

bool ConcurrentLinkedQueue_::offer(void *item) {
    assert(item != NULL);
    Node *n = new (alloc_->allocate(sizeof(Node))) Node(item);
    for (;;) {
        Node *t = tail_.load();
        Node *s = t->get_next();
        HazardPtrGuard hp(t);
        if (t == tail_.load()) {
            if (s == NULL) {
                if (t->cas_next(s, n)) {
                    cas_tail(t, n);
                    return true;
                }
            } else {
                cas_tail(t, s);
            }
        }
    }
}

void *
ConcurrentLinkedQueue_::peek() {
    for (;;) {
        Node *h = head_.load();
        Node *t = tail_.load();
        HazardPtrGuard hp_head(h);
        Node *first = h->get_next();
        if (h == head_.load()) {
            HazardPtrGuard hp_next(first);
            if (h == t) {
                if (first == NULL) {
                    return NULL;
                } else {
                    cas_tail(t, first);
                }
            } else {
                void *item = first->get_item();
                if (item != NULL) {
                    return item;
                } else {
                    cas_head(h, first);
                }
            }
        }                            
    }
}

void *
ConcurrentLinkedQueue_::poll() {
    for (;;) {
        Node *h = head_.load();
        Node *t = tail_.load();
        HazardPtrGuard hp_head(h);
        Node *first = h->get_next();
        if (h == head_.load()) {
            HazardPtrGuard hp_next(first);
            if (h == t) {
                if (first == NULL) {
                    return NULL;
                } else {
                    cas_tail(t, first);
                }
            } else if (cas_head(h, first)) {
                void *item = first->get_item();
                retire(h, alloc_);
                if (item != NULL) {
                    first->set_item(NULL);
                    return item;
                }
                // else, skip over the deleted item
            }
        }
    }
}

size_t ConcurrentLinkedQueue_::size() {
    size_t count = 0;
    for (Node *p = first(); p != NULL; p = p->get_next()) {
        if (p->get_item() != NULL) {
            ++count;
        }
    }
    return count;
}

bool ConcurrentLinkedQueue_::empty() {
    return first() == NULL;
}

Node *
ConcurrentLinkedQueue_::first() {
    for (;;) {
        Node *h = head_.load();
        Node *t = tail_.load();
        Node *first = h->get_next();
        if (h == head_.load()) {
            if (h == t) {
                if (first == NULL) {
                    return NULL;
                } else {
                    cas_tail(t, first);
                }
            } else {
                if (first->get_item() != NULL) {
                    return first;
                } else {
                    cas_head(h, first);
                }
            }
        }
    }
}

bool ConcurrentLinkedQueue_::cas_tail(Node *cmp, Node *val) {
    return tail_.compare_exchange_strong(cmp, val);
}

bool ConcurrentLinkedQueue_::cas_head(Node *cmp, Node *val) {
    return head_.compare_exchange_strong(cmp, val);
}

void ConcurrentLinkedQueue_::retire(Node *old,
                                    const shared_ptr<NodeAllocStrategy> alloc) {
    if (rlist_.get() == NULL) {
        rlist_.reset(new vector<Node *>);
    }
    rlist_->push_back(old);
    if (rlist_->size() >= kRetire) {
        scan(HazardPtrRec::head(), alloc);
    }
}

void ConcurrentLinkedQueue_::scan(HazardPtrRec *head,
                                  const shared_ptr<NodeAllocStrategy> alloc) {
    vector<void *> hp;
    // Scan the hazard pointer list, collecting all non-NULL ptrs
    while (head != NULL) {
        void *p = head->get_hazard();
        if (p != NULL) {
            hp.push_back(p);
        }
        head = head->get_next();
    }

    std::sort(hp.begin(), hp.end(), std::less<void *>());
    std::vector<Node *>::iterator it = rlist_->begin();
    while (it != rlist_->end()) {
        if (!std::binary_search(hp.begin(),
                                hp.end(),
                                *it)) {
            // Found one that can safely be deleted
            alloc->deallocate(*it, 1);
            if (&*it != &(rlist_->back())) {
                *it = rlist_->back();
            }
            rlist_->pop_back();
        } else {
            ++it;
        }
    }
}

} // namespace
