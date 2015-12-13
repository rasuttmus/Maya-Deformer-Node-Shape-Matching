file(GLOB SOURCE_FILES *.cpp)

find_package(Maya REQUIRED)
find_package(Armadillo REQUIRED)

include_directories(${MAYA_INCLUDE_DIR})
include_directories(${ARMADILLO_INCLUDE_DIRS})

link_directories(${MAYA_LIBRARY_DIR})

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${MAYA_LIBRARIES} ${ARMADILLO_LIBRARIES})

MAYA_PLUGIN(${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME} ${MAYA_TARGET_TYPE} DESTINATION plug-ins)
