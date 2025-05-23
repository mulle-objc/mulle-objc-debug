//
//  common.inc
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
#include "mulle-objc-html.h"

#include "include-private.h"

#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>


#pragma mark - printing support
// lazy copout
static char   *html_escape( char *s)
{
   if( ! strchr( s, '&') && ! strchr( s, '<'))
      return( s);

   return( "bad-html");
}


static int   strcmp_r( void * s1, void * s2, void *thunk)
{
    return( strcmp( *(char **) s1, *(char **) s2));
}


//
// stuff shared by graphviz and html, don't want to expose the
// symbols or dick around with linker options
//

static char  *inheritance_description( unsigned int inheritance)
{
   char     *tmp;
   size_t   len;

   tmp = mulle_malloc( 128);
   if( ! tmp)
      return( tmp);

   tmp[ 0] = 0;
   if( ! (inheritance & MULLE_OBJC_CLASS_DONT_INHERIT_SUPERCLASS))
      strcat( tmp, "superclass ");
   if( ! (inheritance & MULLE_OBJC_CLASS_DONT_INHERIT_CATEGORIES))
      strcat( tmp, "categories ");
   if( ! (inheritance & MULLE_OBJC_CLASS_DONT_INHERIT_PROTOCOLS))
      strcat( tmp, "protocols ");
   if( ! (inheritance & MULLE_OBJC_CLASS_DONT_INHERIT_PROTOCOL_CATEGORIES))
      strcat( tmp, "protocol_categories ");

   len = strlen( tmp);
   if( len)
      tmp[ len - 1] = 0;
   return( tmp);
}


static char  *categoryid_describe_row_html( void *value,
                                            struct _mulle_objc_universe *universe,
                                            struct _mulle_objc_htmltablestyle *styling)
{
   mulle_objc_categoryid_t   categoryid;
   char                      *s;
   char                      *result;

   categoryid = (mulle_objc_categoryid_t) (intptr_t) value;
   s          = _mulle_objc_universe_describe_categoryid( universe,
                                                           categoryid);

   mulle_asprintf( &result, "<TR><TD>\"%s\"</TD><TD>%08lx</TD></TR>\n",
         s, (unsigned long) categoryid);
   return( result);
}


static char  *protocolid_describe_row_html( void *value,
                                            struct _mulle_objc_universe *universe,
                                            struct _mulle_objc_htmltablestyle *styling)
{
   mulle_objc_protocolid_t   protocolid;
   char                      *s;
   char                      *result;

   protocolid = (mulle_objc_protocolid_t) (intptr_t) value;
   s          = _mulle_objc_universe_describe_protocolid( universe, protocolid);

   mulle_asprintf( &result, "<TR><TD>\"%s\"</TD><TD>%08lx</TD></TR>\n",
         s, (unsigned long) protocolid);
   return( result);
}


#pragma mark - universe

static void   asprintf_table_header_colspan( char **s,
                                             struct _mulle_objc_htmltablestyle *styling,
                                             unsigned int colspan)
{
   if( styling->classprefix)
      mulle_asprintf( s,
               "<TABLE CLASS=\"%s_table\">\n<TR CLASS=\"%s_table_header\"><TH COLSPAN=\"%u\">%s</TH></TR>\n",
               styling->classprefix,
               styling->classprefix,
               colspan ? colspan : 2,
               styling->title);
   else // graphviz don't like CLASS, can't deal with TH
      mulle_asprintf( s,
               "<TABLE CELLPADDING=\"0\" CELLSPACING=\"0\">\n<TR><TD BGCOLOR=\"%s\" COLSPAN=\"%u\"><FONT COLOR=\"%s\">%s</FONT></TD></TR>\n",
               styling->bgcolor,
               colspan ? colspan : 2,
               styling->color,
               styling->title);
}


static void   asprintf_table_header( char **s, struct _mulle_objc_htmltablestyle *styling)
{
   asprintf_table_header_colspan( s, styling, styling->colspan);
}


static char  *final_concat_malloced_tmp_known_len( char **tmp, unsigned int n, size_t len)
{
   unsigned int  i;
   char          *s;

   s = mulle_realloc( tmp[ 0], len + 1);
   for( i = 1; i < n; i++)
   {
      strcat( s, tmp[ i]);
      mulle_free( tmp[ i]);
   }

   mulle_free( tmp);

   return( s);
}


static char  *final_concat_auto_tmp_known_len( char **tmp, unsigned int n, size_t len)
{
   unsigned int  i;
   char          *s;

   s = mulle_malloc( len + 1);
   s[ 0] = 0;

   for( i = 0; i < n; i++)
   {
      strcat( s, tmp[ i]);
      mulle_free( tmp[ i]);
   }

   return( s);
}


static char  *final_concat_auto_tmp( char **tmp, unsigned int n)
{
   unsigned int  i;
   size_t        len;

   len = 0;
   for( i = 0; i < n; i++)
      len += strlen( tmp[ i]);

   return( final_concat_auto_tmp_known_len( tmp, n, len));
}


