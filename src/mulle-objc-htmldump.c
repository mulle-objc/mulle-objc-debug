//
//  mulle_objc_htmldump.c
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
#include "include.h"

#include "include-private.h"

#include "mulle-objc-htmldump.h"
#include "mulle-objc-html.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "c-set.inc"


static struct _mulle_objc_htmltablestyle    categorytable_style =
{
   "categories",
   "category",
   NULL,
   NULL,
   0
};


static struct _mulle_objc_htmltablestyle    protocoltable_style =
{
   "protocols",
   "protocol",
   NULL,
   NULL,
   0
};


static struct _mulle_objc_htmltablestyle    infraclass_style =
{
   "infraclass",
   "infra",
   NULL,
   NULL,
   0
};


static struct _mulle_objc_htmltablestyle    cachetable_style =
{
   "cache",
   "cache",
   NULL,
   NULL,
   6
};


static char  *ivartable_headers[] =
{
   "name", "signature", "ivarid", "flags"
};


static struct _mulle_objc_htmltablestyle    ivartable_style =
{
   "ivars",
   "ivar",
   NULL,
   NULL,
   4,
   ivartable_headers
};


static struct _mulle_objc_htmltablestyle    propertytable_style =
{
   "properties",
   "property",
   NULL,
   NULL,
   0
};


static struct _mulle_objc_htmltablestyle  classtable_style =
{
   "classes",
   "class",
   NULL,
   NULL,
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


static struct _mulle_objc_htmltablestyle  universe_style =
{
   "universe",
   "universe",
   NULL,
   NULL,
   0
};


static struct _mulle_objc_htmltablestyle  stringtable_style =
{
   "strings",
   "string",
   NULL,
   NULL,
   0
};


static struct _mulle_objc_htmltablestyle  methodlisttable_style =
{
   "methods",
   "method",
   NULL,
   NULL,
   0
};


# pragma mark - small routines to output the html

static char   *html_escape( char *s)
{
   if( ! strchr( s, '&') && ! strchr( s, '<'))
      return( s);

   return( "bad-html");
}


static void  mulle_buffer_add_filename_for_name_ext( struct mulle_buffer *buffer, char *name, char *ext, char *directory)
{
   char     separator;

#ifdef _WIN32
    separator = '\\';
#else
    separator = '/';
#endif

   mulle_buffer_sprintf( buffer, "%s%c%s%s", directory, separator, name, ext);
}


static void  mulle_buffer_add_html_filename_for_name( struct mulle_buffer *buffer, char *name, char *directory)
{
   char   *escaped;

   escaped = html_escape( name);
   mulle_buffer_add_filename_for_name_ext( buffer, escaped, ".html", directory);
}


static void  mulle_buffer_add_filename_for_universe( struct mulle_buffer *buffer, struct _mulle_objc_universe  *universe, char *directory)
{
   char     separator;

#ifdef _WIN32
    separator = '\\';
#else
    separator = '/';
#endif
   assert( directory);

   mulle_buffer_sprintf( buffer, "%s%cindex.html", directory, separator);
}


static void   write_css_if_needed( char *directory)
{
   FILE   *fp;
   static char   css[] = ""
#include "mulle-objc.css.inc"
;

   if( getenv( "MULLE_OBJC_CSS_URL"))
      return;

   mulle_buffer_do( filename_buffer)
   {
      mulle_buffer_add_filename_for_name_ext( filename_buffer, "mulle-objc", ".css", directory);

      fp = fopen( mulle_buffer_get_string( filename_buffer), "w");
      fwrite( css, sizeof( css) - 1, 1, fp);
      fclose( fp);
   }
}



static FILE  *open_for_write( char *name)
{
   FILE  *fp;

   fp = fopen( name, "w");
   if( ! fp)
      perror( "fopen");

   return( fp);
}


static void  _mulle_buffer_print_start( struct mulle_buffer *buffer, char *title)
{
   char   *cssurl;

   cssurl = getenv( "MULLE_OBJC_CSS_URL");
   if( ! cssurl)
      cssurl = "mulle-objc.css";

   mulle_buffer_sprintf( buffer,
                         "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\">\n" // pedantic
                         "<HTML>\n"
                         "<HEAD>\n"
                         "<TITLE>%s</TITLE>\n"
                         "<LINK TYPE=\"text/css\" REL=\"stylesheet\" HREF=\"%s\">\n"
                         "</HEAD>\n"
                         "<BODY>\n",
                         title,
                         cssurl);
}


static void  _mulle_buffer_html_body_end( struct mulle_buffer *buffer)
{
   mulle_buffer_add_string( buffer, "\
</BODY>\n\
</HTML>\n\
");
}


# pragma mark - walker universe callback

static void   _mulle_buffer_print_universe_html( struct mulle_buffer *buffer,
                                                 struct _mulle_objc_universe *universe)
{
   int                                              i;
   struct _mulle_objc_staticstring                  *string;
   struct _mulle_objc_class                         *cls;
   struct mulle_concurrent_pointerarrayenumerator   rover;

