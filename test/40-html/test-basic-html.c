#ifdef __MULLE_OBJC_TPS__
# undef __MULLE_OBJC_TPS__
#endif
#define __MULLE_OBJC_NO_TPS__
#define __MULLE_OBJC_FCS__
#define __MULLE_OBJC_TAO__

#include "include.h"

int   main( void)
{
   struct mulle_buffer                *buffer;
   struct _mulle_objc_htmltablestyle  style;
   char                               *result;

   // Test 1: Basic table without classprefix (graphviz style)
   mulle_printf( "Test 1: Basic table without classprefix\n");
   style.title       = "Basic Table";
   style.classprefix = NULL;
   style.color       = "white";
   style.bgcolor     = "blue";
   style.colspan     = 2;
   style.headers     = NULL;

   mulle_buffer_do_string( buffer, NULL, result)
   {
      mulle_buffer_add_table_header_colspan( buffer, &style, style.colspan);
      mulle_buffer_add_string( buffer, "<TR><TD>Test</TD><TD>Data</TD></TR>\n");
      mulle_buffer_add_string( buffer, "</TABLE>");
   }
   mulle_printf( "%s\n", result);
   mulle_free( result);

   // Test 2: Table with classprefix (HTML style)
   mulle_printf( "\nTest 2: Table with classprefix\n");
   style.title       = "Styled Table";
   style.classprefix = "test";
   style.color       = "white";
   style.bgcolor     = "blue";
   style.colspan     = 3;
   style.headers     = NULL;

   mulle_buffer_do_string( buffer, NULL, result)
   {
      mulle_buffer_add_table_header_colspan( buffer, &style, style.colspan);
      mulle_buffer_add_string( buffer, "<TR><TD>Test</TD><TD>Data</TD><TD>More</TD></TR>\n");
      mulle_buffer_add_string( buffer, "</TABLE>");
   }
   mulle_printf( "%s\n", result);
   mulle_free( result);

   // Test 3: Table with headers
   mulle_printf( "\nTest 3: Table with headers\n");
   char *headers[] = { "Name", "Value", "Description", NULL };

   style.title       = "Table with Headers";
   style.classprefix = "test";
   style.color       = "white";
   style.bgcolor     = "blue";
   style.colspan     = 3;
   style.headers     = headers;

   mulle_buffer_do_string( buffer, NULL, result)
   {
      mulle_buffer_add_table_header_colspan( buffer, &style, style.colspan);
      mulle_buffer_add_string( buffer, "<TR><TD>Item1</TD><TD>100</TD><TD>First item</TD></TR>\n");
      mulle_buffer_add_string( buffer, "<TR><TD>Item2</TD><TD>200</TD><TD>Second item</TD></TR>\n");
      mulle_buffer_add_string( buffer, "</TABLE>");
   }
   mulle_printf( "%s\n", result);
   mulle_free( result);

   return( 0);
}