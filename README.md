# mulle-objc-debug

#### 🐞 Debug support for the mulle-objc-runtime

Dump runtime classes as HTML or Graphviz dot files. There is also CSV
export functionality. This library is used by MulleObjC to provide debugger
and debugging support.


| Release Version
|-----------------------------------
 ![Mulle kybernetiK tag](//img.shields.io/github/tag/mulle-objc/mulle-objc-debug.svg) [![Build Status](//github.com/mulle-objc/mulle-objc-debug/workflows/CI/badge.svg?branch=release)](//github.com/mulle-objc/mulle-objc-debug/actions)


## Required Libraries and Tools


  Name         | Release Version
---------------|---------------------------------
[mulle-objc-runtime](//github.com/mulle-objc/mulle-objc-runtime) | ![Mulle kybernetiK tag](//img.shields.io/github/tag/mulle-objc/objc-runtime.svg) [![Build Status](//github.com/mulle-objc/mulle-objc-runtime/workflows.svg?branch=release)](//github.com/mulle-objc/mulle-objc-runtime/actions)
[mulle-fprintf](//github.com/mulle-core/mulle-fprintf) | ![Mulle kybernetiK tag](//img.shields.io/github/tag/mulle-core/mulle-fprintf.svg) [![Build Status](//github.com/mulle-core/mulle-fprintf/workflows/CI/badge.svg?branch=release)](//github.com/mulle-core/mulle-fprintf/actions)


### Platforms and Compilers

All platforms and compilers supported by
[mulle-c11](//github.com/mulle-c/mulle-c11/) and
[mulle-thread](//github.com/mulle-concurrent/mulle-thread/).

### You are here

![Overview](overview.dot.svg)

## Add

Use [mulle-sde](//github.com/mulle-sde) to add mulle-objc-debug to your project:

```
mulle-sde dependency add --github mulle-objc mulle-objc-debug
```

Executables will need to link with [mulle-objc-runtime-startup](//github.com/mulle-objc/mulle-objc-runtime-startup) as well.


## Install

See [mulle-objc-developer](//github.com/mulle-objc/mulle-objc-developer) for
the preferred way to installation mulle-objc-debug.


### mulle-sde

Use [mulle-sde](//github.com/mulle-sde) to build and install mulle-objc-debug and all dependencies:

```
mulle-sde install --prefix /usr/local \
   //github.com/mulle-objc/mulle-objc-debug/archive/latest.tar.gz
```

## Author

[Nat!](//www.mulle-kybernetik.com/weblog) for
[Mulle kybernetiK](//www.mulle-kybernetik.com) and
[Codeon GmbH](//www.codeon.de)

