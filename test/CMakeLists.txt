set(SRC_LIST http.cpp)
# 添加可执行文件
add_executable(ckserver ${SRC_LIST})

# 链接 mymuduo 动态库 和 pthread
target_link_libraries(ckserver webserver http logger memorypool pthread)
