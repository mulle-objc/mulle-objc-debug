# mulle-objc-debug Library Documentation for AI

## 1. Introduction & Purpose

**mulle-objc-debug** provides debugging utilities and diagnostic functions for mulle-objc runtime. It enables inspection of runtime state, object structures, method dispatch, and runtime behavior. Essential for understanding and debugging Objective-C code on mulle platforms.

This library is particularly useful for:
- Debugging runtime behavior
- Inspecting object state
- Tracing method calls
- Understanding object layout
- Diagnosing runtime issues

## 2. Key Concepts & Design Philosophy

- **Runtime Inspection**: Examine runtime state and data
- **Diagnostic Output**: Print runtime information
- **Low-Level Access**: Direct access to internal structures
- **Development Tools**: For debugging, not production use

## 3. Core API & Data Structures

### Object Inspection

- `void mulle_objc_debug_dump_object(id obj)`
  - Print object structure and state
  - Shows memory layout and instance variables

- `void mulle_objc_debug_print_address(id obj, const char *label)`
  - Print object address and label

### Class Debugging

- `void mulle_objc_debug_dump_class(Class cls)`
  - Print class structure
  - Shows methods, protocols, properties

- `void mulle_objc_debug_print_hierarchy(Class cls)`
  - Print class hierarchy from superclass

### Method Tracing

- `void mulle_objc_debug_trace_method_entry(Class cls, SEL selector)`
  - Trace method entry

- `void mulle_objc_debug_trace_method_exit(Class cls, SEL selector, id result)`
  - Trace method exit

### Runtime State

- `void mulle_objc_debug_dump_runtime()`
  - Print entire runtime state
  - Shows all classes, methods, etc.

- `void mulle_objc_debug_print_statistics()`
  - Print runtime statistics
  - Object counts, memory usage, etc.

## 4. Integration Examples

### Example 1: Debug Object
```objc
NSString *str = @"Hello";
mulle_objc_debug_dump_object(str);
```

### Example 2: Debug Class
```objc
mulle_objc_debug_dump_class([NSString class]);
```

### Example 3: Print Hierarchy
```objc
mulle_objc_debug_print_hierarchy([NSString class]);
```

### Example 4: Runtime Statistics
```objc
mulle_objc_debug_print_statistics();
```

## 5. Dependencies

- **mulle-objc-runtime** - mulle runtime
- Standard C library

## 6. Version Information

mulle-objc-debug version macro: `MULLE_OBJC_DEBUG_VERSION`
