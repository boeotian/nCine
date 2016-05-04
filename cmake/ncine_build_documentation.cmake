if(NCINE_BUILD_DOCUMENTATION)
	find_package(Doxygen)
	if(NOT DOXYGEN_FOUND)
		message(FATAL_ERROR "Doxygen is needed to build the documentation")
	endif()

	file(RELATIVE_PATH DOXYGEN_RELATIVE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR})
	if(DOXYGEN_RELATIVE_PATH STREQUAL "")
		set(DOXYGEN_RELATIVE_PATH ".")
	endif()

	set(DOXYGEN_INPUT_FILES "${DOXYGEN_RELATIVE_PATH}/include")
	if(NCINE_IMPLEMENTATION_DOCUMENTATION)
		set(DOXYGEN_INPUT_FILES "${DOXYGEN_INPUT_FILES} ${DOXYGEN_RELATIVE_PATH}/src")
		set(DOXYGEN_EXCLUDE_FILES "${DOXYGEN_RELATIVE_PATH}/src/tests")
	endif()
	set(DOXYGEN_OUTPUT_DIR docs)

	set(DOXYFILE_IN ${CMAKE_SOURCE_DIR}/Doxyfile.in)
	set(DOXYFILE ${CMAKE_BINARY_DIR}/Doxyfile)
	configure_file(${DOXYFILE_IN} ${DOXYFILE} @ONLY)

	add_custom_target(documentation ALL DEPENDS ${DOXYGEN_OUTPUT_DIR})
	add_custom_command(OUTPUT ${DOXYGEN_OUTPUT_DIR}
		COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		COMMENT "Generating documentation with Doxygen"
	)

	if(WIN32)
		install(DIRECTORY ${CMAKE_BINARY_DIR}/${DOXYGEN_OUTPUT_DIR}/html/ DESTINATION docs)
	else()
		install(DIRECTORY ${CMAKE_BINARY_DIR}/${DOXYGEN_OUTPUT_DIR}/html/ DESTINATION share/doc/ncine)
	endif()
endif()