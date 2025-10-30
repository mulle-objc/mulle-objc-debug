//
//  mulle_objc_dotdump.c
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
#include "mulle-objc-dotdump.h"

#include "include-private.h"

#include "mulle-objc-html.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "c-set.inc"


// static void   mulle_buffer_write_to_file( struct mulle_buffer *buffer, FILE *fp)
// {
//    if( buffer && fp)
//    {
//       size_t   len;
//       char     *data;
//
//       len  = mulle_buffer_get_length( buffer);
//       data = mulle_buffer_get_bytes( buffer);
//
//       if( len > 0 && data)
//          fwrite( data, 1, len, fp);
//    }
// }


static char   *html_escape( char *s)
{
   if( ! strchr( s, '&') && ! strchr( s, '<'))
      return( s);

   return( "bad-html");
}



//// Callback wrapper for mulle_buffer_html_pointer_t
//static void   _mulle_objc_loadcategory_html_buffer_entry( struct mulle_buffer *buffer,
//                                                              void *value,
//                                                              struct _mulle_objc_htmltablestyle *styling,
//                                                              void *userinfo)
//{
//   char   *row;
//
//   row = mulle_buffer_html_loadcategory_element( (intptr_t) 0, value, styling);
//   if( row)
//   {
//      mulle_buffer_add_string( buffer, row);
//      mulle_free( row);
//   }
//}



static void   mulle_buffer_append_dot_filename_for_name( struct mulle_buffer *buffer,
                                                         char *name,
                                                         char *directory)
{
   char   separator;

#ifdef _WIN32
    separator = '\\';
#else
    separator = '/';
#endif

   mulle_buffer_sprintf( buffer, "%s%c%s.dot", directory, separator, html_escape( name));
}


# pragma mark - "styling"

static struct _mulle_objc_htmltablestyle   infraclass_style =
{
   "infraclass",
   NULL,
   "white",
   "blue",
   0
};


static struct _mulle_objc_htmltablestyle   metaclass_style =
{
   "metaclass",
   NULL,
   "white",
   "goldenrod",
   0
};



static struct _mulle_objc_htmltablestyle   methodlist_style =
{
   "methodlist",
   NULL,
   "white",
   "black",
   0
};


static struct _mulle_objc_htmltablestyle   cachetable_style =
{
   "cache",
   NULL,
   "white",
   "black",
   0
};


static struct _mulle_objc_htmltablestyle   ivarlist_style =
{
   "ivarlist",
   NULL,
   "white",
   "black",
   0
};


static struct _mulle_objc_htmltablestyle   propertylist_style =
{
   "propertylist",
   NULL,
   "white",
   "black",
   0
};


static char  *descriptortable_headers[] =
{
   "name", "signature", "methodid", "flags"
};

static struct _mulle_objc_htmltablestyle  descriptortable_style =
{
   "selectors",
   "selector",
   NULL,
   NULL,
   4,
   descriptortable_headers
};


static struct _mulle_objc_htmltablestyle   staticstringtable_title =
{
   "strings",
   NULL,
   "black",
   "chartreuse",
   0
};



static struct _mulle_objc_htmltablestyle   fastclasstable_title =
{
   "fastclasses",
   NULL,
   "white",
   "blue",
   0
};


static struct _mulle_objc_htmltablestyle   classtable_style =
{
   "classes",
   NULL,
   "white",
   "blue",
   0
};


static struct _mulle_objc_htmltablestyle   categorytable_style =
{
   "categories",
   NULL,
   "white",
   "dimgray",
   0
};


static struct _mulle_objc_htmltablestyle   supertable_style =
{
   "supers",
   NULL,
   "white",
   "dimgray",
   0
};


static struct _mulle_objc_htmltablestyle   protocoltable_style =
{
   "protocols",
   NULL,
   "white",
   "dimgray",
   0
};


static struct _mulle_objc_htmltablestyle   classestoload_style =
{
   "classes to load",
   NULL,
   "white",
   "indigo",
   0
};


static struct _mulle_objc_htmltablestyle   categoriestoload_style =
{
   "categories to load",
   NULL,
   "white",
   "indigo",
   0
};


static struct _mulle_objc_htmltablestyle   universe_style =
{
   "universe",
   NULL,
   "white",
   "red",
   0
};


# pragma mark - walker universe callback


// Callback wrapper for mulle_buffer_html_entry_t
static void   _mulle_buffer_html_loadclasslist_entry( struct mulle_buffer *buffer,
                                                      intptr_t hash,
                                                      void *value,
                                                      struct _mulle_objc_htmltablestyle *styling,
                                                      void *userinfo)
{
   struct mulle_concurrent_pointerarray   *array = value;

   mulle_buffer_html_concurrent_pointerarray( buffer,
                                              array,
                                              mulle_buffer_html_loadclass_element,
                                              styling,
                                              userinfo);
}



struct dump_info
{
   c_set           set;
   char            *directory;
   unsigned long   options;
};


struct callback_info
{
   struct mulle_buffer   *buffer;
   struct dump_info      info;
};



static void   _mulle_buffer_dot_universe( struct mulle_buffer *buffer,
                                          struct _mulle_objc_universe *universe,
                                          struct dump_info *info)
{
   int    i;

   mulle_buffer_sprintf( buffer, "\"%p\" [ label=<", universe);
   mulle_buffer_html_universe( buffer, universe, &universe_style);
   mulle_buffer_sprintf( buffer, ">, shape=\"%s\", URL=\"file:///%s/overview.dot\"  ];\n",
                      "component",
                      info->directory);

   if( info->options & MULLE_OBJC_SHOW_SELECTORS)
      if( mulle_concurrent_hashmap_count( &universe->descriptortable))
      {
         mulle_buffer_sprintf( buffer, "\"%p\" -> \"%p\" [ label=\"descriptortable\" ];\n",
                 universe, &universe->descriptortable);

         mulle_buffer_sprintf( buffer, "\"%p\" [ label=<", &universe->descriptortable);
         mulle_buffer_html_concurrent_hashmap( buffer,
                                               &universe->descriptortable,
                                               mulle_buffer_html_descriptor_entry,
                                               &descriptortable_style,
                                               NULL);
         mulle_buffer_sprintf( buffer, ">, shape=\"%s\" ];\n", "box");
      }

