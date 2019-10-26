cmake_minimum_required(VERSION 3.13)

project(nrfledrc LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core Quick Bluetooth REQUIRED)

add_executable( ${PROJECT_NAME} )
target_sources( ${PROJECT_NAME}
	PRIVATE
    	"main.cpp"
        "ledservice.cpp"
    	"qml.qrc"
)
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Quick blebuddy)
