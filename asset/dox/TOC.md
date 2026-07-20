# mulle-objc-debug Library Documentation for AI
<!-- Keywords: debugging, runtime-inspection, graphviz, html-dump, dot-dump -->

## 1. Introduction & Purpose

**mulle-objc-debug** provides offline visualization and debugging tools for the
mulle-objc runtime. Its primary function is to dump the runtime state — classes,
methods, protocols, ivars, properties, caches, and the class hierarchy — as
human-readable HTML pages and Graphviz DOT files. This library enables offline
inspection of runtime structures for debugging, documentation generation, and
understanding of runtime internals.

Key features:
- HTML dump of the runtime universe, classpairs, classes, or individual objects
- Graphviz DOT dump of the class hierarchy with configurable detail levels
- Low-level HTML table rendering utilities used by the dump functions
- Type info dump for method type encodings

Depends on `mulle-objc-runtime` (private dependency, not re-exported).

## 2. Key Concepts & Design Philosophy

- **Offline Dump**: All output goes to files in a directory. No interactive
  debugging — create files, then inspect them.
- **Structured Output**: HTML tables use `struct _mulle_objc_htmltablestyle` for
  consistent styling, with optional CSS class prefixes for custom stylesheets.
  Graphviz DOT output uses declarative graph descriptions.
- **Composable Rendering**: The `mulle-objc-html.h` API provides low-level
  rendering functions (class, methodlist, propertylist, etc.) that higher-level
  dump functions compose. Callback-based iterators render collections
  (pointerarrays, hashmaps, uniqueid arrays, fastclasstables) into HTML tables.
- **Configurable Detail**: The DOT dump supports 20+ bitmask options controlling
  what to display (methods, ivars, properties, caches, selectors, protocols,
  mixins, hyperlinks, etc.).

## 3. Core API & Data Structures

### 3.1. `mulle-objc-debug.h` — Umbrella Header

Main include header. Includes all sub-headers and declares:

- `#define MULLE_OBJC_DEBUG_VERSION  ((0UL << 20) | (25 << 8) | 0)`
  - Version macro in `(MAJOR << 20) | (MINOR << 8) | PATCH` format.

```c
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_objc_reference_gdb_functions( void);
```
- Reference GDB/debugger helper functions so they are not stripped by the
  linker. Call this early in your program if you use GDB integration.

### 3.2. `mulle-objc-dotdump.h` — Graphviz DOT Dump

Generates Graphviz DOT files representing the runtime class hierarchy and
internal structures.

#### `enum mulle_objc_dotdump_options`
Bitmask flags for controlling DOT dump detail:

| Option                                                   | Value       |
|----------------------------------------------------------|-------------|
| `MULLE_OBJC_SHOW_FILELINK`                               | 0x000001 |
| `MULLE_OBJC_SHOW_UNIVERSE`                               | 0x000002 |
| `MULLE_OBJC_SHOW_INFRACLASS`                             | 0x000004 |
| `MULLE_OBJC_SHOW_METACLASS`                              | 0x000008 |
| `MULLE_OBJC_SHOW_SUPERCLASS`                             | 0x000010 |
| `MULLE_OBJC_SHOW_METHODLISTS`                            | 0x000020 |
| `MULLE_OBJC_SHOW_MIXINS`                                 | 0x000040 |
| `MULLE_OBJC_SHOW_SELECTORS`                              | 0x000080 |
| `MULLE_OBJC_SHOW_SUPERS`                                 | 0x000100 |
| `MULLE_OBJC_SHOW_PROTOCOLS`                              | 0x000200 |
| `MULLE_OBJC_SHOW_CATEGORIES`                             | 0x000400 |
| `MULLE_OBJC_SHOW_STRINGS`                                | 0x000800 |
| `MULLE_OBJC_SHOW_FASTCLASSES`                            | 0x001000 |
| `MULLE_OBJC_SHOW_CLASSES`                                | 0x002000 |
| `MULLE_OBJC_SHOW_CLASSESTOLOAD`                          | 0x004000 |
| `MULLE_OBJC_SHOW_CACHE`                                  | 0x008000 |
| `MULLE_OBJC_SHOW_CLASSFIELDS`                            | 0x010000 |
| `MULLE_OBJC_SHOW_METHODLISTFIELDS`                       | 0x020000 |
| `MULLE_OBJC_SHOW_CLASSPAIR`                              | 0x040000 |
| `MULLE_OBJC_SHOW_IVARLIST`                               | 0x080000 |
| `MULLE_OBJC_SHOW_PROPERTYLIST`                           | 0x100000 |
| `MULLE_OBJC_SHOW_CLASS_HYPERLINK`                        | 0x200000 |
| `MULLE_OBJC_SHOW_MIXIN_HYPERLINK`                        | 0x400000 |
| `MULLE_OBJC_SHOW_UNIVERSE_HYPERLINK`                     | 0x800000 |
| `MULLE_OBJC_SHOW_HYPERLINK`                              | 0x800000 + 0x400000 + 0x200000 |
| `MULLE_OBJC_SHOW_ALL`                                    | ~MULLE_OBJC_SHOW_HYPERLINK |
| `MULLE_OBJC_SHOW_DEFAULT`                                | MULLE_OBJC_SHOW_ALL ^ (MULLE_OBJC_SHOW_STRINGS\|MULLE_OBJC_SHOW_SELECTORS) |

