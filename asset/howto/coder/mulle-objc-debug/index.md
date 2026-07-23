<!-- Keywords: dump, html, graphviz, dot, typeinfo, gdb, debugger -->
# mulle-objc-debug: Coder Reference

Debug/inspection library for the mulle-objc runtime. Dumps runtime structures
(classes, methods, protocols, ivars, properties, caches, hierarchies) as HTML
files or Graphviz DOT files, and debugs type encodings.

Use when you need to visualize the runtime state of a mulle-objc program for
debugging, documentation, or runtime analysis.

## Understand first

```bash
mulle-sde api apropos mulle-objc-debug
mulle-sde api cat mulle-objc-debug
mulle-sde howto show --topic mulle-objc-runtime --role coder
```

## Local references

| Surface | Repo path |
|---------|-----------|
| Umbrella header | `src/mulle-objc-debug.h` |
| HTML dump API | `src/mulle-objc-htmldump.h` |
| HTML rendering utilities | `src/mulle-objc-html.h` |
| Graphviz DOT dump API | `src/mulle-objc-dotdump.h` |
| Type info dump | `src/mulle-objc-typeinfodump.h` |
| GDB integration | `src/mulle-objc-gdb.c` |
| Demo/test: universe HTML+DOT dump | `test/30-dump/dump-html.m` |
| Demo/test: HTML table rendering | `test/40-html/test-basic-html.c` |
| Demo/test: HTML headers rendering | `test/40-html/test-headers.c` |
| CSS stylesheet source | `src/mulle-objc.css.inc` |

## API families

| Family | Header | Purpose |
|--------|--------|---------|
| HTML Dump | `mulle-objc-htmldump.h` | Dump universe/classpair/class/object to HTML files |
| HTML Rendering | `mulle-objc-html.h` | Low-level HTML table builders with callback-based collection iteration |
| Dot Dump | `mulle-objc-dotdump.h` | Dump hierarchy/class to Graphviz DOT files |
| Type Info Dump | `mulle-objc-typeinfodump.h` | Print parsed type encodings as text |
| Debugger Integration | `mulle-objc-gdb.c` | GDB lookup helpers for class/selector/IMP resolution |

## Primary workflow

1. Include umbrella header: `#include <mulle-objc-debug/mulle-objc-debug.h>`
2. Obtain a `struct _mulle_objc_universe *` via `mulle_objc_global_get_universe()`
3. Call dump functions at a quiescent point (after all class loading, not during mutation)
4. Output goes to a directory — inspect files offline

For low-level HTML rendering, use `mulle_buffer_do_string` with the `mulle_buffer_html_*` functions and `struct _mulle_objc_htmltablestyle`.

For GDB integration, call `mulle_objc_reference_gdb_functions()` early in `main()` to prevent linker from stripping debugger helper symbols.
