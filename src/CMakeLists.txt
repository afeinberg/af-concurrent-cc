include_directories(${COMMON_INCLUDES})


add_library(${PROJECT_NAME_STR} STATIC concurrent_linked_queue.cc 
  hazard_ptr_rec.cc hazard_ptr_guard.cc)