#### Functions

```c
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_objc_universe_dotdump_to_directory( struct _mulle_objc_universe *universe,
                                                  char *directory);
```
- Dump the entire runtime universe as DOT files in `directory`. Generates an
  index, per-class DOT files, and overview graph.

```c
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_objc_class_dotdump_to_directory( struct _mulle_objc_class *cls,
                                               char *directory,
                                               unsigned long options);
```
- Dump a single class as a DOT file. Use `options` bitmask from
  `mulle_objc_dotdump_options` to control detail.

```c
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_objc_classhierarchy_dotdump_to_stream( struct _mulle_objc_class *cls,
                                                     FILE *fp);
```
- Dump class hierarchy as DOT to an open `FILE*`.

```c
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_objc_classhierarchy_dotdump_to_file( struct _mulle_objc_class *cls,
                                                   char *filename);
```
- Dump class hierarchy as DOT to a named file.

```c
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_objc_universe_dotdump_frame_to_directory( struct _mulle_objc_universe *universe,
                                                        char *directory);
```
- Dump a single "movie frame" of the universe state. For capturing sequential
  snapshots of the runtime as it evolves.

### 3.3. `mulle-objc-htmldump.h` — HTML Dump

Generates HTML files representing runtime structures with CSS styling.

```c
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_objc_classpair_htmldump_to_directory( struct _mulle_objc_classpair *pair,
                                                    char *directory);
```
- Dump a classpair (infraclass + metaclass pair) as HTML to `directory`.

```c
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_objc_universe_htmldump_to_directory( struct _mulle_objc_universe *universe,
                                                   char *directory);
```
- Dump the entire runtime universe as HTML files in `directory`. Generates an
  index and per-class HTML pages with linked navigation.

```c
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_objc_class_htmldump_to_directory( struct _mulle_objc_class *cls,
                                                char *directory);
```
- Dump a single class as HTML to `directory`.

```c
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_objc_object_htmldump_class_to_directory( void *obj,
                                                       char *directory);
```
- Convenience: takes any object pointer, resolves its class, and dumps that
  class as HTML. Useful for `(gdb) call` / GDB integration.

### 3.4. `mulle-objc-html.h` — HTML Rendering Utilities

Low-level HTML table generation functions. All append to `struct mulle_buffer`.

#### `struct _mulle_objc_htmltablestyle`
```c
struct _mulle_objc_htmltablestyle
{
   char   *title;        // table title/caption
   char   *classprefix;  // CSS class prefix; NULL = inline style (graphviz mode)
   char   *color;        // text color (used if classprefix is NULL)
   char   *bgcolor;      // background color (used if classprefix is NULL)
   int    colspan;       // number of columns
   char   **headers;     // NULL or NULL-terminated array of column header strings (only
                         //   rendered if colspan > 2)
};
```

#### Table Header

```c
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_add_table_header_colspan( struct mulle_buffer *buffer,
                                               struct _mulle_objc_htmltablestyle *styling,
                                               unsigned int colspan);
```
- Emit a `<TABLE>` opening tag with title/caption cell spanning `colspan`
  columns. If `headers` is non-NULL and `colspan > 2`, adds a sub-header row
  with column names.

