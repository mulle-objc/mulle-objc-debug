#ifdef __MULLE_OBJC_TPS__
# undef __MULLE_OBJC_TPS__
#endif
#define __MULLE_OBJC_NO_TPS__
#define __MULLE_OBJC_FCS__
#define __MULLE_OBJC_TAO__

#include "include.h"


#pragma mark - test function for headers

char   *mulle_objc_test_headers_html( struct _mulle_objc_htmltablestyle *style)
{
   char   *s;

   mulle_buffer_do_string( buffer, NULL, s)
   {
      mulle_buffer_add_table_header_colspan( buffer, style, style->colspan);

      mulle_buffer_sprintf( buffer, "<TR><TD>item1</TD><TD>signature1</TD><TD>0x01</TD><TD>10</TD></TR>\n");
      mulle_buffer_sprintf( buffer, "<TR><TD>item2</TD><TD>signature2</TD><TD>0x02</TD><TD>20</TD></TR>\n");
      mulle_buffer_add_string( buffer, "</TABLE>");
   }
   return( s);
}


int   main( void)
{
   struct _mulle_objc_htmltablestyle  style;
   char                               *headers[] = { "Name", "Signature", "ID", "Offset", NULL };
   char                               *html;

   // Test 1: Table with headers (4 columns > 2, so headers should appear)
   style.title       = "Test Table with Headers";
   style.classprefix = "test";
   style.color       = "white";
   style.bgcolor     = "blue";
   style.colspan     = 4;
   style.headers     = headers;

   html = mulle_objc_test_headers_html( &style);
   printf( "%s\n", html);
   mulle_free( html);
   return( 0);
}