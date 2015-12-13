SET( ARMADILLO_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/ext/armadillo/include/")
if(WIN32)
    # Windows
elseif(APPLE)
    # Apple
	SET( ARMADILLO_LIBRARIES "${CMAKE_SOURCE_DIR}/ext/armadillo/lib/libarmadillo.dylib" )
else()
    # Linux
endif()