   mulle_buffer_append_string( buffer, "\n<DIV CLASS=\"universe_values\">\n");
   {
      mulle_buffer_append_string( buffer, "<H2>Values</H2>");
      mulle_buffer_html_universe( buffer, universe, &universe_style);
   }
   mulle_buffer_append_string( buffer, "</DIV>\n");

   // need to sort this in the future
   mulle_buffer_append_string( buffer, "\n<DIV CLASS=\"universe_classes\">\n");
   {
      mulle_buffer_html_concurrent_hashmap( buffer,
                                            &universe->classtable,
                                            mulle_buffer_html_class_entry,
                                            &classtable_style,
                                            NULL);
   }
   mulle_buffer_append_string( buffer, "</DIV>\n");

   mulle_buffer_append_string( buffer, "\n<DIV CLASS=\"universe_fastclasses\">\n");
   {
      for( i = 0; i < MULLE_OBJC_S_FASTCLASSES; i++)
         if( _mulle_atomic_pointer_read_nonatomic( &universe->fastclasstable.classes[ i].pointer))
            break;

      if( i < MULLE_OBJC_S_FASTCLASSES)
      {
         mulle_buffer_append_string( buffer, "<H2>Fast Classes</H2>");
         mulle_buffer_append_string( buffer, "<TABLE CLASS=\"universe_fastclass_table\">\n");

         for( i = 0; i < MULLE_OBJC_S_FASTCLASSES; i++)
            if( _mulle_atomic_pointer_read_nonatomic( &universe->fastclasstable.classes[ i].pointer))
            {
               cls = _mulle_atomic_pointer_read_nonatomic( &universe->fastclasstable.classes[ i].pointer);
               mulle_buffer_append_string( buffer, "<TR><TH>");
               mulle_buffer_sprintf( buffer, "%d", i);
               mulle_buffer_append_string( buffer, "</TH><TD>");
               mulle_buffer_html_class_short( buffer, cls, &classtable_style);
               mulle_buffer_append_string( buffer, "</TD></TR>\n");
            }
         mulle_buffer_append_string( buffer, "</TABLE>\n");
      }
   }
   mulle_buffer_append_string( buffer, "</DIV>\n");

   mulle_buffer_append_string( buffer, "\n<DIV CLASS=\"universe_descriptors\">\n");
   {
      if( mulle_concurrent_hashmap_count( &universe->descriptortable))
      {
         mulle_buffer_append_string( buffer, "<H2>Method Descriptors</H2>");
         mulle_buffer_append_string( buffer, "<TABLE CLASS=\"universe_descriptor_table\">\n");
         mulle_buffer_html_concurrent_hashmap( buffer,
                                                &universe->descriptortable,
                                                mulle_buffer_html_descriptor_entry,
                                                &descriptortable_style,
                                                NULL);
         mulle_buffer_append_string( buffer, "</TABLE>\n");
      }
   }
   mulle_buffer_append_string( buffer, "</DIV>\n");

   mulle_buffer_append_string( buffer, "\n<DIV CLASS=\"universe_protocols\">\n");
   {
      if( mulle_concurrent_hashmap_count( &universe->protocoltable))
      {
         mulle_buffer_append_string( buffer, "<H2>Protocols</H2>");
         mulle_buffer_append_string( buffer, "<TABLE CLASS=\"universe_protocol_table\">\n");
         mulle_buffer_html_concurrent_hashmap( buffer,
                                                &universe->protocoltable,
                                                mulle_buffer_html_protocol_entry,
                                                &protocoltable_style,
                                                NULL);
         mulle_buffer_append_string( buffer, "</TABLE>\n");
      }
   }
   mulle_buffer_append_string( buffer, "</DIV>\n");

