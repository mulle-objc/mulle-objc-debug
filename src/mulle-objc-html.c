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

static void  mulle_buffer_add_inheritance_description( struct mulle_buffer *buffer,
                                                       unsigned int inheritance)
{
   int   first = 1;

   if( ! (inheritance & MULLE_OBJC_CLASS_DONT_INHERIT_SUPERCLASS))
   {
      mulle_buffer_add_string( buffer, "superclass");
      first = 0;
   }
   if( ! (inheritance & MULLE_OBJC_CLASS_DONT_INHERIT_CATEGORIES))
   {
      if( ! first)
         mulle_buffer_add_string( buffer, " ");
      mulle_buffer_add_string( buffer, "categories");
      first = 0;
   }
   if( ! (inheritance & MULLE_OBJC_CLASS_DONT_INHERIT_PROTOCOLS))
   {
      if( ! first)
         mulle_buffer_add_string( buffer, " ");
      mulle_buffer_add_string( buffer, "protocols");
      first = 0;
   }
   if( ! (inheritance & MULLE_OBJC_CLASS_DONT_INHERIT_PROTOCOL_CATEGORIES))
   {
      if( ! first)
         mulle_buffer_add_string( buffer, " ");
      mulle_buffer_add_string( buffer, "protocol_categories");
   }
}



void   mulle_buffer_html_categoryid_element( struct mulle_buffer *buffer,
                                     unsigned int row,
                                     mulle_objc_uniqueid_t categoryid,
                                     struct _mulle_objc_htmltablestyle *styling,
                                     void *userinfo)
{
   struct _mulle_objc_universe   *universe = userinfo;
   char                          *s;

   s = _mulle_objc_universe_describe_categoryid( universe, categoryid);
   mulle_buffer_sprintf( buffer, "<TR><TD>\"%s\"</TD><TD>%08lx</TD></TR>\n",
         s, (unsigned long) categoryid);
}



void   mulle_buffer_html_protocolid_element( struct mulle_buffer *buffer,
                                     unsigned int row,
                                     mulle_objc_uniqueid_t protocolid,
                                     struct _mulle_objc_htmltablestyle *styling,
                                     void *userinfo)
{
   struct _mulle_objc_universe *universe = userinfo;
   char                        *s;

   s = _mulle_objc_universe_describe_protocolid( universe, protocolid);
   mulle_buffer_sprintf( buffer, "<TR><TD>\"%s\"</TD><TD>%08lx</TD></TR>\n",
         s, (unsigned long) protocolid);
}



#pragma mark - universe

static void   mulle_buffer_add_column_headers( struct mulle_buffer *buffer,
                                               struct _mulle_objc_htmltablestyle *styling,
                                               int num_columns)
{
   char   **header;
   char   *th;
   int    i;

   if( ! styling->headers || num_columns <= 2)
      return;

   th = styling->classprefix ? "TH" : "TD";

   if( styling->classprefix)
      mulle_buffer_sprintf( buffer, "<TR CLASS=\"%s_table_subheader\">", styling->classprefix);
   else
      mulle_buffer_add_string( buffer, "<TR>");

   for( i = 0, header = styling->headers; i < num_columns && *header; i++, header++)
   {
      mulle_buffer_sprintf( buffer, "<%s>%s</%s>", th, *header, th);
   }

   mulle_buffer_add_string( buffer, "</TR>\n");
}


void   mulle_buffer_add_table_header_colspan( struct mulle_buffer *buffer,
                                              struct _mulle_objc_htmltablestyle *styling,
                                              unsigned int colspan)
{
   int   num_columns;

   num_columns = colspan ? colspan : 2;

   if( styling->classprefix)
      mulle_buffer_sprintf( buffer,
               "<TABLE CLASS=\"%s_table\">\n<TR CLASS=\"%s_table_header\"><TH COLSPAN=\"%u\">%s</TH></TR>\n",
               styling->classprefix,
               styling->classprefix,
               num_columns,
               styling->title);
   else // graphviz don't like CLASS, can't deal with TH
      mulle_buffer_sprintf( buffer,
               "<TABLE CELLPADDING=\"0\" CELLSPACING=\"0\">\n<TR><TD BGCOLOR=\"%s\" COLSPAN=\"%u\"><FONT COLOR=\"%s\">%s</FONT></TD></TR>\n",
               styling->bgcolor,
               num_columns,
               styling->color,
               styling->title);

   mulle_buffer_add_column_headers( buffer, styling, num_columns);
}



