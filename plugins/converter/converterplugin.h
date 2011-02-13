//
// C++ Interface: converterplugin
//
// Description: Converter Plugin.
//
/*
Copyright 2008-2011 Tomas Mecir <kmuddy@kmuddy.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of 
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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

