//	BeAccessibleWindow.cpp

#ifndef _APPLICATION_H
#include <Application.h>
#endif
#ifndef Start_Window_H
#include "StartWindow.h"
#endif

#include <RadioButton.h>
#include <ListView.h>
#include <ScrollView.h>
#include <Button.h>

#include "Constants.h"

StartWindow::StartWindow(BRect frame, const BList* recentFiles)
				: BWindow(frame, "", B_MODAL_WINDOW, B_NOT_ZOOMABLE|B_NOT_RESIZABLE)
{
	BView* view = new BView(BRect(0,0,frame.right,frame.bottom), "view", 
	                        B_FOLLOW_ALL, B_WILL_DRAW);
	view->SetViewColor(216,216,216,0);
	
	//Create 'New Database' and 'Existing Database' radio buttons
	BRect rect(20,15,170,35);
	newDatabase = new BRadioButton(rect, "new_db", "New Database",
	           new BMessage(NEW_DATABASE_MSG));
	view->AddChild(newDatabase);
	
	rect.Set(20,50,170,65);
	existingDatabase = new BRadioButton(rect, "existing_db", "Existing Database", 
	                                    new BMessage(EXISTING_DATABASE_MSG));
	view->AddChild(existingDatabase);

	//Create BListView containing last 5 recently opened databases	
	rect.Set(25,80,225,175);
	dbList = new BListView(rect, "DB_List");
	dbList->AddItem(new BStringItem("More Files..."));
	
	for (int i = 0; i < recentFiles->CountItems(); i++)
	{
		BString* item = (BString*)recentFiles->ItemAt(i);
		dbList->AddItem(new BStringItem(item->String()));
	}

	dbList->SetInvocationMessage(new BMessage(SELECT_DATABASE));
	dbList->SetTarget(be_app);
	
	scrollView = new BScrollView("scroll_dbList", dbList, B_FOLLOW_LEFT|B_FOLLOW_TOP,
	                               0, true, true);
	view->AddChild(scrollView);
	
	//Create 'Cancel' and 'Select' buttons
	cancel = new BButton(BRect(90,208,150,228), "CancelButton", "Cancel", 
	                     new BMessage(CANCEL_START_WINDOW));
	cancel->SetTarget(be_app);
	view->AddChild(cancel);	                     
	
	select = new BButton(BRect(170,208,230,228), "SelectButton", "Select",
	                     new BMessage(SELECT_DATABASE));
	select->SetTarget(be_app);
	view->AddChild(select);
	
	AddChild(view);
	Show();

	//Lock the looper so we can set the default values
	Lock();
	
	//Default radio button is 'Existing Database'
	existingDatabase->SetValue(B_CONTROL_ON);

	//Default selection for the BListView is the first item (if there is one)
	if (dbList->CountItems() > 0)
		dbList->Select(0);

	Unlock();
}

bool
StartWindow::QuitRequested()
{
	BMessenger(be_app).SendMessage(B_QUIT_REQUESTED);
	return (true);
}

void StartWindow::MessageReceived(BMessage* message)
{

	switch(message->what)
	{
		case NEW_DATABASE_MSG:
			if (!scrollView->IsHidden())
				scrollView->Hide();
			select->SetMessage(new BMessage(NEW_DATABASE_MSG));
			break;
		
		case EXISTING_DATABASE_MSG:
			if (scrollView->IsHidden())
				scrollView->Show();
			select->SetMessage(new BMessage(SELECT_DATABASE));
			break;
	}
}

BStringItem*
StartWindow::CurrentSelection()
{
	return (BStringItem*)dbList->ItemAt(dbList->CurrentSelection(0));
}