char   *mulle_objc_universe_describe_html( struct _mulle_objc_universe *universe,
                                           struct _mulle_objc_htmltablestyle *styling)
{
   char   *tmp[ 3];

   // create single lines for each method and two for head/tail
   asprintf_table_header( &tmp[ 0], styling);

   mulle_asprintf( &tmp[ 1],
            "<TR><TD>version</TD><TD>0x%x</TD></TR>\n",
            universe->version);
   mulle_asprintf( &tmp[ 2],
            "</TABLE>");

   return( final_concat_auto_tmp( tmp, 3));
}



#pragma mark - static strings


char  *mulle_objc_staticstring_describe_html( struct _mulle_objc_staticstring *string,
                                              struct _mulle_objc_htmltablestyle *styling)
{
   char   *tmp[ 4];
   char   *th;

   th = styling->classprefix ? "TH" : "TD";
   // create single lines for each method and two for head/tail
   asprintf_table_header( &tmp[ 0], styling);

   mulle_asprintf( &tmp[ 1],
            "<TR><%s>_s</%s><TD>%s</TD></TR>\n",
               th, th,
               html_escape( string->_s ? string->_s : "*null*"));
   mulle_asprintf( &tmp[ 2],
            "<TR><%s>_len</%s><TD>%d</TD></TR>\n",
               th, th,
               string->_len);
   mulle_asprintf( &tmp[ 3],
            "</TABLE>");

   return( final_concat_auto_tmp( tmp, 4));
}


char  *mulle_objc_staticstring_describe_hor_html( struct _mulle_objc_staticstring *string,
                                                  struct _mulle_objc_htmltablestyle *styling)
{
   char *s;

   mulle_asprintf( &s, "%s", html_escape( string->_s ? string->_s : "*null*"));

   return( s);
}


char  *mulle_objc_staticstring_describe_row_html( void *value,
                                                  struct _mulle_objc_htmltablestyle *styling)
{
   struct _mulle_objc_staticstring *string = value;
   char   *s;

   mulle_asprintf( &s,
            "<TR>"
            "<TD COLSPAN=\"2\">%s</TD>"
            "</TR>\n",
            html_escape( string->_s ? string->_s : "*null*"));

   return( s);
}


char  *mulle_objc_fastclassentry_describe_row_html( unsigned int i,
                                                    struct _mulle_objc_infraclass *infra,
                                                    struct _mulle_objc_htmltablestyle *styling)
{
   char   *s;

   mulle_asprintf( &s,
            "<TR>"
            "<TD>%u</TD>"
            "<TD>%s</TD>"
            "</TR>\n",
            i,
            html_escape( infra ? _mulle_objc_infraclass_get_name( infra) : "*null*"));

   return( s);
}

#pragma mark - classes

char   *mulle_objc_class_describe_html_short( struct _mulle_objc_class *cls,
                                              struct _mulle_objc_htmltablestyle *styling)
{
   char  *s;
   char  *name;

   name = html_escape( cls->name);
   mulle_asprintf( &s, "<a href=\"%s.html\">%s</a>\n",
            name,
            name);
   return( s);
}


char   *mulle_objc_class_describe_row_html( intptr_t  classid,
                                            void *cls,
                                            struct _mulle_objc_htmltablestyle *styling)
{
   char   *s;
   char   *name;
   char   *prefix;

   name   = html_escape( _mulle_objc_class_get_name( cls));
   prefix = _mulle_objc_class_is_metaclass( cls) ? "+" : "";

   mulle_asprintf( &s, "<TR><TD><A HREF=\"%s%s.html\">%s</A></TD><TD>%08lx</TD></TR>\n",
            prefix,
            name,
            name,
            (unsigned long) _mulle_objc_class_get_classid( cls));
   return( s);
}


char   *mulle_objc_infraclass_describe_row_html( intptr_t  classid,
                                                 void *cls,
                                                 struct _mulle_objc_htmltablestyle *styling)
{
   char   *s;
   char   *name;

   name = html_escape( _mulle_objc_infraclass_get_name( cls));
   mulle_asprintf( &s, "<TR><TD>%s</TD><TD>%08lx</TD></TR>\n",
            name,
            (unsigned long) _mulle_objc_infraclass_get_classid( cls));
   return( s);
}


char   *mulle_objc_class_describe_html_tiny( struct _mulle_objc_class *cls,
                                             struct _mulle_objc_htmltablestyle *styling)
{
   char          *tmp[ 2];
   unsigned int  i;

   i = 0;
   asprintf_table_header( &tmp[ i++], styling);
   mulle_asprintf( &tmp[ i++], "</TABLE>");

   assert( i <= 2);

   return( final_concat_auto_tmp( tmp, i));
}


