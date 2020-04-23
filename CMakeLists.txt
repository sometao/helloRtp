# CMakeList.txt : CMake project for helloLibevent, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.11)



project ("helloRtp" VERSION 0.0.1)

include(./cmake/checkBuildType.cmake)

#-------------------------------------
#  spdlog settings
#-------------------------------------
include(./cmake/utils.cmake)


include(./.SET_LOCAL_PATH.cmake)


# --------------------------------------------------------------------------
# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-D__STDC_CONSTANT_MACROS")



if(MSVC)
    SET(CMAKE_CXX_FLAGS "/EHsc")  # deal with MSVC Warning C4530
endif(MSVC)







#-------------------------------------
#  jrtplib.
#-------------------------------------
find_path(JRTPLIB_INCLUDE_DIR jrtplib3/rtpsession.h REQUIRED)
if(WIN32)
	if(BUILD_DEBUG) 
		find_library(JRTPLIB_LIBRARY jrtplib_d.lib REQUIRED)
	else()
		find_library(JRTPLIB_LIBRARY jrtplib.lib REQUIRED)
	endif()
elseif(UNIX)

endif()



#-------------------------------------
#  PRE BUILD DEFINE
#-------------------------------------
if(WIN32)

elseIf(UNIX)
	add_definitions(-Wall)
endif()


#-------------------------------------
#  ws2_32 for windows
#-------------------------------------
if(WIN32) # Check if we are on Windows
  find_library(WS2_LIBRARY ws2_32)
endif()

#-------------------------------------
#  Threads
#-------------------------------------
find_package(Threads REQUIRED) 



#-------------------------------------
#  config.h.in settings
#-------------------------------------
set(LOG_FILE_NAME "helloRtp.log")

configure_file(
	"./cmake/config.h.in" 
	"${PROJECT_SOURCE_DIR}/include/config.h"
	@ONLY
)


######################################
#  Information
######################################
message(" ===================== Information ======================")
message(CMAKE_SOURCE_DIR         	    =  ${CMAKE_SOURCE_DIR})
message(PROJECT_SOURCE_DIR            =  ${PROJECT_SOURCE_DIR})
message(CMAKE_INCLUDE_PATH            =  ${CMAKE_INCLUDE_PATH})
message(CMAKE_LIBRARY_PATH            =  ${CMAKE_LIBRARY_PATH})
message(PROJECT_SOURCE_DIR            =  ${PROJECT_SOURCE_DIR})
message(CMAKE_BUILD_TYPE              =  ${CMAKE_BUILD_TYPE})

message(" ===================== INCLUDE_DIR ======================")

message(JRTPLIB_INCLUDE_DIR           =  ${JRTPLIB_INCLUDE_DIR})

message(" ==================== LIBRARY PATH =====================")
message(CMAKE_THREAD_LIBS_INIT		    =  ${CMAKE_THREAD_LIBS_INIT})
message(CMAKE_DL_LIBS	         		    =  ${CMAKE_DL_LIBS})
message(WS2_LIBRARY                   =  ${WS2_LIBRARY})
message(JRTPLIB_LIBRARY               =  ${JRTPLIB_LIBRARY})




# Add source to this project's executable.
add_executable (${PROJECT_NAME}   
  "src/main.cpp" 
  "src/baseUdp/udpClient.cpp" "src/baseUdp/udpServer.cpp")


target_include_directories( ${PROJECT_NAME}  
	PRIVATE 
		${PROJECT_SOURCE_DIR}/include
		${JRTPLIB_INCLUDE_DIR}
)


#spdlog_enable_warnings(${PROJECT_NAME})


target_link_libraries( ${PROJECT_NAME}  
	PRIVATE 
		${JRTPLIB_LIBRARY}
		${WS2_LIBRARY} 
		${CMAKE_THREAD_LIBS_INIT}
		${CMAKE_DL_LIBS}
)







