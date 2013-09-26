//	TableNameWindow.cpp

#ifndef _APPLICATION_H
#include <Application.h>
#endif
#ifndef TableNameWindow_H
#include "TableNameWindow.h"
#endif

#include <TextControl.h>
#include <Button.h>

#include "Constants.h"

TableNameWindow::TableNameWindow(const char* title, const char* table, int mode, BWindow* parent)
				: BWindow(BRect(50,50,250,150), title, B_MODAL_WINDOW, B_NOT_RESIZABLE)
{
	BView* view = new BView(BRect(0,0,250,150), "view", 
	                        B_FOLLOW_ALL, B_WILL_DRAW);
	view->SetViewColor(216,216,216,0);
	
	BTextControl* textbox;
	BButton* ok_btn;
	BButton* cancel_btn;
	BRect rect(5, 5, 155, 35);

	textbox = new BTextControl(rect, "textbox", "Table Name:", table, new BMessage());
	view->AddChild(textbox);

	if (mode == 1)
	{	
		// Saving a new table
		BMessage* msg = new BMessage(SAVE_TABLE_NAME);
		msg->AddPointer("textcontrol", (void*)textbox);
		ok_btn = new BButton(BRect(120, 40, 180, 70), "OkButton", "Ok", msg);
	}
	else if (mode == 2)
	{
		// Renaming an existing table
		BMessage* msg = new BMessage(RENAME_TABLE);
		msg->AddPointer("textcontrol", (void*)textbox);	
		ok_btn = new BButton(BRect(120, 40, 190, 70), "OkButton", "Ok", msg);
	}
	
	ok_btn->SetTarget(parent);
	view->AddChild(ok_btn);

	cancel_btn = new BButton(BRect(40, 40, 100, 70), "CancelButton", "Cancel",
							new BMessage());
	cancel_btn->SetTarget(parent);
	view->AddChild(cancel_btn);
	
	AddChild(view);
	Show();
}

bool
TableNameWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return (true);
}