   mulle_buffer_append_string( buffer, "\n<DIV CLASS=\"universe_categories\">\n");
   {
      if( mulle_concurrent_hashmap_count( &universe->categorytable))
      {
         mulle_buffer_append_string( buffer, "<H2>Categories</H2>");
         mulle_buffer_append_string( buffer, "<TABLE CLASS=\"universe_category_table\">\n");
         mulle_buffer_html_concurrent_hashmap( buffer,
                                                &universe->categorytable,
                                                mulle_buffer_html_loadcategory_entry,
                                                &categorytable_style,
                                                NULL);
         mulle_buffer_append_string( buffer, "</TABLE>\n");
      }
   }
   mulle_buffer_append_string( buffer, "</DIV>\n");

   mulle_buffer_append_string( buffer, "\n<DIV CLASS=\"universe_strings\">\n");
   {
      if( mulle_concurrent_pointerarray_get_count( &universe->staticstrings))
      {
         mulle_buffer_append_string( buffer, "<H2>Strings</H2>");
         mulle_buffer_append_string( buffer, "<TABLE CLASS=\"universe_string_table\">\n");
         rover = mulle_concurrent_pointerarray_enumerate( &universe->staticstrings);
         while( string = _mulle_concurrent_pointerarrayenumerator_next( &rover))
         {
            mulle_buffer_append_string( buffer, "<li>");
            mulle_buffer_html_staticstring( buffer, string, &stringtable_style);
            mulle_buffer_append_string( buffer, "\n");
         }
         mulle_concurrent_pointerarrayenumerator_done( &rover);
         mulle_buffer_append_string( buffer, "</TABLE>\n");
      }
   }
   mulle_buffer_append_string( buffer, "</DIV>\n");
}


static void   _mulle_objc_universe_print_to_directory( struct _mulle_objc_universe *universe,
                                                       char *directory)
{
   FILE   *fp;

   mulle_buffer_do( filename_buffer)
   {
      mulle_buffer_add_filename_for_universe( filename_buffer, universe, directory);

      fp = open_for_write( mulle_buffer_get_string( filename_buffer));
      if( fp)
      {
         mulle_flushablebuffer_do_FILE( buffer, fp)
         {
            _mulle_buffer_print_start( buffer, "universe");
      	   _mulle_buffer_print_universe_html( buffer, universe);
            _mulle_buffer_html_body_end( buffer);
            // mulle_fprintf( fp, "%s\n", mulle_buffer_get_string( buffer));
         }
         fclose( fp);
   	}
   }
}


# pragma mark - walker class callback

struct dump_info
{
   c_set  set;
   char   *directory;
};


static void   _mulle_buffer_print_infraclass_html( struct mulle_buffer *buffer,
                                                   struct _mulle_objc_infraclass *infra)
{
    struct mulle_concurrent_pointerarrayenumerator   rover;
    struct _mulle_objc_protocolclassenumerator       prover;
    struct _mulle_objc_cache                         *cache;
    struct _mulle_objc_class                         *cls;
    struct _mulle_objc_infraclass                    *superclass;
    struct _mulle_objc_metaclass                     *meta;
    struct _mulle_objc_classpair                     *pair;
    struct _mulle_objc_infraclass                    *prop_cls;
    struct _mulle_objc_ivarlist                      *ivarlist;
    struct _mulle_objc_universe                      *universe;
    struct _mulle_objc_methodlist                    *methodlist;
    struct _mulle_objc_propertylist                  *propertylist;
    struct _mulle_objc_htmltablestyle                style;
    struct _mulle_objc_uniqueidarray                 *array;
    unsigned int                                     n;

    // Initialize
    cls = _mulle_objc_infraclass_as_class(infra);
    universe = _mulle_objc_infraclass_get_universe(infra);

    // Print class name (header)
    mulle_buffer_append_string( buffer, "<H2>");
    mulle_buffer_append_string( buffer, cls->name);
    mulle_buffer_append_string( buffer, "</H2>");

    // Class links (universe and superclass links)
    mulle_buffer_append_string( buffer, "<DIV CLASS=\"class_links\">\n");
    {
        mulle_buffer_append_string( buffer, "<DIV CLASS=\"class_universe_link\">\n");
        mulle_buffer_append_string( buffer, "<A HREF=\"index.html\">Universe</a>");
        mulle_buffer_append_string( buffer, "</DIV>\n");

        mulle_buffer_append_string( buffer, "<DIV CLASS=\"class_superclass_link\">\n");
        superclass = _mulle_objc_infraclass_get_superclass(infra);
        if (superclass) {
            mulle_buffer_append_string( buffer, "<H2>Superclass</H2>");
            mulle_buffer_html_class_short( buffer, _mulle_objc_infraclass_as_class(superclass), &classtable_style);
        }
        mulle_buffer_append_string( buffer, "</DIV>\n");
    }
    mulle_buffer_append_string( buffer, "</DIV>\n");