void   mulle_buffer_html_universe( struct mulle_buffer *buffer,
                                   struct _mulle_objc_universe *universe,
                                   struct _mulle_objc_htmltablestyle *styling)
{
   // create single lines for each method and two for head/tail
   mulle_buffer_add_table_header_colspan( buffer, styling, styling->colspan);

   mulle_buffer_sprintf( buffer,
            "<TR><TD>version</TD><TD>0x%x</TD></TR>\n",
            universe->version);
   mulle_buffer_add_string( buffer, "</TABLE>");
}



#pragma mark - supers

void   mulle_buffer_html_super_entry( struct mulle_buffer *buffer,
                                      intptr_t hash,
                                      void *value,
                                      struct _mulle_objc_htmltablestyle *styling,
                                      void *userinfo)
{
   struct _mulle_objc_super *super = value;

   MULLE_C_UNUSED( hash);
   MULLE_C_UNUSED( styling);
   MULLE_C_UNUSED( userinfo);

   mulle_buffer_sprintf( buffer,
                "<TR>"
                "<TD>%s</TD>"
                "<TD>%08lx</TD>"
                "</TR>\n",
                html_escape( _mulle_objc_super_get_name( super)),
                (unsigned long) super->superid);
}



#pragma mark - static strings


void  mulle_buffer_html_staticstring( struct mulle_buffer *buffer,
                                     struct _mulle_objc_staticstring *string,
                                     struct _mulle_objc_htmltablestyle *styling)
{
   char   *th;

   th = styling->classprefix ? "TH" : "TD";
   // create single lines for each method and two for head/tail
   mulle_buffer_add_table_header_colspan( buffer, styling, styling->colspan);

   mulle_buffer_sprintf( buffer,
            "<TR><%s>_s</%s><TD>%s</TD></TR>\n",
               th, th,
               html_escape( string->_s ? string->_s : "*null*"));
   mulle_buffer_sprintf( buffer,
            "<TR><%s>_len</%s><TD>%d</TD></TR>\n",
               th, th,
               string->_len);
   mulle_buffer_add_string( buffer, "</TABLE>");
}


//char  *mulle_buffer_html_staticstring_hor( struct _mulle_objc_staticstring *string,
//                                                  struct _mulle_objc_htmltablestyle *styling)
//{
//   char *s;
//
//   mulle_buffer_do_string( buffer, NULL, s)
//   {
//      mulle_buffer_sprintf( buffer, "%s", html_escape( string->_s ? string->_s : "*null*"));
//   }
//   return( s);
//}
//
//
//

void   mulle_buffer_html_staticstring_element( struct mulle_buffer *buffer,
                                               unsigned int row,
                                               void *value,
                                               struct _mulle_objc_htmltablestyle *styling,
                                               void *userinfo)
{
   struct _mulle_objc_staticstring *string = value;

   mulle_buffer_sprintf( buffer,
            "<TR>"
            "<TD COLSPAN=\"2\">%s</TD>"
            "</TR>\n",
            html_escape( string->_s ? string->_s : "*null*"));
}


//void   mulle_buffer_html_fastclass_row( struct mulle_buffer *buffer,
//                                        unsigned int i,
//                                        struct _mulle_objc_infraclass *infra,
//                                        struct _mulle_objc_htmltablestyle *styling)
//{
//   mulle_buffer_sprintf( buffer,
//            "<TR>"
//            "<TD>%u</TD>"
//            "<TD>%s</TD>"
//            "</TR>\n",
//            i,
//            html_escape( infra ? _mulle_objc_infraclass_get_name( infra) : "*null*"));
//}

#pragma mark - classes

void   mulle_buffer_html_class_short( struct mulle_buffer *buffer,
                                      struct _mulle_objc_class *cls,
                                      struct _mulle_objc_htmltablestyle *styling)
{
   char  *name;

   MULLE_C_UNUSED( styling);

   name = html_escape( cls->name);
   mulle_buffer_sprintf( buffer, "<a href=\"%s.html\">%s</a>\n",
            name,
            name);
}


// Updated to match mulle_buffer_html_entry_t callback signature
void   mulle_buffer_html_class_entry( struct mulle_buffer *buffer,
                                      intptr_t classid,
                                      void *cls,
                                      struct _mulle_objc_htmltablestyle *styling,
                                      void *userinfo)
{
   char   *name;
   char   *prefix;

   name   = html_escape( _mulle_objc_class_get_name( cls));
   prefix = _mulle_objc_class_is_metaclass( cls) ? "+" : "";

   mulle_buffer_sprintf( buffer,
            "<TR><TD><A HREF=\"%s%s.html\">%s</A></TD><TD>%08lx</TD></TR>\n",
            prefix,
            name,
            name,
            (unsigned long) _mulle_objc_class_get_classid( cls));
}


