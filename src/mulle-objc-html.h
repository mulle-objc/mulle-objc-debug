//
//  mulle_objc_html.h
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
#ifndef mulle_objc_html_h__
#define mulle_objc_html_h__

#include "include.h"

#include <stdarg.h>
#include <stdint.h>
#include <mulle-objc-runtime/mulle-objc-runtime.h>

struct mulle_buffer;

struct _mulle_objc_universe;
struct _mulle_objc_staticstring;
struct _mulle_objc_class;
struct _mulle_objc_fastclasstable;
struct _mulle_objc_infraclass;
struct _mulle_objc_ivarlist;
struct _mulle_objc_propertylist;
struct _mulle_objc_cache;
struct _mulle_objc_methodlist;
struct _mulle_objc_descriptor;
struct _mulle_objc_uniqueidarray;

struct mulle_concurrent_pointerarray;
struct mulle_concurrent_hashmap;

struct _mulle_objc_htmltablestyle
{
   char   *title;
   char   *classprefix;  // no class prefix means no css emission
   char   *color;        // used if classprefix is not set
   char   *bgcolor;      // used if classprefix is not set
   int    colspan;       // optional
   char   **headers;     // NULL or array of static strings for column headers (only used if >2 columns)
};

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_universe( struct mulle_buffer *buffer,
                                       struct _mulle_objc_universe *universe,
                                       struct _mulle_objc_htmltablestyle *styling);

MULLE_OBJC_DEBUG_GLOBAL
void  mulle_buffer_describe_staticstring( struct mulle_buffer *buffer,
                                          struct _mulle_objc_staticstring *string,
                                          struct _mulle_objc_htmltablestyle *styling);
MULLE_OBJC_DEBUG_GLOBAL
char   *mulle_buffer_describe_staticstring_hor( struct _mulle_objc_staticstring *string,
                                                struct _mulle_objc_htmltablestyle *styling);
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_staticstring_row( struct mulle_buffer *buffer,
                                               void *value,
                                               struct _mulle_objc_htmltablestyle *styling);
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_staticstring_entry( struct mulle_buffer *buffer,
                                                 void *value,
                                                 struct _mulle_objc_htmltablestyle *styling,
                                                 void *userinfo);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_infraclass_row( struct mulle_buffer *buffer,
                                             intptr_t  classid,
                                             void *cls,
                                             struct _mulle_objc_htmltablestyle *styling);
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_fastclass_row( struct mulle_buffer *buffer,
                                            unsigned int i,
                                            struct _mulle_objc_infraclass *infra,
                                            struct _mulle_objc_htmltablestyle *styling);
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_fastclass( struct mulle_buffer *buffer,
                                        struct _mulle_objc_infraclass *cls,
                                        struct _mulle_objc_htmltablestyle *styling,
                                        void *userinfo);

MULLE_OBJC_DEBUG_GLOBAL
char   *mulle_buffer_describe_classtoload( intptr_t classid,
                                           void *cls,
                                           struct _mulle_objc_htmltablestyle *styling);
MULLE_OBJC_DEBUG_GLOBAL
char   *mulle_buffer_describe_categoriestoload_row( intptr_t classid,
                                                    void *cls,
                                                    struct _mulle_objc_htmltablestyle *styling);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_class( struct mulle_buffer *buffer,
                                    struct _mulle_objc_class *cls,
                                    int show_fields,
                                    struct _mulle_objc_htmltablestyle *styling);
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_class_short( struct mulle_buffer *buffer,
                                          struct _mulle_objc_class *cls,
                                          struct _mulle_objc_htmltablestyle *styling);
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_class_tiny( struct mulle_buffer *buffer,
                                         struct _mulle_objc_class *cls,
                                         struct _mulle_objc_htmltablestyle *styling);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_class_row( struct mulle_buffer *buffer,
                                        intptr_t classid,
                                        void *cls,
                                        struct _mulle_objc_htmltablestyle *styling);

MULLE_OBJC_DEBUG_GLOBAL
char   *mulle_buffer_describe_ivarlist( struct _mulle_objc_ivarlist *list,
                                        struct _mulle_objc_htmltablestyle *styling);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_ivarlist_hor( struct mulle_buffer *buffer,
                                           struct _mulle_objc_ivarlist *list,
                                           struct _mulle_objc_htmltablestyle *styling);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_propertylist( struct mulle_buffer *buffer,
                                           struct _mulle_objc_propertylist *list,
                                           struct _mulle_objc_htmltablestyle *styling);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_cache( struct mulle_buffer *buffer,
                                    struct _mulle_objc_cache *cache,
                                    struct _mulle_objc_universe *universe,
                                    struct _mulle_objc_htmltablestyle *styling);

MULLE_OBJC_DEBUG_GLOBAL
char   *mulle_buffer_describe_descriptor_html( struct _mulle_objc_descriptor *desc,
                                               struct _mulle_objc_htmltablestyle *styling);
