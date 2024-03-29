//
//  mulle_objc_htmldump.h
//  mulle-objc-debug
//
//  Created by Nat! on 10.05.16.
//  Copyright (c) 2016 Nat! - Mulle kybernetiK.
//  Copyright (c) 2016 Codeon GmbH.
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//  Redistributions of source code must retain the above copyright notice, this
//  list of conditions and the following disclaimer.
//
//  Redistributions in binary form must reproduce the above copyright notice,
//  this list of conditions and the following disclaimer in the documentation
//  and/or other materials provided with the distribution.
//
//  Neither the name of Mulle kybernetiK nor the names of its contributors
//  may be used to endorse or promote products derived from this software
//  without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
#ifndef mulle_objc_htmldump_h__
#define mulle_objc_htmldump_h__

#include "include.h"

struct _mulle_objc_classpair;
struct _mulle_objc_universe;
struct _mulle_objc_class;

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_objc_classpair_htmldump_to_directory( struct _mulle_objc_classpair *pair,
                                                  char *directory);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_objc_universe_htmldump_to_directory( struct _mulle_objc_universe *universe,
                                                  char *directory);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_objc_class_htmldump_to_directory( struct _mulle_objc_class *cls,
                                               char *directory);

// sometimes nice in the debugger
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_objc_object_htmldump_class_to_directory( void *obj,
                                                      char *directory);

#endif
