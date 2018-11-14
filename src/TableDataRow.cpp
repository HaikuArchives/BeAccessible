#include <Region.h>
#include <strings.h>
#include <Window.h>
#include <PopUpMenu.h>
#include <MenuItem.h>
#include <String.h>
#include <iostream>
#include <Alert.h>

#include "BeAccessibleApp.h"
#include "TableProperties.h"
#include "ColumnProperties.h"
#include "TableDataRow.h"
#include "TableDataCell.h"
#include "Constants.h"
#include "NewStrings.h"
#include "GridConstants.h"
#include "SQLiteManager.h"

extern SQLiteManager* GlobalSQLMgr;


TableDataRow::TableDataRow(GridView* view, int32 numArgs, BString** args, 
                 PrefilledBitmap** IconList, BString rowId, int32* currentColumn, 
                 bool last = false)
: GridRow(view, numArgs, args, IconList, currentColumn, last)
{
	BString methodName("TableDataRow::TableDataRow");

	fRowID.SetTo(rowId);
}



void TableDataRow::DrawItemColumn(BView* owner, BRect item_column_rect, int32 column_index, 
                             bool complete)
{

//	BRegion Region;
//	Region.Include(item_column_rect);
//	owner->ConstrainClippingRegion(&Region);
//
//	// Cache the new size of the cell's BRect
//	BRect* cached_rect = (BRect*)fCachedRects.ItemAt(column_index);
//	if(cached_rect != NULL)
//		*cached_rect = item_column_rect;
//
//
//	if (fState == NORMAL)
//	{
//		owner->SetLowColor(White);
//		owner->SetHighColor(Black);
//			
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
//		else if (column_index > 0)
//			DrawTextBoxes(owner, item_column_rect, column_index);
//	}
//	else if (fState == HIGHLIGHTED || complete)
//	{
//		owner->MakeFocus(true);
//		
//		owner->SetLowColor(BeListSelectGrey);
//		owner->SetHighColor(BeListSelectGrey);
//		owner->FillRect(item_column_rect);
//		owner->SetHighColor(Black);	
//
//		if (column_index > 0)
//			DrawText(owner, item_column_rect, column_index);
//	}
//
//	// Draw the bottom and right lines 
//	owner->StrokeLine(item_column_rect.LeftBottom(), item_column_rect.RightBottom(), B_MIXED_COLORS);
//  if (column_index >= 0)
//		owner->StrokeLine(item_column_rect.RightBottom(), item_column_rect.RightTop(), B_MIXED_COLORS);
//
//	owner->ConstrainClippingRegion(NULL);
//}
//

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

	if(column_index == 0 && (fState != HIGHLIGHTED) && (selected || fLast))
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
	else if(column_index >= 1 && selected && (fState != HIGHLIGHTED))
	{
		item_column_rect.InsetBy(2.0,2.0);
		if (!fCell[column_index-1])
		{
				fCell[column_index-1] = new TableDataCell(item_column_rect, 
				                                          fText[column_index-1]->String(), 
				                                          this, column_index);
				
				owner->AddChild(fCell[column_index-1]);

				//Set the cell's text to (AutoNumber) if this field is an AutoNumber and this
				//cell is the last cell.
				TableDataView* tdView = (TableDataView*)fView;
				if (fLast && 
				    tdView->GetTableProperties()->GetColumnProperties(column_index-1)->AutoNumber())
				{
					fCell[column_index-1]->SetText("(AutoNumber)");
					//The above line will cause fModified = true.  Undo that.
					fCell[column_index-1]->Modified(false);
					fModified = false;
					SetIcon(NEW_RECORD);
					InvalidateRow();
				}

				if (*fCurrentColumn == column_index)
				{
					fCell[column_index-1]->MakeFocus(true);
					if (fHighlightText)
					{
						//highlight all the text
						fCell[column_index-1]->SelectAll();
						fHighlightText = false;
					}
					else
					{
						if (fPointClicked != BPoint(-1,-1))
						{
							//place the cursor at the point the cell was clicked
							fCell[column_index-1]->ConvertFromParent(&fPointClicked);
							int32 offset = fCell[column_index-1]->OffsetAt(fPointClicked);
							fCell[column_index-1]->Select(offset, offset);
						}
						else
						{
							//the cell wasn't clicked anywhere, so set the cursor at the beginning
							//of the cell.
							fCell[column_index-1]->Select(0,0);
						}
					}
				}
		}
//  well, this is a start.  This code resizes the grid cells when sizing a column,
//    but it still leaves a lot to be desired.
//		else
//		{
//			//textbox already exists...
//			BRect oldFrame = fCell[column_index-1]->Frame();
//			if (cached_rect)
//			{
//				float x = cached_rect->left - oldFrame.left +1;
//				fCell[column_index-1]->MoveBy(x, 0.0);
//				
//			}
//		}

	}
	else if(column_index >= 1)
	{
		// If this field is an AutoNumber and this row is the last row, display the
		//   text as (AutoNumber) 
		TableProperties* tableProps = ((TableDataView*)fView)->GetTableProperties();
		bool isAutoNumber = tableProps->GetColumnProperties(column_index-1)->AutoNumber();
		if (fLast && isAutoNumber)
		{
			owner->DrawString("(AutoNumber)",
				BPoint(item_column_rect.left+4.0,item_column_rect.top+fTextOffset));
		}
		// ... otherwise display whatever text we've got. 
		else
		{
			owner->DrawString(fText[column_index-1]->String(),
				BPoint(item_column_rect.left+4.0,item_column_rect.top+fTextOffset));
		}
	}
	
	owner->ConstrainClippingRegion(NULL);
}