char   *mulle_objc_class_describe_html( struct _mulle_objc_class *cls,
                                        int show_fields,
                                        struct _mulle_objc_htmltablestyle *styling)
{
   char                               *s;
   char                               *tmp[ 8];
   unsigned int                       i;
   struct _mulle_objc_htmltablestyle  style;

   style       = *styling;
   style.title = html_escape( cls->name);

   i = 0;

   // create single lines for each method and two for head/tail
   // fummel for graphviz
   asprintf_table_header( &tmp[ i++], &style);

   if( show_fields)
   {
      mulle_asprintf( &tmp[ i++],
               "<TR><TD>allocationsize</TD><TD>%lu</TD></TR>\n",
               cls->allocationsize);

      s = inheritance_description( _mulle_objc_class_get_inheritance( cls));
      mulle_asprintf( &tmp[ i++],
               "<TR><TD>inheritance</TD><TD>%s</TD></TR>\n",
               s);

      mulle_free( s);

      mulle_asprintf( &tmp[ i++],
               "<TR><TD>state</TD><TD>0x%lx</TD></TR>\n",
               (long) _mulle_atomic_pointer_read_nonatomic( &cls->state));

      if ( _mulle_objc_class_is_infraclass( cls))
      {
         struct _mulle_objc_infraclass   *infra;

         infra = _mulle_objc_class_as_infraclass( cls);
         mulle_asprintf( &tmp[ i++],
                  "<TR><TD>ivarhash</TD><TD>0x%lx</TD></TR>\n",
                  (long) infra->ivarhash);
         mulle_asprintf( &tmp[ i++],
                  "<TR><TD>allocatedInstances</TD><TD>%ld</TD></TR>\n",
                  (long) _mulle_atomic_pointer_read_nonatomic( &infra->allocatedInstances));
      }

      mulle_asprintf( &tmp[ i++],
               "<TR><TD>preloads</TD><TD>%u</TD></TR>\n",
               cls->preloads);
   }
   mulle_asprintf( &tmp[ i++],
            "</TABLE>");

   assert( i <= 8);

   return( final_concat_auto_tmp( tmp, i));
}


#pragma mark - ivarlist

char   *mulle_objc_ivarlist_describe_html( struct _mulle_objc_ivarlist *list,
                                           struct _mulle_objc_htmltablestyle *styling)
{
   size_t         len;
   char           **tmp;
   unsigned int   i;
   unsigned int   j;
   unsigned int   n;
   char           *format;

   n    = list->n_ivars + 2;
   tmp  = mulle_calloc( n, sizeof( char *));

   // create single lines for each method and two for head/tail
   i = 0;
   asprintf_table_header( &tmp[ i], styling);
   len = strlen( tmp[ i]);
   ++i;

   if( mulle_objc_environment_get_yes_no_default( "MULLE_OBJC_TERSE_TABLE", 0))
      format = "<TR>"
               "<TD>%s</TD>"
               "</TR>\n";
   else
      // TODO: hacky, where is the style or class name for this ???
      format = "<TR>"
               "<TD>%s</TD>"
               "<TD>"
                 "<TABLE COLOR=\"lightgray\" CELLPADDING=\"0\" CELLSPACING=\"0\">"
                   "<TR><TD>signature</TD><TD>%s</TD></TR>"
                   "<TR><TD>methodid</TD><TD>0x%lx</TD></TR>"
                   "<TR><TD>offset</TD><TD>%d</TD></TR>"
                 "</TABLE>"
               "</TD>"
               "</TR>\n";

   for( j = 0; j < list->n_ivars; j++)
   {
      mulle_asprintf( &tmp[ i],
                format,
                html_escape( list->ivars[ j].descriptor.name),
                html_escape( list->ivars[ j].descriptor.signature),
                (long) list->ivars[ j].descriptor.ivarid,
                list->ivars[ j].offset);

      len += strlen( tmp[ i]);
      ++i;
   }

   /* sort by name */
   mulle_qsort_r( &tmp[ i - j], j, sizeof( char *), strcmp_r, NULL);

   mulle_asprintf( &tmp[ i], "</TABLE>");
   len += strlen( tmp[ i]);
   ++i;

   assert( i <= n);

   return( final_concat_malloced_tmp_known_len( tmp, i, len));
}


char   *mulle_objc_ivarlist_describe_hor_html( struct _mulle_objc_ivarlist *list,
                                               struct _mulle_objc_htmltablestyle *styling)
{
   size_t         len;
   char           **tmp;
   unsigned int   i;
   unsigned int   j;
   unsigned int   n;
   char           *format;

   n   = list->n_ivars + 2;
   tmp = mulle_calloc( n, sizeof( char *));

   // create single lines for each method and two for head/tail
   i = 0;
   asprintf_table_header_colspan( &tmp[ i], styling, 4);
   len = strlen( tmp[ i]);
   ++i;

