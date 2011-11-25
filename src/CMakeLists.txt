include_directories(${COMMON_INCLUDES})

add_library(${PROJECT_NAME_STR} STATIC concurrent_linked_queue.cc)