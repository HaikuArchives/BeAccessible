#include <Region.h>
#include <strings.h>
#include <Window.h>
#include <PopUpMenu.h>
#include <MenuItem.h>
#include <Alert.h>
#include <String.h>
#include <iostream.h>

#include "BeAccessibleApp.h"
#include "TableProperties.h"
#include "GridView.h"
#include "GridRow.h"
#include "GridCell.h"
#include "GridConstants.h"
#include "NewStrings.h"
#include "Constants.h"


GridRow::GridRow(GridView* view, int32 numArgs, BString** args, 
                 PrefilledBitmap** IconList, int32* currentColumn, bool last = false)
: CLVListItem(0, false, false, 20.0)
{
	BString methodName("GridRow::GridRow");
	
	fView = view;
	fIconList = IconList;
	fLast = last;
	fState = NORMAL;
	fHighlightText = false;
	fModified = false;
	fPointClicked = BPoint(-1, -1);
	fCurrentColumn = currentColumn;
	fTextOffset = 15.0;
	
	if (fLast)
	{
		fCurrentIcon = NEW_RECORD;
	}
	else
	{
		fCurrentIcon = CURRENT_RECORD;
	}

	// Create a cached BRect for the column of icons.
  fCachedRects.AddItem(new BRect(-1,-1,-1,-1));

	InitializeCache(numArgs, args);
}


void
GridRow::InitializeCache(int32 numArgs, BString** args)
{
	fNumColumns = numArgs + 1;   //Include column of icons
	
	if (args)
		fText = args;
	else
	{
		fText = new BString*[numArgs];
		for(int i = 0; i < numArgs; i++)
		{
				//no text was provided, use the empty string
		  	fText[i] = new BString("");
		}
	}
	
	fCell = new GridCell *[fNumColumns-1];

	// Initialize the GridCells and BRect cache
	for(int i = 0; i < numArgs; i++)
	{
		fCell[i] = NULL;
    fCachedRects.AddItem(new BRect(-1,-1,-1,-1));
	}

}


GridRow::~GridRow()
{
	for(int i = 0; i < fNumColumns-1; i++)
	{
		if (fText[i])
		  delete fText[i];
	}
	
	delete[] fText;
}


//Note... This function is never actually called... only the child's version is.
//
void GridRow::DrawItemColumn(BView* owner, BRect item_column_rect, int32 column_index, 
                             bool complete)
{
//	BRegion Region;
//	Region.Include(item_column_rect);
//	owner->ConstrainClippingRegion(&Region);
//
//	// Draw the bottom and right lines
//	owner->StrokeLine(item_column_rect.LeftBottom(), item_column_rect.RightBottom(), B_MIXED_COLORS);
//  if (column_index >= 0)
//		owner->StrokeLine(item_column_rect.RightBottom(), item_column_rect.RightTop(), B_MIXED_COLORS);
//
//	// Cache the new size of the cell's BRect
//	BRect* cached_rect = (BRect*)fCachedRects.ItemAt(column_index);
//	if(cached_rect != NULL)
//		*cached_rect = item_column_rect;
//
//
//	if (fState == NORMAL)
//	{
//		if (column_index == 0 && fLast)
//		{
//			DrawIcon(owner, item_column_rect);
//		}
//		else if (column_index > 0)
//		{
//			DrawText(owner, item_column_rect, column_index);
//		}
//			
//	}
//	else if (fState == TEXTBOX)
//	{
//		if (column_index == 0)
//			DrawIcon(owner, item_column_rect);
//		else
//			DrawTextBoxes(owner, item_column_rect, column_index);
//	}
//	else if (fState == HIGHLIGHTED || complete)
//	{
//		owner->SetHighColor(BeListSelectGrey);	
//		owner->SetHighColor(BeListSelectGrey);
//		owner->FillRect(item_column_rect);
//		
//		if (column_index > 0)
//			DrawText(owner, item_column_rect, column_index);
//	}
//
//	owner->ConstrainClippingRegion(NULL);
}