   if( info->options & MULLE_OBJC_SHOW_PROTOCOLS)
      if( mulle_concurrent_hashmap_count( &universe->protocoltable))
      {
         mulle_buffer_sprintf( buffer, "\"%p\" -> \"%p\" [ label=\"protocoltable\" ];\n",
                 universe, &universe->protocoltable);

         mulle_buffer_sprintf( buffer, "\"%p\" [ label=<", &universe->protocoltable);
         mulle_buffer_html_concurrent_hashmap( buffer,
                                               &universe->protocoltable,
                                               mulle_buffer_html_protocol_entry,
                                               &protocoltable_style,
                                               NULL);
         mulle_buffer_sprintf( buffer, ">, shape=\"%s\" ];\n", "box");
      }

   if( info->options & MULLE_OBJC_SHOW_CATEGORIES)
      if( mulle_concurrent_hashmap_count( &universe->categorytable))
      {
         mulle_buffer_sprintf( buffer, "\"%p\" -> \"%p\" [ label=\"categorytable\" ];\n",
                 universe, &universe->categorytable);

         mulle_buffer_sprintf( buffer, "\"%p\" [ label=<", &universe->categorytable);
         mulle_buffer_html_concurrent_hashmap( buffer,
                                               &universe->categorytable,
                                               mulle_buffer_html_loadcategory_entry,
                                               &categorytable_style,
                                               NULL);
         mulle_buffer_sprintf( buffer, ">, shape=\"%s\" ];\n", "box");
      }

   if( info->options & MULLE_OBJC_SHOW_SUPERS)
      if( mulle_concurrent_hashmap_count( &universe->supertable))
      {
         mulle_buffer_sprintf( buffer, "\"%p\" -> \"%p\" [ label=\"supertable\" ];\n",
                 universe, &universe->supertable);

         mulle_buffer_sprintf( buffer, "\"%p\" [ label=<", &universe->supertable);
         mulle_buffer_html_concurrent_hashmap( buffer,
                                               &universe->supertable,
                                               mulle_buffer_html_super_entry,
                                               &supertable_style,
                                               NULL);
         mulle_buffer_sprintf( buffer, ">, shape=\"%s\" ];\n", "box");
      }

   if( info->options & MULLE_OBJC_SHOW_STRINGS)
      if( mulle_concurrent_pointerarray_get_count( &universe->staticstrings))
      {
         mulle_buffer_sprintf( buffer, "\"%p\" [ label=<", &universe->staticstrings);
         mulle_buffer_html_concurrent_pointerarray( buffer,
                                                    &universe->staticstrings,
                                                    mulle_buffer_html_staticstring_element,
                                                    &staticstringtable_title,
                                                    NULL);
         mulle_buffer_sprintf( buffer, ">, shape=\"%s\" ];\n", "box");
      }

   if( info->options & MULLE_OBJC_SHOW_FASTCLASSES)
   {
      for( i = 0; i < MULLE_OBJC_S_FASTCLASSES; i++)
      {
         if( _mulle_atomic_pointer_read_nonatomic( &universe->fastclasstable.classes[ i].pointer))
            break;
      }

      if( i < MULLE_OBJC_S_FASTCLASSES)
      {
         mulle_buffer_sprintf( buffer, "\"%p\" -> \"%p\" [ label=\"fastclasses\" ];\n",
                 universe, &universe->fastclasstable);
         mulle_buffer_sprintf( buffer, "\"%p\" [ label=<", &universe->fastclasstable);

         mulle_buffer_html_fastclasstable( buffer, &universe->fastclasstable,
                                                   mulle_buffer_html_fastclass_element,
                                                   &fastclasstable_title,
                                                   NULL);

         mulle_buffer_sprintf( buffer, ">, shape=\"%s\" ];\n", "box");
      }
   }

   if( info->options & MULLE_OBJC_SHOW_CLASSES)
      if( mulle_concurrent_hashmap_count( &universe->classtable))
      {
         mulle_buffer_sprintf( buffer, "\"%p\" -> \"%p\" [ label=\"classes\" ];\n",
                 universe, &universe->classtable);

         mulle_buffer_sprintf( buffer, "\"%p\" [ label=<", &universe->classtable);

         mulle_buffer_html_concurrent_hashmap( buffer,
                                               &universe->classtable,
                                               mulle_buffer_html_class_entry,
                                               &classtable_style,
                                               NULL);

         mulle_buffer_sprintf( buffer, ">, shape=\"%s\" ];\n", "box");
      }

   if( info->options & MULLE_OBJC_SHOW_CLASSESTOLOAD)
   {
      if( mulle_concurrent_hashmap_count( &universe->waitqueues.classestoload))
      {
         mulle_buffer_sprintf( buffer, "\"%p\" -> \"%p\" [ label=\"classestoload\" ];\n",
                 universe, &universe->waitqueues.classestoload);

         mulle_buffer_sprintf( buffer, "\"%p\" [ label=<", &universe->waitqueues.classestoload);

         mulle_buffer_html_concurrent_hashmap( buffer,
                                               &universe->waitqueues.classestoload,
                                               _mulle_buffer_html_loadclasslist_entry,
                                               &classestoload_style,
                                               NULL);

         mulle_buffer_sprintf( buffer, ">, shape=\"%s\" ];\n", "box");
      }

      if( mulle_concurrent_hashmap_count( &universe->waitqueues.categoriestoload))
      {
         mulle_buffer_sprintf( buffer, "\"%p\" -> \"%p\" [ label=\"categoriestoload\" ];\n",
                 universe, &universe->waitqueues.categoriestoload);

         mulle_buffer_sprintf( buffer, "\"%p\" [ label=<", &universe->waitqueues.categoriestoload);

         mulle_buffer_html_concurrent_hashmap( buffer,
                                               &universe->waitqueues.categoriestoload,
                                               mulle_buffer_html_loadcategory_entry,
                                               &categoriestoload_style,
                                               NULL);

         mulle_buffer_sprintf( buffer, ">, shape=\"%s\" ];\n", "box");
      }
   }
   mulle_buffer_sprintf( buffer, "\n\n");
}


