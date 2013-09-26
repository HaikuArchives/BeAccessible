//	BeAccessible.h

#ifndef BeAccessibleApp_H
#define BeAccessibleApp_H

#ifndef _APPLICATION_H
#include <Application.h>
#endif
#ifndef _PATH_H
#include <Path.h>
#endif

class sqlite;
class BeAccessibleWindow;
class StartWindow;

class BeAccessibleApp : public BApplication 
{
public:
	BeAccessibleApp();
	~BeAccessibleApp();
	virtual void MessageReceived(BMessage* msg);
	virtual void RefsReceived(BMessage* msg);
	virtual bool QuitRequested();
	virtual void AboutRequested();
	void ReadyToRun();
	bool GetSettings();
	void AddToRecentFiles(BString* newFile);
//	BList* GetRecentFiles();
	
private:

	BeAccessibleWindow *fMainWindow;
	StartWindow* fStartWindow;
	BPath *fSettingsPath;
	BString *fSettingsFilename;
	BMessage *fSettingsMsg;
	BList *fRecentFiles;
	bool fRefReceived;
};

#endif //BeAccessible_H
