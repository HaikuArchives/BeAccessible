#include <Window.h>
#include <InterfaceDefs.h>
#include <iostream>
#include <TextControl.h>

#include "Constants.h"
#include "GridCell.h"
#include "GridRow.h"
#include "GridConstants.h"

#define TEXT_INSET 1.0


GridCell::GridCell(BRect frame, const char* text, GridRow* row, int32 colNum)
: BTextView(frame, NULL, BRect(TEXT_INSET,TEXT_INSET,
  frame.Width() - TEXT_INSET, frame.Height() - TEXT_INSET), 0, 
  B_WILL_DRAW|B_NAVIGABLE)
{
	fRow = row;    
	fColumnNumber = colNum;
	fInitializeText = true;
	fModified = false;
	
	MakeResizable(true);
	SetText(text, strlen(text));

	fInitializeText = false;

	//SetText causes InsertText to be called.  InsertText will set the modified
	//flags to true.  We only want those flags to be true when the users changes
	//something, so we'll set the flags back to false here.
	//fModified = false;
	//fRow->fModified = false;
}


void
GridCell::InsertText(const char* text, int32 length, int32 offset,
                     const text_run_array* runs)
{
	if(strlen(text) && !fModified && !fInitializeText)
	{
		fModified = true;
		fRow->SetModified(true);
	}
	
	BTextView::InsertText(text, length, offset, runs);
}


void
GridCell::KeyDown(const char* bytes, int32 numBytes)
{

	BMessage* msg = Window()->CurrentMessage();
	int32 modifiers = 0;
	msg->FindInt32("modifiers", &modifiers);
	
	int32 start, finish, length;
	GetSelection(&start, &finish);
	length = strlen(Text());

	
	if (bytes[0] == B_LEFT_ARROW && (modifiers & (B_CONTROL_KEY|B_OPTION_KEY)))
	{
		GridView* view = fRow->Parent();
		GridRow* row = (GridRow*)view->ItemAt(view->IndexOf(fRow));
		if (row)
		{
			const char* textToCopyOver = row->GetContentAt(fColumnNumber);
			if (textToCopyOver)
			{
				SetText(textToCopyOver);
				SelectAll();
			}
		}
		return;
	}
	else if (bytes[0] == B_RIGHT_ARROW && (modifiers & (B_CONTROL_KEY|B_OPTION_KEY)))
	{
		GridView* view = fRow->Parent();
		GridRow* row = (GridRow*)view->ItemAt(view->IndexOf(fRow));
		if (row)
		{
			const char* textToCopyOver = row->GetContentAt(fColumnNumber-2);
			if (textToCopyOver)
			{
				SetText(textToCopyOver);
				SelectAll();
			}
		}
		return;
	}
	//Move to the previous field if Shift-Tab is pressed, or if the left
	//arrow is pressed when either text in the current field is highlighted
	//or the insertion point is at the beginning of the current field.
	else if (((modifiers & B_SHIFT_KEY) && bytes[0] == '\t') ||
	     ((bytes[0] == B_LEFT_ARROW) && (start != finish || 
	      (start == 0 && finish == 0))))
	{
		fRow->PreviousField(fColumnNumber);
		return;
	}	
	//Move to the next field if Tab or Enter is pressed, or if the right
	//arrow is pressed when either text in the current field is highlighted
	//or the insertion point is at the end of the current field.	
	else if (bytes[0] == '\t' || bytes[0] == '\n' ||
	         ((bytes[0] == B_RIGHT_ARROW) && (start != finish ||
	          (start == length && finish == length))))
	{
		fRow->NextField(fColumnNumber);
		return;
	}
	else if (bytes[0] == B_DOWN_ARROW)
	{
		if (modifiers & (B_CONTROL_KEY|B_OPTION_KEY))
		{
			GridView* view = fRow->Parent();
			GridRow* row = (GridRow*)view->ItemAt(view->IndexOf(fRow) - 1);
			if (row)
			{
				const char* textToCopyOver = row->GetContentAt(fColumnNumber-1);
				if (textToCopyOver)
				{
					SetText(textToCopyOver);
					SelectAll();
				}
			}
		}
		else
		{
			fRow->NextRow(fColumnNumber);
		}
		return;
	}
	else if (bytes[0] == B_UP_ARROW)
	{
		if (modifiers & (B_CONTROL_KEY|B_OPTION_KEY))
		{
			GridView* view = fRow->Parent();
			GridRow* row = (GridRow*)view->ItemAt(view->IndexOf(fRow) + 1);
			if (row)
			{
				const char* textToCopyOver = row->GetContentAt(fColumnNumber-1);
				if (textToCopyOver)
				{
					SetText(textToCopyOver);
					SelectAll();
				}
			}
		}
		else
		{
			fRow->PreviousRow(fColumnNumber);
		}
		return;
	}
	else if (bytes[0] == B_ESCAPE)
	{
		fRow->Restore();
		return;
	}

	//Any other keys pressed will be passed on.
	BTextView::KeyDown(bytes, numBytes);
}


void
GridCell::DeleteText(int32 start, int32 finish)
{
	if (!fModified)
	{
		fModified = true;
		fRow->SetModified(true);
	}
	     
	BTextView::DeleteText(start, finish);
}


void
GridCell::MouseDown(BPoint where)
{
	fRow->SetColumnClicked(fColumnNumber);
	BTextView::MouseDown(where);
}


bool
GridCell::Modified()
{
	return fModified;
}

void
GridCell::Modified(bool m)
{
	fModified = m;
}

//bool
//GridCell::AcceptsDrop(BMessage *msg)
//{
//	return false;
//}


