#
# cmake/_Dependencies.cmake is generated by `mulle-sde`. Edits will be lost.
#
if( MULLE_TRACE_INCLUDE)
   message( STATUS "# Include \"${CMAKE_CURRENT_LIST_FILE}\"" )
endif()

# sourcetree: MULLE_CONCURRENT;no-all-load,no-import,no-singlephase;
if( NOT MULLE_CONCURRENT_LIBRARY)
   find_library( MULLE_CONCURRENT_LIBRARY NAMES ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-concurrent${CMAKE_STATIC_LIBRARY_SUFFIX} mulle-concurrent)
   message( STATUS "MULLE_CONCURRENT_LIBRARY is ${MULLE_CONCURRENT_LIBRARY}")
   #
   # the order looks ascending, but due to the way this file is read
   # it ends up being descending, which is what we need
   if( MULLE_CONCURRENT_LIBRARY)
      #
      # Add to MULLE_CONCURRENT_LIBRARY list.
      # Disable with: `mark no-cmakeadd`
      #
      set( DEPENDENCY_LIBRARIES
         ${DEPENDENCY_LIBRARIES}
         ${MULLE_CONCURRENT_LIBRARY}
         CACHE INTERNAL "need to cache this"
      )
      #
      # Inherit ObjC loader and link dependency info.
      # Disable with: `mark no-cmakeinherit`
      #
      # // temporarily expand CMAKE_MODULE_PATH
      get_filename_component( _TMP_MULLE_CONCURRENT_ROOT "${MULLE_CONCURRENT_LIBRARY}" DIRECTORY)
      get_filename_component( _TMP_MULLE_CONCURRENT_ROOT "${_TMP_MULLE_CONCURRENT_ROOT}" DIRECTORY)
      #
      #
      # Search for "DependenciesAndLibraries.cmake" to include.
      # Disable with: `mark no-cmakedependency`
      #
      foreach( _TMP_MULLE_CONCURRENT_NAME "mulle-concurrent")
         set( _TMP_MULLE_CONCURRENT_DIR "${_TMP_MULLE_CONCURRENT_ROOT}/include/${_TMP_MULLE_CONCURRENT_NAME}/cmake")
         # use explicit path to avoid "surprises"
         if( EXISTS "${_TMP_MULLE_CONCURRENT_DIR}/DependenciesAndLibraries.cmake")
            unset( MULLE_CONCURRENT_DEFINITIONS)
            list( INSERT CMAKE_MODULE_PATH 0 "${_TMP_MULLE_CONCURRENT_DIR}")
            # we only want top level INHERIT_OBJC_LOADERS, so disable them
            if( NOT NO_INHERIT_OBJC_LOADERS)
               set( NO_INHERIT_OBJC_LOADERS OFF)
            endif()
            list( APPEND _TMP_INHERIT_OBJC_LOADERS ${NO_INHERIT_OBJC_LOADERS})
            set( NO_INHERIT_OBJC_LOADERS ON)
            #
            include( "${_TMP_MULLE_CONCURRENT_DIR}/DependenciesAndLibraries.cmake")
            #
            list( GET _TMP_INHERIT_OBJC_LOADERS -1 NO_INHERIT_OBJC_LOADERS)
            list( REMOVE_AT _TMP_INHERIT_OBJC_LOADERS -1)
            #
            list( REMOVE_ITEM CMAKE_MODULE_PATH "${_TMP_MULLE_CONCURRENT_DIR}")
            set( INHERITED_DEFINITIONS
               ${INHERITED_DEFINITIONS}
               ${MULLE_CONCURRENT_DEFINITIONS}
               CACHE INTERNAL "need to cache this"
            )
            break()
         else()
            message( STATUS "${_TMP_MULLE_CONCURRENT_DIR}/DependenciesAndLibraries.cmake not found")
         endif()
      endforeach()
   else()
      message( FATAL_ERROR "MULLE_CONCURRENT_LIBRARY was not found")
   endif()
endif()


# sourcetree: MULLE_VARARG;no-all-load,no-import,no-singlephase;
if( NOT MULLE_VARARG_LIBRARY)
   find_library( MULLE_VARARG_LIBRARY NAMES ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-vararg${CMAKE_STATIC_LIBRARY_SUFFIX} mulle-vararg)
   message( STATUS "MULLE_VARARG_LIBRARY is ${MULLE_VARARG_LIBRARY}")
   #
   # the order looks ascending, but due to the way this file is read
   # it ends up being descending, which is what we need
   if( MULLE_VARARG_LIBRARY)
      #
      # Add to MULLE_VARARG_LIBRARY list.
      # Disable with: `mark no-cmakeadd`
      #
      set( DEPENDENCY_LIBRARIES
         ${DEPENDENCY_LIBRARIES}
         ${MULLE_VARARG_LIBRARY}
         CACHE INTERNAL "need to cache this"
      )
      #
      # Inherit ObjC loader and link dependency info.
      # Disable with: `mark no-cmakeinherit`
      #
      # // temporarily expand CMAKE_MODULE_PATH
      get_filename_component( _TMP_MULLE_VARARG_ROOT "${MULLE_VARARG_LIBRARY}" DIRECTORY)
      get_filename_component( _TMP_MULLE_VARARG_ROOT "${_TMP_MULLE_VARARG_ROOT}" DIRECTORY)
      #
      #
      # Search for "DependenciesAndLibraries.cmake" to include.
      # Disable with: `mark no-cmakedependency`
      #
      foreach( _TMP_MULLE_VARARG_NAME "mulle-vararg")
         set( _TMP_MULLE_VARARG_DIR "${_TMP_MULLE_VARARG_ROOT}/include/${_TMP_MULLE_VARARG_NAME}/cmake")
         # use explicit path to avoid "surprises"
         if( EXISTS "${_TMP_MULLE_VARARG_DIR}/DependenciesAndLibraries.cmake")
            unset( MULLE_VARARG_DEFINITIONS)
            list( INSERT CMAKE_MODULE_PATH 0 "${_TMP_MULLE_VARARG_DIR}")
            # we only want top level INHERIT_OBJC_LOADERS, so disable them
            if( NOT NO_INHERIT_OBJC_LOADERS)
               set( NO_INHERIT_OBJC_LOADERS OFF)
            endif()
            list( APPEND _TMP_INHERIT_OBJC_LOADERS ${NO_INHERIT_OBJC_LOADERS})
            set( NO_INHERIT_OBJC_LOADERS ON)
            #
            include( "${_TMP_MULLE_VARARG_DIR}/DependenciesAndLibraries.cmake")
            #
            list( GET _TMP_INHERIT_OBJC_LOADERS -1 NO_INHERIT_OBJC_LOADERS)
            list( REMOVE_AT _TMP_INHERIT_OBJC_LOADERS -1)
            #
            list( REMOVE_ITEM CMAKE_MODULE_PATH "${_TMP_MULLE_VARARG_DIR}")
            set( INHERITED_DEFINITIONS
               ${INHERITED_DEFINITIONS}
               ${MULLE_VARARG_DEFINITIONS}
               CACHE INTERNAL "need to cache this"
            )
            break()
         else()
            message( STATUS "${_TMP_MULLE_VARARG_DIR}/DependenciesAndLibraries.cmake not found")
         endif()
      endforeach()
   else()
      message( FATAL_ERROR "MULLE_VARARG_LIBRARY was not found")
   endif()
endif()
