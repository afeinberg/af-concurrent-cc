find_package(GTest REQUIRED)
find_package(Boost REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS} ${COMMON_INCLUDES})

add_executable(concurrent_queue_test concurrent_queue_test.cc)
target_link_libraries(concurrent_queue_test ${PROJECT_NAME_STR} ${GTEST_BOTH_LIBRARIES} pthread 
  libboost_thread.a)

add_test(concurrent_queue_test concurrent_queue_test)
