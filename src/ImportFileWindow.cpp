#include <Message.h>
#include <iostream>
#include <fstream>

#include "ImportFileWindow.h"
#include "ImportFileView.h"
#include "ImportUtilities.c"

//extern SQLiteManager* GlobalSQLMgr;

ImportFileWindow::ImportFileWindow(BRect frame, BString file)
	: BWindow(frame, "Import File", B_FLOATING_WINDOW_LOOK,
	          B_MODAL_APP_WINDOW_FEEL, B_NOT_ZOOMABLE),
	  fFile(file)
{
	fImportFileView = new ImportFileView(Bounds(), file);
	AddChild(fImportFileView);
}





void
ImportFileWindow::MessageReceived(BMessage* msg)
{
	switch(msg->what)
	{
		default:
		{
			BWindow::MessageReceived(msg);
		}
	}
}


