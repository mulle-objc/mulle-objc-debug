<!-- Keywords: dump, html, graphviz, dot, typeinfo, rendering, style -->
# Patterns

## HTML dump — dump universe or object class

```c
#include <mulle-objc-debug/mulle-objc-debug.h>

int main(int argc, char *argv[])
{
   struct _mulle_objc_universe *universe;

   universe = mulle_objc_global_get_universe(__MULLE_OBJC_UNIVERSEID__);
   mulle_objc_universe_htmldump_to_directory(universe, "/tmp/debug");
   return(0);
}
```

Or dump a class from any object pointer — useful in a debugger `call`:

```c
void debug_dump_object_class(void *obj)
{
   mulle_objc_object_htmldump_class_to_directory(obj, "/tmp/debug");
}
```

Dump a single classpair:

```c
mulle_objc_classpair_htmldump_to_directory(pair, "/tmp/classpair");
```

## DOT dump — universe or class hierarchy

Dump the entire universe as DOT files:

```c
mulle_objc_universe_dotdump_to_directory(universe, "/tmp/dot");
```

Dump a single class with filtered options:

```c
mulle_objc_class_dotdump_to_directory(cls, "/tmp/dot",
   MULLE_OBJC_SHOW_DEFAULT);
```

Dump class hierarchy to a stream or named file:

```c
mulle_objc_classhierarchy_dotdump_to_stream(cls, stdout);
mulle_objc_classhierarchy_dotdump_to_file(cls, "/tmp/hierarchy.dot");
```

Capture a sequential snapshot (for "movie" frames):

```c
mulle_objc_universe_dotdump_frame_to_directory(universe, "/tmp/frame");
```

## HTML rendering — build a custom table

Always use `mulle_buffer_do_string` for buffer lifecycle:

```c
struct _mulle_objc_htmltablestyle style;
char *headers[] = { "Name", "Value", "Description", NULL };
char *result;

style.title       = "My Table";
style.classprefix = "myapp";   // non-NULL → CSS classes
style.color       = "white";   // ignored when classprefix is set
style.bgcolor     = "blue";    // ignored when classprefix is set
style.colspan     = 3;
style.headers     = headers;   // only rendered when colspan > 2

mulle_buffer_do_string(buffer, NULL, result)
{
   mulle_buffer_add_table_header_colspan(buffer, &style, style.colspan);
   mulle_buffer_sprintf(buffer,
      "<TR><TD>%s</TD><TD>%d</TD><TD>%s</TD></TR>\n",
      "Item", 100, "First");
   mulle_buffer_add_string(buffer, "</TABLE>");
}
// result is malloced — caller must mulle_free()
mulle_free(result);
```

Render a class into the buffer (use `show_fields` non-zero for detail):

```c
mulle_buffer_html_class(buffer, cls, 1, &style);
mulle_buffer_html_class_short(buffer, cls, &style);  // compact
mulle_buffer_html_class_tiny(buffer, cls, &style);    // even smaller
```

Render sub-components:

```c
mulle_buffer_html_ivarlist_hor(buffer, ivarlist, &style);
mulle_buffer_html_propertylist(buffer, propertylist, &style);
mulle_buffer_html_cache(buffer, cache, universe, &style);
mulle_buffer_html_staticstring(buffer, str, &style);
mulle_buffer_html_methodlist(buffer, methodlist, universe, 0, &style);
mulle_buffer_html_universe(buffer, universe, &style);
```

Iterate collections with callback-based functions (pass pre-built callback):

```c
mulle_buffer_html_fastclasstable(buffer, fct, NULL, &style, NULL);
mulle_buffer_html_concurrent_pointerarray(buffer, parray,
   mulle_buffer_html_staticstring_element, &style, NULL);
mulle_buffer_html_concurrent_hashmap(buffer, hashmap,
   mulle_buffer_html_class_entry, &style, NULL);
mulle_buffer_html_uniqueidarray(buffer, uidarray,
   mulle_buffer_html_protocolid_element, &style, NULL);
```

Pre-built callbacks for `mulle_buffer_html_concurrent_pointerarray`:
- `mulle_buffer_html_fastclass_element` — fast class table rows
- `mulle_buffer_html_staticstring_element` — static string/selector list rows
- `mulle_buffer_html_loadclass_element` — load-class list rows

Pre-built callbacks for `mulle_buffer_html_concurrent_hashmap`:
- `mulle_buffer_html_class_entry` — class-by-name entries
- `mulle_buffer_html_loadcategory_entry` — load-category entries
- `mulle_buffer_html_super_entry` — superclass entries
- `mulle_buffer_html_descriptor_entry` — method descriptor entries
- `mulle_buffer_html_category_entry` — category entries
- `mulle_buffer_html_protocol_entry` — protocol entries

Pre-built callbacks for `mulle_buffer_html_uniqueidarray`:
- `mulle_buffer_html_protocolid_element` — protocol unique-ID rows
- `mulle_buffer_html_categoryid_element` — category unique-ID rows

## HTML rendering — classprefix mode vs. inline style

- `classprefix = NULL` → inline `BGCOLOR`/`COLOR` attributes (graphviz-style).
- `classprefix = "xxx"` → CSS class attributes (`xxx_table`, `xxx_table_header`, etc.).
- The bundled CSS is in `src/mulle-objc.css.inc`; it is auto-copied to the output directory during dump operations.

## Type info dump — debug method signatures

```c
struct mulle_objc_typeinfo info = ...;
mulle_objc_typeinfo_dump_to_file(&info, "  ", stderr);
```

Prints: type string, invocation offset, natural/bits size, alignment, member count, and
whether the type contains an object.

## GDB integration — prevent linker stripping

```c
#include <mulle-objc-debug/mulle-objc-debug.h>

int main(int argc, char *argv[])
{
   mulle_objc_reference_gdb_functions();  // call early in main()
   // ...
}
```

This references `mulle_objc_gdb_lookup_class`, `mulle_objc_gdb_lookup_selector`,
`mulle_objc_gdb_lookup_implementation`, and `sel_get_any_uid` so the linker
does not discard them.
