#include <Region.h>
#include <strings.h>
#include <Window.h>
#include <MenuBar.h>
#include <PopUpMenu.h>
#include <MenuItem.h>
#include <String.h>
#include <iostream.h>

#include "TableDesignRow.h"
#include "MyMenuField.h"
#include "NewStrings.h"
#include "TableWindow.h"
#include "GridView.h"
#include "GridCell.h"
#include "GridConstants.h"
#include "Constants.h"
#include "ColumnDesignView.h"
#include "BeAccessibleApp.h"
#include "ColumnProperties.h"

TableDesignRow::TableDesignRow(GridView* view, ColumnProperties props, 
                 PrefilledBitmap** IconList, BString** args, int32* currentColumn, 
                 bool last = false)
: GridRow(view, 1, args, IconList, currentColumn, last)
{
	fColumnProps = new ColumnProperties(props);
	
	if (fColumnProps->PrimaryKey())
		fCurrentIcon = PRIMARY_KEY;
		
	fMenuBar = NULL;
	fPopUpMenu = NULL;

	//Add an extra entry to the fCachedRects lists for the datatype menu.  GridRow's
	//constructor only adds entries to fCachedRects if the column is a textbox.
	fCachedRects.AddItem(new BRect(-1,-1,-1,-1));
}


TableDesignRow::~TableDesignRow()
{
  delete fColumnProps;
  
  if (fView && fMenuBar)
  {
  	fView->RemoveChild(fMenuBar);
  	delete fMenuBar;
  }
}


void TableDesignRow::DrawItemColumn(BView* owner, BRect item_column_rect, int32 column_index, 
                             bool complete)
{
	rgb_color color;
	bool selected = IsSelected();
	
	if(fState == HIGHLIGHTED)
		color = BeListSelectGrey;
	else
		color = White;

	owner->SetLowColor(color);
	owner->SetDrawingMode(B_OP_COPY);

	if((fState == HIGHLIGHTED) || complete)
	{
		fView->MakeFocus();
		owner->SetHighColor(color);
		owner->FillRect(item_column_rect);
	}

	BRegion Region;
	Region.Include(item_column_rect);
	owner->ConstrainClippingRegion(&Region);
	owner->SetHighColor(Black);

	owner->StrokeLine(item_column_rect.LeftBottom(), item_column_rect.RightBottom(), B_MIXED_COLORS);
  if (column_index >= 0)
		owner->StrokeLine(item_column_rect.RightBottom(), item_column_rect.RightTop(), B_MIXED_COLORS);

	//Cache the new size of the cell's BRect
	BRect* cached_rect = (BRect*)fCachedRects.ItemAt(column_index);
	if(cached_rect != NULL)
		*cached_rect = item_column_rect;


	if (selected)
	{
			TableWindow* window = (TableWindow*)fView->Window();
			window->GetColumnDesignView()->DisplayColumnProperties(fView->IndexOf(this));
	}
	

	bool primKey = fColumnProps->PrimaryKey();
	int32 numSelectedRows = fView->CountSelectedItems();

	if(column_index == 0 && (selected || primKey))
	{
		if (primKey && selected && (fState != HIGHLIGHTED))
			fCurrentIcon = SELECTED_PRIMARY_KEY;
		else if (primKey)
			fCurrentIcon = PRIMARY_KEY;
		else
			fCurrentIcon = CURRENT_RECORD;		
	
		//FIXME - cache the icon column rect here
		if (primKey)
			DrawIcon(item_column_rect, owner);
		else if ((fState != HIGHLIGHTED) && selected && numSelectedRows == 1)
			DrawIcon(item_column_rect, owner);

	}
	else if(column_index == 1 && selected && (fState != HIGHLIGHTED))
	{
		item_column_rect.InsetBy(2.0,2.0);
		if (!fCell[0])
		{
				fCell[0] = new GridCell(item_column_rect, 
				                     fColumnProps->Name().String(), 
				                     this, column_index);
				
				owner->AddChild(fCell[0]);

				if (*fCurrentColumn == column_index)
				{
					fCell[0]->MakeFocus(true);
					if (fHighlightText)
					{
						fCell[0]->SelectAll();
						fHighlightText = false;
					}
					else
					{
						fCell[0]->ConvertFromParent(&fPointClicked);
						int32 offset = fCell[0]->OffsetAt(fPointClicked);
						fCell[0]->Select(offset, offset);
					}
				}
		}
	}
	else if(column_index == 1)   
	{
		//Draw Field Name with no text box, just using primitives
		owner->DrawString(fColumnProps->Name().String(),
			BPoint(item_column_rect.left+4.0,item_column_rect.top+fTextOffset));
	}
	else if (column_index == 2)
	{
		//Draw the PopUpMenu for the FieldType
		
		if (!fMenuBar)
		{
			//FieldType menu doesn't exist, create it
			fMenuBar = new BMenuBar(item_column_rect, "menubar", B_FOLLOW_NONE,
			                        B_ITEMS_IN_COLUMN, false);
			fPopUpMenu = new BPopUpMenu("", true, true);
			PopulateDataTypeMenu();

			fMenuBar->AddItem(fPopUpMenu);
			owner->AddChild(fMenuBar);

			//Select the menuItem based on the type of this field
			if (fColumnProps->Type() == "Text")
			{
				BMenuItem* item = fPopUpMenu->FindItem("Text");
				item->SetMarked(true);
			}
			else if (fColumnProps->Type() == "DateTime")
			{
				BMenuItem* item = fPopUpMenu->FindItem("DateTime");
				item->SetMarked(true);
			}
			else if (fColumnProps->Type() == "Boolean")
			{
				BMenuItem* item = fPopUpMenu->FindItem("Boolean");
				item->SetMarked(true);
			}
			else if (fColumnProps->Type() != "")
			{
				//Any other data type will be considered either a Number or Autonumber.
				if (fColumnProps->AutoNumber())
				{
					//AutoNumber	
					BMenuItem* item = fPopUpMenu->FindItem("AutoNumber");
					item->SetMarked(true);
				}
				else
				{
					//Number
					BMenuItem* item = fPopUpMenu->FindItem("Number");
					item->SetMarked(true);
				}
			}
		}
	}
	
	owner->ConstrainClippingRegion(NULL);
}




