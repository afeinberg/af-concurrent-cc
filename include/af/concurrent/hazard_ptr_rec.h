// -*-c++-*-

#ifndef AF_CONCURRENT_HAZARD_PTR_REC_
#define AF_CONCURRENT_HAZARD_PTR_REC_

#include <atomic>
#include <cstdlib>

namespace af {

using std::atomic;
using std::atomic_flag;

class HazardPtrRec {
    
  public:
    static HazardPtrRec *head();
    
    void *get_hazard() const;
    void set_hazard(void *hazard);

    HazardPtrRec *get_next() const;

    static HazardPtrRec *acquire();
    static void release(HazardPtrRec *hp);

  private:
    atomic_flag active_;
    atomic<HazardPtrRec *> next_;
    atomic<void *> hazard_;

    static atomic<HazardPtrRec *> head_;
    static atomic<int> list_len_;    
};

} 

#endif // AF_CONCURRENT_HAZARD_PTR_REC