   if( mulle_objc_environment_get_yes_no_default( "MULLE_OBJC_TERSE_TABLE", 0))
      format = "<TR>"
               "<TD>%s</TD>"
               "</TR>\n";
   else
      format = "<TR>"
                 "<TD>%s</TD>"
                 "<TD>%s</TD>"
                 "<TD>%08x</TD>"
                 "<TD>%d</TD>"
               "</TR>\n";

   for( j = 0; j < list->n_ivars; j++)
   {
      mulle_asprintf( &tmp[ i],
               format,
               html_escape( list->ivars[ j].descriptor.name),
               html_escape( list->ivars[ j].descriptor.signature),
               list->ivars[ j].descriptor.ivarid,
               list->ivars[ j].offset);
      len += strlen( tmp[ i]);
      ++i;
   }

   mulle_asprintf( &tmp[ i], "</TABLE>");
   len += strlen( tmp[ i]);
   ++i;

   assert( i <= n);

   return( final_concat_malloced_tmp_known_len( tmp, i, len));
}


#pragma mark - descriptor


char   *mulle_objc_descriptor_describe_html( struct _mulle_objc_descriptor *desc,
                                             struct _mulle_objc_htmltablestyle *styling)
{
   char   *tmp[ 2];
   char   *th;
   char   *format;

   th = styling->classprefix ? "TH" : "TD";

   asprintf_table_header( &tmp[ 0], styling);

   if( mulle_objc_environment_get_yes_no_default( "MULLE_OBJC_TERSE_TABLE", 0))
      format = "<TR>"
               "<%s>%s</%s>"
               "</TR>"
               "</TABLE>";
   else
      format = "<TR><%s>name</%s><TD>%s</TD></TR>"
               "<TR><%s>signature</%s><TD>%s</TD></TR>"
               "<TR><%s>methodid</%s><TD>%08lx</TD></TR>"
               "<TR><%s>bits</%s><TD>0x%x</TD></TR>"
               "</TABLE>";

   mulle_asprintf( &tmp[ 1],
                   format,
                   th, th,
                   html_escape( desc->name),
                   th, th,
                   html_escape( desc->signature),
                   th, th,
                   (unsigned long) desc->methodid,
                   th, th,
                   desc->bits);

   return( final_concat_auto_tmp( tmp, 2));
}


char   *mulle_objc_descriptor_describe_hor_html( struct _mulle_objc_descriptor *desc)
{
   char   *s;
   char   *format;

   if( mulle_objc_environment_get_yes_no_default( "MULLE_OBJC_TERSE_TABLE", 0))
      format = "<TR>"
               "<TD>%s</TD>"
               "</TR>";
   else
      format = "<TR>"
               "<TD>%s</TD>"
               "<TD>%s</TD>"
               "<TD>%08lx</TD>"
               "<TD>0x%x</TD>"
               "</TR>";

   mulle_asprintf( &s,
            format,
            html_escape( desc->name),
            html_escape( desc->signature),
            (unsigned long) desc->methodid,
            desc->bits);

   return( s);
}


char   *mulle_objc_descriptor_describe_row_html( intptr_t  methodid,
                                                 void *value,
                                                 struct _mulle_objc_htmltablestyle *styling)
{
   struct _mulle_objc_descriptor    *desc = value;
   char                             *s;
   char                             *format;

   if( mulle_objc_environment_get_yes_no_default( "MULLE_OBJC_TERSE_TABLE", 0))
      format = "<TR>"
               "<TD>%s</TD>"
               "</TR>\n";
   else
      format = "<TR>"
               "<TD>%s</TD>"
               "<TD>%s</TD>"
               "<TD>%08lx</TD>"
               "<TD>0x%x</TD>"
               "</TR>\n";

   mulle_asprintf( &s,
             format,
             html_escape( desc->name),
             html_escape( desc->signature),
             (unsigned long) desc->methodid,
             desc->bits);

   return( s);
}


#pragma mark - categories

char   *mulle_objc_category_describe_row_html( intptr_t  categoryid,
                                               void *value,
                                               struct _mulle_objc_htmltablestyle *styling)
{
   char  *s;

   mulle_asprintf( &s,
                   "<TR>"
                   "<TD>%s</TD>"
                   "<TD>%08lx</TD>"
                   "</TR>\n",
                   html_escape( value),
                   (unsigned long) categoryid);

   return( s);
}

#pragma mark - supers

char   *mulle_objc_super_describe_row_html( intptr_t  superid,
                                            void *value,
                                            struct _mulle_objc_htmltablestyle *styling)
{
   struct _mulle_objc_super  *superinfo = value;
   char  *s;

   mulle_asprintf( &s,
                   "<TR>"
                   "<TD>%s</TD>"
                   "<TD>%08lx</TD>"
                   "</TR>\n",
                   html_escape( _mulle_objc_super_get_name( superinfo)),
                   (unsigned long) superid);

   return( s);
}


#pragma mark - protocols

