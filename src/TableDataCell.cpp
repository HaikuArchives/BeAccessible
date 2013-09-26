#include <ClassInfo.h>
#include <Window.h>
#include "TableDataCell.h"
#include "Constants.h"
#include "GridRow.h"

TableDataCell::TableDataCell(BRect frame, const char* text, GridRow* row, 
int32 colNum) : GridCell(frame, text, row, colNum)
{

}


void
TableDataCell::MouseDown(BPoint where)
{
	if (!is_kind_of(fRow, GridRow))
		return;

	BMessage* message = Window()->CurrentMessage();
	if (message->FindInt32("buttons") == B_SECONDARY_MOUSE_BUTTON)
	{
		BPopUpMenu* menu = new BPopUpMenu("rightClick", false, false);
		BMenuItem *cut, *copy, *paste, *sortAsc, *sortDesc, 
		          *filterFor, *filterBy, *filterExcluding, *removeFilter;
	
		// Determine what to filter on (ie, what text is selected).  If no text is
		// selected, assume the entire contents of the cell is the filter, unless the 
		// cell is blank.  In that case, filter on null.
		int32 start, finish;
		GetSelection(&start, &finish);
		
		char charToEscape = '\'';
		
		BString filterByStr, filterExclStr;
		if (start != finish)
		{
			// Find the currently highlighted text; that will be the filter
			char* text = new char[finish-start+1];
			GetText(start, finish-start, text);
			
			// Escape single-quotes that are part of the filter text
			BString filterText(text);
			filterText.CharacterEscape(&charToEscape, '\'');
			
			filterByStr += "LIKE '%";
			filterByStr += filterText;
			filterByStr += "%'";
			
			filterExclStr.SetTo("NOT ");
			filterExclStr.Append(filterByStr);
			delete text;
		}
		else
		{
			if (strlen(Text()) == 0)
			{
				filterByStr += "is null";
				filterExclStr += "is not null";
			}
			else
			{
				// Escape single-quotes that are part of the filter text
				BString filterText(Text());
				filterText.CharacterEscape(&charToEscape, '\'');
			
				filterByStr += "'";
				filterByStr += filterText;
				filterByStr += "'";
				
				filterExclStr += "<> ";
				filterExclStr += filterByStr;

				filterByStr.Prepend("= ");
			}
					
		}
		
		BMessage* fBy = new BMessage(MSG_FILTER_BY);
		fBy->AddInt32("columnNumber", fColumnNumber);
		fBy->AddString("filter", filterByStr);

		BMessage* fExcluding = new BMessage(MSG_FILTER_BY);
		fExcluding->AddInt32("columnNumber", fColumnNumber);
		fExcluding->AddString("filter", filterExclStr);
		
		BMessage* fAskFor = new BMessage(MSG_ASK_FOR_FILTER);
		fAskFor->AddInt32("columnNumber", fColumnNumber);
		fAskFor->AddPoint("point", ConvertToScreen(Bounds().LeftTop()));
		
		filterBy = new BMenuItem("Filter By Selection", fBy);
		filterExcluding = new BMenuItem("Filter Excluding Selection", fExcluding);
		filterFor = new BMenuItem("Filter For...", fAskFor);  
		removeFilter = new BMenuItem("Remove Filter/Sort", 
		                             new BMessage(MSG_REMOVE_FILTER_SORT));
		menu->AddItem(filterBy);
		menu->AddItem(filterExcluding);
		menu->AddItem(filterFor);
		menu->AddItem(removeFilter);
		menu->AddSeparatorItem();

		BMessage* asc = new BMessage(MSG_SORT_ASCENDING);
		asc->AddInt32("columnNumber", fColumnNumber);
		sortAsc = new BMenuItem("Sort Ascending", asc);

		BMessage* desc = new BMessage(MSG_SORT_DESCENDING);
		desc->AddInt32("columnNumber", fColumnNumber);
		sortDesc = new BMenuItem("Sort Descending", desc);
		
		menu->AddItem(sortAsc);
		menu->AddItem(sortDesc);
		menu->AddSeparatorItem();
	
		cut = new BMenuItem("Cut", new BMessage());
		copy = new BMenuItem("Copy", new BMessage());
		paste = new BMenuItem("Paste", new BMessage());
		menu->AddItem(cut);
		menu->AddItem(copy);
		menu->AddItem(paste);
		
		cut->SetEnabled(false);
		copy->SetEnabled(false);
		paste->SetEnabled(false);
	
		if (fRow->IsLast())
		{
			filterBy->SetEnabled(false);
			filterExcluding->SetEnabled(false);
			filterFor->SetEnabled(false);
		}
		
		BPoint point = where;
		ConvertToScreen(&point);
		menu->SetTargetForItems(fRow->Parent());
		
		BRect openRect;
		openRect.top = point.y - 5;
		openRect.bottom = point.y + 5;
		openRect.left = point.x - 5;
		openRect.right = point.x + 5;
			
		menu->Go(point, true, false, openRect);
	
		delete menu;
	}
	
	GridCell::MouseDown(where);
}