void GridRow::ColumnWidthChanged(int32 column_index, float column_width, ColumnListView *the_view)
{
	BRect* cached_rect = (BRect*)fCachedRects.ItemAt(column_index);
	if(cached_rect == NULL || *cached_rect == BRect(-1,-1,-1,-1))
		return;

	//Determine amount column width has changed	
	float width_delta = column_width-(cached_rect->right-cached_rect->left);		
	cached_rect->right += width_delta;

	//Resize cached rectangles
	int num_columns = fCachedRects.CountItems();
	for(int column = 0; column < num_columns; column++)
	{
		if(column != column_index)
		{
			BRect* other_rect = (BRect*)fCachedRects.ItemAt(column);
			if(other_rect->left > cached_rect->left)
			{
					other_rect->OffsetBy(width_delta,0);
			}
		}
	}

	if (!cached_rect->Intersects(the_view->Bounds()))
		return;

	BRect invalidRect = *cached_rect;
	invalidRect.bottom -= 1;

	// Redraw the cell's right vertical gridline.
	if (width_delta > 0)
	{
		invalidRect.left = cached_rect->right - width_delta;
	}
	else
	{
		invalidRect.left = cached_rect->right;
		invalidRect.right = cached_rect->right;
	}
	
	the_view->Invalidate(invalidRect);
	
	
	// Redraw the row's bottom horizontal gridline
	if(width_delta < 0)
	{
		BRect rect = GetRowSize();
		rect.left = cached_rect->right + width_delta;
		rect.top = rect.bottom;
		the_view->Invalidate(rect);
	}
}


bool GridRow::ExitRequested()
{
	if (State() == TEXTBOX && Modified())
		return SaveChanges();
	else
		return true;
}


void GridRow::Exit(bool doInvalidate)
{
	for (int i = 0; i < fNumColumns-1; i++)
	{
		if (fCell[i])
		{
			//Remove the BTextView from the view so we can delete it
			fView->RemoveChild(fCell[i]);

			delete fCell[i];
			fCell[i] = NULL;
		}
	}
	
	if (fLast)
	{
		AddRow(1, false);
		fCurrentIcon = NEW_RECORD;
	}
	else
		fCurrentIcon = CURRENT_RECORD;

	SetState(NORMAL);
	
	
  if (doInvalidate)
		InvalidateRow();	
}


void GridRow::PrintToStream()
{
//		cout << "(";
//		
//		for(int i = 0; i < fNumColumns-1; i++)
//		{
//			if (i == fNumColumns-1)
//				cout << fText[i]->String();
//			else
//				cout << fText[i]->String() << ", ";
//		}
//
//		cout << "): ";
}



void GridRow::PreviousField(int32 currentColumn)
{
	// If the current column is the first field, then scroll to the last field
	// of the previous row
	if (currentColumn == 1)
	{
			float gridWidth = GetRowSize().Width();
			BRect bounds = fView->Bounds();

			// If there are off-screen columns, scroll to the last column, provided 
			// this isn't the first row
			if (gridWidth > bounds.Width() && fView->IndexOf(this) != 0)
			{
				BPoint newPoint(gridWidth-bounds.Width(), bounds.top);
				fView->ScrollTo(newPoint);
				fView->Invalidate();
			}
			
			PreviousRow(fNumColumns-1);
	}
	else
	{
		// If the previous field isn't completely within Bounds(), then we need to adjust
		// the scrollbars so that it is.
		float prevColumnWidth = fView->ColumnAt(currentColumn-1)->Width();
		BRect* cachedRect = (BRect*)fCachedRects.ItemAt(currentColumn);
		
		BPoint prevColRightBottom(cachedRect->LeftBottom().x - prevColumnWidth,
		                          cachedRect->LeftBottom().y);

		BRect bounds = fView->Bounds();

		if (!bounds.Contains(prevColRightBottom))
		{
			BPoint leftTop = bounds.LeftTop();

			float x_delta = leftTop.x - cachedRect->LeftBottom().x + prevColumnWidth;

			// If the previous field is the first column in the table, then move the
			// scrollbars an extra amount so the icon column is displayed.
			if (currentColumn == 2)
				x_delta += fView->ColumnAt(0)->Width();

			BPoint scrollPoint = BPoint(leftTop.x - 2.0 - x_delta, leftTop.y);
			fView->ScrollTo(scrollPoint);
			fView->Invalidate();
		}

		SetColumnClicked(currentColumn - 1);
		
		if (fCell[currentColumn-2])
		{
			fCell[currentColumn-2]->MakeFocus(true);
			fCell[currentColumn-2]->SelectAll();
		}
		else
		{
			// The textbox for the prev field hasn't been drawn yet.  So, we'll remove the
			// focus for the current field so that the prev field will have the focus when
			// it's drawn.
			fCell[currentColumn-1]->MakeFocus(false);
		}
	}

}