static void   _mulle_buffer_dot_hyper_universe( struct mulle_buffer *buffer,
                                                struct _mulle_objc_universe *universe,
                                                struct dump_info *info)
{
   mulle_buffer_sprintf( buffer, "\"%p\" [ label=<", universe);
   mulle_buffer_html_universe( buffer, universe, &universe_style);
   mulle_buffer_sprintf( buffer, ">, shape=\"%s\", URL=\"file:///%s/universe.dot\"  ];\n",
      "component", info->directory);
}


# pragma mark - walker class callback


static void   _mulle_buffer_dot_infraclass( struct mulle_buffer *buffer,
                                            struct _mulle_objc_infraclass *infra,
                                            struct dump_info *info);
static void   _mulle_buffer_dot_metaclass( struct mulle_buffer *buffer,
                                           struct _mulle_objc_metaclass *meta,
                                           struct dump_info *info);
static void   _mulle_buffer_dot_hyper_infraclass( struct mulle_buffer *buffer,
                                                  struct _mulle_objc_infraclass *infra,
                                                  struct dump_info *info);

extern char   *_mulle_objc_grapviz_html_header_description( char *name, int is_meta);


static void   _mulle_buffer_dot_protocolclasses( struct mulle_buffer *buffer,
                                                 struct _mulle_objc_class *cls,
                                                 struct dump_info *info)
{
   struct _mulle_objc_protocolclassenumerator   rover;
   struct _mulle_objc_infraclass                *prop_cls;
   unsigned int                                 i;
   struct _mulle_objc_classpair                 *pair;

   i     = 0;
   pair  = _mulle_objc_class_get_classpair( cls);
   rover = _mulle_objc_classpair_enumerate_protocolclasses( pair);
   while( prop_cls = _mulle_objc_protocolclassenumerator_next( &rover))
   {
      mulle_buffer_sprintf( buffer, "\"%p\" -> \"%p\"  [ label=\"protocol class #%u\" ];\n",
              cls, prop_cls, i++);

      if( ! c_set_member( &info->set, prop_cls))
      {
         c_set_add( &info->set, prop_cls);
         if( info->options & MULLE_OBJC_SHOW_PROTOCOLCLASS_HYPERLINK)
            _mulle_buffer_dot_hyper_infraclass( buffer, prop_cls, info);
         else
         {
            _mulle_buffer_dot_infraclass( buffer, prop_cls, info);
            if( info->options & MULLE_OBJC_SHOW_METACLASS)
               _mulle_buffer_dot_metaclass( buffer, _mulle_objc_infraclass_get_metaclass( prop_cls), info);
         }
      }
   }
   _mulle_objc_protocolclassenumerator_done( &rover);
}


static void   _mulle_buffer_dot_methodlists( struct mulle_buffer *buffer,
                                             struct _mulle_objc_class *cls,
                                             struct dump_info *info)
{
   unsigned int                                     i;
   struct mulle_concurrent_pointerarrayenumerator   rover;
   struct _mulle_objc_methodlist                    *methodlist;
   struct _mulle_objc_universe                      *universe;

   universe = _mulle_objc_class_get_universe( cls);

   i = 0;
   rover = mulle_concurrent_pointerarray_enumerate( &cls->methodlists);
   while( methodlist = _mulle_concurrent_pointerarrayenumerator_next( &rover))
   {
      if( methodlist->n_methods)
      {
         mulle_buffer_sprintf( buffer, "\"%p\" -> \"%p\"  [ label=\"methodlist #%u\" ];\n",
                  cls, methodlist, i++);

         mulle_buffer_sprintf( buffer, "\"%p\" [ label=<", methodlist);
         mulle_buffer_html_methodlist( buffer,
                                       methodlist,
                                       universe,
                                       info->options & MULLE_OBJC_SHOW_METHODLISTFIELDS,
                                       &methodlist_style);
         mulle_buffer_sprintf( buffer, ">, shape=\"none\" ];\n");
      }
   }
   mulle_concurrent_pointerarrayenumerator_done( &rover);
}


static void   _mulle_buffer_dot_cache( struct mulle_buffer *buffer,
                                       struct _mulle_objc_class *cls,
                                       struct dump_info *info)
{
   struct _mulle_objc_cache        *cache;
   struct _mulle_objc_universe     *universe;

   universe = _mulle_objc_class_get_universe( cls);

   cache = _mulle_objc_cachepivot_get_cache_atomic( &cls->cachepivot.pivot);
   if( _mulle_atomic_pointer_read_nonatomic( &cache->n))
   {
      mulle_buffer_sprintf( buffer, "\"%p\" -> \"%p\"  [ label=\"cache\" ];\n",
              cls, cache);

      mulle_buffer_sprintf( buffer, "\"%p\" [ label=<", cache);
      mulle_buffer_html_cache( buffer, cache, universe, &cachetable_style);
      mulle_buffer_sprintf( buffer, ">, shape=\"none\" ];\n");
   }
}



