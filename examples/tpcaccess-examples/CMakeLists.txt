if(EXAMPLES_USE_PREBUILT_TPCACCESS AND EXAMPLES_USE_STATIC_TPCACCESS)
	message(FATAL_ERROR "EXAMPLES_USE_PREBUILT_TPCACCESS is incompatible with EXAMPLES_USE_STATIC_TPCACCESS")
elseif(EXAMPLES_USE_PREBUILT_TPCACCESS)	
	add_subdirectory(
		${CMAKE_CURRENT_LIST_DIR}/../tpcaccess-prebuilt
		${CMAKE_CURRENT_BINARY_DIR}/tpcaccess-prebuilt
	)
	set(tpcaccess-lib tpcaccess-prebuilt)
elseif(EXAMPLES_USE_STATIC_TPCACCESS)
	set(tpcaccess-lib tpcaccess_static)
else()
	set(tpcaccess-lib tpcaccess)
endif()

add_library(tpcserver-examples::tpcaccess ALIAS ${tpcaccess-lib})

add_subdirectory(cpp/cont-example)
add_subdirectory(cpp/ecr-example)
add_subdirectory(cpp/scope-example)