void GridRow::NextField(int32 currentColumn)
{
	// If the current column is the last field in the row, then scroll to the first
	// field in the next row */
	if (currentColumn == fNumColumns-1)
	{
			// Scroll to the first column, provided this isn't the last row, and we're
			// currently in the last column
			if (!fLast)
			{
					fView->ScrollTo(BPoint(0, fView->Bounds().top));
			}
			NextRow(1);
	}
	else
	{
		// If the next field isn't completely within the Bounds(), then we need to adjust
		// the scrollbars.
		float nextColumnWidth = fView->ColumnAt(currentColumn+1)->Width();
		BRect* cachedRect = (BRect*)fCachedRects.ItemAt(currentColumn);
		
		BPoint nextColRightBottom(cachedRect->RightBottom().x+nextColumnWidth,
		                          cachedRect->RightBottom().y);

		BRect bounds = fView->Bounds();

		if (!bounds.Contains(nextColRightBottom))
		{
			BPoint rightBottom = bounds.RightBottom();
			BPoint leftTop = bounds.LeftTop();

			float x_delta = cachedRect->RightBottom().x + nextColumnWidth - rightBottom.x;

			BPoint scrollPoint = BPoint(leftTop.x + 1.0 + x_delta, leftTop.y);
			fView->ScrollTo(scrollPoint);
			fView->Invalidate();
		}

		SetColumnClicked(currentColumn + 1);
		
		if (fCell[currentColumn])
		{
			fCell[currentColumn]->MakeFocus(true);
			fCell[currentColumn]->SelectAll();
		}
		else
		{
			// The textbox for the next field hasn't been drawn yet.  So, we'll remove the
			// focus for the current field so that the next field will have the focus when
			// it's drawn.
			fCell[currentColumn-1]->MakeFocus(false);
		}

	}

}


void GridRow::NextRow(int32 column)
{
	BString methodName("GridRow::NextRow()");
	
	if (ExitRequested())
		Exit(false);
	else
		return;
	
	// If this is not the last row, then scroll to the next one
	if (!fLast)
	{
		int32 currentIndex = fView->IndexOf(this);
		
		// If the next row exists, inform the Window to move to the next row 
		if (currentIndex + 1 < fView->CountItems())
		{
			BMessage nextRowMsg(GRID_GOTO_ROW_MSG);
			nextRowMsg.AddInt32("rowNumber", currentIndex + 1);
			nextRowMsg.AddInt32("column", column);
			nextRowMsg.AddInt32("direction", int32(1));
			BMessenger(fView).SendMessage(&nextRowMsg);
		}
	}
}



void GridRow::PreviousRow(int32 column)
{
	BString methodName("GridRow::PreviousRow()");
	
	if (ExitRequested())
		Exit(false);
	else
		return;
		
	int32 currentIndex = fView->IndexOf(this);
	
	// Make sure the previous row exists, then inform Window to move to it.
	if (currentIndex > 0)
	{
		BMessage nextRowMsg(GRID_GOTO_ROW_MSG);
		nextRowMsg.AddInt32("rowNumber", currentIndex - 1);
		nextRowMsg.AddInt32("column", column);
		nextRowMsg.AddInt32("direction", int32(-1));
		BMessenger(fView).SendMessage(&nextRowMsg, fView);
	}
}


bool GridRow::SaveChanges()
{
	for (int i = 0; i < fNumColumns-1; i++)
	{
		if (fCell[i])
		{
			//Create the new value based on the BTextView contents
			fText[i]->SetTo(fCell[i]->Text());
		}
	}

	return true;
}


void GridRow::InvalidateRow()
{
	fView->InvalidateItem(fView->IndexOf(this));
}


void
GridRow::AddRow(int32 column, bool makeFocus = true)
{
		if(!fModified)
		{
			fCurrentIcon = NEW_RECORD;
			//This last row has to be modified before a new row can be created.
			return;
		}
		fModified = false;
	
		// This row will no longer be the last one.  Change icon and flag accordingly.
		fCurrentIcon = CURRENT_RECORD;
		fLast = false;
	
		BMessage addRowMsg(GRID_APPEND_ROW_MSG);
		addRowMsg.AddInt32("currentRow", fView->IndexOf(this));		
		addRowMsg.AddInt32("column", column);
		addRowMsg.AddBool("makeFocus", makeFocus);
			
		BMessenger(fView).SendMessage(&addRowMsg, fView);
}



BRect GridRow::GetRowSize()
{
	// top, left, bottom are from the first column
	float left = ((BRect*)fCachedRects.ItemAt(0))->left;
	float top = ((BRect*)fCachedRects.ItemAt(0))->top;
	float bottom = ((BRect*)fCachedRects.ItemAt(0))->bottom;

	// right is from the Bounds
	float right = fView->Bounds().right;
	
	return (BRect(left, top, right, bottom));
}



