// This file is copied from kdelibs. Licensed under LGPL.

#ifndef KMUDDY_EXPORT_H
#define KMUDDY_EXPORT_H

/* needed for KDE_EXPORT and KDE_IMPORT macros */
#include <kdemacros.h>

#ifndef KMUDDY_EXPORT
# if defined(MAKE_KMUDDYCORE_LIB)
   /* We are building this library */
#  define KMUDDY_EXPORT KDE_EXPORT
# else
   /* We are using this library */
#  define KMUDDY_EXPORT KDE_IMPORT
# endif
#endif

#endif