    // Protocol class links
    mulle_buffer_append_string( buffer, "<DIV CLASS=\"class_protocolclass_links\">\n");
    {
        pair = _mulle_objc_infraclass_get_classpair(infra);
        if (!(_mulle_objc_infraclass_get_inheritance(infra) & MULLE_OBJC_CLASS_DONT_INHERIT_PROTOCOLS) &&
            mulle_concurrent_pointerarray_get_count(&pair->protocolclasses)) {
            mulle_buffer_append_string( buffer, "<H2>Inherited Protocol Classes</H2>");

            mulle_buffer_append_string( buffer, "<OL>\n");

            prover = _mulle_objc_classpair_enumerate_protocolclasses(pair);
            while (prop_cls = _mulle_objc_protocolclassenumerator_next(&prover)) {
                mulle_buffer_append_string( buffer, "<LI>");
                mulle_buffer_html_class_short( buffer, _mulle_objc_infraclass_as_class(prop_cls), &classtable_style);
                mulle_buffer_append_string( buffer, "</LI>\n");
            }
            _mulle_objc_protocolclassenumerator_done(&prover);

            mulle_buffer_append_string( buffer, "</OL>\n");
        }
    }
    mulle_buffer_append_string( buffer, "</DIV>\n");

    // Class values section
    mulle_buffer_append_string( buffer, "<DIV CLASS=\"class_values\">\n");
    {
        style = infraclass_style;
        style.title = cls->name;
        mulle_buffer_append_string( buffer, "<H2>Values</H2>");
        mulle_buffer_html_class( buffer, cls, 1, &style);
    }
    mulle_buffer_append_string( buffer, "</DIV>\n");

    // Properties section
    mulle_buffer_append_string( buffer, "<DIV CLASS=\"class_properties\">\n");
    {
        n = mulle_concurrent_pointerarray_get_count( &infra->propertylists);
        if( n)
        {
            mulle_buffer_append_string( buffer, "<H2>Property Lists</H2>");
            if( n == 1)
            {
               propertylist = mulle_concurrent_pointerarray_get( &infra->propertylists, 0);
               if( propertylist->n_properties == 0)
                  goto skip_property_table;
            }

            rover = mulle_concurrent_pointerarray_enumerate(&infra->propertylists);
            while (propertylist = _mulle_concurrent_pointerarrayenumerator_next(&rover))
            {
                mulle_buffer_html_propertylist( buffer, propertylist, &propertytable_style);
            }
            mulle_concurrent_pointerarrayenumerator_done(&rover);
skip_property_table:
            ;
        }
    }
    mulle_buffer_append_string( buffer, "</DIV>\n");

    // Instance Variables section
    mulle_buffer_append_string( buffer, "<DIV CLASS=\"class_ivars\">\n");
    {
        n = mulle_concurrent_pointerarray_get_count(&infra->ivarlists);
        if( n)
        {
            mulle_buffer_append_string( buffer, "<H2>Instance Variable Lists</H2>");
            if( n == 1)
            {
               ivarlist = mulle_concurrent_pointerarray_get( &infra->ivarlists, 0);
               if( ivarlist->n_ivars == 0)
                  goto skip_ivar_table;
            }

            rover = mulle_concurrent_pointerarray_enumerate( &infra->ivarlists);
            while (ivarlist = _mulle_concurrent_pointerarrayenumerator_next(&rover))
            {
                mulle_buffer_html_ivarlist_hor( buffer, ivarlist, &ivartable_style);
            }
            mulle_concurrent_pointerarrayenumerator_done(&rover);
        }
skip_ivar_table:
        ;
    }
    mulle_buffer_append_string( buffer, "</DIV>\n");

