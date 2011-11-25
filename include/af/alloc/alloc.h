// -*-c++-*-

#ifndef AF_ALLOC_ALLOC_H_
#define AF_ALLOC_ALLOC_H_

#include <cstdlib>
#include <memory>

namespace af {

using std::shared_ptr;

template <typename T>
class AllocationStrategy {
  public:

    virtual T *allocate(size_t n, const void *p = NULL) = 0;
    virtual void deallocate(T *p, size_t n) = 0;
    virtual void construct(T *p, const T &t) = 0;
    virtual void destroy(T *p) = 0;

    virtual ~AllocationStrategy() { }
};

template <>
class AllocationStrategy<void> {
  public:

    virtual void *allocate(size_t n, const void *p = NULL) = 0;
    virtual void deallocate(void *p, size_t n) = 0;

    virtual ~AllocationStrategy() { }
};

template <typename T, class Alloc>
class AllocatorAdapter : public AllocationStrategy<T> {
  public:

    explicit AllocatorAdapter(const shared_ptr<Alloc> &alloc)
            :alloc_(alloc) { }

    virtual ~AllocatorAdapter() { }

    virtual T *allocate(size_t n, const void *p = NULL) {
        return alloc_->allocate(n, p);
    }

    virtual void deallocate(T *p, size_t n) {
        alloc_->deallocate(p, n);
    }

    virtual void construct(T *p, const T &t) {
        alloc_->construct(p, t);
    }

    virtual void destroy(T *p) {
        alloc_->destroy(p);
    }

  private:
    const shared_ptr<Alloc>  alloc_;
};

template <class Alloc>
class AllocatorAdapter<void, Alloc>
        : public AllocationStrategy<void> {
  public:

    explicit AllocatorAdapter(const shared_ptr<Alloc> &alloc = new Alloc)
            :alloc_(alloc) { }

    virtual ~AllocatorAdapter() { }

    virtual void *allocate(size_t n, const void *p = NULL) {
        return alloc_->allocate(n, p);
    }

    virtual void deallocate(void *p, size_t n) {
        alloc_->deallocate(static_cast<unsigned char *>(p), n);
    }
 
  private:       
    const shared_ptr<Alloc> alloc_;
};

}

#endif // AF_ALLOC_ALLOC_H_
