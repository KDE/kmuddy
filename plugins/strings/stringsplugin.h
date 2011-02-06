//
// C++ Interface: stringsplugin
//
// Description: Strings Plugin.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef STRINGSPLUGIN_H
#define STRINGSPLUGIN_H

#include <cplugin.h>
#include <QVariantList>

struct cStringsPluginPrivate;

class cStringsPlugin : public cPlugin
{
public:
  cStringsPlugin (QObject *, const QVariantList &);
  virtual ~cStringsPlugin ();
protected:
  void registerFunctions ();
  void unregisterFunctions ();
  cStringsPluginPrivate *d;
};


#endif // STRINGSPLUGIN_H