/*
void
TableDataRow::DrawText(BView* owner, BRect item_column_rect, int32 column_index)
{
	// If this field is an AutoNumber and this row is the last row, display the
	// text as (AutoNumber)
	TableProperties* tableProps = ((TableDataView*)fView)->GetTableProperties();
	bool isAutoNumber = tableProps->GetColumnProperties(column_index-1)->AutoNumber();
	if (fLast && isAutoNumber)
	{
		owner->DrawString("(AutoNumber)",
			BPoint(item_column_rect.left+4.0,item_column_rect.top+fTextOffset));
	}
	// ... otherwise display whatever text we've got.
	else
	{
		owner->DrawString(fText[column_index-1]->String(),
			BPoint(item_column_rect.left+3.0,item_column_rect.top+fTextOffset));
	}
}

void
TableDataRow::DrawTextBoxes(BView* owner, BRect item_column_rect, int32 column_index)
{
		item_column_rect.InsetBy(2.0,2.0);
		if (!fCell[column_index-1])
		{
				fCell[column_index-1] = new TableDataCell(item_column_rect, 
				                                          fText[column_index-1]->String(), 
				                                          this, column_index);
				
				owner->AddChild(fCell[column_index-1]);

				//Set the cell's text to (AutoNumber) if this field is an AutoNumber and this
				//cell is the last cell.
				TableDataView* tdView = (TableDataView*)fView;
				if (fLast && 
				    tdView->GetTableProperties()->GetColumnProperties(column_index-1)->AutoNumber())
				{
					fCell[column_index-1]->SetText("(AutoNumber)");
					//The above line will cause fModified = true.  Undo that.
					fCell[column_index-1]->fModified = false;
					fModified = false;
					SetIcon(NEW_RECORD);
					InvalidateRow();
				}

				if (*fCurrentColumn == column_index)
				{
					fCell[column_index-1]->MakeFocus(true);
					if (fHighlightText)
					{
						//highlight all the text
						fCell[column_index-1]->SelectAll();
						fHighlightText = false;
					}
					else
					{
						if (fPointClicked != BPoint(-1,-1))
						{
							//place the cursor at the point the cell was clicked
							fCell[column_index-1]->ConvertFromParent(&fPointClicked);
							int32 offset = fCell[column_index-1]->OffsetAt(fPointClicked);
							fCell[column_index-1]->Select(offset, offset);
						}
						else
						{
							//the cell wasn't clicked anywhere, so set the cursor at the beginning
							//of the cell.
							fCell[column_index-1]->Select(0,0);
						}
					}
				}
			
				//fHasTextBox = true;
		}
}
*/



