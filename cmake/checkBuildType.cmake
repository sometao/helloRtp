

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message("Setting build type to 'RelWithDebInfo' as none was specified.")
	set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)

	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS 
		"Debug" 
		"Release"
		"MinSizeRel" 
		"RelWithDebInfo"
	)
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    message( "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")
		set(BUILD_DEBUG 1)
elseif(CMAKE_BUILD_TYPE MATCHES "Release" 
	OR CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo"
	OR CMAKE_BUILD_TYPE MATCHES "MinSizeRel" ) 
		message("CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")
		set(BUILD_DEBUG 0)
else()
    message(WARNING "unknown CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}, try build RELEASE.")
		set(BUILD_DEBUG 0)
endif()