    // Class Methods section
    mulle_buffer_append_string( buffer, "<DIV CLASS=\"class_classmethods\">\n");
    {
        meta = _mulle_objc_infraclass_get_metaclass(infra);
        if( mulle_concurrent_pointerarray_get_count(&meta->base.methodlists))
        {
            mulle_buffer_append_string( buffer, "<H2>+ Method Lists</H2>");
            if( n == 1)
            {
               methodlist = mulle_concurrent_pointerarray_get( &meta->base.methodlists, 0);
               if( methodlist->n_methods == 0)
                  goto skip_metamethod_table;
            }

            rover = mulle_concurrent_pointerarray_enumerate(&meta->base.methodlists);
            while (methodlist = mulle_concurrent_pointerarrayenumerator_next(&rover)) {
                style = methodlisttable_style;
                style.title = _mulle_objc_methodlist_get_categoryname(methodlist);
                style.title = style.title ? style.title : "class";
                mulle_buffer_html_methodlist_hor( buffer, methodlist, &style);
            }
            mulle_concurrent_pointerarrayenumerator_done(&rover);
skip_metamethod_table:
            ;
        }
    }
    mulle_buffer_append_string( buffer, "</DIV>\n");

    // Instance Methods section
    mulle_buffer_append_string( buffer, "<DIV CLASS=\"class_instancemethods\">\n");
    {
        if( mulle_concurrent_pointerarray_get_count(&infra->base.methodlists))
        {
            mulle_buffer_append_string( buffer, "<H2>- Method Lists</H2>");
            if( n == 1)
            {
               methodlist = mulle_concurrent_pointerarray_get( &infra->base.methodlists, 0);
               if( methodlist->n_methods == 0)
                  goto skip_inframethod_table;
            }

            rover = mulle_concurrent_pointerarray_enumerate(&infra->base.methodlists);
            while (methodlist = mulle_concurrent_pointerarrayenumerator_next(&rover)) {
                style = methodlisttable_style;
                style.title = _mulle_objc_methodlist_get_categoryname(methodlist);
                style.title = style.title ? style.title : "class";
                mulle_buffer_html_methodlist_hor( buffer, methodlist, &style);
            }
            mulle_concurrent_pointerarrayenumerator_done(&rover);
        }
skip_inframethod_table:
        ;
    }
    mulle_buffer_append_string( buffer, "</DIV>\n");

    // Protocols section
    mulle_buffer_append_string( buffer, "<DIV CLASS=\"class_protocols\">\n");
    {
        array = _mulle_atomic_pointer_read(&pair->p_protocolids.pointer);
        if (array->n) {
            mulle_buffer_append_string( buffer, "<H2>Protocols</H2>");
            mulle_buffer_html_uniqueidarray( buffer, array,
                                                     mulle_buffer_html_protocolid_element,
                                                     &protocoltable_style,
                                                     universe);
        }
    }
    mulle_buffer_append_string( buffer, "</DIV>\n");

    // Categories section
    mulle_buffer_append_string( buffer, "<DIV CLASS=\"class_categories\">\n");
    {
        array = _mulle_atomic_pointer_read(&pair->p_categoryids.pointer);
        if (array->n) {
            mulle_buffer_append_string( buffer, "<H2>Categories</H2>");
            mulle_buffer_html_uniqueidarray( buffer, array,
                                                     mulle_buffer_html_categoryid_element,
                                                     &categorytable_style,
                                                     universe);
        }
    }
    mulle_buffer_append_string( buffer, "</DIV>\n");

    // Instance Cache section
    mulle_buffer_append_string( buffer, "<DIV CLASS=\"class_cache\">\n");
    {
        cache = _mulle_objc_cachepivot_get_cache_atomic(&cls->cachepivot.pivot);
        mulle_buffer_append_string( buffer, "<H2>Instance Cache</H2>");
        mulle_buffer_html_cache( buffer, cache, universe, &cachetable_style);
    }
    mulle_buffer_append_string( buffer, "</DIV>\n");