void TableDesignRow::Exit(bool doInvalidate)
{
	if (fState == TEXTBOX)
	{
			if (fLast)
				AddRow(1, false);
		
			//Remove the BTextView from the view so we can delete it
			if (fCell[0])
			{
				if (fView->Window()->Lock())
				{
					fView->RemoveChild(fCell[0]);
					fView->Window()->Unlock();
				}
		
				delete fCell[0];
				fCell[0] = NULL;
			}

	}
	
	if (fCurrentIcon == SELECTED_PRIMARY_KEY)
		fCurrentIcon = PRIMARY_KEY;
	else if (fCurrentIcon != PRIMARY_KEY)
			fCurrentIcon = CURRENT_RECORD;
	
  if (doInvalidate)
		InvalidateRow();	
}


void TableDesignRow::PrintToStream()
{
		//cout << "(";
		
		//cout << fColumnProps->Name().String();

		//cout << "): ";
}



void TableDesignRow::PreviousField(int32 currentColumn)
{
	PreviousRow(1);
}


void TableDesignRow::NextField(int32 currentColumn)
{
	NextRow(1);
}


void TableDesignRow::NextRow(int32 column)
{
	BString methodName("TableDesignRow::NextRow()");
	
	if (ExitRequested())
		Exit(false);
	else
		return;
		
	if (!fLast)
	{
		int32 currentIndex = fView->IndexOf(this);
		
		//If the next row exists, inform the Window to move to the next row
		if (currentIndex + 1 < fView->CountItems())
		{
			BMessage nextRowMsg(GRID_GOTO_ROW_MSG);
			nextRowMsg.AddInt32("rowNumber", currentIndex + 1);
			nextRowMsg.AddInt32("column", column);
			BMessenger(fView).SendMessage(&nextRowMsg, fView);
		}
	}
}