static void   _mulle_buffer_dot_class( struct mulle_buffer *buffer,
                                       struct _mulle_objc_class *cls,
                                       struct dump_info *info,
                                       int is_meta)
{
   struct _mulle_objc_htmltablestyle   style;


   if( info->options & MULLE_OBJC_SHOW_UNIVERSE)
   {
      struct _mulle_objc_universe   *universe;

      universe = _mulle_objc_class_get_universe( cls);
      if( universe)
         mulle_buffer_sprintf( buffer, "\"%p\" -> \"%p\"  [ label=\"universe\" ];\n",
                     cls,
                     _mulle_objc_class_get_universe( cls));
   }

   // meta superclass is boring
   if( info->options & MULLE_OBJC_SHOW_SUPERCLASS)
   {
      if( ! is_meta)
      {
         struct _mulle_objc_class   *superclass;

         superclass = _mulle_objc_class_get_superclass( cls);
         if( superclass)
         {
            mulle_buffer_sprintf( buffer, "\"%p\" -> \"%p\"  [ label=\"super\"; penwidth=\"%d\" ];\n", cls, superclass,
                    _mulle_objc_class_is_infraclass( cls)
                    ? 3 : 1);
//
//            // also show protocolclasses of superclass
//            if( ! (_mulle_objc_class_get_inheritance( cls) & MULLE_OBJC_CLASS_DONT_INHERIT_PROTOCOLS))
//            {
//               struct dump_info   secondary_info;
//
//               secondary_info         = *info;
//               secondary_info.options = MULLE_OBJC_SHOW_INFRACLASS|MULLE_OBJC_SHOW_FILELINK;
//               _mulle_buffer_dot_protocolclasses( superclass, &secondary_info);
//            }
         }
      }
   }

   style       = is_meta ? metaclass_style : infraclass_style;
   style.title = cls->name;

   mulle_buffer_sprintf( buffer, "\"%p\" [ label=<", cls);
   mulle_buffer_html_class( buffer, cls, info->options & MULLE_OBJC_SHOW_CLASSFIELDS, &style);
   mulle_buffer_sprintf( buffer, ">, shape=\"%s\"", is_meta ? "component" : "box");

   if( info->options & MULLE_OBJC_SHOW_FILELINK)
   {
      //
      // graphviz seemingly can only open absolute path links.
      // For svg you need to rewrite it anyway to .svg extension
      // so cut off the "file:///tmp/" prefix there with
      // `sed 's|URL=\"file:///tmp/\([^"]*\).dot\"|URL=\"\1.svg\"|'`
      // batch convert:
      // for i in *.dot;
      // do
      //    name="`basename -- "$i" .dot`"
      //    sed 's|URL=\"file:///tmp/\([^"]*\).dot\"|URL=\"\1.svg\"|' "$i" | dot -Tsvg -o "${name}.svg"
      //  done
      // Note: URL must be uppercase
      mulle_buffer_sprintf( buffer, " URL=\"file:///%s/%s.dot\"", info->directory, html_escape( cls->name));
   }
   mulle_buffer_sprintf( buffer, " ];\n");


   // dont draw relationships, if metaclass is not drawn
   if( info->options & MULLE_OBJC_SHOW_METACLASS)
   {
      struct _mulle_objc_metaclass   *meta;

      meta = _mulle_objc_class_get_metaclass( cls);
      if( meta)
         mulle_buffer_sprintf( buffer, "\"%p\" -> \"%p\"  [ label=\"isa/meta\"; color=\"%s\"; fontcolor=\"%s\" ];\n",
                                  cls, meta, metaclass_style.bgcolor, metaclass_style.bgcolor);

      if( info->options & MULLE_OBJC_SHOW_INFRACLASS)
      {
         struct _mulle_objc_infraclass   *infra;

         infra = _mulle_objc_class_get_infraclass( cls);
         if( infra)
            mulle_buffer_sprintf( buffer, "\"%p\" -> \"%p\"  [ label=\"infra\"; color=\"%s\"; fontcolor=\"%s\" ];\n",
                                      cls, infra, infraclass_style.bgcolor, infraclass_style.bgcolor);
      }
   }

   if( info->options & MULLE_OBJC_SHOW_PROTOCOLCLASSES)
   {
      if( ! (_mulle_objc_class_get_inheritance( cls) & MULLE_OBJC_CLASS_DONT_INHERIT_PROTOCOLS))
      {
         _mulle_buffer_dot_protocolclasses( buffer, cls, info);
         mulle_buffer_sprintf( buffer, "\n\n");
      }
   }

   if( info->options & MULLE_OBJC_SHOW_METHODLISTS)
   {
      _mulle_buffer_dot_methodlists( buffer, cls, info);
   }

   if( info->options & MULLE_OBJC_SHOW_CACHE)
   {
      _mulle_buffer_dot_cache( buffer, cls, info);
   }
}


static void   _mulle_buffer_dot_classpair( struct mulle_buffer *buffer,
                                           struct _mulle_objc_classpair *pair,
                                           struct _mulle_objc_class *cls,
                                           struct dump_info *info)
{
   struct _mulle_objc_universe        *universe;
   struct _mulle_objc_uniqueidarray   *array;

   universe = _mulle_objc_classpair_get_universe( pair);

   array = _mulle_atomic_pointer_read( &pair->p_protocolids.pointer);
   if( array->n)
   {
      mulle_buffer_sprintf( buffer, "\"%p\" -> \"%p\"  [ label=\"protocolids\" ];\n",
              cls, array);

      mulle_buffer_sprintf( buffer, "\"%p\" [ label=<", array);
      mulle_buffer_html_uniqueidarray( buffer, array,
                                               mulle_buffer_html_protocolid_element,
                                               &protocoltable_style,
                                               universe);
      mulle_buffer_sprintf( buffer, ">, shape=\"none\" ];\n");
   }

   array = _mulle_atomic_pointer_read( &pair->p_categoryids.pointer);
   if( array->n)
   {
      mulle_buffer_sprintf( buffer, "\"%p\" -> \"%p\"  [ label=\"categoryids\" ];\n",
              cls, array);

      mulle_buffer_sprintf( buffer, "\"%p\" [ label=<", array);
      mulle_buffer_html_uniqueidarray( buffer, array,
                                               mulle_buffer_html_categoryid_element,
                                               &categorytable_style,
                                               universe);
      mulle_buffer_sprintf( buffer, ">, shape=\"none\" ];\n");
   }
}


static void   _mulle_buffer_dot_infraclass( struct mulle_buffer *buffer,
                                            struct _mulle_objc_infraclass *infra,
                                            struct dump_info *info)
{
   struct mulle_concurrent_pointerarrayenumerator   rover;
   struct _mulle_objc_ivarlist                      *ivarlist;
   struct _mulle_objc_propertylist                  *propertylist;
   unsigned int                                     i;