char   *mulle_objc_protocol_describe_row_html( intptr_t  protocolid,
                                               void *value,
                                               struct _mulle_objc_htmltablestyle *styling)
{
   struct _mulle_objc_protocol  *protocol = value;
   char  *s;

   mulle_asprintf( &s,
                   "<TR>"
                   "<TD>%s</TD>"
                   "<TD>%08lx</TD>"
                   "</TR>\n",
                   html_escape( _mulle_objc_protocol_get_name( protocol)),
                   (unsigned long) protocolid);

   return( s);
}



#pragma mark - propertylist

char   *mulle_objc_propertylist_describe_html( struct _mulle_objc_propertylist *list,
                                               struct _mulle_objc_htmltablestyle *styling)
{
   size_t         len;
   char           **tmp;
   unsigned int   i;
   unsigned int   j;
   unsigned int   n;
   char           *format;

   n   = list->n_properties + 2;
   tmp = mulle_calloc( n, sizeof( char *));

   // create single lines for each method and two for head/tail
   i = 0;
   asprintf_table_header( &tmp[ i], styling);
   len = strlen( tmp[ i]);
   ++i;

   if( mulle_objc_environment_get_yes_no_default( "MULLE_OBJC_TERSE_TABLE", 0))
      format = "<TR>"
               "<TD>%s</TD>"
               "</TR>\n";
   else
      format = "<TR>"
               "<TD>%s</TD>"
               "<TD>"
                 "<TABLE COLOR=\"lightgray\" CELLPADDING=\"0\" CELLSPACING=\"0\">"
                   "<TR><TD>signature</TD><TD>%s</TD></TR>"
                   "<TR><TD>propertyid</TD><TD>%08lx</TD></TR>"
                   "<TR><TD>ivarid</TD><TD>%08lx</TD></TR>"
                   "<TR><TD>getter</TD><TD>%08lx</TD></TR>"
                   "<TR><TD>setter</TD><TD>%08lx</TD></TR>"
                   "<TR><TD>bits</TD><TD>0x%08lx</TD></TR>"
                 "</TABLE>"
               "</TD>"
               "</TR>\n";

   for( j = 0; j < list->n_properties; j++)
   {
      mulle_asprintf( &tmp[ i],
                format,
                html_escape( list->properties[ j].name),
                html_escape( list->properties[ j].signature),
                (unsigned long) list->properties[ j].propertyid,
                (unsigned long) list->properties[ j].ivarid,
                (unsigned long) list->properties[ j].getter,
                (unsigned long) list->properties[ j].setter,
                (unsigned long) list->properties[ j].bits);

      len += strlen( tmp[ i]);
      ++i;
   }

   mulle_qsort_r( &tmp[ i - j], j, sizeof( char *), strcmp_r, NULL);

   mulle_asprintf( &tmp[ i], "</TABLE>");
   len += strlen( tmp[ i]);
   ++i;

   assert( i <= n);

   return( final_concat_malloced_tmp_known_len( tmp, i, len));
}


#pragma mark - cache

char   *mulle_objc_cache_describe_html( struct _mulle_objc_cache *cache,
                                        struct _mulle_objc_universe *universe,
                                        struct _mulle_objc_htmltablestyle *styling)
{
   size_t                  len;
   char                    **tmp;
   unsigned int            i;
   unsigned int            j;
   unsigned int            n;
   int                     index;
   mulle_objc_methodid_t   sel;
   int                     colspan;
   char                    *s;

#ifdef MULLE_OBJC_CACHEENTRY_REMEMBERS_THREAD_CLASS
   colspan = 6;
#else
   colspan = 5;
#endif
   n   = cache->size + 3 + 2;
   tmp = mulle_calloc( n, sizeof( char *));

   i = 0;
   asprintf_table_header_colspan( &tmp[ i], styling, colspan);
   len = strlen( tmp[ i]);
   ++i;

   mulle_asprintf( &tmp[ i],
                   "<TR><TD>n</TD><TD COLSPAN=\"%d\">%lu</TD></TR>\n",
                   colspan,
                   (long) _mulle_atomic_pointer_read_nonatomic( &cache->n));
   len += strlen( tmp[ i]);
   ++i;
   mulle_asprintf( &tmp[ i],
                   "<TR><TD>mask</TD><TD COLSPAN=\"%d\">0x%lx</TD></TR>\n",
                   colspan,
                   (long) cache->mask);
   len += strlen( tmp[ i]);
   ++i;