//void   mulle_buffer_html_class_row( struct mulle_buffer *buffer,
//                                    intptr_t  classid,
//                                    void *cls,
//                                    struct _mulle_objc_htmltablestyle *styling)
//{
//   char   *name;
//   char   *prefix;
//
//   MULLE_C_UNUSED( classid);
//   MULLE_C_UNUSED( styling);
//
//   name   = html_escape( _mulle_objc_class_get_name( cls));
//   prefix = _mulle_objc_class_is_metaclass( cls) ? "+" : "";
//
//   mulle_buffer_sprintf( buffer, "<TR><TD><A HREF=\"%s%s.html\">%s</A></TD><TD>%08lx</TD></TR>\n",
//            prefix,
//            name,
//            name,
//            (unsigned long) _mulle_objc_class_get_classid( cls));
//}
//
//
//void   mulle_buffer_html_infraclass_row( struct mulle_buffer *buffer,
//                                         intptr_t  classid,
//                                         void *cls,
//                                         struct _mulle_objc_htmltablestyle *styling)
//{
//   char   *name;
//
//   MULLE_C_UNUSED( classid);
//   MULLE_C_UNUSED( styling);
//
//   name = html_escape( _mulle_objc_infraclass_get_name( cls));
//   mulle_buffer_sprintf( buffer, "<TR><TD>%s</TD><TD>%08lx</TD></TR>\n",
//            name,
//            (unsigned long) _mulle_objc_infraclass_get_classid( cls));
//}


void   mulle_buffer_html_class_tiny( struct mulle_buffer *buffer,
                                            struct _mulle_objc_class *cls,
                                            struct _mulle_objc_htmltablestyle *styling)
{
   mulle_buffer_add_table_header_colspan( buffer, styling, styling->colspan);
   mulle_buffer_add_string( buffer, "</TABLE>");
}


void   mulle_buffer_html_class( struct mulle_buffer *buffer,
                                struct _mulle_objc_class *cls,
                                int show_fields,
                                struct _mulle_objc_htmltablestyle *styling)
{
   struct _mulle_objc_htmltablestyle  style;

   style       = *styling;
   style.title = html_escape( cls->name);

   // create single lines for each method and two for head/tail
   // fummel for graphviz
   mulle_buffer_add_table_header_colspan( buffer, &style, style.colspan);

   if( show_fields)
   {
      mulle_buffer_sprintf( buffer,
               "<TR><TD>allocationsize</TD><TD>%lu</TD></TR>\n",
               cls->allocationsize);

      mulle_buffer_sprintf( buffer,
               "<TR><TD>inheritance</TD><TD>");
      mulle_buffer_add_inheritance_description( buffer, _mulle_objc_class_get_inheritance( cls));
      mulle_buffer_add_string( buffer, "</TD></TR>\n");

      mulle_buffer_sprintf( buffer,
               "<TR><TD>state</TD><TD>%p</TD></TR>\n",
               _mulle_atomic_pointer_read_nonatomic( &cls->state));

      if ( _mulle_objc_class_is_infraclass( cls))
      {
         struct _mulle_objc_infraclass   *infra;

         infra = _mulle_objc_class_as_infraclass( cls);
         mulle_buffer_sprintf( buffer,
                  "<TR><TD>ivarhash</TD><TD>0x%lx</TD></TR>\n",
                  (long) infra->ivarhash);
         mulle_buffer_sprintf( buffer,
                  "<TR><TD>allocatedInstances</TD><TD>%td</TD></TR>\n",
                  (intptr_t) _mulle_atomic_pointer_read_nonatomic( &infra->allocatedInstances));
      }
   }
   mulle_buffer_add_string( buffer, "</TABLE>");
}


#pragma mark - ivarlist