void TableDesignRow::PreviousRow(int32 column)
{
	BString methodName("TableDesignRow::PreviousRow()");
	
	if (ExitRequested())
		Exit(false);
	else
		return;
		
	int32 currentIndex = fView->IndexOf(this);
	
	//Make sure the previous row exists, then inform the Window to move to it.
	if (currentIndex > 0)
	{
		BMessage nextRowMsg(GRID_GOTO_ROW_MSG);
		nextRowMsg.AddInt32("rowNumber", currentIndex - 1);
		nextRowMsg.AddInt32("column", column);
		BMessenger(fView).SendMessage(&nextRowMsg, fView);	
	}
}


bool TableDesignRow::SaveChanges()
{
	if (!fCell[0] || (fState != TEXTBOX))
		return false;
	

	BString columnName(fCell[0]->Text());

	//if there's text in the column name cell, then make sure a column type is selected
	if (columnName.Length() > 0)
	{
		//if no column type has been selected, default to 'Text'
		if (!fPopUpMenu->FindMarked())
		{
			fPopUpMenu->FindItem("Text")->SetMarked(true);
		}		
	}
	
	SaveColumnProperties();
	return true;
}


void
TableDesignRow::AddRow(int32 column, bool makeFocus = true)
{
		BString methodName("TableDesignRow::AddRow()");
		
		if(!fModified)
		{
			fCurrentIcon = NEW_RECORD;
			//This last row has to be modified before a new row can be created.
			return;
		}
		fModified = false;
	
		//This row will no longer be the last one.  Change icon and flag accordingly.
		if (fCurrentIcon == SELECTED_PRIMARY_KEY)
			fCurrentIcon = PRIMARY_KEY;
		else
			fCurrentIcon = CURRENT_RECORD;
		fLast = false;
	
		BMessage addRowMsg(GRID_APPEND_ROW_MSG);
		addRowMsg.AddInt32("currentRow", fView->IndexOf(this));
		addRowMsg.AddInt32("column", column);
		addRowMsg.AddBool("makeFocus", makeFocus);
			
		BMessenger(fView->Window()).SendMessage(&addRowMsg, fView);
}


//
//void TableDesignRow::Restore()
//{
//	if (!fHasTextBox)
//		return;
//
//	for (int i = 0; i < fNumColumns-1; i++)
//	{
//		fCell[0]->SetText(fColumnProps->Name().String(), fColumnProps->Name().Length());
//		fCell[0]->fModified = false;
//	}
//
//	fModified = false;
//	fCurrentIcon = CURRENT_RECORD;
//}
//

void TableDesignRow::PopulateDataTypeMenu()
{
	BMessage *textMsg = new BMessage(DATA_TYPE_CHANGED);
	textMsg->AddPointer("row", (void*)this);
	fPopUpMenu->AddItem(new BMenuItem("Text", textMsg));

	BMessage *numMsg = new BMessage(DATA_TYPE_CHANGED);
	numMsg->AddPointer("row", (void*)this);
	fPopUpMenu->AddItem(new BMenuItem("Number", numMsg));

	BMessage *autoNumMsg = new BMessage(DATA_TYPE_CHANGED);
	autoNumMsg->AddPointer("row", (void*)this);
	fPopUpMenu->AddItem(new BMenuItem("AutoNumber", autoNumMsg));

	BMessage *boolMsg = new BMessage(DATA_TYPE_CHANGED);
	boolMsg->AddPointer("row", (void*)this);
	fPopUpMenu->AddItem(new BMenuItem("Boolean", boolMsg));

	BMessage *dateMsg = new BMessage(DATA_TYPE_CHANGED);
	dateMsg->AddPointer("row", (void*)this);
	fPopUpMenu->AddItem(new BMenuItem("DateTime", dateMsg));
	
	fPopUpMenu->SetTargetForItems(fView);
}


