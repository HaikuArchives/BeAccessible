#include <TextControl.h>
#include <Button.h>
#include <RadioButton.h>
#include <Box.h>
#include <iostream.h>

#include "PasteTableView.h"
#include "Constants.h"

PasteTableView::PasteTableView(BRect frame)
: BView(frame, "PasteTableView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW)
{
	SetViewColor(216,216,216,0);
	
	BRect tableRect(10,25,150,40);
	fNewTextField = new BTextControl(tableRect,"TableTextField",
	                NULL, "", new BMessage(PASTE_TABLE_AS_MSG), B_FOLLOW_LEFT, 
	                B_NAVIGABLE|B_WILL_DRAW);
	AddChild(fNewTextField);
	
	BRect boxRect(13, 61, 150, 120);
	fBox = new BBox(boxRect);
	fBox->SetLabel("Paste Options:");
	
	AddChild(fBox);
	
	//BRect structRect(20,70,130,30);
	BRect structRect(5,15,115,15);
	fStructOnlyButton = new BRadioButton(structRect, "StructOnly", 
	                           "Structure Only", NULL);
	fBox->AddChild(fStructOnlyButton);
	
	//BRect structDataRect(20,90,149,100);
	BRect structDataRect(5,35,135,15);
	fStructDataButton = new BRadioButton(structDataRect, "StructData", 
	                           "Structure and Data", NULL);
	fBox->AddChild(fStructDataButton);

	BRect okRect(170,10,230,30);
	fOkButton = new BButton(okRect, "OkButton", "Ok", 
	                                 new BMessage(PASTE_TABLE_AS_MSG));
	AddChild(fOkButton);
	
	BRect cancelRect(170,40,230,60);
	fCancelButton = new BButton(cancelRect, "CancelButton", "Cancel", 
									new BMessage(B_QUIT_REQUESTED));
	AddChild(fCancelButton);


	fNewTextField->MakeFocus(true);
	fStructDataButton->SetValue(1);
	
}

void
PasteTableView::AllAttached()
{
	fNewTextField->MakeFocus(true);
	BView::AllAttached();
}

//
//void
//PasteTableView::SetTargets()
//{
//	if (fNewTextField->SetTarget(this) != B_OK)
//		cout << "set target failed" << endl;
//	fStructOnlyButton->SetTarget(this);
//	fStructDataButton->SetTarget(this);
//	fOkButton->SetTarget(this);
//	fCancelButton->SetTarget(this);
//}
//

void
PasteTableView::Draw(BRect rect)
{
	DrawString("Table Name:", BPoint(10,20));
//	DrawString("Paste Options", BPoint(20,65));
//	SetHighColor(130,130,130,0);
//	StrokeLine(BPoint(13,61), BPoint(18,61));
//	StrokeLine(BPoint(13,61), BPoint(13,116));
//	StrokeLine(BPoint(13,116), BPoint(150,116));
//	StrokeLine(BPoint(150,116), BPoint(150,61));
//	StrokeLine(BPoint(20+StringWidth("Paste Options")+2,61),BPoint(150,61));
}

void
PasteTableView::MessageReceived(BMessage* msg)
{
	switch(msg->what)
	{
		default:
		{
			BView::MessageReceived(msg);
		}
	}
}

BString
PasteTableView::GetNewTableName()
{
	return fNewTextField->Text();
}

int
PasteTableView::GetPasteType()
{
	if (fStructOnlyButton->Value())
		return STRUCTURE_ONLY;
	else if (fStructDataButton->Value())
		return STRUCTURE_AND_DATA;
	else
		return -1;
}

