#include "af/concurrent/hazard_ptr_guard.h"

namespace af {

HazardPtrGuard::HazardPtrGuard(void *hazard)
        :hazard_(HazardPtrRec::acquire()) {
    hazard_->set_hazard(hazard);
}

HazardPtrGuard::~HazardPtrGuard() {
    release();
}

void HazardPtrGuard::release() {
    if (!released_) {
        HazardPtrRec::release(hazard_);
        released_ = true;
    }
}

void *
HazardPtrGuard::get_hazard() const {
    return hazard_->get_hazard();
}

} // namespace
