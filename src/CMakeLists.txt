target_sources(blebuddy
	PRIVATE
		bledevice.cpp
		bledevicefinder.cpp
		bleserviceinfo.cpp
		uartservice.cpp
	PUBLIC
		bledevice.hpp
		bledevicefinder.hpp
		bleserviceinfo.hpp
		uartservice.hpp
)

find_package(Qt5 COMPONENTS Core Bluetooth REQUIRED)
target_link_libraries(blebuddy PUBLIC Qt5::Core Qt5::Bluetooth)

#target_compile_definitions(blebuddy PUBLIC USE_)
target_include_directories(blebuddy PUBLIC ${CMAKE_CURRENT_LIST_DIR})