//char   *mulle_objc_ivarlist_html( struct _mulle_objc_ivarlist *list,
//                                        struct _mulle_objc_htmltablestyle *styling)
//{
//   char   *s;
//
//   mulle_buffer_do_string( buffer, NULL, s)
//   {
//      struct mulle_pointerarray   array;
//      char                        *format;
//      unsigned int                j;
//
//      mulle_buffer_add_table_header_colspan( buffer, styling, styling->colspan);
//
//      if( mulle_objc_environment_get_yes_no_default( "MULLE_OBJC_TERSE_TABLE", 0))
//         format = "<TR>"
//                  "<TD>%s</TD>"
//                  "</TR>\n";
//      else
//         // TODO: hacky, where is the style or class name for this ???
//         format = "<TR>"
//                  "<TD>%s</TD>"
//                  "<TD>"
//                    "<TABLE COLOR=\"lightgray\" CELLPADDING=\"0\" CELLSPACING=\"0\">"
//                      "<TR><TD>signature</TD><TD>%s</TD></TR>"
//                      "<TR><TD>methodid</TD><TD>0x%lx</TD></TR>"
//                      "<TR><TD>offset</TD><TD>%d</TD></TR>"
//                    "</TABLE>"
//                  "</TD>"
//                  "</TR>\n";
//
//      mulle_pointerarray_init( &array, 0, NULL);
//
//      for( j = 0; j < list->n_ivars; j++)
//      {
//         char   *row;
//
//         mulle_buffer_do_string( tmp_buffer, NULL, row)
//         {
//            mulle_buffer_sprintf( tmp_buffer,
//                      format,
//                      html_escape( list->ivars[ j].descriptor.name),
//                      html_escape( list->ivars[ j].descriptor.signature),
//                      (long) list->ivars[ j].descriptor.ivarid,
//                      list->ivars[ j].offset);
//         }
//         mulle_pointerarray_add( &array, row);
//      }
//
//      /* sort by name */
//      mulle_qsort_r( _mulle_pointerarray_get_storage( &array),
//                     mulle_pointerarray_get_count( &array),
//                     sizeof( char *),
//                     strcmp_r,
//                     NULL);
//
//      {
//         char   *row;
//
//         mulle_pointerarray_for( &array, row)
//         {
//            mulle_buffer_add_string( buffer, row);
//            mulle_free( row);
//         }
//      }
//
//      mulle_pointerarray_done( &array);
//
//      mulle_buffer_add_string( buffer, "</TABLE>");
//   }
//
//   return( s);
//}


void   mulle_buffer_html_ivarlist_hor( struct mulle_buffer *buffer,
                                       struct _mulle_objc_ivarlist *list,
                                       struct _mulle_objc_htmltablestyle *styling)
{
   char   *format;

   mulle_buffer_add_table_header_colspan( buffer, styling, 4);

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

   mulle_buffer_do( tmp_buffer)
   {
      unsigned int   j;

      for( j = 0; j < list->n_ivars; j++)
      {
         mulle_buffer_sprintf( tmp_buffer,
                  format,
                  html_escape( list->ivars[ j].descriptor.name),
                  html_escape( list->ivars[ j].descriptor.signature),
                  list->ivars[ j].descriptor.ivarid,
                  list->ivars[ j].offset);
      }
      mulle_buffer_add_string( buffer, mulle_buffer_get_string( tmp_buffer));
   }

   mulle_buffer_add_string( buffer, "</TABLE>");
}


#pragma mark - descriptor


// char   *mulle_objc_descriptor_html( struct _mulle_objc_descriptor *desc,
//                                            struct _mulle_objc_htmltablestyle *styling)
// {
//    char   *s;
//    char   *th;
//    char   *format;
//
//    mulle_buffer_do_string( buffer, NULL, s)
//    {
//       th = styling->classprefix ? "TH" : "TD";
//
//       mulle_buffer_add_table_header_colspan( buffer, styling, styling->colspan);
//
//       if( mulle_objc_environment_get_yes_no_default( "MULLE_OBJC_TERSE_TABLE", 0))
//          format = "<TR>"
//                   "<%s>%s</%s>"
//                   "</TR>"
//                   "</TABLE>";
//       else
//          format = "<TR><%s>name</%s><TD>%s</TD></TR>"
//                   "<TR><%s>signature</%s><TD>%s</TD></TR>"
//                   "<TR><%s>methodid</%s><TD>%08lx</TD></TR>"
//                   "<TR><%s>bits</%s><TD>0x%x</TD></TR>"
//                   "</TABLE>";
//
//       mulle_buffer_sprintf( buffer,
//                       format,
//                       th, th,
//                       html_escape( desc->name),
//                       th, th,
//                       html_escape( desc->signature),
//                       th, th,
//                       (unsigned long) desc->methodid,
//                       th, th,
//                       desc->bits);
//    }
//    return( s);
// }
//
//
// char   *mulle_objc_descriptor_html_hor( struct _mulle_objc_descriptor *desc)
// {
//    char   *s;
//    char   *format;
//
//    mulle_buffer_do_string( buffer, NULL, s)
//    {
//       if( mulle_objc_environment_get_yes_no_default( "MULLE_OBJC_TERSE_TABLE", 0))
//          format = "<TR>"
//                   "<TD>%s</TD>"
//                   "</TR>";
//       else
//          format = "<TR>"
//                   "<TD>%s</TD>"
//                   "<TD>%s</TD>"
//                   "<TD>%08lx</TD>"
//                   "<TD>0x%x</TD>"
//                   "</TR>";
//
//       mulle_buffer_sprintf( buffer,
//                             format,
//                             html_escape( desc->name),
//                             html_escape( desc->signature),
//                             (unsigned long) desc->methodid,
//                             desc->bits);
//    }
//    return( s);
// }



