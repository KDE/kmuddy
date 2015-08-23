/***************************************************************************
                               cmapclipboard.h
                             -------------------
    begin                : Wed Jul 3 2002
    copyright            : (C) 2002 by Kmud Developer Team
    email                : kmud-devel@kmud.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CMAPCLIPBOARD_H
#define CMAPCLIPBOARD_H

#include <qobject.h>
#include <q3valuelist.h>
#include <kconfiggroup.h>

class KAction;
class CMapManager;
class KMemConfig;
class CMapZone;
class CMapPath;
class CMapView;

/**This class contains all the mapper clipboard code
  *@author Kmud Developer Team
  */

class CMapClipboard : public QObject
{
   Q_OBJECT
public: 
	CMapClipboard(CMapManager *mapManager, CMapView *view, QObject *parent=0);
	~CMapClipboard();

public:
	/** This method is used to set the enabled start of the actions */
	void enableActions(bool enabled);

public slots:
	/** This method is called to copy the selected elements into the clipboard */
	void slotCopy();
	/** This method is called to copy the selected elements into the clipboard, then delete them */
	void slotCut();
	/** This method is called to paste the elements in the clipboard onto the current map */
	void slotPaste();
	/** This slot is called to delete all the selected objects in the current view */
	void slotDelete(void);
	/** This slot is called when the select all menu option is selected */
	void slotSelectAll(void);
	/** This slot is called when the unselect all menu option is selected */
	void slotUnselectAll(void);
	/** This slot is called when the invert selection menu option is called */
	void slotInvertSelection(void);

private:
	/** This is used to create the clipboard actions */
	void initActions(void);
	/** This method is used to copy a path to the clipboard */
	void copyPath(int *pathGroup,CMapPath *path);
	/** This method is used to paste elements that are not paths or linked text elements */
	void pasteElements();
	/** This method is used to paste path elements */
	void pastePaths();
	/** This method is used to update linked text elements with the correct properties from the clibboard */
	void pasteLinks();


private:
  KMemConfig *m_clipboard;
  CMapManager *m_mapManager;
  CMapView *m_view;
  QObject *m_parent;

  KAction *m_editSelectAll;
  KAction *m_editUnselectAll;
  KAction *m_editSelectInvert;
  KAction *m_editDelete;
  KAction *m_editCopy;
  KAction *m_editCut;
  KAction *m_editPaste;

};

#endif
