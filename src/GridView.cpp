#include <iostream>

#include <Clipboard.h>
#include <Window.h>

#include "GridConstants.h"
#include "GridIcons.h"
#include "GridRow.h"
#include "GridView.h"


GridView::GridView(BRect bounds, CLVContainerView** contView, const char* name)
 : ColumnListView(bounds, contView, name, B_FOLLOW_ALL_SIDES,
   	B_WILL_DRAW|B_FRAME_EVENTS|B_NAVIGABLE,
   	B_MULTIPLE_SELECTION_LIST,false,true,true,false,B_NO_BORDER)
{
	SetClickedItem(NULL);
	InitializeIcons();
	fCurrentColumn = 1;
//	fTempList = new BList();
//	fTableName = name;
}


void GridView::InitializeIcons()
{
	GridIconList[CURRENT_RECORD] = new PrefilledBitmap(BRect(0.0,0.0,15.0,15.0), 
                               B_COLOR_8_BIT,GridCurrentRecordIcon,false,false);

	GridIconList[NEW_RECORD] = new PrefilledBitmap(BRect(0.0,0.0,15.0,15.0),
                               B_COLOR_8_BIT,GridNewRecordIcon,false,false);

	GridIconList[EDIT_RECORD] = new PrefilledBitmap(BRect(0.0,0.0,15.0,15.0),
								B_COLOR_8_BIT,GridEditRecordIcon,false,false);

	GridIconList[PRIMARY_KEY] = new PrefilledBitmap(BRect(0.0,0.0,15.0,15.0),
								B_COLOR_8_BIT,PrimaryKey_Icon,false,false);

	GridIconList[SELECTED_PRIMARY_KEY] = new PrefilledBitmap(BRect(0.0,0.0,15.0,15.0),
								B_COLOR_8_BIT,SelectedPrimaryKey_Icon,false,false);	
	
	fIconList = GridIconList;							
}

void GridView::DeleteAllRows()
{
    // If the currently selected row has text boxes, make sure to clear them 
	int32 index = CurrentSelection(int32(0));
	GridRow* row = (GridRow*)ItemAt(index);
	if (row)
		row->Exit(false);

	SetClickedItem(NULL);

	BList temp;
	int32 count = CountItems();

	for(int i = 0; i < count; i++)
		temp.AddItem(ItemAt(i));
		
	MakeEmpty();
	
	GridRow* item;	
	while(!temp.IsEmpty())
	{
		item = (GridRow*)temp.RemoveItem(int32(0));
		delete item;
	}
}


void
GridView::AddLastRow()
{
	int32 numColumns = CountColumns() - 1;
	
	//Add last row for a new record
	AddItem(new GridRow(this,numColumns, NULL, GridIconList, &fCurrentColumn, true));
}

void
GridView::MessageReceived(BMessage* msg)
{
	switch(msg->what)
	{
		case B_COPY:
		{
			CopySelection();
			break;
		}
		
		case B_PASTE:
		{
			break;
		}
				
		case GRID_GOTO_ROW_MSG:
		{
			int32 rowNumber, column=0; //, direction;
			msg->FindInt32("rowNumber", &rowNumber);
			
			status_t found = msg->FindInt32("column", &column);
			if (found != B_OK)
			{
				// column was not in the BMessage, use the currently selected
				// row's column instead
 				column = fCurrentColumn;
			}
			
			GoToRow(rowNumber, column);
			break;
		}
		
		case GRID_GOTO_PREV_ROW_MSG:
		{
			int32 currentRow = CurrentSelection();
			if (currentRow >= 0)
				GoToRow(currentRow-1, fCurrentColumn);
			break;
		}
		
		case GRID_GOTO_NEXT_ROW_MSG:
		{
			int32 currentRow = CurrentSelection();
			if (currentRow >= 0)
				GoToRow(currentRow+1, fCurrentColumn);
			break;
		}
		
		case GRID_GOTO_LAST_ROW_MSG:
		{
			GoToRow(CountItems()-2, fCurrentColumn);
			break;
		}

		case GRID_GOTO_NEW_ROW_MSG:
		{
			GoToRow(CountItems()-1, fCurrentColumn);
			break;
		}

		
		case GRID_APPEND_ROW_MSG:
		{
			AddLastRow();

			int32 currentRow, column;
			bool makeFocus;
			
			msg->FindInt32("currentRow", &currentRow);
			msg->FindInt32("column", &column);
			msg->FindBool("makeFocus", &makeFocus);
	
			if (makeFocus)			
				GoToRow(currentRow+1, column);
			break;
		 }

		
		case GRID_INSERT_ROW_MSG:
		{
			break;
		}
		
		case GRID_DELETE_ROW_MSG:
		{
			break;
		}
		
		default:
		{
			BListView::MessageReceived(msg);
			break;
		}
	}
}


