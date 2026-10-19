### 0.26.1











* mulle-objc-debug can now be consumed as a cmake subproject, automatically inheriting include directories and dependency targets
* dependencies are re-exported transitively, with whole-archive force-linking preserved for initialization libraries
* add-as-subproject instructions simplified: only mulle-objc-runtime is required now

## 0.26.0



feature: support concurrent hashtables for html/dot dumping

* new ``mulle_buffer_html_concurrent_hashtable`` API for dumping concurrent hashtable contents
* dotdump now renders hashtables for classestoload/categoriestoload waitqueues
* updated CMake to propagate all-load and startup dependencies transitively for `add_subdirectory` consumers


## 0.25.0

Various small improvements
