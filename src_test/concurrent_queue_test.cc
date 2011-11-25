#include "gtest/gtest.h"

#include "af/concurrent/concurrent_linked_queue.h"

namespace {

using namespace af;

using std::unique_ptr;
using std::shared_ptr;

class ConcurrentQueueTest : public ::testing::Test {

protected:
    
    static const size_t kSize = 20;
    
    ConcurrentQueueTest()
            :queue_(new ConcurrentLinkedQueue<void *>()) { }
    
    virtual ~ConcurrentQueueTest() { }

    void populate(size_t n) {
        ASSERT_TRUE(queue_->empty());
        for (size_t i = 0; i < n; ++i) {
            ASSERT_TRUE(queue_->offer(new size_t(i)));
        }
        ASSERT_FALSE(queue_->empty());
        ASSERT_EQ(n, queue_->size());
    }

    virtual void TearDown() {
        while (!queue_->empty()) {
            void *p = queue_->poll();
            if (p != NULL) {
                free(p);
            }
        }
    }
    
    unique_ptr<ConcurrentQueue<void *> > queue_;
};

TEST_F(ConcurrentQueueTest, empty) {
    EXPECT_TRUE(queue_->empty());
    queue_->offer(new int(1));
    EXPECT_FALSE(queue_->empty());
    queue_->offer(new int(2));    
}

TEST_F(ConcurrentQueueTest, size) {
    ASSERT_EQ(queue_->size(), 0u);
    populate(kSize);
    for (size_t i = 0; i < kSize; ++i) {
        ASSERT_EQ(kSize - i, queue_->size());
        void *p = queue_->poll();
        free(p);
    }
    for (size_t i = 0; i < kSize; ++i) {
        ASSERT_EQ(i, queue_->size());
        queue_->offer(new size_t(i));
    }
}

TEST_F(ConcurrentQueueTest, poll) {
    populate(kSize);
    for (size_t i = 0; i < kSize; ++i) {
        size_t *p = static_cast<size_t *>(queue_->poll());
        size_t actual = *p;
        free(p);
        ASSERT_EQ(i, actual);        
    }
    ASSERT_EQ(NULL, queue_->poll());
}

TEST_F(ConcurrentQueueTest, peek) {
    populate(kSize);
    for (size_t i = 0; i < kSize; ++i) {
        size_t *p_peek = static_cast<size_t *>(queue_->peek());
        size_t *p_poll = static_cast<size_t *>(queue_->poll());
        EXPECT_EQ(i, *p_peek);
        EXPECT_EQ(i, *p_poll);
        free(p_poll);
        p_peek = static_cast<size_t *>(queue_->peek());
        ASSERT_TRUE(p_peek == NULL || *p_peek != i);
    }
    ASSERT_EQ(NULL, queue_->peek());
}

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