   s = "";
   for( j = 0; j < cache->size; j++)
   {
      index = 0;
      sel   = cache->entries[ j].key.uniqueid;
      if( sel)
         index = _mulle_objc_cache_probe_entryindex( cache, sel);

#ifdef MULLE_OBJC_CACHEENTRY_REMEMBERS_THREAD_CLASS
      mulle_asprintf( &s, "<TD>%p</TD>", cache->entries[ j].thread);
#endif
      mulle_asprintf( &tmp[ i], "<TR><TD>#%ld</TD><TD>%08lx</TD><TD>%s</TD>"
                                "<TD>%p</TD><TD>%d (%x)</TD>%s</TR>\n",
                                j,
                                sel,
                                (unsigned long) _mulle_objc_universe_describe_methodid( universe, sel),
                                cache->entries[ j].value.functionpointer,
                                index,
                                sel & cache->mask,
                                s);
#ifdef MULLE_OBJC_CACHEENTRY_REMEMBERS_THREAD_CLASS
      mulle_free( s);
#endif
      len += strlen( tmp[ i]);
      ++i;
   }

   mulle_asprintf( &tmp[ i], "</TABLE>");
   len += strlen( tmp[ i]);
   ++i;

   assert( i <= n);

   return( final_concat_malloced_tmp_known_len( tmp, i, len));
}


#pragma mark - methodlist

char   *mulle_objc_methodlist_describe_html( struct _mulle_objc_methodlist *list,
                                             struct _mulle_objc_universe *universe,
                                             int show_fields,
                                             struct _mulle_objc_htmltablestyle *styling)
{
   size_t         len;
   char           **tmp;
   unsigned int   i;
   unsigned int   j;
   unsigned int   n;
   char           *format;
   char           *name;

   n   = list->n_methods + 3;
   tmp = mulle_calloc( n, sizeof( char *));

   // create single lines for each method and two for head/tail
   i = 0;
   asprintf_table_header( &tmp[ i], styling);
   len = strlen( tmp[ i]);
   ++i;

   name = _mulle_objc_methodlist_get_categoryname( list);
   if( name)
   {
      mulle_asprintf( &tmp[ i], "<TR><TD>category</TD><TD>%s</TD></TR>\n", name);
      len += strlen( tmp[ i]);
      ++i;
   }

   if( show_fields)
      format = "<TR>"
               "<TD>%s</TD>"
               "<TD>"
                 "<TABLE COLOR=\"lightgray\" CELLPADDING=\"0\" CELLSPACING=\"0\">"
                   "<TR><TD>signature</TD><TD>%s</TD></TR>"
                   "<TR><TD>methodid</TD><TD>0x%lx</TD></TR>"
                   "<TR><TD>bits</TD><TD>0x%lx</TD></TR>"
                   "<TR><TD>implementation</TD><TD>%s</TD></TR>"
                 "</TABLE>"
               "</TD>"
               "</TR>\n";
   else
      format = "<TR>"
               "<TD COLSPAN=\"2\">%s</TD>"
               "</TR>\n";

   mulle_buffer_do( buffer)
   {
      for( j = 0; j < list->n_methods; j++)
      {
         mulle_buffer_reset( buffer);
         mulle_buffer_sprintf_functionpointer( buffer,
                                               (mulle_functionpointer_t) &list->methods[ i].value);

         mulle_asprintf( &tmp[ i],
                   format,
                   html_escape( list->methods[ j].descriptor.name),
                   html_escape( list->methods[ j].descriptor.signature),
                   (unsigned long) list->methods[ j].descriptor.methodid,
                   (unsigned long) list->methods[ j].descriptor.bits,
                   mulle_buffer_get_string( buffer));

         len += strlen( tmp[ i]);
         ++i;
      }
   }

   /* sort by name */
   mulle_qsort_r( &tmp[ i - j], j, sizeof( char *), strcmp_r, NULL);

   mulle_asprintf( &tmp[ i], "</TABLE>");
   len += strlen( tmp[ i]);
   ++i;

   assert( i <= n);

   return( final_concat_malloced_tmp_known_len( tmp, i, len));
}


char   *mulle_objc_methodlist_describe_hor_html( struct _mulle_objc_methodlist *list,
                                                 struct _mulle_objc_htmltablestyle *styling)
{
   size_t         len;
   char           **tmp;
   unsigned int   i;
   unsigned int   j;
   unsigned int   n;
   char           *format;

   n   = list->n_methods + 3;
   tmp = mulle_calloc( n, sizeof( char *));

   // create single lines for each method and two for head/tail
   i = 0;
   asprintf_table_header_colspan( &tmp[ i], styling, 5);
   len = strlen( tmp[ i]);
   ++i;


   if( list->n_methods)
   {
      if( mulle_objc_environment_get_yes_no_default( "MULLE_OBJC_TERSE_TABLE", 0))
      {
         if( styling->classprefix)
            format = "<TR>"
                        "<TH>name</TH>"
                     "</TR>\n";
         else
            format = "<TR>"
                        "<TD>name</TD>"
                     "</TR>\n";
      }
      else
      {
         if( styling->classprefix)
            format = "<TR>"
                        "<TH>name</TH>"
                        "<TH>signature</TH>"
                        "<TH>methodid</TH>"
                        "<TH>bits</TH>"
                        "<TH>implementation</TH>"
                     "</TR>\n";
         else
            format =  "<TR>"
                        "<TD>name</TD>"
                        "<TD>signature</TD>"
                        "<TD>methodid</TD>"
                        "<TD>bits</TD>"
                        "<TD>implementation</TD>"
                      "</TR>\n";
      }

      mulle_asprintf( &tmp[ i],
                format);

      len += strlen( tmp[ i]);
      ++i;
   }

