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

// Core table header function
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_add_table_header_colspan( struct mulle_buffer *buffer,
                                              struct _mulle_objc_htmltablestyle *styling,
                                              unsigned int colspan);


// Internal function used by dotdump.c

//MULLE_OBJC_DEBUG_GLOBAL
//void   mulle_buffer_html_infraclass_row( struct mulle_buffer *buffer,
//                                         intptr_t  classid,
//                                         void *cls,
//                                         struct _mulle_objc_htmltablestyle *styling);
//MULLE_OBJC_DEBUG_GLOBAL
//void   mulle_buffer_html_fastclass_row( struct mulle_buffer *buffer,
//                                        unsigned int i,
//                                        struct _mulle_objc_infraclass *infra,
//                                        struct _mulle_objc_htmltablestyle *styling);


//MULLE_OBJC_DEBUG_GLOBAL
//char   *mulle_buffer_html_classtoload( intptr_t classid,
//                                       void *cls,
//                                       struct _mulle_objc_htmltablestyle *styling);
//MULLE_OBJC_DEBUG_GLOBAL
//char   *mulle_buffer_html_categoriestoload_row( intptr_t classid,
//                                                void *cls,
//                                                struct _mulle_objc_htmltablestyle *styling);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_class( struct mulle_buffer *buffer,
                                struct _mulle_objc_class *cls,
                                int show_fields,
                                struct _mulle_objc_htmltablestyle *styling);
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_class_short( struct mulle_buffer *buffer,
                                      struct _mulle_objc_class *cls,
                                      struct _mulle_objc_htmltablestyle *styling);
MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_class_tiny( struct mulle_buffer *buffer,
                                     struct _mulle_objc_class *cls,
                                     struct _mulle_objc_htmltablestyle *styling);



MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_ivarlist_hor( struct mulle_buffer *buffer,
                                       struct _mulle_objc_ivarlist *list,
                                       struct _mulle_objc_htmltablestyle *styling);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_propertylist( struct mulle_buffer *buffer,
                                       struct _mulle_objc_propertylist *list,
                                       struct _mulle_objc_htmltablestyle *styling);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_cache( struct mulle_buffer *buffer,
                                struct _mulle_objc_cache *cache,
                                struct _mulle_objc_universe *universe,
                                struct _mulle_objc_htmltablestyle *styling);


MULLE_OBJC_DEBUG_GLOBAL
void  mulle_buffer_html_staticstring( struct mulle_buffer *buffer,
                                      struct _mulle_objc_staticstring *string,
                                      struct _mulle_objc_htmltablestyle *styling);


MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_methodlist( struct mulle_buffer *buffer,
                                     struct _mulle_objc_methodlist *list,
                                     struct _mulle_objc_universe *universe,
                                     int show_fields,
                                     struct _mulle_objc_htmltablestyle *styling);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_methodlist_hor( struct mulle_buffer *buffer,
                                         struct _mulle_objc_methodlist *list,
                                         struct _mulle_objc_htmltablestyle *styling);



MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_universe( struct mulle_buffer *buffer,
                                   struct _mulle_objc_universe *universe,
                                   struct _mulle_objc_htmltablestyle *styling);

// Callback functions for array usage
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


MULLE_OBJC_DEBUG_GLOBAL
char   *mulle_buffer_html_loadcategory_element( struct mulle_buffer *buffer,
                                                unsigned int row,
                                                void *value,
                                                struct _mulle_objc_htmltablestyle *styling,
                                                void *userinfo);

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

// Callback functions for hashmap usage
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

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_super_entry( struct mulle_buffer *buffer,
                                      intptr_t  superid,
                                      void *value,
                                      struct _mulle_objc_htmltablestyle *styling,
                                      void *userinfo);



typedef void   mulle_buffer_html_fastclasstable_callback_t( struct mulle_buffer *,
                                                            unsigned int row,
                                                            struct _mulle_objc_infraclass *,
                                                            struct _mulle_objc_htmltablestyle *,
                                                            void *userinfo);


MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_fastclasstable( struct mulle_buffer *buffer,
                                         struct _mulle_objc_fastclasstable *fastclasstable,
                                         mulle_buffer_html_fastclasstable_callback_t *row_description,
                                         struct _mulle_objc_htmltablestyle *styling,
                                         void *userinfo);


typedef void   mulle_buffer_html_concurrent_pointerarray_callback_t( struct mulle_buffer *,
                                                                     unsigned int row,
                                                                     void *,
                                                                     struct _mulle_objc_htmltablestyle *,
                                                                     void *userinfo);


MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_concurrent_pointerarray( struct mulle_buffer *buffer,
                                                  struct mulle_concurrent_pointerarray *list,
                                                  mulle_buffer_html_concurrent_pointerarray_callback_t *row_description,
                                                  struct _mulle_objc_htmltablestyle *styling,
                                                  void *userinfo);


typedef void   mulle_buffer_html_hashmap_callback_t( struct mulle_buffer *,
                                                     intptr_t,
                                                     void *,
                                                     struct _mulle_objc_htmltablestyle *,
                                                     void *userinfo);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_concurrent_hashmap( struct mulle_buffer *buffer,
                                                struct mulle_concurrent_hashmap *map,
                                                mulle_buffer_html_hashmap_callback_t *row_description,
                                                struct _mulle_objc_htmltablestyle *styling,
                                                void *userinfo);


typedef void   mulle_buffer_html_uniqueidarray_callback_t( struct mulle_buffer *,
                                                           unsigned int row,
                                                           mulle_objc_uniqueid_t uniqueid,
                                                           struct _mulle_objc_htmltablestyle *,
                                                           void *userinfo);

MULLE_OBJC_DEBUG_GLOBAL
void   mulle_buffer_html_uniqueidarray( struct mulle_buffer *buffer,
                                        struct _mulle_objc_uniqueidarray *array,
                                        mulle_buffer_html_uniqueidarray_callback_t  *row_description,
                                        struct _mulle_objc_htmltablestyle *styling,
                                        void *userinfo);


//MULLE_OBJC_DEBUG_GLOBAL
//char   *mulle_buffer_html_staticstring_hor( struct _mulle_objc_staticstring *string,
//                                            struct _mulle_objc_htmltablestyle *styling);
//
//
////MULLE_OBJC_DEBUG_GLOBAL
//char   *mulle_objc_ivarlist_html( struct _mulle_objc_ivarlist *list,
//                                  struct _mulle_objc_htmltablestyle *styling);
//
// MULLE_OBJC_DEBUG_GLOBAL
// char   *mulle_objc_descriptor_html( struct _mulle_objc_descriptor *desc,
//                                     struct _mulle_objc_htmltablestyle *styling);
//
// MULLE_OBJC_DEBUG_GLOBAL
// char   *mulle_objc_descriptor_html_hor( struct _mulle_objc_descriptor *desc);


#endif /* mulle_objc_html_h */