MULLE_OBJC_DEBUG_GLOBAL
char   *mulle_buffer_describe_descriptor_hor( struct _mulle_objc_descriptor *desc);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_descriptor( struct mulle_buffer *buffer,
                                         intptr_t  methodid,
                                         void *descriptor,
                                         struct _mulle_objc_htmltablestyle *styling);
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_category( struct mulle_buffer *buffer,
                                       intptr_t  categoryid,
                                       void *value,
                                       struct _mulle_objc_htmltablestyle *styling);
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_protocol( struct mulle_buffer *buffer,
                                       intptr_t  protocolid,
                                       void *value,
                                       struct _mulle_objc_htmltablestyle *styling);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_super( struct mulle_buffer *buffer,
                                    intptr_t  superid,
                                    void *value,
                                    struct _mulle_objc_htmltablestyle *styling);
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_super_hashmap_entry( struct mulle_buffer *buffer,
                                                  intptr_t hash,
                                                  void *value,
                                                  struct _mulle_objc_htmltablestyle *styling,
                                                  void *userinfo);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_methodlist( struct mulle_buffer *buffer,
                                         struct _mulle_objc_methodlist *list,
                                         struct _mulle_objc_universe *universe,
                                         int show_fields,
                                         struct _mulle_objc_htmltablestyle *styling);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_methodlist_hor( struct mulle_buffer *buffer,
                                             struct _mulle_objc_methodlist *list,
                                             struct _mulle_objc_htmltablestyle *styling);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_protocolids( struct mulle_buffer *buffer,
                                          struct _mulle_objc_uniqueidarray *array,
                                          struct _mulle_objc_universe *universe,
                                          struct _mulle_objc_htmltablestyle *styling);
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_categoryids( struct mulle_buffer *buffer,
                                          struct _mulle_objc_uniqueidarray *array,
                                          struct _mulle_objc_universe *universe,
                                          struct _mulle_objc_htmltablestyle *styling);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_loadclass_row( struct mulle_buffer *buffer,
                                            void *value,
                                            struct _mulle_objc_htmltablestyle *styling,
                                            void *userinfo);
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_loadclass( struct mulle_buffer *buffer,
                                        void *value,
                                        struct _mulle_objc_htmltablestyle *styling,
                                        void *userinfo);
// Internal callback functions for hashmap usage
void   mulle_buffer_describe_class_hashmap_entry( struct mulle_buffer *buffer,
                                                  intptr_t classid,
                                                  void *cls,
                                                  struct _mulle_objc_htmltablestyle *styling,
                                                  void *userinfo);
void   mulle_buffer_describe_descriptor_hashmap_entry( struct mulle_buffer *buffer,
                                                       intptr_t methodid,
                                                       void *value,
                                                       struct _mulle_objc_htmltablestyle *styling,
                                                       void *userinfo);
void   mulle_buffer_describe_protocol_hashmap_entry( struct mulle_buffer *buffer,
                                                     intptr_t protocolid,
                                                     void *value,
                                                     struct _mulle_objc_htmltablestyle *styling,
                                                     void *userinfo);
void   mulle_buffer_describe_loadcategory_hashmap_entry( struct mulle_buffer *buffer,
                                                         intptr_t categoryid,
                                                         void *value,
                                                         struct _mulle_objc_htmltablestyle *styling,
                                                         void *userinfo);

MULLE_OBJC_DEBUG_GLOBAL
char   *mulle_buffer_describe_loadcategory( void *value,
                                            struct _mulle_objc_htmltablestyle *styling,
                                            void *userinfo);



typedef void   mulle_buffer_describe_class_t( struct mulle_buffer *,
                                              struct _mulle_objc_infraclass *,
                                              struct _mulle_objc_htmltablestyle *,
                                              void *userinfo);


MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_fastclasstable( struct mulle_buffer *buffer,
                                             struct _mulle_objc_fastclasstable *fastclasstable,
                                             mulle_buffer_describe_class_t *row_description,
                                             struct _mulle_objc_htmltablestyle *styling,
                                             void *userinfo);


typedef void   mulle_buffer_describe_pointer_t( struct mulle_buffer *,
                                                void *,
                                                struct _mulle_objc_htmltablestyle *,
                                                void *userinfo);


MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_concurrent_pointerarray( struct mulle_buffer *buffer,
                                                     struct mulle_concurrent_pointerarray *list,
                                                     mulle_buffer_describe_pointer_t *row_description,
                                                     struct _mulle_objc_htmltablestyle *styling,
                                                     void *userinfo);


typedef void   mulle_buffer_describe_hashmap_entry_t( struct mulle_buffer *,
                                                      intptr_t,
                                                      void *,
                                                      struct _mulle_objc_htmltablestyle *,
                                                      void *userinfo);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_concurrent_hashmap( struct mulle_buffer *buffer,
                                                struct mulle_concurrent_hashmap *map,
                                                mulle_buffer_describe_hashmap_entry_t *row_description,
                                                struct _mulle_objc_htmltablestyle *styling,
                                                void *userinfo);


typedef void   mulle_buffer_describe_uniqueid_t( struct mulle_buffer *,
                                                 mulle_objc_uniqueid_t uniqueid,
                                                 struct _mulle_objc_htmltablestyle *,
                                                 void *userinfo);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_describe_uniqueidarray( struct mulle_buffer *buffer,
                                                struct _mulle_objc_uniqueidarray *array,
                                                mulle_buffer_describe_uniqueid_t  *row_description,
                                                struct _mulle_objc_universe *universe,
                                                struct _mulle_objc_htmltablestyle *styling);

// Test function for new header functionality
MULLE_OBJC_DEBUG_GLOBAL
char   *mulle_objc_test_headers_html( void);

#endif /* mulle_objc_html_h */
