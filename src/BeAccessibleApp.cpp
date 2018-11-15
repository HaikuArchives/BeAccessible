#include <iostream>

#ifndef BeAccessible_Window_H
#include "BeAccessibleWindow.h"
#endif
#ifndef BeAccessibleApp_H
#include "BeAccessibleApp.h"
#endif

#include <Alert.h>
#include <Bitmap.h>
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <FilePanel.h>
#include <ListView.h>
#include <Mime.h>
#include <Path.h>
#include <storage/FindDirectory.h>
#include <String.h>

#include "Constants.h"
#include "FileTypeIcons.h"
#include "PrefilledBitmap.h"
#include "StartWindow.h"
#include "SQLiteManager.h"

extern SQLiteManager* GlobalSQLMgr;


int main(int, char**)
{	
	BeAccessibleApp	myApplication;
	myApplication.Run();

	return(0);
}

BeAccessibleApp::BeAccessibleApp()
		  		  : BApplication("application/x-vnd.BeAccessible")
{
	fSettingsFilename = new BString("BeAccessible_settings");
	fSettingsPath = new BPath;
	fSettingsMsg = new BMessage();
	fRecentFiles = new BList();
	
	GlobalSQLMgr = new SQLiteManager();
	
	GetSettings();

	BRect			aRect;
	//Create the BeAccessibleWindow
	aRect.Set(100, 80, 560, 320);
	fMainWindow = new BeAccessibleWindow(aRect, fRecentFiles);


	BBitmap	large_icon(BRect(0, 0, B_LARGE_ICON - 1, B_LARGE_ICON - 1), B_COLOR_8_BIT);
	BBitmap	mini_icon(BRect(0, 0, B_MINI_ICON - 1, B_MINI_ICON - 1), B_COLOR_8_BIT);

	BMimeType mime("BeAccessible File");
	mime.SetType("application/x-vnd.BeAccessible.File");
	bool install = !mime.IsInstalled();
	if (!install)
	{
		BBitmap* junkptr = 0;
		if (mime.GetIcon(junkptr, B_LARGE_ICON))
			install = true;
	}
	if (install)
	{
		mime.Install();
		large_icon.SetBits(kLargeBeAccessibleFileIcon, large_icon.BitsLength(), 0, B_COLOR_8_BIT);
		mini_icon.SetBits(kSmallBeAccessibleFileIcon, mini_icon.BitsLength(), 0, B_COLOR_8_BIT);
		mime.SetShortDescription("BeAccessible File");
		mime.SetLongDescription("Database File for BeAccessible");
		mime.SetIcon(&large_icon, B_LARGE_ICON);
		mime.SetIcon(&mini_icon, B_MINI_ICON);
		mime.SetPreferredApp("application/x-vnd.BeAccessible");
		BMessage msg;
		msg.AddString("extensions", "db");
		mime.SetFileExtensions(&msg);
	}

	fRefReceived = false;
}

void
BeAccessibleApp::ReadyToRun()
{
	if (!fRefReceived)
	{
		//Display the StartWindow	
	   	BRect frame = fMainWindow->Frame();
	   	BRect startRect(frame.Width()/2 - 125 + frame.left, 
	   	                frame.Height()/2 - 125 + frame.top, 
	   	                frame.Width()/2 + 125 + frame.left,
		   	            frame.Height()/2 + 125 + frame.top);
	   	           
	   	fStartWindow = new StartWindow(startRect, fRecentFiles);
	}
}

BeAccessibleApp::~BeAccessibleApp()
{
	// Save Settings
	BMessage msg;
	int MaxNumFiles = 5, limit;
	int count = fRecentFiles->CountItems();
	
	if (count <= MaxNumFiles)
		limit = count;
	if (count > MaxNumFiles)
		limit = MaxNumFiles;
	

	for (int i = limit-1; i >= 0; i--)
	{
		BString* fileName = (BString*)fRecentFiles->ItemAt(i);
		msg.AddString("recent_file", fileName->String());
	}
	
	BFile file;
	if (file.SetTo(fSettingsPath->Path(), B_WRITE_ONLY | B_CREATE_FILE) == B_OK)
	{
		msg.Flatten(&file);
	}
	
	void* anItem;
	for ( int32 i = 0; (anItem = fRecentFiles->ItemAt(i)); i++)
		delete anItem;
	delete fRecentFiles;

	if (fSettingsPath)
		delete fSettingsPath;
	if (fSettingsFilename)
		delete fSettingsFilename;
	if (fSettingsMsg)
		delete fSettingsMsg;
	if (GlobalSQLMgr)
		delete GlobalSQLMgr;
}

