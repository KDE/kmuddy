//
// C++ Interface: varviewerplugin
//
// Description: VarViewer Plugin.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef VARVIEWERPLUGIN_H
#define VARVIEWERPLUGIN_H

#include <cplugin.h>
#include <QVariantList>

struct cVarViewerPluginPrivate;

class cVarViewerPlugin : public cPlugin
{
public:
  cVarViewerPlugin (QObject *, const QVariantList &);
  virtual ~cVarViewerPlugin ();
protected:
  cVarViewerPluginPrivate *d;
};


#endif // VARVIEWERPLUGIN_H