void
GridRow::RightClickMenu()
{
	if (fState != HIGHLIGHTED)
	{
		return;
	}
	
	BPopUpMenu* menu = new BPopUpMenu("rightClick", false, false);
	BMenuItem *cut, *copy, *paste, *newRecord, *deleteRecord;

	BMessage* gotoRow = new BMessage(GRID_GOTO_ROW_MSG);
	gotoRow->AddInt32("rowNumber", fView->CountItems()-1);
	gotoRow->AddInt32("column", int32(1));
	newRecord = new BMenuItem("New Record", gotoRow);
	
	deleteRecord = new BMenuItem("Delete Record", new BMessage(GRID_DELETE_ROW_MSG));
	menu->AddItem(newRecord);
	menu->AddItem(deleteRecord);
	menu->AddSeparatorItem();
	
	cut = new BMenuItem("Cut", new BMessage(), 'X');
	copy = new BMenuItem("Copy", new BMessage(B_COPY), 'C');
	paste = new BMenuItem("Paste", new BMessage(), 'V');
	menu->AddItem(cut);
	menu->AddItem(copy);
	menu->AddItem(paste);
	menu->SetTargetForItems(fView);
	
	cut->SetEnabled(false);
	//copy->SetEnabled(false);
	paste->SetEnabled(false);

	if (fLast && (fView->CountSelectedItems() == 1))
	{
	  //if this is the last and only selected item, don't display the new and delete items
		newRecord->SetEnabled(false);
		deleteRecord->SetEnabled(false);

		cut->SetEnabled(false);
		copy->SetEnabled(false);

	}
	
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


void GridRow::Restore()
{
	if (fState != TEXTBOX)
		return;

	for (int i = 0; i < fNumColumns-1; i++)
	{
		if (fCell[i])
		{
			fCell[i]->SetText(fText[i]->String(), fText[i]->Length());
			
			fCell[i]->Modified(false);
		}
	}

	fModified = false;
	
	if (fLast)
		fCurrentIcon = NEW_RECORD;
	else
		fCurrentIcon = CURRENT_RECORD;
	
	// Redraw the icon
	fView->Invalidate(*((BRect*)fCachedRects.ItemAt(0)));
}


void GridRow::SetModified(bool status)
{
	fModified = status;
	
	if (fModified)
		SetIcon(EDIT_RECORD);
	
	InvalidateRow();
}


bool GridRow::Modified()
{
	return fModified;
}

void GridRow::SetIcon(int icon)
{
	fCurrentIcon = icon;
}

void GridRow::SetColumnClicked(int32 column)
{
	*fCurrentColumn = column;
	
	if (column != 0)
		fHighlightText = true;
}

void GridRow::SetColumnClicked(BPoint where)
{
	BRect* columnRect;

	for (int i = 0; i < fCachedRects.CountItems(); i++)
	{
		columnRect = (BRect*) fCachedRects.ItemAt(i);
		
		if (columnRect->Contains(where))
		{
			*fCurrentColumn = i;
		}
		
	}//End For loop

	fPointClicked = where;
}


bool GridRow::IsLast()
{
  return fLast;
}


void GridRow::SetLast(bool last)
{
	fLast = last;
	
	if (fLast)
		SetIcon(NEW_RECORD);
	
	InvalidateRow();
}


const char*
GridRow::GetContentAt(int32 column)
{
	if (column >= 0 && column < fNumColumns-1)
	{
		if (fState == TEXTBOX)
		{
			// Get the text from the GridCell, instead of from the text cache, since the
			// GridCell may have text that has changed and not been saved in the cache
			   return fCell[column]->Text();
		}
		else
		{
			// No GridCells exist for this row, so grab the text out of the cache.
				return fText[column]->String();
		}
	}
	else
	{
		return NULL;
	}
}


int32
GridRow::CurrentColumn()
{
	return *fCurrentColumn;
}


BString
GridRow::ToString()
{
	// Convert the contents of the row into a string using tabs to separate columns
	// and newline to indicate end of row
	BString text;
	for(int i=0; i<fNumColumns-1; i++)
	{
		text.Append(*fText[i]);

		if (i == fNumColumns-2)  //last column, add newline
			text.Append('\n', 1);
		else										 //there are other columns, add tab
			text.Append('\t', 1);
	}
	return text;
}

BRect
GridRow::ColumnRect(int column)
{
	BRect* currentRect = (BRect*)fCachedRects.ItemAt(column);
	return *currentRect;
}


void
GridRow::SetState(RowState newState)
{
	fState = newState;
}

RowState
GridRow::State()
{
	return fState;
}


GridView*
GridRow::Parent()
{
	return fView;
}