   _mulle_buffer_dot_class( buffer, _mulle_objc_infraclass_as_class( infra), info, 0);

   if( info->options & MULLE_OBJC_SHOW_IVARLIST)
   {
      i = 0;
      rover = mulle_concurrent_pointerarray_enumerate( &infra->ivarlists);
      while( ivarlist = _mulle_concurrent_pointerarrayenumerator_next( &rover))
      {
         if( ivarlist->n_ivars)
         {
            mulle_buffer_sprintf( buffer, "\"%p\" -> \"%p\"  [ label=\"ivarlist #%u\" ];\n",
                    infra, ivarlist, i++);

            mulle_buffer_sprintf( buffer, "\"%p\" [ label=<", ivarlist);
            mulle_buffer_html_ivarlist_hor( buffer, ivarlist, &ivarlist_style);
            mulle_buffer_sprintf( buffer, ">, shape=\"none\" ];\n");
         }
      }
      mulle_concurrent_pointerarrayenumerator_done( &rover);
   }

   if( info->options & MULLE_OBJC_SHOW_PROPERTYLIST)
   {
      i = 0;
      rover = mulle_concurrent_pointerarray_enumerate( &infra->propertylists);
      while( propertylist = _mulle_concurrent_pointerarrayenumerator_next( &rover))
      {
         if( propertylist->n_properties)
         {
            mulle_buffer_sprintf( buffer, "\"%p\" -> \"%p\"  [ label=\"propertylist #%u\" ];\n",
                    infra, propertylist, i++);

            mulle_buffer_sprintf( buffer, "\"%p\" [ label=<", propertylist);
            mulle_buffer_html_propertylist( buffer, propertylist, &propertylist_style);
            mulle_buffer_sprintf( buffer, ">, shape=\"none\" ];\n");
         }
      }
      mulle_concurrent_pointerarrayenumerator_done( &rover);
   }

   if( info->options & MULLE_OBJC_SHOW_CLASSPAIR)
      _mulle_buffer_dot_classpair( buffer,
                                   _mulle_objc_infraclass_get_classpair( infra),
                                   _mulle_objc_infraclass_as_class( infra),
                                   info);
}


static void   _mulle_buffer_dot_hyper_infraclass( struct mulle_buffer *buffer,
                                                  struct _mulle_objc_infraclass *infra,
                                                  struct dump_info *info)
{
   struct _mulle_objc_htmltablestyle   style;
   struct _mulle_objc_infraclass       *superclass;

   style       = infraclass_style;
   style.title = html_escape( infra->base.name);

   mulle_buffer_sprintf( buffer, "\"%p\" [ label=<", infra);
   mulle_buffer_html_class_tiny( buffer, _mulle_objc_infraclass_as_class( infra), &style);
   mulle_buffer_sprintf( buffer, ">, shape=\"box\", URL=\"file:///%s/%s.dot\" ];\n", info->directory, style.title);

   if( info->options & MULLE_OBJC_SHOW_UNIVERSE)
      if( _mulle_objc_infraclass_get_universe( infra))
         mulle_buffer_sprintf( buffer, "\"%p\" -> \"%p\"  [ label=\"universe\" ];\n", infra,  _mulle_objc_infraclass_get_universe( infra));

   if( info->options & MULLE_OBJC_SHOW_SUPERCLASS)
   {
      superclass = _mulle_objc_infraclass_get_superclass( infra);
      if( superclass)
         mulle_buffer_sprintf( buffer, "\"%p\" -> \"%p\"  [ label=\"super\" ];\n",
                 infra, superclass);
   }
}



static void   _mulle_buffer_dot_metaclass( struct mulle_buffer *buffer,
                                           struct _mulle_objc_metaclass *meta,
                                           struct dump_info *info)
{
   _mulle_buffer_dot_class( buffer, _mulle_objc_metaclass_as_class( meta), info, 1);

   if( info->options & MULLE_OBJC_SHOW_CLASSPAIR)
      _mulle_buffer_dot_classpair( buffer,
                                   _mulle_objc_metaclass_get_classpair( meta),
                                   _mulle_objc_metaclass_as_class( meta),
                                   info);
}


static mulle_objc_walkcommand_t
   dotdump_callback( struct _mulle_objc_universe *universe,
                     void *p,
                     enum mulle_objc_walkpointertype_t type,
                     char *key,
                     void *parent,
                     void *userinfo)
{
   struct callback_info            *cinfo  = userinfo;
   struct _mulle_objc_infraclass   *infra;
   struct _mulle_objc_metaclass    *meta;
   struct dump_info                *info;
   struct mulle_buffer             *buffer;

   info   = &cinfo->info;
   buffer = cinfo->buffer;

   assert( p);

   if( key)
      return( mulle_objc_walk_ok);

   if( c_set_member( &info->set, p))
      return( mulle_objc_walk_dont_descend);
   c_set_add( &info->set, p);

   switch( type)
   {
   case mulle_objc_walkpointer_is_category  :
   case mulle_objc_walkpointer_is_protocol  :
   case mulle_objc_walkpointer_is_classpair :
      break;

   case mulle_objc_walkpointer_is_universe  :
      if( info->options & MULLE_OBJC_SHOW_UNIVERSE)
      {
         universe = p;
         if( info->options & MULLE_OBJC_SHOW_UNIVERSE_HYPERLINK)
            _mulle_buffer_dot_hyper_universe( buffer, universe, info);
         else
            _mulle_buffer_dot_universe( buffer, universe, info);
      }
      break;

   case mulle_objc_walkpointer_is_infraclass :
      infra = p;
      if( info->options & MULLE_OBJC_SHOW_INFRACLASS)
      {
         if( info->options & MULLE_OBJC_SHOW_CLASS_HYPERLINK)
            _mulle_buffer_dot_hyper_infraclass( buffer, infra, info);
         else
            _mulle_buffer_dot_infraclass( buffer, infra, info);
      }
      break;

   case mulle_objc_walkpointer_is_metaclass :
      meta = p;
      if( info->options & MULLE_OBJC_SHOW_METACLASS)
      {
         if( ! (info->options & MULLE_OBJC_SHOW_CLASS_HYPERLINK))
            _mulle_buffer_dot_metaclass( buffer, meta, info);
      }
      break;

   case mulle_objc_walkpointer_is_method :
   case mulle_objc_walkpointer_is_property :
   case mulle_objc_walkpointer_is_ivar :
      break;
   }

   return( mulle_objc_walk_ok);
}