void 
GridView::KeyDown(const char* bytes, int32 numBytes)
{
	//cout << "GridView::KeyDown()" << endl;
	if (numBytes == 1)
	{
		switch (bytes[0])
		{
			case B_UP_ARROW:
			{
				int32 currentIndex = CurrentSelection();
				if (currentIndex < 0)
					break;

				BMessage msg(GRID_GOTO_ROW_MSG);
				msg.AddInt32("rowNumber", currentIndex-1);
				msg.AddInt32("column", int32(0));
				MessageReceived(&msg);
				break;
			}
			
			case B_DOWN_ARROW:
			{
				int32 currentIndex = CurrentSelection();
				if (currentIndex < 0)
					break;
					
				BMessage msg(GRID_GOTO_ROW_MSG);
				msg.AddInt32("rowNumber", currentIndex+1);
				msg.AddInt32("column", int32(0));
				MessageReceived(&msg);
				break;
			}
			
			case B_DELETE:
			{
				//BMessage msg(GRID_DELETE_ROW_MSG);
				//BWindow* window = Window();
				BMessenger(this).SendMessage(GRID_DELETE_ROW_MSG);
				break;
			}

			case B_HOME:
			{
				BMessage msg(GRID_GOTO_ROW_MSG);
				msg.AddInt32("rowNumber", int32(0));
				msg.AddInt32("column", int32(0));
				MessageReceived(&msg);
				break;
			}

			case B_END:
			{
				BMessage msg(GRID_GOTO_ROW_MSG);
				msg.AddInt32("rowNumber", CountItems()-1);
				msg.AddInt32("column", int32(0));
				MessageReceived(&msg);
				break;
			}
		
			default:
			{
				ColumnListView::KeyDown(bytes, numBytes);
				break;
			}
		}
	}
}


bool
GridView::GoToRow(int32 rowNumber, int32 column)
{
	int32 oldRowNumber = -1; 
		
	// Make sure the new rowNumber to go to is within bounds
	if (rowNumber < CountItems() && rowNumber >= 0)
	{
		if (clicked_item)
			oldRowNumber = IndexOf(clicked_item);

		if (rowNumber == oldRowNumber)
			return false;
				
		if (!DeselectAll())
			return false;

		clicked_item = (GridRow*)ItemAt(rowNumber);
		clicked_item->SetColumnClicked(column);		

		if (column == 0)
			GridSelect(HIGHLIGHTED, rowNumber);
		else
			GridSelect(TEXTBOX, rowNumber);

		AdjustVerticalScrollBar(rowNumber, oldRowNumber);
		return true;
	}
	else
	{
		return false;
	}
}


void
GridView::AdjustVerticalScrollBar(int32 rowNumber, int32 oldRowNumber)
{
	if (Window()->Lock())
	{
		BRect bounds = Bounds();
		GridRow* row = (GridRow*)ItemAt(rowNumber);
		BRect columnFrame = row->ColumnRect(row->CurrentColumn());

		if (rowNumber > oldRowNumber)
		{
			// Scrolling down
			bounds.bottom -= B_H_SCROLL_BAR_HEIGHT;
            if (!bounds.Contains(columnFrame.LeftBottom()))
			{
				ScrollToSelection();
				Invalidate();
			}
			else
			{
				ScrollToSelection();
			}
		}
		else
		{
			// Scrolling up
			bounds.top += 10;
			if (!bounds.Contains(columnFrame.LeftTop()))
			{
				ScrollToSelection();
				Invalidate();
			}
			else
			{
				ScrollToSelection();
			}
		}

		Window()->Unlock();
	}
}


