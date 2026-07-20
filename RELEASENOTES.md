## 0.25.0

Various small improvements


## 0.24.0








feature: expose gdb reference and unify debug logging

* add public declaration `mulle_objc_reference_gdb_functions()` so debuggers can reference runtime helpers
* replace direct fprintf() with `mulle_fprintf()` across dotdump, htmldump, gdb, lldb and typeinfodump for consistent, project-aware debug output
* use cache `thread_id` in HTML output for correct thread reporting
