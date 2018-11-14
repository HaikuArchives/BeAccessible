// BeAccessibleWindow.cpp

#ifndef _APPLICATION_H
#include <Application.h>
#endif
#ifndef BeAccessible_Window_H
#include "BeAccessibleWindow.h"
#endif
#include <MenuBar.h>
#include <MenuItem.h>
#include <FilePanel.h>
#include <Path.h>
#include <String.h>
#include <iostream>
#include <ClassInfo.h>
#include <FilePanel.h>
#include <Button.h>

#include "DBListView.h"
#include "DBTableTabView.h"
#include "DBQueryTabView.h"
#include "TabView.h"
#include "DBWindow.h"
#include "BeAccessibleApp.h"
#include "Constants.h"
#include "SQLiteManager.h"
#include "ImportFileWindow.h"

extern SQLiteManager* GlobalSQLMgr;

BeAccessibleWindow::BeAccessibleWindow(BRect frame, BList* recentFiles)
				: BWindow(frame, "BeAccessible", B_TITLED_WINDOW, 0)
{
	//Create MenuBar
	BRect rect = Bounds();
	rect.bottom = MENU_BAR_HEIGHT;
	
	fMenuBar = new BMenuBar(rect, "MenuBar");
	AddChild(fMenuBar);
	
	//Create Menus
	fFileMenu	= new BMenu("File");
	fMenuBar->AddItem(fFileMenu);
	fOpenRecentMenu = new BMenu("Open Recent");
	fToolsMenu = new BMenu("Tools");
	fMenuBar->AddItem(fToolsMenu);
	
	//Create MenuItems
	fFileMenu->AddItem(new BMenuItem("New", new BMessage(MENU_FILE_NEW_MSG), 'N'));
	fFileMenu->AddItem(new BMenuItem("Open", new BMessage(MENU_FILE_OPEN_MSG), 'O'));
	fFileMenu->AddSeparatorItem();
	
	BMenu* externalDataMenu = new BMenu("Get External Data");
	fFileMenu->AddItem(externalDataMenu);
	fImportItem = new BMenuItem("Import Text File", new BMessage(IMPORT_FILE_REQUESTED_MSG));
	externalDataMenu->AddItem(fImportItem);
	fFileMenu->AddSeparatorItem();
	
	fCloseItem = new BMenuItem("Close", new BMessage(MENU_FILE_CLOSE_MSG), 'W');
	fFileMenu->AddItem(fCloseItem);
	fFileMenu->AddSeparatorItem();

	BMenuItem* about = new BMenuItem("About BeAccessible", new BMessage(B_ABOUT_REQUESTED));
	fFileMenu->AddItem(about);
	about->SetTarget(be_app);
	
	fFileMenu->AddSeparatorItem();
	BMenuItem* quitItem = new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED), 'Q');
	quitItem->SetTarget(be_app);
	fFileMenu->AddItem(quitItem);


	fCompactItem = new BMenuItem("Compact", new BMessage(COMPACT_DATABASE_MSG));
	fToolsMenu->AddItem(fCompactItem);
	
	// Create MenuItems for recent files
	for(int i = 0; i < recentFiles->CountItems(); i++)
	{
		BString* file = (BString*)recentFiles->ItemAt(i);
		BPath path(file->String());
		
		BMessage* fileMsg = new BMessage(OPEN_DATABASE_MSG);
		fileMsg->AddString("path", file->String());
		fOpenRecentMenu->AddItem(new BMenuItem(path.Leaf(), fileMsg));
	}
	
	// Add the Open Recent menu between the Open and Close menu items
	fFileMenu->AddItem(fOpenRecentMenu, 2);
	
	fCloseItem->SetEnabled(false);
	fCompactItem->SetEnabled(false);
	fImportItem->SetEnabled(false);

    //Set up View
	frame.OffsetTo(B_ORIGIN);
	fMainView = new BView(frame, "BeAccessibleView", B_FOLLOW_ALL, B_WILL_DRAW);
	fMainView->SetViewColor(216,216,216,0);
	
	AddChild(fMainView);
	Show();
	
    //Set up File->Open and File->Save panels
    BMessage newMsg(NEW_FILE_REQUESTED), importMsg(IMPORT_FILE_MSG);
   	fOpenFile = new BFilePanel(B_OPEN_PANEL, NULL, NULL, 0, false);
   	fNewFile = new BFilePanel(B_SAVE_PANEL, NULL, NULL, 0, false, &newMsg);
   	fImportFile = new BFilePanel(B_OPEN_PANEL, NULL, NULL, 0, false, &importMsg);
   	
   	//Edit the look of the ImportFile panel
   	fImportFile->Window()->SetTitle("BeAccessible: Import");
	fImportFile->SetButtonLabel(B_DEFAULT_BUTTON, "Import");
	   	
   	//Initalize data members
   	fTabView = NULL;
   	fTableTab = NULL;
   	fQueryTab = NULL;
   	fNewGrid = NULL;
   	fPath = new BString;
}