int32
GridView::CountSelectedItems()
{
	int32 selected; 
	int32 i = 0;
	
	while ((selected = CurrentSelection(i)) >= 0)
	{
		i++;
	}
	
	return i;
}

//
//void
//GridView::SetTableProperties(TableProperties* props)
//{
//	tableProps = props;
//}
//
//TableProperties*
//GridView::GetTableProperties()
//{
//	return tableProps;
//}
//

void
GridView::SetClickedItem(GridRow* item)
{
	clicked_item = item;
}

bool
GridView::DeselectAll()
{
	int32 selection_index = 0;
	int32 selection_item_index;

	// Cycle through all the currently selected item and unhighlight them.
	while ((selection_item_index = CurrentSelection(selection_index)) >= 0)
	{
		GridRow* row = (GridRow*)BListView::ItemAt(selection_item_index);
		
		if (row->State() == TEXTBOX)
		{
			if(row->ExitRequested())
				row->Exit(false);
			else
				return false;
		}
			
		row->SetState(NORMAL);
		row->InvalidateRow();  // required... ensures the icon gets redrawn
		
		selection_index++;
	}

	BListView::DeselectAll();
	return true;
}


bool
GridView::InitiateDrag(BPoint point, int32 index, bool wasSelected)
{
	return true;
}

