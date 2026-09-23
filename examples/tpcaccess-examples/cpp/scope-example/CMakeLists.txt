add_executable(scope-example)
add_tpc_project(scope-example)
target_link_libraries(scope-example PRIVATE tpcserver-examples::tpcaccess)