bool
BeAccessibleWindow::QuitRequested()
{
	BMessenger(be_app).SendMessage(B_QUIT_REQUESTED);
	return (true);
}


void BeAccessibleWindow::MessageReceived(BMessage* msg)
{
	switch(msg->what)
	{
		case COMPACT_DATABASE_MSG:
		{
			GlobalSQLMgr->Compact();
			break;
		}
		
		
		case OPEN_DATABASE_MSG:
		{
			BString path;
			if (msg->FindString("path", &path) == B_OK)
			{
				OpenDatabase(BPath(path.String()));
			}
			break;
		}
	
		case MENU_FILE_NEW_MSG:
		{
			fNewFile->Show();
			break;
		}
			
		case MENU_FILE_OPEN_MSG:
		{
  			fOpenFile->Show();
		  	break;
		}
		
		case MENU_FILE_CLOSE_MSG:
		{
			CloseDatabase();
			break;
		}
		
		case OPEN_BUTTON:
		{
			BString name;
			msg->FindString("TabName", &name);
			
			if (name == "Table" || name == "TableList")
				OpenTable(DATA_VIEW);
			//else if (name == "Query")
		
			break;
		}
		
		case DESIGN_BUTTON:
		{
			BString name;
			msg->FindString("TabName", &name);
			
			if (name == "Table")
				OpenTable(DESIGN_VIEW);	
			//else if (name == "Query")
			break;
		}
		
		case NEW_BUTTON:
		{
			BString name;
			msg->FindString("TabName", &name);
			
			if (name == "Table")
				NewTable(DESIGN_VIEW);
			//else if (name == "Query")
		}
		
		case ENABLE_OPEN_BUTTON:
		{
			BString listName;
			msg->FindString("ListName", &listName);
			
			if (listName == "TableList")
			{
				fTableTab->fOpenButton->SetEnabled(true);
				fTableTab->fDesignButton->SetEnabled(true);
			}
			//else if (listName == "QueryList")
			
			break;
		}
		
		case DISABLE_OPEN_BUTTON:
		{
			BString listName;
			msg->FindString("ListName", &listName);
			
			if (listName == "TableList")
			{
				fTableTab->fOpenButton->SetEnabled(false);
				fTableTab->fDesignButton->SetEnabled(false);
			}
			//else if (listName == "QueryList")
			break;
		}
		
		case B_QUIT_REQUESTED:
		{
			BMessenger(be_app).SendMessage(B_QUIT_REQUESTED);
			break;
		}
		
		case IMPORT_FILE_REQUESTED_MSG:
		{
			fImportFile->SetTarget(BMessenger(this));
			fImportFile->Show();
			break;
		}
		
		case IMPORT_FILE_MSG:
		{
			entry_ref ref;
			msg->FindRef("refs", &ref);
						
			const BEntry entry(&ref, true);
			BPath dbPath(&entry);
			
			BRect frame(200,200,600,700);
			ImportFileWindow* importWindow = new ImportFileWindow(frame, dbPath.Path());
			importWindow->Show();
			break;
		}
		
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
}


void BeAccessibleWindow::OpenDatabase(BPath path)
{
	//If a database is already open, close it before opening a new one.
	if (GlobalSQLMgr->IsOpen())
		CloseDatabase();
	
	//Open the given database
	bool databaseOpened = GlobalSQLMgr->Open(path.Path());
	
	if (databaseOpened)
	{
		//Add database to list of recently opened files
		BeAccessibleApp* myApp = static_cast<BeAccessibleApp*>(be_app);
		myApp->AddToRecentFiles(new BString(path.Path()));
		
		//Create the beaccessible_master table if it's not there
		if (!GlobalSQLMgr->TableExists("beaccessible_master"))
		{
			BString sql = "create table beaccessible_master (name Text, type Text, "
			              "value Text, primary key (name, type));";
			GlobalSQLMgr->Execute(sql.String());
		}
	}

	Lock();
	
	//Change title of window to include database name
	BString newTitle("BeAccessible : ");
	newTitle += path.Leaf();
	SetTitle(newTitle.String());
	
	//Create tab for Tables (hopefully someday Queries, too!)
	BRect frame = Bounds();
	BRect menuHeight = fMenuBar->Bounds();
	frame.top += menuHeight.Height() + 1;
	frame.InsetBy(10.0, 10.0);

	const char* tabNames[1] = {"Tables"};

	fTabView = new TabView(frame, NULL, tabNames, 1, B_FOLLOW_ALL_SIDES); 
	fTabView->SetViewColor(216,216,216,0);
	BRect ContentRect = fTabView->GetContentArea();
	ContentRect.InsetBy(5,5);

	fTableTab = new DBTabView(ContentRect, "Table");

	BView* tabs[1];
	tabs[0] = (BView*)fTableTab;
	fTabView->AddViews(tabs);
	fMainView->AddChild(fTabView);
	
	//Select the first table as default
	fTableTab->fListView->Select(0);	
	
	//Enable certain menu items since there is now a database open
	EnableMenuItems();
	Unlock();
	
	// Save the database's full path to a string
	fPath->SetTo(path.Path());
}


//Closes the current database.  Closes any open windows and performs a variety of 
//cleanup tasks.
void BeAccessibleWindow::CloseDatabase()
{
	//Close the currently opened database file.
	GlobalSQLMgr->Close();
	
	CloseAllWindows();
	
	//Change title to remove the database name
	SetTitle("BeAccessible");

	//Get rid of the tabs, which are specific to the db being closed.
	if (fMainView && fTabView && Lock())
	{
	    if (fMainView->RemoveChild(fTabView))
    		delete fTabView;
    	Unlock();
	}
	
	//There's no db open, so disable certain menu item.
	if (fCloseItem && fCompactItem && Lock())
	{
		DisableMenuItems();
		Unlock();
	}
}


//Closes all open windows that are Tables... other windows like OpenFile/ShowFile
//should still be left open.
void BeAccessibleWindow::CloseAllWindows()
{
	int i = 0;
	BWindow* window;

	while((window = be_app->WindowAt(i)))
	{
		//Close all instances of TableWindow
		if (window && is_instance_of(window, TableWindow))
		{
			if (window->Lock())
				window->Quit();
				//No need to adjust i here, since we just closed a window, i now
				//points to the next window
		}
		else
		{
			//Don't close the window at i, go on to the next one
			i++;
		}
	}
}


//Determines if a given window is currently open.
bool BeAccessibleWindow::WindowExists(BString name)
{
	//Loop through all open windows
	for (int i = 0; i < be_app->CountWindows(); i++)
	{
		BWindow* window = be_app->WindowAt(i);
		BString windowTitle = window->Title();
		
		if (name == windowTitle)
			return true;
	}
	
	return false;
}


//Opens the table that is currently selected using the given view.  Or, if the table
//is already open, brings it to the front.
void BeAccessibleWindow::OpenTable(int view)
{
	DBTabView* tab = (DBTabView*)fTabView->GetCurrentTab();
	BStringItem* selectedTable;
	
	//Find the currently selected table
	if (tab->fListView->CurrentSelection() >= 0)
	{
		selectedTable = (BStringItem*)tab->fListView->ItemAt(
		  					        tab->fListView->CurrentSelection() );
	}
	else
	{
		//No table is selected
		return;
	}
	
	
	if (!WindowExists(selectedTable->Text()))
	{
		//Open a new table window if it isn't already
		fNewGrid = new TableWindow(view, selectedTable->Text());
	}
	else
	{
		//If the table is already open, bring it to the front
		BringWindowToFront(selectedTable->Text());
	}
}


//Finds the given window name and brings it to the front.  Used when a user tries to
//open a window that is already open.
void
BeAccessibleWindow::BringWindowToFront(BString windowName)
{
	//Loop through all open windows
	for (int i = 0; i < be_app->CountWindows(); i++)
	{
		BWindow* window = be_app->WindowAt(i);
		BString windowTitle = window->Title();
		
		//Bring the window to the front if the titles match
		if (windowName == windowTitle)
		{
			window->Activate(true);
			return;
		}
	}
}



//Creates a new TableWindow in design view with a default table name
void
BeAccessibleWindow::NewTable(int view)
{
	BString tableName("Table");
	
	int i = 1;
	tableName << i;
	
	while (GlobalSQLMgr->TableExists(tableName))
	{
		tableName.SetTo("Table");
		tableName << ++i;
	}

	fNewGrid = new TableWindow(DESIGN_VIEW, tableName.String(), true);
}


void
BeAccessibleWindow::AddTableToList(BString tableName)
{
	DBTabView* tab = (DBTabView*)fTabView->GetCurrentTab();
	
	if (Lock())
	{	
		tab->fListView->AddItem(new BStringItem(tableName.String()));
		tab->fListView->Sort();
		Unlock();
	}
}


//Disables certain menu items when there is no database open
void
BeAccessibleWindow::DisableMenuItems()
{
	fCloseItem->SetEnabled(false);
	fCompactItem->SetEnabled(false);
	fImportItem->SetEnabled(false);
}


//Enables certain menu items when a database is open
void
BeAccessibleWindow::EnableMenuItems()
{
	fCloseItem->SetEnabled(true);
	fCompactItem->SetEnabled(true);
	fImportItem->SetEnabled(true);
}


