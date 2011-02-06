//
// C++ Interface: converterplugin
//
// Description: Converter Plugin.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CONVERTERPLUGIN_H
#define CONVERTERPLUGIN_H

#include <cplugin.h>
#include <QVariantList>

struct cConverterPluginPrivate;

class cConverterPlugin : public cPlugin
{
Q_OBJECT
public:
  cConverterPlugin (QObject *, const QVariantList &);
  virtual ~cConverterPlugin ();
protected slots:
  void converterDialog ();
protected:
  void convertProfile (const QString &path, const QString &name);
  cConverterPluginPrivate *d;
};


#endif // CONVERTERPLUGIN_H

