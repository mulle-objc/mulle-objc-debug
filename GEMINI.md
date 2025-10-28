# mulle-objc-debug Project Overview

This document provides an overview of the `mulle-objc-debug` project, its structure, how to build and run it, and its development conventions.

## Project Overview

`mulle-objc-debug` is a C/Objective-C library designed to provide comprehensive debugging support for the `mulle-objc-runtime`. Its primary functions include:

*   **Runtime Class Dumping:** Generates visual representations of runtime classes in various formats, including HTML and Graphviz dot files.
*   **CSV Export:** Offers functionality to export runtime information in CSV format.
*   **Debugger Integration:** Used by MulleObjC to enhance debugger and debugging capabilities.

The project leverages `CMake` for its build system and `mulle-sde` for streamlined project management and dependency handling.

## Building and Running

### Dependencies

The core dependency for `mulle-objc-debug` is `mulle-objc-runtime`.

### Building and Installation with `mulle-sde`

`mulle-sde` is the recommended tool for managing and building `mulle-objc-debug`.

*   **Adding to a Project:**
    ```sh
    mulle-sde add github:mulle-objc/mulle-objc-debug
    ```
    Ensure your source files use `#include "include-private.h"` and headers use `#include "include.h"`.

*   **Installing:**
    ```sh
    mulle-sde install --prefix /usr/local https://github.com/mulle-objc/mulle-objc-debug/archive/latest.tar.gz
    ```

### Building and Installation with `CMake` (Legacy)

For a more traditional build process using `CMake`:

1.  **Install Dependencies:** Ensure `mulle-objc-runtime` is installed.
2.  **Build and Install:**
    ```sh
    PREFIX_DIR="/usr/local"
    cmake -B build \
          -DMULLE_SDK_PATH="${PREFIX_DIR}" \
          -DCMAKE_INSTALL_PREFIX="${PREFIX_DIR}" \
          -DCMAKE_PREFIX_PATH="${PREFIX_DIR}" \
          -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build --config Release && \
    cmake --install build --config Release
    ```

### Testing

The project includes a `test/` directory. Tests can be run using `mulle-sde test`.

*   **Running Tests:**
    ```sh
    mulle-sde test
    ```

## Development Conventions

*   **Header Inclusion:**
    *   Source files should use `#include "include-private.h"`.
    *   Header files should use `#include "include.h"`.
*   **Code Style:** The codebase generally follows a consistent C/Objective-C style, utilizing `//` for comments, `static` for internal functions, and standard C library functions for I/O.
*   **Project Structure:** The project adheres to a structured layout, with configuration managed under the `.mulle/` directory, indicating a `mulle-sde`-driven environment. Key project metadata (name, type, language, source directory) is defined in `.mulle/etc/env/environment-project.sh`.
