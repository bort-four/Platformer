# CMakeLists.txt
cmake_minimum_required(VERSION 3.1.1)

# initialize project
project(Test)

# find sources
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTORCC ON)
include_directories(src)

file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5Widgets)

# configure project
set(TARGET_NAME ${PROJECT_NAME})
add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS})
set_target_properties(${TARGET_NAME} PROPERTIES   VERSION "0.1")

# set compiler features
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

target_compile_features(${TARGET_NAME} PUBLIC
    cxx_auto_type
    cxx_explicit_conversions
    cxx_lambdas
    cxx_nullptr
    cxx_override
    cxx_rvalue_references
    )

# link other libraries
target_link_libraries(${TARGET_NAME} Qt5::Core Qt5::Gui Qt5::Widgets)



#set(TARGET_DIRECTORY "${CMAKE_SOURCE_DIR}/../_target")
#include_directories(${TARGET_DIRECTORY}/include)
#link_directories(${TARGET_DIRECTORY}/Linux/lib)

#target_link_libraries(${PROJECT_NAME} Afx)
#target_link_libraries(${PROJECT_NAME} AfxEx)
#target_link_libraries(${PROJECT_NAME} DpsStd)
#target_link_libraries(${PROJECT_NAME} Dps.EventLog)
#target_link_libraries(${PROJECT_NAME} Dps.Prot)

# install
#[[
set(CMAKE_INSTALL_PREFIX ${TARGET_DIRECTORY})

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION Linux/lib
    COMPONENT developer
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                GROUP_READ GROUP_WRITE GROUP_EXECUTE
                WORLD_READ             WORLD_EXECUTE)

install(DIRECTORY public/
    DESTINATION include
    COMPONENT developer
    FILES_MATCHING PATTERN "*.h")

]]#