   if( mulle_objc_environment_get_yes_no_default( "MULLE_OBJC_TERSE_TABLE", 0))
      format = "<TR>"
                 "<TD>%s</TD>"
               "</TR>\n";
   else
      format = "<TR>"
                 "<TD>%s</TD>"
                 "<TD>%s</TD>"
                 "<TD>%08lx</TD>"
                 "<TD>0x%lx</TD>"
                 "<TD>%s</TD>"
               "</TR>\n";

   mulle_buffer_do( buffer)
   {
      for( j = 0; j < list->n_methods; j++)
      {
         mulle_buffer_reset( buffer);
         mulle_buffer_sprintf_functionpointer( buffer,
                                               (mulle_functionpointer_t) &list->methods[ i].value);
         mulle_asprintf( &tmp[ i],
                         format,
                         html_escape( list->methods[ j].descriptor.name),
                         html_escape( list->methods[ j].descriptor.signature),
                         (unsigned long) list->methods[ j].descriptor.methodid,
                         (unsigned long) list->methods[ j].descriptor.bits,
                         mulle_buffer_get_string( buffer));
         len += strlen( tmp[ i]);
         ++i;
      }
   }

   mulle_qsort_r( &tmp[ i - j], j, sizeof( char *), strcmp_r, NULL);

   mulle_asprintf( &tmp[ i], "</TABLE>");
   len += strlen( tmp[ i]);
   ++i;

   assert( i <= n);

   return( final_concat_malloced_tmp_known_len( tmp, i, len));
}


#pragma mark - loadclass

char   *mulle_objc_loadclass_describe_row_html( void *value,
                                                struct _mulle_objc_htmltablestyle *styling)
{
   struct _mulle_objc_loadclass   *loadcls = value;
   char   *s;

   mulle_asprintf( &s, "<TR><TD>%s</TD><TD>%08lx</TD></TR>\n",
            loadcls->classname,
            (unsigned long) loadcls->classid);
   return( s);
}


#pragma mark - loadcategory

char   *mulle_objc_loadcategory_describe_row_html( void *value,
                                                   struct _mulle_objc_htmltablestyle *styling)
{
   struct _mulle_objc_loadcategory   *loadcat = value;
   char   *s;

   mulle_asprintf( &s, "<TR><TD>%s( %s)</TD><TD>%08lx</TD></TR>\n",
                       loadcat->classname,
                       loadcat->categoryname,
                       (unsigned long) loadcat->categoryid);
   return( s);
}



#pragma mark - protocols

char   *mulle_objc_protocols_describe_html( struct _mulle_objc_uniqueidarray *array,
                                            struct _mulle_objc_universe *universe,
                                            struct _mulle_objc_htmltablestyle *styling)
{
   return( mulle_objc_uniqueidarray_describe_html( array,
                                                   protocolid_describe_row_html,
                                                   universe,
                                                   styling));
}


#pragma mark - categories

char   *mulle_objc_categories_describe_html( struct _mulle_objc_uniqueidarray *array,
                                             struct _mulle_objc_universe *universe,
                                             struct _mulle_objc_htmltablestyle *styling)
{
   return( mulle_objc_uniqueidarray_describe_html( array,
                                                   categoryid_describe_row_html,
                                                   universe,
                                                   styling));
}


#pragma mark - fastclasses

char   *mulle_objc_fastclasstable_describe_html( struct _mulle_objc_fastclasstable *fastclasstable,
                                                 char *(row_description)( unsigned int i,
                                                                          struct _mulle_objc_infraclass *,
                                                                          struct _mulle_objc_htmltablestyle *),
                                                 struct _mulle_objc_htmltablestyle *styling)

{
   size_t         count;
   size_t         len;
   char           **tmp;
   unsigned int   i;
   unsigned int   j;
   unsigned int   n;
   void           *value;

   count = MULLE_OBJC_S_FASTCLASSES;

   n   = (unsigned int) count + 2;
   tmp = mulle_calloc( n, sizeof( char *));

   tmp[ 0] = NULL;

   i   = 0;
   len = 0;

   if( styling)
   {
      asprintf_table_header( &tmp[ i], styling);
      len += strlen( tmp[ i]);
      ++i;
   }

   for( j = 0; j < MULLE_OBJC_S_FASTCLASSES; j++)
   {
      value = _mulle_atomic_pointer_read_nonatomic( &fastclasstable->classes[ j].pointer);
      tmp[ i] = (*row_description)( j, value, styling);
      len    += strlen( tmp[ i]);
      ++i;
   }