void   mulle_buffer_html_descriptor_entry( struct mulle_buffer *buffer,
                                           intptr_t  methodid,
                                           void *value,
                                           struct _mulle_objc_htmltablestyle *styling,
                                           void *userinfo)
{
   struct _mulle_objc_descriptor    *desc = value;
   char                             *format;

   MULLE_C_UNUSED( methodid);
   MULLE_C_UNUSED( styling);

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

   mulle_buffer_sprintf( buffer,
             format,
             html_escape( desc->name),
             html_escape( desc->signature),
             (unsigned long) desc->methodid,
             desc->bits);
}


#pragma mark - categories

// Callback wrapper for hashmap usage
void   mulle_buffer_html_loadcategory_entry( struct mulle_buffer *buffer,
                                             intptr_t categoryid,
                                             void *value,
                                             struct _mulle_objc_htmltablestyle *styling,
                                             void *userinfo)
{
   mulle_buffer_sprintf( buffer,
                "<TR>"
                "<TD>%s</TD>"
                "<TD>%08lx</TD>"
                "</TR>\n",
                html_escape( value),
                (unsigned long) categoryid);
}


void   mulle_buffer_html_category_entry( struct mulle_buffer *buffer,
                                         intptr_t  categoryid,
                                         void *value,
                                         struct _mulle_objc_htmltablestyle *styling,
                                         void *userinfo)
{
   MULLE_C_UNUSED( styling);
   MULLE_C_UNUSED( userinfo);

   mulle_buffer_sprintf( buffer,
                "<TR>"
                "<TD>%s</TD>"
                "<TD>%08lx</TD>"
                "</TR>\n",
                html_escape( value),
                (unsigned long) categoryid);
}


#pragma mark - protocols

// Callback wrapper for hashmap usage
void   mulle_buffer_html_protocol_entry( struct mulle_buffer *buffer,
                                         intptr_t protocolid,
                                         void *value,
                                         struct _mulle_objc_htmltablestyle *styling,
                                         void *userinfo)
{
   struct _mulle_objc_protocol  *protocol = value;

   mulle_buffer_sprintf( buffer,
                "<TR>"
                "<TD>%s</TD>"
                "<TD>%08lx</TD>"
                "</TR>\n",
                html_escape( _mulle_objc_protocol_get_name( protocol)),
                (unsigned long) protocolid);
}


void   mulle_buffer_html_protocol( struct mulle_buffer *buffer,
                                   intptr_t  protocolid,
                                   void *value,
                                   struct _mulle_objc_htmltablestyle *styling)
{
   struct _mulle_objc_protocol  *protocol = value;

   MULLE_C_UNUSED( styling);

   mulle_buffer_sprintf( buffer,
                "<TR>"
                "<TD>%s</TD>"
                "<TD>%08lx</TD>"
                "</TR>\n",
                html_escape( _mulle_objc_protocol_get_name( protocol)),
                (unsigned long) protocolid);
}



#pragma mark - propertylist

void   mulle_buffer_html_propertylist( struct mulle_buffer *buffer,
                                       struct _mulle_objc_propertylist *list,
                                       struct _mulle_objc_htmltablestyle *styling)
{
   struct mulle_pointerarray   array;
   char                        *format;
   unsigned int                j;
   int                         terse;
   char                        *s;

   terse = mulle_objc_environment_get_yes_no_default( "MULLE_OBJC_TERSE_TABLE", 0);

   if( styling)
      mulle_buffer_add_table_header_colspan( buffer, styling, styling->colspan);

   if( terse)
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

   mulle_pointerarray_init( &array, 0, NULL);
   for( j = 0; j < list->n_properties; j++)
   {
      mulle_buffer_do_string( tmp_buffer, NULL, s)
      {
         mulle_buffer_sprintf( tmp_buffer,
                   format,
                   html_escape( list->properties[ j].name),
                   html_escape( list->properties[ j].signature),
                   (unsigned long) list->properties[ j].propertyid,
                   (unsigned long) list->properties[ j].ivarid,
                   (unsigned long) list->properties[ j].getter,
                   (unsigned long) list->properties[ j].setter,
                   (unsigned long) list->properties[ j].bits);
      }
      mulle_pointerarray_add( &array, s);
   }