# pragma mark - class dump

static void   _mulle_objc_class_dotdump_to_buffer( struct _mulle_objc_class *cls,
                                                   char *directory,
                                                   struct mulle_buffer *buffer,
                                                   unsigned long options)
{
   struct _mulle_objc_classpair    *pair;
   struct callback_info            cinfo;

   memset( &cinfo, 0, sizeof( cinfo));

   mulle_buffer_sprintf( buffer, "digraph mulle_objc_class\n{\n");

   cinfo.buffer = buffer;

   c_set_init( &cinfo.info.set);

   cinfo.info.directory = directory;
   cinfo.info.options   = options ? options : (MULLE_OBJC_SHOW_DEFAULT & ~MULLE_OBJC_SHOW_UNIVERSE);
   cinfo.info.options  &= ~MULLE_OBJC_SHOW_CLASS_HYPERLINK;

   pair = NULL;
   do
   {
      pair = _mulle_objc_class_get_classpair( cls);
      mulle_objc_classpair_walk( pair, dotdump_callback, &cinfo);

      // turn on hyperlink class on demand
      cinfo.info.options |= (options & MULLE_OBJC_SHOW_CLASS_HYPERLINK);
   }
   while( cls = _mulle_objc_class_get_superclass( cls));

   mulle_buffer_sprintf( buffer, "}\n");

   c_set_done( &cinfo.info.set);
}


static void   _mulle_objc_class_dotdump_to_file( struct _mulle_objc_class *cls,
                                                 char *directory,
                                                 char *filename,
                                                 unsigned long options)
{
   FILE   *fp;

   fp = fopen( filename, "w");
   if( ! fp)
   {
      perror( "fopen:");
      return;
   }

   mulle_flushablebuffer_do_FILE( buffer, fp)
   {
      _mulle_objc_class_dotdump_to_buffer( cls, directory, buffer, options);
      //mulle_buffer_write_to_file( buffer, fp);
   }
   fclose( fp);
}


static void   mulle_objc_class_dotdump_to_file( struct _mulle_objc_class *cls,
                                                char *directory,
                                                char *filename,
                                                unsigned long options)
{
   _mulle_objc_class_dotdump_to_file( cls, directory, filename, options);
   fprintf( stderr, "Written dot file \"%s\"\n", filename);
}


//static void   _mulle_objc_class_dotdump_to_directory( struct _mulle_objc_class *cls,
//                                                      char *directory)
//{
//   char   *path;
//
//   if( ! cls)
//      return;
//
//   path = dot_filename_for_name( _mulle_objc_class_get_name( cls), directory);
//   mulle_objc_class_dotdump_to_file( cls, directory, path);
//   mulle_free( path);
//}



# pragma mark - overview dump


static void
   _mulle_objc_universe_dotdump_overview_to_buffer( struct _mulle_objc_universe *universe,
                                                    char *directory,
                                                    struct mulle_buffer *buffer)
{
   struct callback_info   cinfo;

   memset( &cinfo, 0, sizeof( cinfo));

   cinfo.buffer                = buffer;

   c_set_init( &cinfo.info.set);

   cinfo.info.directory        = directory;
   cinfo.info.options          = MULLE_OBJC_SHOW_DEFAULT|MULLE_OBJC_SHOW_HYPERLINK;

   mulle_buffer_sprintf( buffer, "digraph mulle_objc_universe\n{\n");
   mulle_objc_universe_walk( universe, dotdump_callback, &cinfo);
   mulle_buffer_sprintf( buffer, "}\n");

   c_set_done( &cinfo.info.set);
}


static void
   _mulle_objc_universe_dotdump_overview_to_fp( struct _mulle_objc_universe *universe,
                                                char *directory,
                                                FILE *fp)
{
   mulle_flushablebuffer_do_FILE( buffer, fp)
   {
      _mulle_objc_universe_dotdump_overview_to_buffer( universe, directory, buffer);
//      mulle_buffer_write_to_file( buffer, fp);
   }
}


#pragma mark - dump universe classes


static void   _mulle_objc_universe_dotdump_classes_to_directory( struct _mulle_objc_universe *universe,
                                                                 char *directory)
{
   intptr_t                        classid;
   struct _mulle_objc_infraclass   *infra;

   mulle_buffer_do( filename_buffer)
   {
      mulle_concurrent_hashmap_for( &universe->classtable, classid, infra)
      {
         mulle_buffer_reset( filename_buffer);
         mulle_buffer_append_dot_filename_for_name( filename_buffer, infra->base.name, directory);
         _mulle_objc_class_dotdump_to_file( _mulle_objc_infraclass_as_class( infra),
                                            directory,
                                            mulle_buffer_get_string( filename_buffer),
                                            MULLE_OBJC_SHOW_ALL|MULLE_OBJC_SHOW_HYPERLINK);
      }
   }
}


#pragma mark - dump universe overview


static void
   _mulle_objc_universe_dotdump_overview_to_file( struct _mulle_objc_universe *universe,
                                                  char *directory,
                                                  char *filename)
{
   FILE   *fp;

   if( ! universe)
   {
      fprintf( stderr, "No universe found!\n");
      return;
   }

   fp = fopen( filename, "w");
   if( ! fp)
   {
      perror( "fopen:");
      return;
   }

   _mulle_objc_universe_dotdump_overview_to_fp( universe, directory, fp);
   fclose( fp);

   fprintf( stderr, "Written dot file \"%s\"\n", filename);
}


