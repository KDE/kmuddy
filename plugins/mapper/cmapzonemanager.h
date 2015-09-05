/***************************************************************************
                          cmapzonemanager.h
                             -------------------
    begin                : Sat Aug 15, 2015
    copyright            : (C) 2015 by Tomas Mecir
    email                : mecirt@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CMAPZONEMANAGER_H
#define CMAPZONEMANAGER_H

#include <QString>

class QStandardItemModel;
class CMapManager;

// Information held about each zone
struct cZoneInformation {
  QString name, file;
};


/** This exists per each connection, holding the list of zones */
class CMapZoneManager {
  public:
    CMapZoneManager(int sessId, CMapManager *manager);
    virtual ~CMapZoneManager();

    QStandardItemModel *zonesModel();
    QString getPath(int idx);
    void save(bool always = false);
    void loadZone(int idx);
    void deleteZone(int idx);
    void createZone(const QString &name, const QString &file = QString());
    void renameZone(int idx, const QString &name);
    void loadMapList();
    void saveMapList();
    int activeZone();
  private:
    int createZoneEntry(const QString &name, const QString &file);
    cZoneInformation *getZoneEntry(int idx);
    struct Private;
    Private *d;
};


#endif  // CMAPZONEMANAGER_H
