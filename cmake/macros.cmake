macro(setMsvcLinkType linkType)
	if(WIN32 AND MSVC)
		set(variables
			  CMAKE_C_FLAGS_DEBUG
			  CMAKE_C_FLAGS_MINSIZEREL
			  CMAKE_C_FLAGS_RELEASE
			  CMAKE_C_FLAGS_RELWITHDEBINFO
			  CMAKE_CXX_FLAGS_DEBUG
			  CMAKE_CXX_FLAGS_MINSIZEREL
			  CMAKE_CXX_FLAGS_RELEASE
			  CMAKE_CXX_FLAGS_RELWITHDEBINFO
		)
		if(${linkType} STREQUAL "static")
			message(STATUS
				"MSVC -> forcing use of statically-linked runtime."
			)
			foreach(variable ${variables})
				if(${variable} MATCHES "/MD")
					string(REGEX REPLACE "/MD" "/MT" ${variable} "${${variable}}")
				endif()
			endforeach()
		else()
			message(STATUS
				"MSVC -> forcing use of dynamically-linked runtime."
			)
			foreach(variable ${variables})
				if(${variable} MATCHES "/MT")
					string(REGEX REPLACE "/MT" "/MD" ${variable} "${${variable}}")
				endif()
			endforeach()
		endif()
	endif()
endmacro()