void
BeAccessibleApp::MessageReceived(BMessage* msg)
{
	switch(msg->what)
	{
		case NEW_FILE_REQUESTED:
		{
			entry_ref ref;
			const char* name;
			
			msg->FindRef("directory", &ref);
			msg->FindString("name", &name);
			
			const BDirectory dir(&ref);
			BPath dbPath(&dir, name);
			
			bool rc = GlobalSQLMgr->Open(dbPath.Path());
			
			if (rc)
			{
				AddToRecentFiles(new BString(dbPath.Path()));
				fMainWindow->OpenDatabase(dbPath);
			}
				
			break;
		}
		
		case CLOSE_DB_REQUESTED:
		{
			fMainWindow->CloseDatabase();
		}
		
		case B_QUIT_REQUESTED:
		{
			fMainWindow->CloseDatabase();
			BApplication::QuitRequested();
			break;
		}
		
		case CANCEL_START_WINDOW:
		{
			if (fStartWindow->Lock()) 
				fStartWindow->Quit();
			break;
		}
		
		case SELECT_DATABASE:
		{
			BString selection(fStartWindow->CurrentSelection()->Text());
			
			//Display the Open File dialog box if "More Files..." was selected
			if (selection.Compare("More Files...") == 0)
			{
				if (fStartWindow->Lock()) fStartWindow->Quit();
				fMainWindow->fOpenFile->Show();
				break;
			}
			
			//Otherwise a filename was selected, so open it.
			BPath dbPath(fStartWindow->CurrentSelection()->Text(), NULL, true);
			
			if (GlobalSQLMgr->Open(selection.String()))
			{
				AddToRecentFiles(new BString(dbPath.Path()));
				fMainWindow->OpenDatabase(dbPath);
				
				if (fStartWindow->Lock()) 
					fStartWindow->Quit();
			}
			else
			{
				//FIXME - need to remove the invalid file from the list of recent files
			}
			break;
		}
		
		case NEW_DATABASE_MSG:
		{
			if (fStartWindow->Lock()) 
				fStartWindow->Quit();
			fMainWindow->fNewFile->Show();
			break;
		}
		
		case ADD_TABLE_TO_LIST_MSG:
		{
			BString tableName;
			msg->FindString("table", &tableName);
			fMainWindow->AddTableToList(tableName);
			break;
		}
		
		default:
		{
			BApplication::MessageReceived(msg);
			break;	
		}
	
	}
}


void
BeAccessibleApp::RefsReceived(BMessage* msg)
{
	fRefReceived = true;
	
	switch(msg->what)
	{
		case B_REFS_RECEIVED:
		{
			entry_ref ref;
			msg->FindRef("refs", &ref);
						
			const BEntry entry(&ref, true);
			BPath dbPath(&entry);
			fMainWindow->OpenDatabase(dbPath);			
			break;
		}
		
		default:
			break;	
	
	}

}


bool BeAccessibleApp::QuitRequested()
{
	fMainWindow->CloseDatabase();
	return BApplication::QuitRequested();
}



bool BeAccessibleApp::GetSettings()
{
	BFile file;
	
	status_t status = find_directory(B_USER_SETTINGS_DIRECTORY, fSettingsPath);
	if (status != B_OK)
		return false;
	
	fSettingsPath->Append(fSettingsFilename->String());
	status = file.SetTo(fSettingsPath->Path(), B_READ_ONLY);

	if (status != B_OK)
		return false;

	status = fSettingsMsg->Unflatten(&file);
	
	if (status != B_OK)
	{
		return false;
	}
	
	//Extract the list of recently opened files
	int i = 0;
	BString* fileName = new BString();
	
	while (fSettingsMsg->FindString("recent_file", i, fileName) == B_OK)
	{
		BEntry database(fileName->String());
		if (!database.Exists())
		{
			i++;
			continue;
		}
		
		fRecentFiles->AddItem((void*)fileName, 0);
		fileName = new BString();
		i++;
	}
	delete fileName;
	
	return true;	
}

void
BeAccessibleApp::AddToRecentFiles(BString *bFileName)
{
	//See if this filename already exists in the list
	bool unique = true;
	for (int i = 0; i < fRecentFiles->CountItems(); i++)
	{
		BString* item = (BString*)fRecentFiles->ItemAt(i);
		
		if (item->Compare(bFileName->String()) == 0)
		{
			// File is already in the list; move it to the top
			fRecentFiles->RemoveItem((void*)item);
			fRecentFiles->AddItem((void*)item, 0);
			unique = false;
		}
	}
	
	// File is not in the list; add it to the top
	if (unique)
		fRecentFiles->AddItem((void*)bFileName, 0);
}
  

void
BeAccessibleApp::AboutRequested()
{
	BString message;
	message << "BeAccessible v0.5\n\n" << 
		"An Access-like front-end to the SQLite database.  " <<
		"Developed by randar.\n\n" <<
		"randar@mesanetworks.net\n" <<
		"http://www.bebits.com/app/3613";
	BAlert *alert = new BAlert("About", message.String(), "Ok");
	alert->Go();
}

//
//BList*
//BeAccessibleApp::GetRecentFiles()
//{
//	return fRecentFiles;
//}
//

