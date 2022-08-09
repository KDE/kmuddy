/***************************************************************************
                          dlgeditprofile.h  -  Edit Profile dialog
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Pá Aug 9 2002
    copyright            : (C) 2002-2008 by Tomas Mecir
    email                : kmuddy@kmuddy.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DLGEDITPROFILE_H
#define DLGEDITPROFILE_H

#include <QDialog>

class KLineEdit;
class KTextEdit;
class KRestrictedLine;

/**
Dialog used to edit basic profile settings (server, port, ...)
  *@author Tomas Mecir
  */

class dlgEditProfile : public QDialog {
   Q_OBJECT
public: 
  dlgEditProfile (QWidget *parent=nullptr);
  ~dlgEditProfile();

  virtual QSize sizeHint() const override;
  /** values in LineEdits */
  QString name ();
  QString server ();
  int port ();
  QString login ();
  QString password ();
  QStringList connectionString ();

  /** set new values! */
  void setName (QString name);
  void setServer (QString server);
  void setPort (int port);
  void setLogin (QString login);
  void setPassword (QString password);
  void setConnectionString (QStringList conn);  
protected slots:
  void accept() override;
  void openMudList ();

protected:
  KLineEdit *ed1, *ed2, *ed4, *ed5;
  KTextEdit *connstr;
  KRestrictedLine *ed3;
};

#endif