#### Class Rendering

```c
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_class( struct mulle_buffer *buffer,
                                 struct _mulle_objc_class *cls,
                                 int show_fields,
                                 struct _mulle_objc_htmltablestyle *styling);
```
- Render a class as HTML table rows (multiple rows). If `show_fields` is
  non-zero, detailed fields are included.

```c
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_class_short( struct mulle_buffer *buffer,
                                       struct _mulle_objc_class *cls,
                                       struct _mulle_objc_htmltablestyle *styling);
```
- Render a compact (single-row) class representation for use in linked
  navigation lists.

```c
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_class_tiny( struct mulle_buffer *buffer,
                                      struct _mulle_objc_class *cls,
                                      struct _mulle_objc_htmltablestyle *styling);
```
- Render an even more compact class representation.

#### Component Renderers

```c
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_ivarlist_hor( struct mulle_buffer *buffer,
                                        struct _mulle_objc_ivarlist *list,
                                        struct _mulle_objc_htmltablestyle *styling);
```
- Render instance variable list as HTML table row(s).

```c
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_propertylist( struct mulle_buffer *buffer,
                                        struct _mulle_objc_propertylist *list,
                                        struct _mulle_objc_htmltablestyle *styling);
```
- Render property list as HTML table row(s).

```c
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_cache( struct mulle_buffer *buffer,
                                 struct _mulle_objc_cache *cache,
                                 struct _mulle_objc_universe *universe,
                                 struct _mulle_objc_htmltablestyle *styling);
```
- Render method dispatch cache as HTML table row(s).

```c
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_staticstring( struct mulle_buffer *buffer,
                                        struct _mulle_objc_staticstring *string,
                                        struct _mulle_objc_htmltablestyle *styling);
```
- Render a static string constant (used for selectors, class names).

```c
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_methodlist( struct mulle_buffer *buffer,
                                      struct _mulle_objc_methodlist *list,
                                      struct _mulle_objc_universe *universe,
                                      int show_fields,
                                      struct _mulle_objc_htmltablestyle *styling);
```
- Render a method list. If `show_fields` is non-zero, includes detailed fields
  (implementation pointers, flags).

```c
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_methodlist_hor( struct mulle_buffer *buffer,
                                          struct _mulle_objc_methodlist *list,
                                          struct _mulle_objc_htmltablestyle *styling);
```
- Render method list in horizontal (compact) layout.

```c
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_universe( struct mulle_buffer *buffer,
                                    struct _mulle_objc_universe *universe,
                                    struct _mulle_objc_htmltablestyle *styling);
```
- Render the entire universe's global state (global tables, load lists,
  hash lookups) as HTML.

#### Collection Renderers (Callback-based)

These functions iterate over collections and invoke callbacks for each
element/entry. This is how individual rows in dump output are generated.

**Type definitions:**
```c
typedef void   mulle_buffer_html_fastclasstable_callback_t( struct mulle_buffer *,
                                                             unsigned int row,
                                                             struct _mulle_objc_infraclass *,
                                                             struct _mulle_objc_htmltablestyle *,
                                                             void *userinfo);

typedef void   mulle_buffer_html_concurrent_pointerarray_callback_t( struct mulle_buffer *,
                                                                      unsigned int row,
                                                                      void *,
                                                                      struct _mulle_objc_htmltablestyle *,
                                                                      void *userinfo);

typedef void   mulle_buffer_html_hashmap_callback_t( struct mulle_buffer *,
                                                      intptr_t,
                                                      void *,
                                                      struct _mulle_objc_htmltablestyle *,
                                                      void *userinfo);

typedef void   mulle_buffer_html_uniqueidarray_callback_t( struct mulle_buffer *,
                                                            unsigned int row,
                                                            mulle_objc_uniqueid_t uniqueid,
                                                            struct _mulle_objc_htmltablestyle *,
                                                            void *userinfo);
```

