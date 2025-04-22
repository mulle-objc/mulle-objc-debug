//
//  mulle_objc_dotdump.h
//  mulle-objc-debug
//
//  Created by Nat! on 25.10.15.
//  Copyright (c) 2015 Nat! - Mulle kybernetiK.
//  Copyright (c) 2015 Codeon GmbH.
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
#ifndef mulle_objc_dotdump_h__
#define mulle_objc_dotdump_h__

#include "include.h"

#include <stdio.h>

struct _mulle_objc_universe;
struct _mulle_objc_class;



enum mulle_objc_dotdump_options
{
   MULLE_OBJC_SHOW_FILELINK                = 0x000001,
   MULLE_OBJC_SHOW_UNIVERSE                = 0x000002,
   MULLE_OBJC_SHOW_INFRACLASS              = 0x000004,
   MULLE_OBJC_SHOW_METACLASS               = 0x000008,
   MULLE_OBJC_SHOW_SUPERCLASS              = 0x000010,
   MULLE_OBJC_SHOW_METHODLISTS             = 0x000020,
   MULLE_OBJC_SHOW_PROTOCOLCLASSES         = 0x000040,
   MULLE_OBJC_SHOW_SELECTORS               = 0x000080,
   MULLE_OBJC_SHOW_SUPERS                  = 0x000100,
   MULLE_OBJC_SHOW_PROTOCOLS               = 0x000200,
   MULLE_OBJC_SHOW_CATEGORIES              = 0x000400,
   MULLE_OBJC_SHOW_STRINGS                 = 0x000800,
   MULLE_OBJC_SHOW_FASTCLASSES             = 0x001000,
   MULLE_OBJC_SHOW_CLASSES                 = 0x002000,
   MULLE_OBJC_SHOW_CLASSESTOLOAD           = 0x004000,
   MULLE_OBJC_SHOW_CACHE                   = 0x008000,
   MULLE_OBJC_SHOW_CLASSFIELDS             = 0x010000,
   MULLE_OBJC_SHOW_METHODLISTFIELDS        = 0x020000,
   MULLE_OBJC_SHOW_CLASSPAIR               = 0x040000,
   MULLE_OBJC_SHOW_IVARLIST                = 0x080000,
   MULLE_OBJC_SHOW_PROPERTYLIST            = 0x100000,
   // more options not in show all
   MULLE_OBJC_SHOW_CLASS_HYPERLINK         = 0x200000,
   MULLE_OBJC_SHOW_PROTOCOLCLASS_HYPERLINK = 0x400000,
   MULLE_OBJC_SHOW_UNIVERSE_HYPERLINK      = 0x800000,
   MULLE_OBJC_SHOW_HYPERLINK               = 0x800000 + 0x400000 + 0x200000, // the above
   MULLE_OBJC_SHOW_ALL                     = ~MULLE_OBJC_SHOW_HYPERLINK,
   MULLE_OBJC_SHOW_DEFAULT                 = MULLE_OBJC_SHOW_ALL ^ (MULLE_OBJC_SHOW_STRINGS|MULLE_OBJC_SHOW_SELECTORS)
};


#pragma mark - preferred ABI

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_objc_universe_dotdump_to_directory( struct _mulle_objc_universe *universe,
                                                 char *directory);
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_objc_class_dotdump_to_directory( struct _mulle_objc_class *cls,
                                              char *directory,
                                              unsigned long options);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_objc_classhierarchy_dotdump_to_stream( struct _mulle_objc_class *cls,
                                                    FILE *fp);
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_objc_classhierarchy_dotdump_to_file( struct _mulle_objc_class *cls,
                                                  char *filename);

#pragma mark - "movie" support

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_objc_universe_dotdump_frame_to_directory( struct _mulle_objc_universe *universe,
                                                       char *directory);

#endif