   if( styling)
   {
      mulle_asprintf( &tmp[ i], "</TABLE>");
      len += strlen( tmp[ i]);
      ++i;
   }

   assert( i <= n);

   return( final_concat_malloced_tmp_known_len( tmp, i, len));
}


#pragma mark - pointerarray

char   *mulle_concurrent_pointerarray_describe_html( struct mulle_concurrent_pointerarray *list,
                                                     char *(row_description)( void *,
                                                                              struct _mulle_objc_htmltablestyle *),
                                                     struct _mulle_objc_htmltablestyle *styling)

{
   struct mulle_concurrent_pointerarrayenumerator   rover;
   size_t                                           count;
   size_t                                           len;
   char                                             **tmp;
   unsigned int                                     i;
   unsigned int                                     n;
   void                                             *value;

   count = mulle_concurrent_pointerarray_get_count( list);

   n   = (unsigned int) count + 2;
   tmp = mulle_calloc( n, sizeof( char *));

   tmp[ 0] = NULL;

   i   = 0;
   len = 0;

   if( styling)
   {
      asprintf_table_header( &tmp[ i], styling);
      len += strlen( tmp[ i]);
      ++i;
   }

   rover = mulle_concurrent_pointerarray_enumerate( list);
   while( value = _mulle_concurrent_pointerarrayenumerator_next( &rover))
   {
      tmp[ i] = (*row_description)( value, styling);
      len    += strlen( tmp[ i]);
      ++i;
   }

   if( styling)
   {
      mulle_asprintf( &tmp[ i], "</TABLE>");
      len += strlen( tmp[ i]);
      ++i;
   }

   assert( i <= n);

   return( final_concat_malloced_tmp_known_len( tmp, i, len));
}


#pragma mark - hashmap

char   *mulle_concurrent_hashmap_describe_html( struct mulle_concurrent_hashmap *map,
                                                char *(row_description)( intptr_t,
                                                                         void *,
                                                                         struct _mulle_objc_htmltablestyle *),
                                                struct _mulle_objc_htmltablestyle *styling)
{
   char           *null_description;
   intptr_t       uniqueid;
   size_t         count;
   size_t         len;
   char           **tmp;
   unsigned int   i;
   unsigned int   n;
   unsigned int   j;
   void           *value;

   count = mulle_concurrent_hashmap_count( map);

   n   = (unsigned int) count + 2;
   tmp = mulle_calloc( n, sizeof( char *));


   i   = 0;
   len = 0;

   if( styling)
   {
      asprintf_table_header( &tmp[ i], styling);
      len += strlen( tmp[ i]);
      ++i;
   }

   null_description = "*null*";

   j = 0;

   mulle_concurrent_hashmap_for( map, uniqueid, value)
   {
      if( ! value)
         value = null_description;
      tmp[ i] = (*row_description)( uniqueid, value, styling);
      len    += strlen( tmp[ i]);
      ++i;
      ++j;
   }

   mulle_qsort_r( &tmp[ i - j], j, sizeof( char *), strcmp_r, NULL);

   if( styling)
   {
      mulle_asprintf( &tmp[ i], "</TABLE>");
      len += strlen( tmp[ i]);
      ++i;
   }

   assert( i <= n);

   return( final_concat_malloced_tmp_known_len( tmp, i, len));
}


#pragma mark - uniqueidarray

char   *mulle_objc_uniqueidarray_describe_html( struct _mulle_objc_uniqueidarray *array,
                                                char *(row_description)( void *,
                                                                         struct _mulle_objc_universe *,
                                                                         struct _mulle_objc_htmltablestyle *),
                                                struct _mulle_objc_universe *universe,
                                                struct _mulle_objc_htmltablestyle *styling)

{
   unsigned int            count;
   size_t                  len;
   char                    **tmp;
   unsigned int            i;
   unsigned int            n;
   mulle_objc_uniqueid_t   *p;
   mulle_objc_uniqueid_t   *sentinel;

   count = array->n;

   n   = count + 2;
   tmp = mulle_calloc( n, sizeof( char *));

   tmp[ 0] = NULL;

   i   = 0;
   len = 0;

   if( styling)
   {
      asprintf_table_header( &tmp[ i], styling);
      len += strlen( tmp[ i]);
      ++i;
   }

   p        = array->entries;
   sentinel = &p[ count];

   while( p < sentinel)
   {
      tmp[ i] = (*row_description)( (void *) (uintptr_t) *p++, universe, styling);
      len    += strlen( tmp[ i]);
      ++i;
   }

   mulle_qsort_r( &tmp[ i - count], count, sizeof( char *), strcmp_r, NULL);

   if( styling)
   {
      mulle_asprintf( &tmp[ i], "</TABLE>");
      len += strlen( tmp[ i]);
      ++i;
   }

   assert( i <= n);

   return( final_concat_malloced_tmp_known_len( tmp, i, len));
}