void GridView::MouseDown(BPoint where)
{
	//BListView::MouseDown gives undesirable behavior during click-and drag (it reselects instead of
	//doing nothing to allow for initiating drag & drop), so I need to replace the selection behavior,
	//and while I'm at it, I'll just do an InitiateDrag hook function.
	//Debug_Info(METHOD_ENTRY, "ColumnListView::MouseDown()", NULL);

	BMessage* message = Window()->CurrentMessage();
	int32 item_index = IndexOf(where);
	int32 modifier_keys = message->FindInt32("modifiers");
	//int32 clicks = message->FindInt32("clicks");

	//Ignore the Tertiary mouse button
	if(message->FindInt32("buttons") == B_TERTIARY_MOUSE_BUTTON)
	{
		return;
	}
	
	// Save the previously clicked item; to be used later.
	GridRow* oldClickedItem = clicked_item;
	
	if(item_index >= 0)
	{
		clicked_item = (GridRow*)BListView::ItemAt(item_index);
		clicked_item->SetColumnClicked(where);
		int32 column = clicked_item->CurrentColumn();

		// Handle secondary mouse click
		if (message->FindInt32("buttons") == B_SECONDARY_MOUSE_BUTTON)
		{
			if(!clicked_item->IsSelected())
			{
				if (!DeselectAll())
					return;

				if (column == 0)
					GridSelect(HIGHLIGHTED, item_index, false);
				else
					GridSelect(TEXTBOX, item_index, false);
			}
			else if (clicked_item->State() == TEXTBOX)
			{
				// Clear the selection, make sure to invalidate the row to
				// get rid of the text boxes
				if (clicked_item->ExitRequested())
					clicked_item->Exit(true);
				else
					return;

				if (column == 0)
					GridSelect(HIGHLIGHTED, item_index, false);
				else
					GridSelect(TEXTBOX, item_index, false);
			}
			clicked_item->SetColumnClicked(where);
			clicked_item->RightClickMenu();
			return;
		}
		

		// Handle primary mouse click.  (At this point, the tertiary and secondary 
		// mouse clicks have already been handled)
		
		if (oldClickedItem && (oldClickedItem->State() == TEXTBOX))
			oldClickedItem->Exit(true);
		
		if(!clicked_item->IsSelected())
		{
			//Clicked a new item...select it.
			if(modifier_keys & B_SHIFT_KEY)
			{
				//If shift held down, expand the selection to include all intervening items.
				int32 min_selection = 0x7FFFFFFF;
				int32 max_selection = -1;
				int32 selection_index = 0;
				int32 selection_item_index;
				while((selection_item_index = CurrentSelection(selection_index)) != -1)
				{
					if(min_selection > selection_item_index)
						min_selection = selection_item_index;
					if(max_selection < selection_item_index)
						max_selection = selection_item_index;
					selection_index++;
				}
				if(min_selection == 0x7FFFFFFF)
					//FIXME - not sure what the 0x7F's are all about/if the line below
					//is even correct
					GridSelect(HIGHLIGHTED, item_index,false);
				else
				{
					if(min_selection > item_index)
						min_selection = item_index;
					if(max_selection < item_index)
						max_selection = item_index;
						
					// Force all selected rows to be highlighted */
					for (int i = min_selection; i <= max_selection; i++)
					{
						GridRow* item = (GridRow*)BListView::ItemAt(i);
						item->SetState(HIGHLIGHTED);
					}

					GridSelect(min_selection,max_selection,false);
				}
			}
			else if(modifier_keys & B_OPTION_KEY)
			{
				int32 selection_index = 0;
				int32 selection_item_index;
				
				// Force all selected rows to turn on highlighting
				while ((selection_item_index = CurrentSelection(selection_index)) != -1)
				{
					GridRow* item = (GridRow*)BListView::ItemAt(selection_item_index);
					item->SetState(HIGHLIGHTED);
					
					selection_index++;
				}
			
				//If option held down, expand the selection to include just it.
				clicked_item->SetState(HIGHLIGHTED);
				GridSelect(HIGHLIGHTED, item_index,true);
			}
			else
			{
				//If neither key held down, select this item alone.
				
				// Force any previously selected rows to turn off highlighting
				DeselectAll();

				if (column == 0)
					GridSelect(HIGHLIGHTED, item_index,false);
				else
					GridSelect(TEXTBOX, item_index,false);

				AdjustVerticalScrollBar(item_index, IndexOf(oldClickedItem));
			}

			//Also watch for drag of the new selection
			fLastMouseDown = where;
			fNoKeyMouseDownItemIndex = -1;
			fWatchingForDrag = true;
		}
		else
		{
			//Clicked an already selected item...
			if(modifier_keys & B_OPTION_KEY)
			{
				// If option held down, deselect current row.  Also, force the 
				// row to remove highlighting
				clicked_item->SetState(NORMAL);
				Deselect(item_index);
			}
			else if(modifier_keys & B_SHIFT_KEY)
			{
				//If shift held down, ignore it and just watch for drag.
				fLastMouseDown = where;
				fNoKeyMouseDownItemIndex = -1;
				fWatchingForDrag = true;
			}
			else
			{
				//Remove highlighting from currently selected rows
				DeselectAll();
				clicked_item->SetColumnClicked(where);

				//Select this row alone
				if (column == 0)
					GridSelect(HIGHLIGHTED, item_index,false);
				else
					GridSelect(TEXTBOX, item_index,false);					
				
				
				//If neither key held down, watch for drag, but if no drag and just a click on
				//this item, select it alone at mouse up.
				fLastMouseDown = where;
				fNoKeyMouseDownItemIndex = item_index;
				fWatchingForDrag = true;
			}
		}
		if(fWatchingForDrag)
			SetMouseEventMask(B_POINTER_EVENTS,B_NO_POINTER_HISTORY);

	}
	else
	{
		//Clicked outside of any items.  If no shift or option key, deselect all.
		if((!(modifier_keys & B_SHIFT_KEY)) && (!(modifier_keys & B_OPTION_KEY)))
			DeselectAll();
	}
}


