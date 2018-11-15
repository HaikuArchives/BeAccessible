//	ColumnDesignView.cpp
#include <iostream>

#ifndef ColumnDesignView_H
#include "ColumnDesignView.h"
#endif

#include <Bitmap.h>
#include <CheckBox.h>
#include <MenuItem.h>
#include <PopUpMenu.h>
#include <RadioButton.h>
#include <String.h>

#include "BeAccessibleApp.h"
#include "BetterScrollView.h"
#include "Colors.h"
#include "ColumnListView.h"
#include "ColumnProperties.h"
#include "DBListView.h"
#include "TableDesignRow.h"




ColumnDesignView::ColumnDesignView(ColumnListView* view, BRect rect, 
     const char *name, BPicture* on, BPicture* off)
	   	   : BView(rect, name, B_FOLLOW_BOTTOM|B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW)
{
	fColumnListView = view;
	SetViewColor(216,216,216,0);
	fPrimaryKeyButton = NULL;
	fIndexMenuField = NULL;
	fIndexMenu = NULL;
	fNoIndex = NULL;
	fDuplicateIndex = NULL;
	fUniqueIndex = NULL;
	fDefaultValueBox = NULL;
	fRequiredMenu = NULL;
	fRequiredMenuField = NULL;
	fRequired = NULL;
	fNotRequired = NULL;
	fOnPrimaryKey = on;
	fOffPrimaryKey = off;
	fCurrentColumnNumber = -1;
	
}


void
ColumnDesignView::Draw(BRect updateRect)
{
  DrawString("Primary Key:", BPoint(10, 23));
}

void
ColumnDesignView::AddPrimaryKeyOptions(ColumnProperties* columnProps)
{
	if (!fPrimaryKeyButton)
	{
		BRect rect(98, 9, 130, 25);
		fPrimaryKeyButton = new BPictureButton(rect, "PrimaryKeyButton", fOffPrimaryKey,
		       fOnPrimaryKey, new BMessage(PRIMARY_KEY_BUTTON), B_TWO_STATE_BUTTON);
		AddChild(fPrimaryKeyButton);
	}
	
	if (Window()->Lock())
	{	
		if (columnProps->PrimaryKey())
			fPrimaryKeyButton->SetValue(B_CONTROL_ON);
		else
			fPrimaryKeyButton->SetValue(B_CONTROL_OFF);
		
		Window()->Unlock();
	}
}

void
ColumnDesignView::AddIndexOptions(ColumnProperties* columnProps)
{
	if (!fIndexMenuField)
	{
		fIndexMenu = new BPopUpMenu("IndexPopupMenu");
		BRect box(7, 65, 230, 81);
		fIndexMenuField = new BMenuField(box, "indexMenu", "Index:", fIndexMenu);
		fIndexMenuField->SetDivider(88);
		AddChild(fIndexMenuField);
		
		BMessage* noIndexMsg = new BMessage(NO_INDEX);
		BMessage* dupIndexMsg = new BMessage(DUP_INDEX);
		BMessage* uniqueIndexMsg = new BMessage(UNIQUE_INDEX);
		
		fNoIndex = new BMenuItem("No", noIndexMsg);
		fDuplicateIndex = new BMenuItem("Yes (Duplicates OK)", dupIndexMsg);
		fUniqueIndex = new BMenuItem("Yes (No Duplicates)", uniqueIndexMsg);
		
		fIndexMenu->AddItem(fNoIndex);
		fIndexMenu->AddItem(fDuplicateIndex);
		fIndexMenu->AddItem(fUniqueIndex);
	}

	if (Window()->Lock())
	{		
		if (columnProps->HasIndex())
		{
			if (columnProps->UniqueIndex() && !fUniqueIndex->IsMarked())
				fUniqueIndex->SetMarked(true);
			else if (!columnProps->UniqueIndex() && !fDuplicateIndex->IsMarked())
				fDuplicateIndex->SetMarked(true);
		}
		else if (!fNoIndex->IsMarked())
		{	//no index
			fNoIndex->SetMarked(true);
		}
	
		Window()->Unlock();
	}
}

