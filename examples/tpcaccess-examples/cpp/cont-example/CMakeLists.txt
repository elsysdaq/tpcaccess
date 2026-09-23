add_executable(cont-example)
add_tpc_project(cont-example)
target_link_libraries(cont-example PRIVATE tpcserver-examples::tpcaccess)