static void   _mulle_objc_universe_dotdump_overview_to_directory( struct _mulle_objc_universe *universe,
                                                                  char *directory)
{
   mulle_buffer_do( filename_buffer)
   {
      mulle_buffer_append_dot_filename_for_name( filename_buffer, "overview", directory);
      _mulle_objc_universe_dotdump_overview_to_file( universe,
                                                     directory,
                                                     mulle_buffer_get_string( filename_buffer));
   }
}



# pragma mark - universe dump

static void   _mulle_objc_universe_dotdump_to_buffer( struct _mulle_objc_universe *universe,
                                                     char *directory,
                                                     struct mulle_buffer *buffer)
{
   struct dump_info   info;

   memset( &info, 0, sizeof( info));

   c_set_init( &info.set);

   info.directory = directory;
   info.options   = MULLE_OBJC_SHOW_DEFAULT;

   if( mulle_objc_environment_get_yes_no_default( "MULLE_OBJC_DRAW_STRING_TABLE", 0))
      info.options |= MULLE_OBJC_SHOW_STRINGS;
   if( mulle_objc_environment_get_yes_no_default( "MULLE_OBJC_DRAW_SELECTOR_TABLE", 0))
      info.options |= MULLE_OBJC_SHOW_SELECTORS;

   mulle_buffer_sprintf( buffer, "digraph mulle_objc_universe\n{\n");
   _mulle_buffer_dot_universe( buffer, universe, &info);
   mulle_buffer_sprintf( buffer, "}\n");

   c_set_done( &info.set);
}


static void   _mulle_objc_universe_dotdump_to_fp( struct _mulle_objc_universe *universe,
                                                  char *directory,
                                                  FILE *fp)
{
   mulle_flushablebuffer_do_FILE( buffer, fp)
   {
      _mulle_objc_universe_dotdump_to_buffer( universe, directory, buffer);
//      mulle_buffer_write_to_file( buffer, fp);
   }
}


static void   mulle_objc_universe_dotdump_to_file( struct _mulle_objc_universe *universe,
                                                   char *directory,
                                                   char *filename,
                                                   int log)
{
   FILE   *fp;

   if( ! universe)
   {
      fprintf( stderr, "No universe found!\n");
      return;
   }

   fp = fopen( filename, "w");
   if( ! fp)
   {
      perror( "fopen:");
      return;
   }

   _mulle_objc_universe_dotdump_to_fp( universe, directory, fp);
   fclose( fp);

   if( log)
      fprintf( stderr, "Written dot file \"%s\"\n", filename);
}


static void   _mulle_objc_universe_dotdump_to_directory( struct _mulle_objc_universe *universe,
                                                         char *directory,
                                                         int log)
{
   mulle_buffer_do( filename_buffer)
   {
      mulle_buffer_append_dot_filename_for_name( filename_buffer, "universe", directory);
      mulle_objc_universe_dotdump_to_file( universe,
                                           directory,
                                           mulle_buffer_get_string( filename_buffer),
                                           log);
   }
}


void   mulle_objc_universe_dotdump_to_directory( struct _mulle_objc_universe *universe,
                                                 char *directory)
{
   if( ! universe || ! directory)
      return;

   _mulle_objc_universe_dotdump_overview_to_directory( universe, directory);
   _mulle_objc_universe_dotdump_to_directory( universe, directory, 0);
   _mulle_objc_universe_dotdump_classes_to_directory( universe, directory);
}


void   mulle_objc_class_dotdump_to_directory( struct _mulle_objc_class *cls,
                                              char *directory,
                                              unsigned long options)
{
   if( ! cls || ! directory)
      return;

   mulle_buffer_do( filename_buffer)
   {
      mulle_buffer_append_dot_filename_for_name( filename_buffer,
                                                 _mulle_objc_class_get_name( cls),
                                                 directory);
      mulle_objc_class_dotdump_to_file( cls,
                                        directory,
                                        mulle_buffer_get_string( filename_buffer),
                                        options);
   }
}


void
   mulle_objc_universe_dotdump_frame_to_directory( struct _mulle_objc_universe *universe,
                                                   char *directory)
{
   static mulle_atomic_pointer_t   counter;
   int                             nr;
   int                             max;
   char                            *s;

   nr = (int) (intptr_t) _mulle_atomic_pointer_increment( &counter);
   s  = getenv( "MULLE_OBJC_DOTDUMP_MAX");
   if( s)
   {
      max = atoi( s);
      if( max && nr >= max)
         return;
   }

   mulle_buffer_do( filename_buffer)
   {
      mulle_buffer_sprintf( filename_buffer, "universe_%06d.dot", nr);
      mulle_objc_universe_dotdump_to_file( universe, directory, mulle_buffer_get_string( filename_buffer), 0);
   }
}



#pragma mark - hierarchy dump

/*
 * Hierarchy dumper. Dump a class and all superclasses and protocol classes
 */
struct print_hierarchy_info
{
   struct c_set               *class_set;
   struct mulle_buffer        *buffer;

   struct _mulle_objc_class   *other;
   char                       *relationship;
   int                        counter;
   char                       is_meta;
};


static inline void   print_hierarchy_info_init( struct print_hierarchy_info *info,
                                                struct _mulle_objc_class   *other,
                                                char *relationship,
                                                struct c_set *set,
                                                struct mulle_buffer *buffer)
{
   info->other        = other;
   info->counter      = 0;
   info->is_meta      = 0;
   info->buffer       = buffer;
   info->relationship = relationship;
   info->class_set    = set;
}


static mulle_objc_walkcommand_t
   classpair_callback( struct _mulle_objc_infraclass *infra,
                              struct _mulle_objc_classpair *pair,
                              void *userinfo);


