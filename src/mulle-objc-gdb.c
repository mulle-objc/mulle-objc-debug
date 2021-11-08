//
//  mulle-objc-gdb.h
//  mulle-objc-debug
//
//  Created by Nat! on 14.10.19
//  Copyright © 2019 Mulle kybernetiK. All rights reserved.
//  Copyright © 2019 Codeon GmbH. All rights reserved.
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

//
// used by the mulle-gdb debugger
//
struct _mulle_objc_class  *
   mulle_objc_gdb_lookup_class( char *name)
{
   struct _mulle_objc_universe     *universe;
   mulle_objc_classid_t            classid;
   struct _mulle_objc_infraclass   *infra;

   classid  = mulle_objc_uniqueid_from_string( name);
   universe = mulle_objc_global_get_universe_inline( MULLE_OBJC_DEFAULTUNIVERSEID);
   infra    = _mulle_objc_universe_lookup_infraclass( universe, classid);
   return( infra ? _mulle_objc_infraclass_as_class( infra) : NULL);
}


mulle_objc_methodid_t
   mulle_objc_gdb_lookup_selector( char *name)
{
   struct _mulle_objc_universe     *universe;
   mulle_objc_methodid_t           sel;
   struct _mulle_objc_descriptor   *desc;

   sel      = mulle_objc_uniqueid_from_string( name);
   universe = mulle_objc_global_get_universe_inline( MULLE_OBJC_DEFAULTUNIVERSEID);
   desc     = _mulle_objc_universe_lookup_descriptor( universe, sel);

   return( desc ? sel : MULLE_OBJC_NO_METHODID);
}



mulle_objc_implementation_t
   mulle_objc_gdb_lookup_implementation( void *obj,
                                         mulle_objc_methodid_t methodid,
                                         void *class_or_superid,
                                         int calltype)

{
   struct _mulle_objc_class        *cls;
   struct _mulle_objc_infraclass   *super;
   mulle_objc_implementation_t     imp;
   mulle_objc_superid_t            superid;
   int                             preserve;

   preserve = errno;

   if( ! obj || mulle_objc_uniqueid_is_sane( MULLE_OBJC_NO_METHODID))
   {
      errno = preserve;
      return( 0);
   }

   //
   // the resulting IMP is the one, the debugger will step "thru"
   // if we want to step into forwarded methods, we would have to do the
   // complete forward: resolution, which we can't at this level.
   //
   switch( calltype)
   {
   default :
   case 0  :
      cls = _mulle_objc_object_get_isa( obj);
      imp = _mulle_objc_class_lookup_implementation_nocache( cls, methodid);
      break;

   case 1 :
      imp = _mulle_objc_class_lookup_implementation_nocache( class_or_superid,
                                                             methodid);
      break;

      // doing this nofail, is bad. Tracing into [super forwardedMessage]
      // will bring grief
   case 2 :
      superid = (mulle_objc_superid_t) (uintptr_t) class_or_superid;
      imp     = _mulle_objc_object_superlookup_implementation_inline_nofail( obj,
                                                                            superid);
   }

   errno = preserve;

   return( imp);
}


// sel_getUID will be added by the compat layer, so use this
void  *sel_get_any_uid( char *name)
{
   struct _mulle_objc_universe    *universe;
   mulle_objc_methodid_t          methodid;
   struct _mulle_objc_descriptor  *desc;

   if( ! name)
      return( NULL);

   methodid = mulle_objc_methodid_from_string( name);
   universe =  mulle_objc_global_get_universe( __MULLE_OBJC_UNIVERSEID__);
   desc     = _mulle_objc_universe_lookup_descriptor( universe, methodid);
   return( desc ? (void *) (intptr_t) methodid : NULL);
}


// doesn't work in gdb, and I am not really sure why
#if 0
//
// reuse gnustep like lookup mechanism for our purposes
// likely to fail for metaABI though, is just hack (we check if method does
// MetaABI and if yes, say so...)

void   *objc_msg_lookup( void *obj, void *cmd)
{
   mulle_objc_methodid_t       sel;
   struct _mulle_objc_class    *cls;
   struct _mulle_objc_method   *method;
   char                        *signature;

   if( ! obj)
      return( NULL);

   sel = (mulle_objc_methodid_t) (uintptr_t) cmd;
   if( ! mulle_objc_methodid_is_sane( sel))
   {
      fprintf( stderr, "objc_msg_lookup: incoming selector is broken\n");
      return( NULL);
   }

   cls = _mulle_objc_object_get_isa( obj);
   // we no forward
   method = mulle_objc_class_defaultsearch_method( cls, sel);
   if( ! method)
      return( NULL);

   signature = mulle_objc_method_get_signature( method);
   switch( mulle_objc_signature_get_metaabiparamtype( signature))
   {
   case mulle_metaabi_param_struct :
   case mulle_metaabi_param_error :
      fprintf( stderr, "objc_msg_lookup: method requires MetaABI _param, gdb can't do metaABI yet.\n");
      return( NULL);
   }

   switch( mulle_objc_signature_get_metaabiparamtype( signature))
   {
   case mulle_metaabi_param_struct :
   case mulle_metaabi_param_error :
      fprintf( stderr, "objc_msg_lookup: return value will not be available, gdb can't do metaABI yet.\n");
   }
   return( mulle_objc_method_get_implementation( method));
}
#endif

#if 0
void   *objc_msgSend( void *obj, void *cmd, void *param)
{
   mulle_objc_methodid_t   sel;


   if( (uintptr_t) cmd & (~(mulle_objc_methodid_t) -1))
   {
      fprintf( stderr, "objc_msgSend: incoming selector is not a mulle-objc selector\n");
      return( NULL);
   }

   sel = (mulle_objc_methodid_t) (uintptr_t) cmd;
   if( ! mulle_objc_methodid_is_sane( sel))
   {
      fprintf( stderr, "objc_msgSend: incoming selector is broken\n");
      return( NULL);
   }

   return( mulle_objc_object_call( obj, (mulle_objc_methodid_t) (uintptr_t) sel, param));
}

void   *objc_msgSend_stret( void *obj, void *cmd, void *param)
{
   fprintf( stderr, "objc_msgSend_stret: doesn't work with mulle-objc");
   return( NULL);
}
#endif


void   mulle_objc_reference_gdb_functions( void);

void   mulle_objc_reference_gdb_functions( void)
{
   mulle_objc_gdb_lookup_class( 0);
   mulle_objc_gdb_lookup_selector( 0);
   mulle_objc_gdb_lookup_implementation( 0, 0, 0, 0);
//   objc_msgSend( NULL, NULL, NULL);
   sel_get_any_uid( NULL);
}