   mulle_qsort_r( _mulle_pointerarray_get_storage( &array),
                  mulle_pointerarray_get_count( &array),
                  sizeof( char *),
                  strcmp_r,
                  NULL);

   mulle_pointerarray_for( &array, s)
   {
      mulle_buffer_add_string( buffer, s);
      mulle_free( s);
   }

   mulle_pointerarray_done( &array);

   mulle_buffer_add_string( buffer, "</TABLE>");
}


#pragma mark - cache

void   mulle_buffer_html_cache( struct mulle_buffer *buffer,
                                struct _mulle_objc_cache *cache,
                                struct _mulle_objc_universe *universe,
                                struct _mulle_objc_htmltablestyle *styling)
{
   unsigned int            j;
   int                     index;
   mulle_objc_methodid_t   sel;
   int                     colspan;

#ifdef MULLE_OBJC_CACHEENTRY_REMEMBERS_THREAD_CLASS
   colspan = 6;
#else
   colspan = 5;
#endif
   mulle_buffer_add_table_header_colspan( buffer, styling, colspan);

   mulle_buffer_sprintf( buffer,
                   "<TR><TD>n</TD><TD COLSPAN=\"%d\">%tu</TD></TR>\n",
                   colspan,
                   (uintptr_t) _mulle_atomic_pointer_read_nonatomic( &cache->n));
   mulle_buffer_sprintf( buffer,
                   "<TR><TD>mask</TD><TD COLSPAN=\"%d\">0x%lx</TD></TR>\n",
                   colspan,
                   (long) cache->mask);

   for( j = 0; j < cache->size; j++)
   {
      index = 0;
      sel   = cache->entries[ j].key.uniqueid;
      if( sel)
         index = _mulle_objc_cache_probe_entryindex( cache, sel);

      mulle_buffer_sprintf( buffer, "<TR><TD>#%ld</TD><TD>%08lx</TD><TD>%s</TD>"
                                "<TD>%p</TD><TD>%d (%x)</TD>",
                                j,
                                sel,
                                _mulle_objc_universe_describe_methodid( universe, sel),
                                cache->entries[ j].value.functionpointer,
                                index,
                                sel & cache->mask);
#ifdef MULLE_OBJC_CACHEENTRY_REMEMBERS_THREAD_CLASS
      mulle_buffer_sprintf( buffer, "<TD>%p</TD>", cache->entries[ j].thread_id);
#endif
      mulle_buffer_add_string( buffer, "</TR>\n");
   }

   mulle_buffer_add_string( buffer, "</TABLE>");
}


#pragma mark - methodlist

void   mulle_buffer_html_methodlist( struct mulle_buffer *buffer,
                                     struct _mulle_objc_methodlist *list,
                                     struct _mulle_objc_universe *universe,
                                     int show_fields,
                                     struct _mulle_objc_htmltablestyle *styling)
{
   struct mulle_pointerarray   array;
   char                        *format;
   char                        *name;
   char                        *s;
   unsigned int                j;

   // create single lines for each method and two for head/tail
   mulle_buffer_add_table_header_colspan( buffer, styling, styling->colspan);

   name = _mulle_objc_methodlist_get_categoryname( list);
   if( name)
      mulle_buffer_sprintf( buffer, "<TR><TD>category</TD><TD>%s</TD></TR>\n", name);

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

   mulle_pointerarray_init( &array, 0, NULL);

   mulle_buffer_do( tmp_buffer)
   {
      for( j = 0; j < list->n_methods; j++)
      {
         mulle_buffer_reset( tmp_buffer);
         mulle_buffer_sprintf_functionpointer( tmp_buffer,
                                               (mulle_functionpointer_t) &list->methods[ j].value);

         mulle_buffer_do_string( tmp_buffer2, NULL, s)
         {
            mulle_buffer_sprintf( tmp_buffer2,
                      format,
                      html_escape( list->methods[ j].descriptor.name),
                      html_escape( list->methods[ j].descriptor.signature),
                      (unsigned long) list->methods[ j].descriptor.methodid,
                      (unsigned long) list->methods[ j].descriptor.bits,
                      mulle_buffer_get_string( tmp_buffer));
         }
         mulle_pointerarray_add( &array, s);
      }
   }

   /* sort by name */
   mulle_qsort_r( _mulle_pointerarray_get_storage( &array),
                  mulle_pointerarray_get_count( &array),
                  sizeof( char *),
                  strcmp_r,
                  NULL);

