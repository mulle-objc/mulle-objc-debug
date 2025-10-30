# MATCHPLAN.md: Refactoring mulle-objc-debug to use mulle-buffer

## Project Context

The goal is to refactor the `mulle-objc-debug` library, which provides debugging support for the `mulle-objc-runtime`, to utilize the `mulle-buffer` library for all dynamic string and I/O operations. Currently, the project heavily relies on `mulle_asprintf` and manual `mulle_malloc`/`strcat` patterns.

## Core Principles and Lessons Learned

To avoid past mistakes and ensure a successful refactoring, the following principles will be strictly adhered to:

1.  **API Stability is Paramount:**
    *   **DO NOT** change the function signatures of any non-static (public API) functions declared in `src/mulle-objc-html.h` (or any other public header). These functions *must* continue to return `char *` as they did originally.
    *   **DO NOT** change the function signatures of any callbacks passed to public API functions (e.g., `row_description` function pointers in `mulle_buffer_describe_concurrent_pointerarray`). These must also remain stable.

2.  **Internal Refactoring Only:**
    *   All `mulle-buffer` integration will occur within `static` helper functions.
    *   `static` helper functions *will* have their signatures modified to accept `struct mulle_buffer *` as an argument and typically return `void`.

3.  **`mulle_buffer` Usage Patterns:**
    *   **Public API Functions (returning `char *`):**
        *   Will use `char *s; mulle_buffer_do_string( buffer, NULL, s) { ... }` to create a `mulle_buffer`, populate it, and ensure the allocated string `s` is returned.
        *   They will call the *new or modified internal static helper functions*, passing the `buffer` as an argument.
    *   **Internal Static Functions (accepting `struct mulle_buffer *`):**
        *   Will use `mulle_buffer_sprintf( buffer, ...)` or `mulle_buffer_add_string( buffer, ...)` to append content directly to the passed `buffer`.
        *   They will return `void`.
    *   **Temporary Internal Buffers:** If an internal static function needs a temporary buffer that is not part of the main output, it will use `mulle_buffer_do( tmp_buffer) { ... }`.

4.  **Sorting Strategy with `mulle_pointerarray`:**
    *   For functions that require sorting (e.g., `mulle_buffer_describe_ivarlist`, `mulle_buffer_describe_concurrent_pointerarray`), the following pattern will be used:
        *   We store the string there for comparison.
        *   **Sort `mulle_pointerarray`:** The `mulle_pointerarray` will be sorted using `mulle_qsort_r` with a custom comparison function (e.g., `strcmp_row_data_r`).
        *   **Append to Main Buffer:** After sorting, iterate through the `mulle_pointerarray`, append each stored `char *` to the main `mulle_buffer` (passed from the public function), and then `mulle_free` the individual `char *`
        *   **Destroy `mulle_pointerarray`:** Finally, `mulle_pointerarray_done` will be called.

5.  **Correct `mulle_pointerarray_init` Usage:**
    *   All calls to `mulle_pointerarray_init` will use the signature `mulle_pointerarray_init(&array, 0, NULL);` (where `0` is the initial capacity and `NULL` indicates the default allocator).

6.  **Tool Usage Precision:**
    *   **`replace` Tool:** Before *every* `replace` call, I will:
        *   `read_file` the *entire relevant code block* (e.g., the full function) to get the absolute latest content.
        *   Construct the `old_string` parameter as an **exact, literal match** of the text to be replaced, including all whitespace, indentation, and sufficient surrounding context (at least 3 lines before and after the target change).
        *   Ensure the `new_string` is also an exact, literal replacement.
    *   **One Change at a Time:** Only one `replace` operation will be performed per step.
    *   **Re-read After Each Change:** After each successful `replace`, I will re-read the modified file content to ensure my internal state is synchronized.

7.  **Frequent Verification:**
    *   After each logical chunk of refactoring (e.g., all static helper functions for a particular public API, or one complete public API function), I will run the build command: `mulle-sde craft -- --no-hook`.
    *   Any compilation errors will be addressed immediately and systematically.