void
TableDesignRow::RightClickMenu()
{
	if (fState != HIGHLIGHTED)
		return;
	
	BPopUpMenu* menu = new BPopUpMenu("rightClick", false, false);
	menu->AddItem(new BMenuItem("Insert Record", new BMessage(GRID_INSERT_ROW_MSG)));
	menu->AddItem(new BMenuItem("Delete Record", new BMessage(GRID_DELETE_ROW_MSG)));
	menu->AddSeparatorItem();
	
	BMenuItem* cut, *copy, *paste;
	cut = new BMenuItem("Cut", new BMessage(), 'X');
	copy = new BMenuItem("Copy", new BMessage(B_COPY), 'C');
	paste = new BMenuItem("Paste", new BMessage(), 'V');
	menu->AddItem(cut);
	menu->AddItem(copy);
	menu->AddItem(paste);
	
	cut->SetEnabled(false);
	//copy->SetEnabled(false);
	paste->SetEnabled(false);
	
	BPoint point = fPointClicked;
	fView->ConvertToScreen(&point);
	menu->SetTargetForItems(fView);	

	BRect openRect;
	openRect.top = point.y - 5;
	openRect.bottom = point.y + 5;
	openRect.left = point.x - 5;
	openRect.right = point.x + 5;
	
	menu->Go(point, true, false, openRect);

	delete menu;
}


void
TableDesignRow::SaveColumnProperties()
{
	// Save the column name
	BString columnName;
	if (fCell[0])
		columnName.SetTo(fCell[0]->Text());
	else
		columnName.SetTo(*fText[0]);
	
	fColumnProps->SetName(columnName);

	TableWindow* window = (TableWindow*)fView->Window();
	ColumnDesignView* columnDesign = window->GetColumnDesignView();
	
	//Go through all the controls in the ColumnDesignView and based on their state
	//update the ColumnProperties structure.  Note that we don't save the PrimaryKey
	//settings b/c it gets saved by the TableWindow when it gets the BMessage from the
	//PrimaryKey button being pressed.
	fColumnProps->SetDefaultValue(columnDesign->DefaultValue());
	
	if (columnDesign->HasIndex())
		fColumnProps->SetIndex("create_index", columnDesign->UniqueIndex());
	else
		fColumnProps->RemoveIndex();		
	
	fColumnProps->SetRequired(columnDesign->Required());
	
	//To save the column type, just go to the PopUpMenu that's part of this class
	BMenuItem* item = fPopUpMenu->FindMarked();
	
	if (item)
		fColumnProps->SetType(item->Label());

	window->GetColumnDesignView()->DisplayColumnProperties(fView->IndexOf(this), true);
}


void
TableDesignRow::DeleteMenuBar()
{
  if (fView && fMenuBar)
  {
  	fView->RemoveChild(fMenuBar);
  	delete fMenuBar;
		fMenuBar = NULL;
  }
}

void
TableDesignRow::DrawIcon(BRect item_column_rect, BView* owner)
{
  item_column_rect.left += 2.0;
	item_column_rect.right = item_column_rect.left + 15.0;
	item_column_rect.top += 2.0;
	if(Height() > 20.0)
		item_column_rect.top += ceil((Height()-20.0)/2.0);
	item_column_rect.bottom = item_column_rect.top + 15.0;
	owner->SetDrawingMode(B_OP_OVER);
	owner->DrawBitmap(fIconList[fCurrentIcon], BRect(0.0,0.0,15.0,15.0),
	                  item_column_rect);
	owner->SetDrawingMode(B_OP_COPY);
}

BString
TableDesignRow::ToString()
{
	// Convert the contents of the row into a string using tabs to separate columns
	// and newline to indicate end of row
	BString text;
	text << fColumnProps->Name() << "\t" << fColumnProps->Type() << "\n";

	return text;
}
