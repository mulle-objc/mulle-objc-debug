<!-- Keywords: pitfalls, thread-safety, dead-code, css, style -->
# Quirks

## Thread safety

All dump functions are **not thread-safe**. Call them from a single thread at
a quiescent point — after all class loading is complete, at exit, or from a
debugger breakpoint. The runtime must not be modifying classes during the dump.

## Output directory

Dump functions write multiple files into the given directory. The directory
**must exist** before calling — functions do not create intermediate directories.

## DOT options use unsigned long bitmask

`mulle_objc_class_dotdump_to_directory` takes an `unsigned long options`
parameter. OR individual `MULLE_OBJC_SHOW_*` flags together. Use
`MULLE_OBJC_SHOW_DEFAULT` (which omits `MULLE_OBJC_SHOW_STRINGS` and
`MULLE_OBJC_SHOW_SELECTORS`) for typical output. Use `MULLE_OBJC_SHOW_ALL`
for maximum detail (but also see hyperlink note below).

Hyperlink flags (`MULLE_OBJC_SHOW_CLASS_HYPERLINK`, `MULLE_OBJC_SHOW_MIXIN_HYPERLINK`,
`MULLE_OBJC_SHOW_UNIVERSE_HYPERLINK`) are NOT included in `MULLE_OBJC_SHOW_ALL`
(it is `~MULLE_OBJC_SHOW_HYPERLINK`). Combine explicitly if you want cross-references.

## HTML tolerance for special characters

The `html_escape` function used internally does not escape `&` or `<` — it
returns `"bad-html"` if either character is present. Class and method names
containing these characters will produce broken HTML. Avoid such names for
debugging.

## Headers array only rendered when colspan > 2

`struct _mulle_objc_htmltablestyle.headers` is a `NULL`-terminated `char **`
array. It only produces a sub-header row when `colspan` is greater than 2.
Setting headers with `colspan <= 2` silently ignores them.

## LLDB integration is dead code

`src/mulle-objc-lldb.c` and the `test/20-lldb/lldbconststring.m` test are
compiled out by `#if 0` blocks. The `mulle_objc_reference_lldb_functions`
symbol is not declared in any public header. Do not rely on LLDB integration;
use the GDB API instead (`mulle_objc_reference_gdb_functions`).

## Type info dump uses FILE*, not buffer

`mulle_objc_typeinfo_dump_to_file` writes directly to a `FILE*` with a prefix
indent string. It does not use `mulle_buffer`. For programmatic access to type
info, use the runtime's `mulle_objc_typeinfo` API directly.

## CSS class naming convention

When using `classprefix`, the generated CSS class names follow this pattern:
- `{prefix}_table` — `<TABLE>` element
- `{prefix}_table_header` — table title `<TH>` row
- `{prefix}_table_subheader` — column headers row (when `headers` is set)
- Specific runtime panels: `{prefix}_values`, `{prefix}_methods`, `{prefix}_ivars`, `{prefix}_properties`, `{prefix}_cache`, etc.

## No streaming output

All dump output goes to files in a directory. There is no streaming or
real-time API for DOT or HTML dump. For programmatic HTML table rendering,
use the `mulle_buffer_html_*` functions with `mulle_buffer_do_string` and
read the resulting string.