## Step-by-Step Refactoring Process for `src/mulle-objc-html.c`

### Phase 1: Refactor Internal Static Functions (Signature Change + `mulle_buffer` usage)

For each of the following `static` functions, modify their signature to accept `struct mulle_buffer *buffer` as the first argument and change their return type to `void`. Update their implementation to write directly to the `buffer` using `mulle_buffer_sprintf` or `mulle_buffer_add_string`.

1.  `static void asprintf_table_header_colspan( struct mulle_buffer *buffer, ...)`
2.  `static void asprintf_table_header( struct mulle_buffer *buffer, ...)`
3.  `static void categoryid_describe_row_html( struct mulle_buffer *buffer, ...)`
4.  `static void protocolid_describe_row_html( struct mulle_buffer *buffer, ...)`
5.  `static void mulle_buffer_describe_staticstring_row( struct mulle_buffer *buffer, ...)`
6.  `static void mulle_buffer_describe_fastclass_row( struct mulle_buffer *buffer, ...)`
7.  `static void mulle_buffer_describe_class_row( struct mulle_buffer *buffer, ...)`
8.  `static void mulle_buffer_describe_infraclass_row( struct mulle_buffer *buffer, ...)`
9.  `static void mulle_buffer_describe_descriptor( struct mulle_buffer *buffer, ...)`
10. `static void mulle_buffer_describe_category( struct mulle_buffer *buffer, ...)`
11. `static void mulle_buffer_describe_protocol( struct mulle_buffer *buffer, ...)`
12. `static void mulle_buffer_describe_super( struct mulle_buffer *buffer, ...)`
13. `static void mulle_buffer_describe_loadclass_row( struct mulle_buffer *buffer, ...)`
14. `static void mulle_buffer_describe_loadcategory( struct mulle_buffer *buffer, ...)`

### Phase 2: Refactor Public API Functions (Maintain Signature + Call Static Helpers)

For each of the following public API functions, maintain their original `char *` return type and signature. Implement them to:
1.  Create a `mulle_buffer` using `mulle_buffer_do_string( buffer, NULL, s)`.
2.  Call the appropriate `static` helper functions (from Phase 2), passing the `buffer`.
3.  For functions requiring sorting, use the `mulle_pointerarray` approach described in "Core Principles".
4.  Return `s`.

1.  `char *mulle_buffer_describe_universe(...)`
2.  `char *mulle_buffer_describe_staticstring(...)`
3.  `char *mulle_buffer_describe_staticstring_hor(...)`
4.  `char *mulle_buffer_describe_class_short(...)`
5.  `char *mulle_buffer_describe_class_tiny(...)`
6.  `char *mulle_buffer_describe_class(...)`
7.  `char *mulle_buffer_describe_ivarlist(...)`
8.  `char *mulle_buffer_describe_ivarlist_hor(...)`
9.  `char *mulle_buffer_describe_propertylist(...)`
10. `char *mulle_buffer_describe_cache(...)`
11. `char *mulle_buffer_describe_descriptor_html(...)`
12. `char *mulle_buffer_describe_descriptor_hor(...)`
13. `char *mulle_buffer_describe_methodlist(...)`
14. `char *mulle_buffer_describe_methodlist_hor(...)`
15. `char *mulle_buffer_describe_protocolids(...)`
16. `char *mulle_buffer_describe_categoryids(...)`
17. `char *mulle_buffer_describe_fastclasstable(...)`
18. `char *mulle_buffer_describe_concurrent_pointerarray(...)`
19. `char *mulle_buffer_describe_concurrent_hashmap(...)`
20. `char *mulle_buffer_describe_uniqueidarray(...)`

### Phase 3: Cleanup

1.  **Remove Obsolete Functions:** Remove `final_concat_auto_tmp` and `final_concat_malloced_tmp_known_len` once all their call sites are refactored.

## Verification

After each logical phase or significant change, the project will be built using:

```bash
mulle-sde craft -- --no-hook
```

Any compilation errors will be addressed immediately. Once the build is clean, I will investigate how to run the project's tests (if any are identified) to ensure functional correctness.
