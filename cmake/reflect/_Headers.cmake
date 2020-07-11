#
# cmake/reflect/_Headers.cmake is generated by `mulle-sde reflect`. Edits will be lost.
#
if( MULLE_TRACE_INCLUDE)
   MESSAGE( STATUS "# Include \"${CMAKE_CURRENT_LIST_FILE}\"" )
endif()

set( INCLUDE_DIRS
src
src/debug
src/reflect
)

set( DEBUG_HEADERS
src/debug/c-set.inc
src/debug/mulle-objc-csvdump.h
src/debug/mulle-objc-dotdump.h
src/debug/mulle-objc-gdb.h
src/debug/mulle-objc-htmldump.h
src/debug/mulle-objc-html.h
src/debug/mulle-objc-lldb.h
src/debug/mulle-objc-symbolizer.h
src/debug/mulle-objc-typeinfodump.h
)

set( PRIVATE_HEADERS
src/include-private.h
src/reflect/_mulle-objc-runtime-include-private.h
)

set( PUBLIC_HEADERS
src/include.h
src/minimal.h
src/mulle-metaabi.h
src/mulle-objc-atomicpointer.h
src/mulle-objc-builtin.h
src/mulle-objc-cache.h
src/mulle-objc-call.h
src/mulle-objc-callqueue.h
src/mulle-objc-class-convenience.h
src/mulle-objc-class.h
src/mulle-objc-classpair.h
src/mulle-objc-class-search.h
src/mulle-objc-class-struct.h
src/mulle-objc-fastclasstable.h
src/mulle-objc-fastenumeration.h
src/mulle-objc-fastmethodtable.h
src/mulle-objc-fnv1a.h
src/mulle-objc-fnv1.h
src/mulle-objc-infraclass.h
src/mulle-objc-ivar.h
src/mulle-objc-ivarlist.h
src/mulle-objc-jit.inc
src/mulle-objc-kvccache.h
src/mulle-objc-load.h
src/mulle-objc-loadinfo.h
src/mulle-objc-metaclass.h
src/mulle-objc-method.h
src/mulle-objc-methodidconstants.h
src/mulle-objc-methodlist.h
src/mulle-objc-object-convenience.h
src/mulle-objc-object.h
src/mulle-objc-objectheader.h
src/mulle-objc-property.h
src/mulle-objc-propertylist.h
src/mulle-objc-protocol.h
src/mulle-objc-protocollist.h
src/mulle-objc-retain-release.h
src/mulle-objc-runtime.h
src/mulle-objc-signature.h
src/mulle-objc-super.h
src/mulle-objc-taggedpointer.h
src/mulle-objc-try-catch-finally.h
src/mulle-objc-uniqueidarray.h
src/mulle-objc-uniqueid.h
src/mulle-objc-universe-class.h
src/mulle-objc-universe-exception.h
src/mulle-objc-universe-fail.h
src/mulle-objc-universe-global.h
src/mulle-objc-universe.h
src/mulle-objc-universe-struct.h
src/mulle-objc-version.h
src/mulle-objc-walktypes.h
src/reflect/_mulle-objc-runtime-include.h
)
