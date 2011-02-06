//
// C++ Interface: cgenericitem
//
// Description: A generic item that can only store data.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CGENERICITEM_H
#define CGENERICITEM_H

#include <clistobject.h>

class cGenericItem : public cListObject {
public:
  virtual ~cGenericItem ();

protected:
  friend class cGenericList;
  cGenericItem (cList *list);
};

#endif  // CGENERICITEM_H
