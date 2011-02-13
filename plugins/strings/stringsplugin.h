//
// C++ Interface: stringsplugin
//
// Description: Strings Plugin.
//
/*
Copyright 2007-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