void
ColumnDesignView::AddRequiredOptions(ColumnProperties* columnProps)
{
	if (!fRequiredMenuField)
	{
		fRequiredMenu = new BPopUpMenu("RequiredPopupMenu");
		BRect box(7, 96, 230, 116);
		fRequiredMenuField = new BMenuField(box, "required", "Required:", fRequiredMenu);
		fRequiredMenuField->SetDivider(88);
		AddChild(fRequiredMenuField);
		
		fRequired = new BMenuItem("Yes", new BMessage());
		fNotRequired = new BMenuItem("No", new BMessage());
		
		fRequiredMenu->AddItem(fRequired);
		fRequiredMenu->AddItem(fNotRequired);
	}

	if (Window()->Lock())
	{		
		if (columnProps->Required() && !fRequired->IsMarked())
			fRequired->SetMarked(true);
		else if (!columnProps->Required() && !fNotRequired->IsMarked())
			fNotRequired->SetMarked(true);

		Window()->Unlock();
	}
}

void
ColumnDesignView::AddDefaultValueBox(ColumnProperties* columnProps)
{
	BMessage* msg = new BMessage(DEFAULT_VALUE_MSG);
	msg->AddInt32("index", fCurrentColumnNumber);

	if (!fDefaultValueBox)
	{
		BRect box(7, 36, 220, 45);
		fDefaultValueBox = new BTextControl(box, "defaultValue", "Default Value:",
                                            NULL, msg);
        fDefaultValueBox->SetDivider(86);
	    AddChild(fDefaultValueBox);
	}

	//Set the value of the default value box.
	BString defaultValue = columnProps->DefaultValue();
	
	if (Window()->Lock())
	{
		fDefaultValueBox->SetText(defaultValue.String());
		Window()->Unlock();
	}
}



void
ColumnDesignView::DrawTextOptions()
{

}

void
ColumnDesignView::DrawNumberOptions()
{

}

void
ColumnDesignView::DrawBooleanOptions()
{

}

void
ColumnDesignView::DrawDateTimeOptions()
{

}

int
ColumnDesignView::GetConstant(BString* type)
{
	if(type->ICompare("TEXT") == 0)
		return TEXT;
	else if (type->ICompare("NUMBER") == 0)
		return NUMBER;
	else if (type->ICompare("DATETIME") == 0)
		return DATETIME;
	else if (type->ICompare("AUTONUMBER") == 0)
		return AUTONUMBER;
	else if (type->ICompare("BOOLEAN") == 0)
		return BOOLEAN;
	else
		return 0;
}

void
ColumnDesignView::DisplayColumnProperties(int32 columnNumber, bool forceRedraw = false)
{
	//No need to redraw all the options every single time this gets called (BTW, this
	//gets called every time a TableDesignRow gets redrawn).  If this is being called
	//for a different row than last time, proceed.  Otherwise, return. This helps to
	//reduce flickering. Of course, forceRedraw == true changes all of that.
	if (!forceRedraw && (fCurrentColumnNumber == columnNumber))
		return;
	
	fCurrentColumnNumber = columnNumber;
	
	TableDesignRow* row = (TableDesignRow*)fColumnListView->ItemAt(fCurrentColumnNumber);
	ColumnProperties* columnProps = row->fColumnProps;

	if ((columnProps->Name() != "") || (columnProps->Type() != ""))
	{
		if (IsHidden() && Window()->Lock())
		{
			Show();
			Window()->Unlock();
		}
	}
	else
	{
		if (!IsHidden() && Window()->Lock())
		{
			Hide();
			Window()->Unlock();
		}
	
	}
		
	AddPrimaryKeyOptions(columnProps);
	AddIndexOptions(columnProps);
	AddDefaultValueBox(columnProps);
	AddRequiredOptions(columnProps);
}


BString
ColumnDesignView::DefaultValue()
{
	BString value(fDefaultValueBox->Text());
	return value;
}

bool
ColumnDesignView::HasIndex()
{
	BMenu* menu = fIndexMenuField->Menu();
	BMenuItem* item = menu->FindMarked();

	const char* label;
		
	if (item)
		label = item->Label();
	else
		return false;
		
	if (strcmp(label, "No") == 0)
		return false;
	else
		return true;
}

bool
ColumnDesignView::UniqueIndex()
{
	BMenu* menu = fIndexMenuField->Menu();
	BMenuItem* item = menu->FindMarked();
	
	const char* label;
	
	if (item)
		label = item->Label();
	else
		return false;
	
	if (strcmp(label, "Yes (Duplicates OK)") == 0)
		return false;
	else if (strcmp(label, "Yes (No Duplicates)") == 0)
		return true;
	else
		return false;
}


bool
ColumnDesignView::Required()
{
	BMenu* menu = fRequiredMenuField->Menu();
	BMenuItem* item = menu->FindMarked();
	
	const char* label;
	
	if (item)
		label = item->Label();
	else
		return false;	// default value for Required field
	
	if (strcmp(label, "Yes") == 0)
		return true;
	else
		return false;	
}
