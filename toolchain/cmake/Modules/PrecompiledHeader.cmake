#macro ( ADD_PRECOMPILED_HEADER HEADER OUTPUT_PCH ATTACHED INC_DIRS )
#    add_custom_target ( ${OUTPUT_PCH} )
#    add_dependencies( ${ATTACHED} ${OUTPUT_PCH} )

#    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
#        add_custom_command ( TARGET ${OUTPUT_PCH}
#            COMMAND "${CMAKE_CXX_COMPILER}" -cc1 -x c++ "${HEADER}" -std=c++14 ${CMAKE_CXX_FLAGS} -I/usr/include/x86_64-linux-gnu -I/usr/include -I/usr/include/linux -I/usr/include/c++/5 ${INC_DIRS} -emit-pch -o ${OUTPUT_PCH}
#            )
#    endif()

#    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -i ${OUTPUT_PCH}" )
#endmacro()

#macro ( CREATE_PCH_FILES TARGET HEADERS )
#    get_directory_property(dirs INCLUDE_DIRECTORIES)

#    set ( INC "" )

#    foreach ( dir ${dirs} )
#        list ( APPEND INC "-I${dir}" )
#    endforeach()

#    message ( "${INC}" )

#    foreach(HEAD ${HEADERS})
#        get_filename_component( HEAD_FN "${HEAD}" NAME )
#        add_precompiled_header ( "${HEAD}" "${HEAD_FN}.pch" ${TARGET} "${INC}" )
#    endforeach()
#endmacro()