void
GridView::CopySelection()
{
	// Loop through all selected rows, convert each to string, and copy that string
	// to the clipboard.
	BMessage* clip = NULL;
	if (be_clipboard->Lock())
	{
		be_clipboard->Clear();
		if ((clip = be_clipboard->Data()))
		{
			BString text;
			int32 index, i=0;
			while ((index = CurrentSelection(i)) >= 0)
			{
				GridRow* row = (GridRow*)ItemAt(index);
				if (row)
				{
					// The last (aka new) row won't contain any text, so we shouldn't
					// bother copying it to the clipboard.
					if (!row->IsLast())
						text.Append(row->ToString());
				}
				i++;
			}

			clip->AddData("text/plain", B_MIME_TYPE, 
			               text.String(), text.Length());
			be_clipboard->Commit();

		}
		be_clipboard->Unlock();
	}
}

void
GridView::GridSelect(RowState state, int32 index, bool extend = false)
{
	GridRow* row = (GridRow*)ItemAt(index);
	if (row)
	{
		row->SetState(state);
		Select(index, extend);
	}
}


void 
GridView::GridSelect(int32 start, int32 finish, bool extend = false)
{
	// Force all selected rows to be highlighted
	for (int i = start; i <= finish; i++)
	{
		GridRow* item = (GridRow*)BListView::ItemAt(i);
		item->SetState(HIGHLIGHTED);
	}

	Select(start, finish, extend);
}

void
GridView::SelectAllRows()
{
	if (!DeselectAll())
		return;

	for(int i=0; i < CountItems(); i++)
	{
		GridRow* row = (GridRow*)ItemAt(i);
		if (row)
			row->SetState(HIGHLIGHTED);
	}

	Select(0,CountItems()-1);
	Invalidate();
}


float
GridView::GetWidth()
{
	float totalGridWidth = 0;
	float columnWidth = 0;
	
	for (int i = 0; i < CountColumns(); i++)
	{
		columnWidth = ColumnAt(i)->Width();	
		totalGridWidth += columnWidth;
	}
	
	return totalGridWidth;
}


//void GridView::MouseUp(BPoint where)
//{
//	if(fWatchingForDrag && fNoKeyMouseDownItemIndex != -1)
//	{
//
//	}
//
//	fNoKeyMouseDownItemIndex = -1;
//	fPreviousMouseMovedIndex = -1;
//	fWatchingForDrag = false;
//
//}
//
//
//void GridView::MouseMoved(BPoint where, uint32 code, const BMessage *message)
//{
//	
//	if (fWatchingForDrag && (code == B_INSIDE_VIEW))
//	{
//		int32 currentIndex = IndexOf(where);	
//		
//		if (currentIndex == fNoKeyMouseDownItemIndex)
//			return;
//		else if (currentIndex > fPreviousMouseMovedIndex)
//		{
//			GridRow* item = (GridRow*)BListView::ItemAt(currentIndex);
//			item->SetHighlightRow(true);
//			Select(currentIndex, true);
//		}
//		else if (currentIndex < fPreviousMouseMovedIndex)
//		{
//			GridRow* item = (GridRow*)BListView::ItemAt(fPreviousMouseMovedIndex);
//			item->SetHighlightRow(false);
//			item->Exit(true);
//			Deselect(fPreviousMouseMovedIndex);
//		}
//		
//		fPreviousMouseMovedIndex = currentIndex;
//	}
//	else
//	{
//		fWatchingForDrag = false;
//	}
//
//	if ((where.y > Bounds().bottom) && (code == B_EXITED_VIEW))
//	{
//		BPoint point = Bounds().LeftTop();
//		point.x++; point.y++;
//		ScrollTo(point);
//	}
//
//	if(fWatchingForDrag && (where.x<fLastMouseDown.x-4 || where.x>fLastMouseDown.x+4 ||
//		where.y<fLastMouseDown.y-4 || where.y>fLastMouseDown.y+4))
//	{
//		InitiateDrag(fLastMouseDown,IndexOf(fLastMouseDown),true);
//		fNoKeyMouseDownItemIndex = -1;
//		fWatchingForDrag = false;
//		SetMouseEventMask(0);
//	}
//
//}
//
//
//PasteParser::PasteParser(const char* pastedText)
//{
//	fText.SetTo(pastedText);
//	
//	 parse the string; create a new BString for each row; add it to the BList 
//}
//
//int32
//PastParser::CountColumns()
//{
//	int32
//}