**Collection iteration functions:**
```c
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_fastclasstable( struct mulle_buffer *buffer,
                                          struct _mulle_objc_fastclasstable *fastclasstable,
                                          mulle_buffer_html_fastclasstable_callback_t *row_description,
                                          struct _mulle_objc_htmltablestyle *styling,
                                          void *userinfo);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_concurrent_pointerarray( struct mulle_buffer *buffer,
                                                   struct mulle_concurrent_pointerarray *list,
                                                   mulle_buffer_html_concurrent_pointerarray_callback_t *row_description,
                                                   struct _mulle_objc_htmltablestyle *styling,
                                                   void *userinfo);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_concurrent_hashmap( struct mulle_buffer *buffer,
                                              struct mulle_concurrent_hashmap *map,
                                              mulle_buffer_html_hashmap_callback_t *row_description,
                                              struct _mulle_objc_htmltablestyle *styling,
                                              void *userinfo);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_uniqueidarray( struct mulle_buffer *buffer,
                                         struct _mulle_objc_uniqueidarray *array,
                                         mulle_buffer_html_uniqueidarray_callback_t *row_description,
                                         struct _mulle_objc_htmltablestyle *styling,
                                         void *userinfo);
```

#### Pre-built Callback Functions

```c
// For concurrent_pointerarray (fastclasses table rows, static strings, load
// classes):
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_fastclass_element( struct mulle_buffer *buffer,
                                             unsigned int row,
                                             struct _mulle_objc_infraclass *cls,
                                             struct _mulle_objc_htmltablestyle *styling,
                                             void *userinfo);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_staticstring_element( struct mulle_buffer *buffer,
                                                unsigned int row,
                                                void *value,
                                                struct _mulle_objc_htmltablestyle *styling,
                                                void *userinfo);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_loadclass_element( struct mulle_buffer *buffer,
                                             unsigned int row,
                                             void *value,
                                             struct _mulle_objc_htmltablestyle *styling,
                                             void *userinfo);

// For concurrent_hashmap entries:
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_class_entry( struct mulle_buffer *buffer,
                                       intptr_t classid,
                                       void *cls,
                                       struct _mulle_objc_htmltablestyle *styling,
                                       void *userinfo);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_loadcategory_entry( struct mulle_buffer *buffer,
                                              intptr_t categoryid,
                                              void *value,
                                              struct _mulle_objc_htmltablestyle *styling,
                                              void *userinfo);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_super_entry( struct mulle_buffer *buffer,
                                       intptr_t hash,
                                       void *value,
                                       struct _mulle_objc_htmltablestyle *styling,
                                       void *userinfo);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_descriptor_entry( struct mulle_buffer *buffer,
                                            intptr_t  methodid,
                                            void *descriptor,
                                            struct _mulle_objc_htmltablestyle *styling,
                                            void *userinfo);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_category_entry( struct mulle_buffer *buffer,
                                          intptr_t  categoryid,
                                          void *value,
                                          struct _mulle_objc_htmltablestyle *styling,
                                          void *userinfo);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_protocol_entry( struct mulle_buffer *buffer,
                                          intptr_t  protocolid,
                                          void *value,
                                          struct _mulle_objc_htmltablestyle *styling,
                                          void *userinfo);

// For uniqueid arrays:
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_protocolid_element( struct mulle_buffer *buffer,
                                              unsigned int row,
                                              mulle_objc_uniqueid_t protocolid,
                                              struct _mulle_objc_htmltablestyle *styling,
                                              void *userinfo);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_categoryid_element( struct mulle_buffer *buffer,
                                              unsigned int row,
                                              mulle_objc_uniqueid_t categoryid,
                                              struct _mulle_objc_htmltablestyle *styling,
                                              void *userinfo);

MULLE_OBJC_DEBUG_GLOBAL
char   *mulle_buffer_html_loadcategory_element( struct mulle_buffer *buffer,
                                                 unsigned int row,
                                                 void *value,
                                                 struct _mulle_objc_htmltablestyle *styling,
                                                 void *userinfo);
```

### 3.5. `mulle-objc-typeinfodump.h` — Type Info Dump

```c
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_objc_typeinfo_dump_to_file( struct mulle_objc_typeinfo *info,
                                          char *indent,
                                          FILE *fp);
```
- Dump parsed Objective-C type encoding info (from `mulle_objc_typeinfo`) as
  human-readable text to `fp`, with `indent` prefix on each line. Useful for
  debugging method signatures and property types.

## 4. Performance Characteristics