    // Meta Cache section
    mulle_buffer_append_string( buffer, "<DIV CLASS=\"class_cache\">\n");
    {
        cache = _mulle_objc_cachepivot_get_cache_atomic(&meta->base.cachepivot.pivot);
        mulle_buffer_append_string( buffer, "<H2>Meta Cache</H2>");
        mulle_buffer_html_cache( buffer, cache, universe, &cachetable_style);
    }
    mulle_buffer_append_string( buffer, "</DIV>\n");
}


static void   infraclass_dump( struct _mulle_objc_infraclass *infra, char *directory)
{
   FILE   *fp;

   mulle_buffer_do( filename_buffer)
   {
      mulle_buffer_add_html_filename_for_name( filename_buffer, infra->base.name, directory);

      fp = open_for_write( mulle_buffer_get_string( filename_buffer));
      if( fp)
      {
         mulle_flushablebuffer_do_FILE( buffer, fp)
         {
            _mulle_buffer_print_start( buffer, infra->base.name);
      	  _mulle_buffer_print_infraclass_html( buffer, infra);
            _mulle_buffer_html_body_end( buffer);
           // mulle_fprintf( fp, "%s\n", mulle_buffer_get_string( buffer));
         }
         fclose( fp);
      }
   }
}


static mulle_objc_walkcommand_t   callback( struct _mulle_objc_universe *universe,
                                            void *p,
                                            enum mulle_objc_walkpointertype_t type,
                                            char *key,
                                            void *parent,
                                            void *userinfo)
{
   char                            *directory;
   struct _mulle_objc_infraclass   *infra;
   struct dump_info                *info;

   assert( p);

   if( key)
      return( mulle_objc_walk_ok);

   info      = userinfo;
   directory = info->directory;

   if( c_set_member( &info->set, p))
      return( mulle_objc_walk_dont_descend);
   c_set_add( &info->set, p);

   switch( type)
   {
   case mulle_objc_walkpointer_is_category :
   case mulle_objc_walkpointer_is_protocol :
   case mulle_objc_walkpointer_is_classpair :
      break;

   case mulle_objc_walkpointer_is_universe  :
      universe = p;
      _mulle_objc_universe_print_to_directory( universe, directory);
      break;

   case mulle_objc_walkpointer_is_infraclass :
      infra = p;
      infraclass_dump( infra, directory);
      break;

   case mulle_objc_walkpointer_is_metaclass :
   case mulle_objc_walkpointer_is_method :
   case mulle_objc_walkpointer_is_property :
   case mulle_objc_walkpointer_is_ivar :
      break;
   }

   return( mulle_objc_walk_ok);
}


# pragma mark - universe dump

void
   mulle_objc_universe_htmldump_to_directory( struct _mulle_objc_universe *universe,
                                              char *directory)
{
   struct dump_info  info;

   c_set_init( &info.set);
   info.directory = directory;

   if( mulle_objc_universe_walk( universe, callback, &info) != mulle_objc_walk_error)
      fprintf( stderr, "Dumped HTML to \"%s\"\n", directory);

   c_set_done( &info.set);
   write_css_if_needed( directory);
}



#pragma mark - class dump

void
   mulle_objc_classpair_htmldump_to_directory( struct _mulle_objc_classpair *pair,
                                               char *directory)
{
   struct dump_info   info;

   if( ! pair)
      return;

   c_set_init( &info.set);
   info.directory = directory;

   mulle_objc_classpair_walk( pair, callback, &info);

   c_set_done( &info.set);
}


void   mulle_objc_class_htmldump_to_directory( struct _mulle_objc_class *cls,
                                               char *directory)
{
   struct _mulle_objc_classpair   *pair;

   if( ! cls)
      return;

   do
   {
      pair = _mulle_objc_class_get_classpair( cls);
      mulle_objc_classpair_htmldump_to_directory( pair, directory);
   }
   while( cls = _mulle_objc_class_get_superclass( cls));

   write_css_if_needed( directory);
   fprintf( stderr, "Dumped HTML to \"/%s\"\n", directory);
}


void   mulle_objc_object_htmldump_class_to_directory( void *obj,
                                                      char *directory)
{
   struct _mulle_objc_class   *cls;

   if( obj)
   {
      cls = _mulle_objc_object_get_isa( obj);
      mulle_objc_class_htmldump_to_directory( cls, directory);
   }
}


// void   mulle_objc_htmldump_universes_to_directory( char *directory)
// {
//    struct _mulle_objc_universe   *universe;
//    size_t                        n_universes;
//
//    n_universes = __mulle_objc_global_get_alluniverses( NULL, 0);
//    {
//       struct _mulle_objc_universe   *universes[ n_universes];
//       struct _mulle_objc_universe   **p;
//       struct _mulle_objc_universe   **sentinel;
//
//
//       _mulle_objc_global_get_alluniverses( universes, n_universes);
//
//       p        = universes;
//       sentinel = &p[ n_universes];
//       while( p < sentinel)
//       {
//          universe = *p;
//          if( universe)
//          {
//             if( ! _mulle_objc_universe_is_default( universe))
//             {
//
//             }
//             mulle_objc_universe_htmldump_to_directory( universe, directory);
//          }
//          ++p;
//       }
//    }
// }