   mulle_pointerarray_for( &array, s)
   {
      mulle_buffer_add_string( buffer, s);
      mulle_free( s);
   }

   mulle_pointerarray_done( &array);

   mulle_buffer_add_string( buffer, "</TABLE>");
}


void   mulle_buffer_html_methodlist_hor( struct mulle_buffer *buffer,
                                         struct _mulle_objc_methodlist *list,
                                         struct _mulle_objc_htmltablestyle *styling)
{
   char   *format;
   int    terse;

   terse = mulle_objc_environment_get_yes_no_default( "MULLE_OBJC_TERSE_TABLE", 0);
   mulle_buffer_add_table_header_colspan( buffer, styling, 5);

   if( list->n_methods)
   {
      if( terse)
      {
         if( styling->classprefix)
            format = "<TR>"
                        "<TH COLSPAN=5>name</TH>"
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

      mulle_buffer_add_string( buffer, format);
   }

   if( terse)
      format = "<TR>"
                 "<TD COLSPAN=5>%s</TD>"
               "</TR>\n";
   else
      format = "<TR>"
                 "<TD>%s</TD>"
                 "<TD>%s</TD>"
                 "<TD>%08lx</TD>"
                 "<TD>0x%lx</TD>"
                 "<TD>%s</TD>"
               "</TR>\n";

   mulle_buffer_do( tmp_buffer)
   {
      unsigned int   j;

      for( j = 0; j < list->n_methods; j++)
      {
         char   *impl_str;

         mulle_buffer_reset( tmp_buffer);
         mulle_buffer_sprintf_functionpointer( tmp_buffer,
                                               (mulle_functionpointer_t) &list->methods[ j].value);
         impl_str = mulle_buffer_get_string( tmp_buffer);

         mulle_buffer_sprintf( buffer,
                               format,
                               html_escape( list->methods[ j].descriptor.name),
                               html_escape( list->methods[ j].descriptor.signature),
                               (unsigned long) list->methods[ j].descriptor.methodid,
                               (unsigned long) list->methods[ j].descriptor.bits,
                               impl_str);
      }
   }

   mulle_buffer_add_string( buffer, "</TABLE>");
}


#pragma mark - loadclass

void   mulle_buffer_html_loadclass_element( struct mulle_buffer *buffer,
                                            unsigned int row,
                                            void *value,
                                            struct _mulle_objc_htmltablestyle *styling,
                                            void *userinfo)
{
   struct _mulle_objc_loadclassbase   *loadcls = value;

   MULLE_C_UNUSED( userinfo);

   mulle_buffer_sprintf( buffer, "<TR><TD>%s</TD><TD>%08lx</TD></TR>\n",
                                     loadcls->classname,
                                     (unsigned long) loadcls->classid);
}


#pragma mark - loadcategory

char   *mulle_buffer_html_loadcategory_element( struct mulle_buffer *buffer,
                                                unsigned int row,
                                                void *value,
                                                struct _mulle_objc_htmltablestyle *styling,
                                                void *userinfo)
{
   struct _mulle_objc_loadcategory   *loadcat = value;
   char                              *s;

   MULLE_C_UNUSED( userinfo);

   mulle_buffer_do_string( buffer, NULL, s)
   {
      mulle_buffer_sprintf( buffer, "<TR><TD>%s( %s)</TD><TD>%08lx</TD></TR>\n",
                          loadcat->classname,
                          loadcat->categoryname,
                          (unsigned long) loadcat->categoryid);
   }
   return( s);
}



#pragma mark - protocols


#pragma mark - categories


#pragma mark - fastclasses

void   mulle_buffer_html_fastclass_element( struct mulle_buffer *buffer,
                                            unsigned int row,
                                            struct _mulle_objc_infraclass *cls,
                                            struct _mulle_objc_htmltablestyle *styling,
                                            void *userinfo)
{
   mulle_buffer_sprintf( buffer,
            "<TR>"
            "<TD>%u</TD>"
            "<TD>%s</TD>"
            "</TR>\n",
            row,
            html_escape( cls ? _mulle_objc_infraclass_get_name( cls) : "*null*"));
}



void   mulle_buffer_html_fastclasstable( struct mulle_buffer *buffer,
                                         struct _mulle_objc_fastclasstable *fastclasstable,
                                         mulle_buffer_html_fastclasstable_callback_t *row_description,
                                         struct _mulle_objc_htmltablestyle *styling,
                                         void *userinfo)
{
   unsigned int   i;
   void           *value;

   if( styling)
      mulle_buffer_add_table_header_colspan( buffer, styling, styling->colspan);

   for( i = 0; i < MULLE_OBJC_S_FASTCLASSES; i++)
   {
      value = _mulle_atomic_pointer_read_nonatomic( &fastclasstable->classes[ i].pointer);
      (*row_description)( buffer, i, value, styling, userinfo);
   }

   if( styling)
      mulle_buffer_add_string( buffer, "</TABLE>");
}


#pragma mark - pointerarray

void   mulle_buffer_html_concurrent_pointerarray( struct mulle_buffer *buffer,
                                                  struct mulle_concurrent_pointerarray *list,
                                                  mulle_buffer_html_concurrent_pointerarray_callback_t *row_description,
                                                  struct _mulle_objc_htmltablestyle *styling,
                                                  void *userinfo)
{
   struct mulle_concurrent_pointerarrayenumerator   rover;
   void                                             *value;
   unsigned int                                     i;

   if( styling)
      mulle_buffer_add_table_header_colspan( buffer, styling, styling->colspan);

   i = 0;
   rover = mulle_concurrent_pointerarray_enumerate( list);
   while( value = _mulle_concurrent_pointerarrayenumerator_next( &rover))
   {
      (*row_description)( buffer, i, value, styling, userinfo);
      ++i;
   }

   if( styling)
      mulle_buffer_add_string( buffer, "</TABLE>");
}


#pragma mark - hashmap

void   mulle_buffer_html_concurrent_hashmap( struct mulle_buffer *buffer,
                                             struct mulle_concurrent_hashmap *map,
                                             mulle_buffer_html_hashmap_callback_t *row_description,
                                             struct _mulle_objc_htmltablestyle *styling,
                                             void *userinfo)
{
   struct mulle_pointerarray   ptr_array;
   intptr_t                    hash;
   void                        *value;
   char                        *null_description;
   char                        *s;

   if( styling)
      mulle_buffer_add_table_header_colspan( buffer, styling, styling->colspan);

   null_description = "*null*";

   mulle_pointerarray_init( &ptr_array, 0, NULL);

   mulle_concurrent_hashmap_for( map, hash, value)
   {
      if( ! value)
         value = null_description;

      mulle_buffer_do_string( tmp_buffer, NULL, s)
      {
         (*row_description)( tmp_buffer, hash, value, styling, userinfo);
      }
      mulle_pointerarray_add( &ptr_array, s);
   }

   mulle_qsort_r( _mulle_pointerarray_get_storage( &ptr_array),
                   mulle_pointerarray_get_count( &ptr_array),
                   sizeof( char *),
                   strcmp_r,
                   NULL);

   mulle_pointerarray_for( &ptr_array, s)
   {
      mulle_buffer_add_string( buffer, s);
      mulle_free( s);
   }

   mulle_pointerarray_done( &ptr_array);

   if( styling)
      mulle_buffer_add_string( buffer, "</TABLE>");
}


#pragma mark - uniqueidarray

void   mulle_buffer_html_uniqueidarray( struct mulle_buffer *buffer,
                                        struct _mulle_objc_uniqueidarray *array,
                                        mulle_buffer_html_uniqueidarray_callback_t *row_description,
                                        struct _mulle_objc_htmltablestyle *styling,
                                        void *userinfo)
{
   struct mulle_pointerarray   ptr_array;
   mulle_objc_uniqueid_t       *p;
   mulle_objc_uniqueid_t       *sentinel;
   char                        *s;

   if( styling)
      mulle_buffer_add_table_header_colspan( buffer, styling, styling->colspan);

   mulle_pointerarray_init( &ptr_array, 0, NULL);

   p        = array->entries;
   sentinel = &p[ array->n];

   while( p < sentinel)
   {

      mulle_buffer_do_string( tmp_buffer, NULL, s)
      {
         (*row_description)( tmp_buffer, (unsigned int) (p - array->entries), *p, styling, userinfo);
      }
      mulle_pointerarray_add( &ptr_array, s);
      p++;
   }

   mulle_qsort_r( _mulle_pointerarray_get_storage( &ptr_array),
                  mulle_pointerarray_get_count( &ptr_array),
                  sizeof( char *),
                  strcmp_r,
                  NULL);

   mulle_pointerarray_for( &ptr_array, s)
   {
      mulle_buffer_add_string( buffer, s);
      mulle_free( s);
   }

   mulle_pointerarray_done( &ptr_array);

   if( styling)
      mulle_buffer_add_string( buffer, "</TABLE>");
}

