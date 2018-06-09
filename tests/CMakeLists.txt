
set(CMAKE_AUTOMOC OFF)
find_package(GMock REQUIRED)
set(CMAKE_AUTOMOC ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${GMOCK_INCLUDE_DIRS})
include_directories(${GTEST_INCLUDE_DIRS})

set(
	UNIT_TESTS_SRC
	main.cpp
	libqtdbusmock/TestDBusMock.cpp
)

add_executable(
	unit-tests
	${UNIT_TESTS_SRC}
)

target_link_libraries(
    unit-tests
    Qt5::Core
    Qt5::DBus
    Qt5::Test
)

target_link_libraries(
	unit-tests
	qtdbusmock
	${QTDBUSTEST_LIBRARIES}
	${GTEST_LIBRARIES}
	${GMOCK_LIBRARIES}
)

add_test(
	unit-tests
	unit-tests
)