static void   _mulle_buffer_dot_hierarchy( struct mulle_buffer *buffer,
                                           void *value,
                                           void *userinfo)
{
   struct _mulle_objc_class       *cls  = value;
   struct print_hierarchy_info    *info = userinfo;
   struct _mulle_objc_classpair   *pair;
   struct _mulle_objc_class       *supercls;
   struct dump_info               dumpinfo;

   if( ! cls)
      return;

   /* output each class only once */
   if( c_set_member( info->class_set, cls))
      return;
   c_set_add( info->class_set, cls);

   /*
    * Dump super classes
    */
   supercls = _mulle_objc_class_get_superclass( cls);
   if( supercls)
   {
      struct  print_hierarchy_info   subinfo;

      // wrap around to root infra for meta, but not if cls is a protocolclass
      if( _mulle_objc_class_is_metaclass( supercls) == info->is_meta
          || (info->is_meta
              && _mulle_objc_class_get_superclass( supercls) == NULL
              && ! mulle_objc_infraclass_is_protocolclass( _mulle_objc_class_get_infraclass( cls))))
      {
         print_hierarchy_info_init( &subinfo, cls, "superclass", info->class_set, info->buffer);
         subinfo.is_meta = info->is_meta;
         _mulle_buffer_dot_hierarchy( buffer, supercls, &subinfo);
      }
   }

   /*
    * Dump protocol classes
    */
   {
      struct  print_hierarchy_info   subinfo;

      print_hierarchy_info_init( &subinfo, cls, "protocolclass", info->class_set, info->buffer);
      subinfo.counter = 1;
      subinfo.is_meta = info->is_meta;

      pair = _mulle_objc_class_get_classpair( cls);
      _mulle_objc_classpair_walk_protocolclasses( pair, 0, classpair_callback, &subinfo);
   }

   /*
    * Finally class itself. Will then hopefully be at the bottom
    * in Graphviz
    */
   memset( &dumpinfo, 0, sizeof( dumpinfo));

   dumpinfo.options = MULLE_OBJC_SHOW_METHODLISTS|MULLE_OBJC_SHOW_PROPERTYLIST;

   if( _mulle_objc_class_is_metaclass( cls))
      _mulle_buffer_dot_metaclass( buffer, _mulle_objc_class_as_metaclass( cls), &dumpinfo);
   else
      _mulle_buffer_dot_infraclass( buffer, _mulle_objc_class_as_infraclass( cls), &dumpinfo);

   if( info->other)
   {
      if( info->counter)
      {
         mulle_buffer_sprintf( buffer, "\"%p\" -> \"%p\" [ label=\" %s #%d\" dir=\"back\" ]\n",
                                       cls,
                                       info->other,
                                       info->relationship,
                                       info->counter - 1);
         ++info->counter;
      }
      else
         mulle_buffer_sprintf( buffer, "\"%p\" -> \"%p\" [ label=\" %s\" dir=\"back\" ]\n",
                                       cls,
                                       info->other,
                                       info->relationship);
   }
}


static mulle_objc_walkcommand_t
   classpair_callback( struct _mulle_objc_infraclass *infra,
                       struct _mulle_objc_classpair *pair,
                       void *userinfo)
{
   struct print_hierarchy_info    *info = userinfo;
   struct _mulle_objc_metaclass   *meta;

   if( info->is_meta)
   {
      meta = _mulle_objc_infraclass_get_metaclass( infra);
      _mulle_buffer_dot_hierarchy( info->buffer,
                                   _mulle_objc_metaclass_as_class( meta),
                                   userinfo);
   }
   else
      _mulle_buffer_dot_hierarchy( info->buffer,
                                   _mulle_objc_infraclass_as_class( infra),
                                   userinfo);
   return( mulle_objc_walk_ok);
}


static void   _mulle_objc_classhierarchy_dotdump_to_buffer( struct _mulle_objc_class *cls,
                                                            struct mulle_buffer *buffer)
{
   struct print_hierarchy_info   info;
   struct c_set                  set;

   mulle_buffer_sprintf( buffer, "digraph mulle_classhierarchy\n{\n");

   if( cls)
   {
      c_set_init( &set);

      print_hierarchy_info_init( &info, NULL, NULL, &set, buffer);

      info.is_meta = _mulle_objc_class_is_metaclass( cls);
      _mulle_buffer_dot_hierarchy( buffer, cls, &info);

      c_set_done( &set);
   }

   mulle_buffer_sprintf( buffer, "}\n");
}


void   mulle_objc_classhierarchy_dotdump_to_stream( struct _mulle_objc_class *cls,
                                                    FILE *fp)
{
   if( ! fp)
      fp = stdout;

   mulle_flushablebuffer_do_FILE( buffer, fp)
   {
      _mulle_objc_classhierarchy_dotdump_to_buffer( cls, buffer);
//      mulle_buffer_write_to_file( buffer, fp);
   }
}


void   mulle_objc_classhierarchy_dotdump_to_file( struct _mulle_objc_class *cls,
                                                  char *filename)
{
   FILE   *fp;

   fp = fopen( filename, "w");
   if( ! fp)
   {
      perror( "fopen:");
      return;
   }
   mulle_objc_classhierarchy_dotdump_to_stream( cls, fp);
   fclose( fp);
}



//
// just don't output stuff with ampersands for now
// What is this used for ? (apparently the debugger ?)
//
void   mulle_objc_methodlist_dump( struct _mulle_objc_methodlist *list)
{
   unsigned int   i;

   mulle_buffer_do( buffer)
   {
      for( i = 0; i < list->n_methods; i++)
      {
         mulle_buffer_reset( buffer);
         mulle_buffer_sprintf_functionpointer( buffer, (mulle_functionpointer_t) &list->methods[ i].implementation);

         printf( "{ "
                 "name = \"%s\""
                 "signature = \"%s\""
                 "methodid = %08lx"
                 "bits = 0x%x"
                 "implementation = %s"
                 " }",
                  list->methods[ i].descriptor.name,
                  list->methods[ i].descriptor.signature,
                  (unsigned long) list->methods[ i].descriptor.methodid,
                  list->methods[ i].descriptor.bits,
                  mulle_buffer_get_string( buffer));
      }
   }
}