- All dump operations are **O(n)** in the number of classes/methods/etc.
- All output is to files/directories — not real-time, not streaming.
- **Not thread-safe.** Call dump functions from a single thread, preferably at a
  quiescent point (e.g., at exit or from a debugger breakpoint).
- Memory usage proportional to the size of the HTML/DOT output buffers.
- DOT hyperlink options (`MULLE_OBJC_SHOW_HYPERLINK`) produce larger output
  files with cross-references between class pages.

## 5. AI Usage Recommendations & Patterns

- **Always include `<mulle-objc-debug/mulle-objc-debug.h>`** as the single
  umbrella header. Do not include individual sub-headers.
- **Dump at quiescent points.** The runtime should not be modifying classes
  while dumping. Ideal call sites: `atexit` handlers, debugger breakpoints,
  after all class loading is complete.
- **Use `mulle_buffer_do_string`** for HTML rendering to a string, as shown in
  the test examples. This pattern manages buffer lifecycle automatically.
- **`struct _mulle_objc_htmltablestyle` determines render mode**: Set
  `classprefix` to NULL for inline-styled (graphviz-style) HTML tables. Set it
  to a string (e.g., `"test"`) to use CSS classes like `class="test_table"`.
- **Headers arrays** must be NULL-terminated and are only rendered when
  `colspan > 2`.
- **The `mulle_objc_dotdump_options` enum values can be OR'd** together. Use
  `MULLE_OBJC_SHOW_DEFAULT` for typical output; `MULLE_OBJC_SHOW_ALL` for
  comprehensive detail.
- **CSS stylesheet**: When using HTML dump, copy `mulle-objc.css` alongside the
  output. The CSS file is provided as `src/mulle-objc.css.inc` and is
  auto-copied during dump operations to the output directory.
- **GDB reference**: Call `mulle_objc_reference_gdb_functions()` early in
  `main()` to prevent the linker from stripping debugger helper symbols when
  using GDB/LLDB integration.

## 6. Integration Examples

### Example 1: Dump Universe as HTML and DOT

```c
#include <mulle-objc-debug/mulle-objc-debug.h>

int   main( int argc, char *argv[])
{
   struct _mulle_objc_universe   *universe;

   universe = mulle_objc_global_get_universe( __MULLE_OBJC_UNIVERSEID__);
   mulle_objc_universe_htmldump_to_directory( universe, ".");
   mulle_objc_universe_dotdump_to_directory( universe, ".");
   return( 0);
}
```

### Example 2: Dump a Single Class as DOT with Options

```c
#include <mulle-objc-debug/mulle-objc-debug.h>

void   dump_class_as_dot( struct _mulle_objc_class *cls)
{
   mulle_objc_class_dotdump_to_directory( cls,
                                          ".",
                                          MULLE_OBJC_SHOW_DEFAULT);
}
```

### Example 3: Build an HTML Table with Headers

```c
#include <mulle-objc-debug/mulle-objc-debug.h>

char   *build_html_table( void)
{
   struct _mulle_objc_htmltablestyle   style;
   char                                *headers[] = { "Name", "Value", "Description", NULL };
   char                                *result;

   style.title       = "My Table";
   style.classprefix = "myapp";   // use CSS classes
   style.color       = "white";   // unused with classprefix
   style.bgcolor     = "blue";    // unused with classprefix
   style.colspan     = 3;
   style.headers     = headers;

   mulle_buffer_do_string( buffer, NULL, result)
   {
      mulle_buffer_add_table_header_colspan( buffer, &style, style.colspan);
      mulle_buffer_sprintf( buffer, "<TR><TD>%s</TD><TD>%d</TD><TD>%s</TD></TR>\n",
                            "Item", 100, "First");
      mulle_buffer_add_string( buffer, "</TABLE>");
   }
   return( result);  // caller must mulle_free()
}
```

### Example 4: Dump Class from an Object Pointer (Debugger Use)

```c
#include <mulle-objc-debug/mulle-objc-debug.h>

void   debug_dump_object_class( void *obj)
{
   mulle_objc_object_htmldump_class_to_directory( obj, "/tmp/debug");
}
```

## 7. Dependencies

- `mulle-objc-runtime` (private — not re-exported to consumers)
