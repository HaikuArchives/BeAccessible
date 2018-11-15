#include <iostream>

#include <Application.h>
#include <String.h>

#include "Constants.h"
#include "PasteTableView.h"
#include "PasteTableWindow.h"
#include "SQLiteManager.h"

extern SQLiteManager* GlobalSQLMgr;

PasteTableWindow::PasteTableWindow(BRect frame, BString table)
  : BWindow(frame, "Paste Table As", B_FLOATING_WINDOW_LOOK,
	          B_MODAL_APP_WINDOW_FEEL, B_NOT_ZOOMABLE),
  fTable(table)
{
	fPasteView = new PasteTableView(Bounds());
	AddChild(fPasteView);
	Show();
}

void
PasteTableWindow::MessageReceived(BMessage* msg)
{
	switch(msg->what)
	{
		case PASTE_TABLE_AS_MSG:
		{
			bool rc = false;
		
			if (fPasteView->GetPasteType() == STRUCTURE_ONLY)
			{
				rc = GlobalSQLMgr->CopyTableStructure(fTable, 
					       					    fPasteView->GetNewTableName());
			}
			else if (fPasteView->GetPasteType() == STRUCTURE_AND_DATA)
			{
				rc = GlobalSQLMgr->CopyTableData(fTable, 
				                           fPasteView->GetNewTableName());
			}
			
			if (rc && LockLooper())
			{
				BMessage msg(ADD_TABLE_TO_LIST_MSG);
				msg.AddString("table", fPasteView->GetNewTableName());
				BMessenger(be_app).SendMessage(&msg);
				Quit();
			}
			
			break;
		}
		
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}

}