bool TableDataRow::SaveChanges()
{
	BString columns, values, sql, table;
	int32 numModifiedColumns = 0, autoNumberColumn = -1;
	table = fView->Name();
	char charToEscape = '\'';	  // Need to escape any single quotes
	
	if (fLast)
	{
		// Find all the TableDataCells that have been modified
		for(int i = 0; i < fNumColumns-1; i++)
		{
			if (!fCell[i]) continue;
		
			// Well... this is a bit of a hack.  If this column is an AutoNumber column,
			// then flag it as modified so it will be processed in the block below.
			if (strcmp(fCell[i]->Text(), "(AutoNumber)") == 0)
				fCell[i]->Modified(true);
		
			if (fCell[i]->Modified())
			{
				if (numModifiedColumns != 0)
				{
					columns += ", ";
					values += ", ";
				}

				// Find the name of the column
				columns += "[";
				columns += fView->ColumnAt(i+1)->GetLabel();
				columns += "]";
				
				// Get the value to insert.  If we've got an AutoNumber field on the last
				// row and the text is (AutoNumber) or null, then use null in the SQL to autoincrement
				TableDataView* tdView = (TableDataView*)fView;
				if (fLast && tdView->GetTableProperties()->GetColumnProperties(i)->AutoNumber() 
				    && ((strcmp(fCell[i]->Text(), "(AutoNumber)") == 0) ||
				        (strlen(fCell[i]->Text()) == 0)))
				{
					values += "null";
					autoNumberColumn = i;
				}
				else
				{
					BString value(fCell[i]->Text());
					// If a field is blank, insert null instead of an empty string
					if (value.Length() > 0)
					{
						// Escape any single-quotes there might be.
						value.CharacterEscape(&charToEscape, '\'');
						
						values += "'"; 
						values += value; 
						values += "'";
					}
					else
					{
					  values += "null";
					}
				}

				numModifiedColumns++;
			}	
		}

		// Construct the insert sql
		sql += "INSERT INTO [";
		sql +=  table << "] (";
		sql +=  columns << ") VALUES (";
		sql +=  values;
		sql +=  "); ";
		
		bool sqlSuccess = GlobalSQLMgr->Execute(sql.String());

		if (!sqlSuccess)
		{
			return false;
		}
		else
		{
			// Find the rowid of the newly inserted row.
			fRowID << GlobalSQLMgr->GetLastInsertRowId();
			
			if (autoNumberColumn != -1)
				fCell[autoNumberColumn]->SetText(fRowID.String());
			
			GridRow::SaveChanges();
			return true;
		}
	}
	else
	{
		// Construct the update sql
		sql = "UPDATE [";
		sql += table << "] SET ";
		
		for (int i = 0; i < fNumColumns-1; i++)
		{
			if (!fCell[i]) continue;
		
			if (fCell[i]->Modified())
			{
				if (numModifiedColumns != 0)
				{
					sql += ", ";
				}
				
				sql += "[";
				sql += fView->ColumnAt(i+1)->GetLabel();
				sql += "] = ";
				
				// Get the value to update
				BString value(fCell[i]->Text());
	
				if (value.Length() > 0)
				{
					// Escape any single-quotes there might be.
					value.CharacterEscape(&charToEscape, '\'');
					
					sql += "'"; 
					sql += value; 
					sql += "'";
				}
				else
				{
					// If a field is blank, insert null instead of an empty string
				  sql += "null";
				}
				
				numModifiedColumns++;
			}
		}
		
		sql += " WHERE ROWID = ";
		sql << fRowID << ";";
		
		bool sqlSuccess = GlobalSQLMgr->Execute(sql.String());

		if (!sqlSuccess)
		{
			return false;
		}
		else
		{
			fModified = false;
			GridRow::SaveChanges();
			return true;
		}
	}
}



void TableDataRow::Restore()
{
	if (fState != TEXTBOX)
		return;

	for (int i = 0; i < fNumColumns-1; i++)
	{
		if (fCell[i])
		{
			//Is this an AutoNumber field and the last row?  If so, you know what to do!
			TableDataView* tdView = (TableDataView*)fView;
			if (fLast && tdView->GetTableProperties()->GetColumnProperties(i)->AutoNumber())
			{
				fCell[i]->SetText("(AutoNumber)");
				// The above line causes fModified = true.  Undo that.
				fModified = false;
				fCell[i]->Modified(false);
				SetIcon(NEW_RECORD);
				InvalidateRow();
			}
			else	
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

BString TableDataRow::GetRowID()
{
	return fRowID;
}

