// This file is copied from kdelibs. Licensed under LGPL.

#ifndef KMUDDY_EXPORT_H
#define KMUDDY_EXPORT_H

#include <QtGlobal>

#ifndef KMUDDY_EXPORT
# if defined(MAKE_KMUDDYCORE_LIB)
   /* We are building this library */
#  define KMUDDY_EXPORT Q_DECL_EXPORT
# else
   /* We are using this library */
#  define KMUDDY_EXPORT Q_DECL_IMPORT
# endif
#endif

#endif
