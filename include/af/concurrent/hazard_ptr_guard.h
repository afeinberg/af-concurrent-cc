// -*-c++-*-

#ifndef AF_CONCURRENT_HAZARD_PTR_GUARD_H_
#define AF_CONCURRENT_HAZARD_PTR_GUARD_H_

#include "af/concurrent/hazard_ptr_rec.h"

namespace af {

class HazardPtrGuard {
  public:
    HazardPtrGuard(void *hazard);
    ~HazardPtrGuard();

    void release();
    void *get_hazard() const;

  private:
    HazardPtrRec *hazard_;
    bool released_;
};

}

#endif // AF_CONCURRENT_HAZARD_PTR_GUARD_H_
