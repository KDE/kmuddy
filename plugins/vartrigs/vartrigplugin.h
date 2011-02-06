//
// C++ Interface: vartrigplugin
//
// Description: vartrigplugin
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef VARTRIGPLUGIN_H
#define VARTRIGPLUGIN_H

#include <cactionbase.h>
#include <cplugin.h>
#include <QVariantList>

class cVarTrigPlugin : public cPlugin, public cActionBase
{
public:
  cVarTrigPlugin (QObject *, const QVariantList &);
  virtual ~cVarTrigPlugin ();

protected:
  virtual void eventStringHandler (QString event, int session, QString &par1, const QString &);
};


#endif // SCRIPTINGPLUGIN_H

