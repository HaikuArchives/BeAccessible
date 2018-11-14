//  BeAccessibleWindow.h

#ifndef BeAccessible_Window_H
#define BeAccessible_Window_H

#ifndef _WINDOW_H
#include <Window.h>
#endif
#include <Path.h>
#include <Menu.h>
#include <StorageKit.h>

class BeAccessibleView;
class sqlite;
class DBWindow;
class TabView;
class DBTabView;
class TableWindow;

class BeAccessibleWindow : public BWindow 
{
public:
	BeAccessibleWindow(BRect frame, BList* recentFiles); 
	virtual bool QuitRequested();
	void MessageReceived(BMessage* msg);
	void OpenDatabase(BPath adb);
	void CloseDatabase();
	void AddTableToList(BString table);
	BFilePanel* fOpenFile;
	BFilePanel* fNewFile;
	BFilePanel* fImportFile;

private:
	bool WindowExists(BString name);
	void OpenTable(int view);
	void NewTable(int view);
	void CloseAllWindows();
	void BringWindowToFront(BString windowName);
	void DisableMenuItems();
	void EnableMenuItems();

	// The full path of the last database to be opened.  This will still have a value
	// if no database is open
	BString* fPath;
	
	TabView* fTabView;
	BView *fMainView;
	BMenuBar* fMenuBar;
	BMenu* fFileMenu;
	BMenu* fOpenRecentMenu;
	BMenu* fToolsMenu;
	BMenuItem* fCloseItem;
	BMenuItem* fImportItem;
	BMenuItem* fCompactItem;
	DBTabView* fTableTab;
	DBTabView* fQueryTab;
	TableWindow* fNewGrid;
};

#endif //BeAccessible_Window_H
