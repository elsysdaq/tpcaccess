add_executable(ecr-example)
add_tpc_project(ecr-example)
target_link_libraries(ecr-example PRIVATE tpcserver-examples::tpcaccess)
