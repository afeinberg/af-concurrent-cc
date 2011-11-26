#include "af/concurrent/hazard_ptr_rec.h"

namespace af {

atomic<HazardPtrRec *> HazardPtrRec::head_ = { NULL };
atomic<int> HazardPtrRec::list_len_ = { 0 };

HazardPtrRec *
HazardPtrRec::head() {
    return head_.load();
}

void *
HazardPtrRec::get_hazard() const {
    return hazard_.load();
}

void HazardPtrRec::set_hazard(void *hazard) {
    hazard_.store(hazard);
}

HazardPtrRec *
HazardPtrRec::get_next() const {
    return next_.load();
}

HazardPtrRec *
HazardPtrRec::acquire() {
    HazardPtrRec *p = head();
    for (; p != NULL; p = p->get_next()) {
        if (p->active_.test_and_set()) {
            continue;
        }
        return p;
    }
    ++list_len_;

    HazardPtrRec *old;
    p = new HazardPtrRec;
    p->active_.test_and_set();
    p->hazard_ = { NULL };
    do {
        old = head();
        p->next_.store(old);
    } while (!head_.compare_exchange_strong(old, p));
    return p;
}

void HazardPtrRec::release(HazardPtrRec *p) {
    p->set_hazard(NULL);
    p->active_.clear();
}

} // namespace 
