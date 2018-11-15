#include <iostream>

#include "PreviewView.h"
#include "Import.h"
#include "ImportUtilities.c"

PreviewView::PreviewView(BRect frame, BString file)
: BView(frame, "PreviewView", B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW),
  fPreviewLines(),
  fDelimiter(""),
  fFieldNamesOnFirstRow(false)
{
	fColumnListView = NULL;
	fContainerView = NULL;

	//grab the lines to preview
	FILE* inputFile = fopen(file.String(), "rb");
	for(int i = 0; i < 25; i++)
		fPreviewLines.push_back(local_getline(0,inputFile));
	fclose(inputFile);
	
	DrawPreview();
}

void
PreviewView::Draw(BRect updateRect)
{
/*	SetFont(be_fixed_font);
		
	float height = 15;
	//cout << "ascent = " << fontHeight.ascent << endl;
	//cout << "descent = " << fontHeight.descent << endl;
	//cout << "leading = " << fontHeight.leading << endl;
	for(int i = 0; i < fPreviewLines.CountItems(); i++)
	{
		BString* line = (BString*)fPreviewLines.ItemAt(i);
		DrawString(line->String(), BPoint(3,height*(i+1)));
	}
*/
}

void
PreviewView::SetDelimiter(char c)
{
	if (c == '\0')
		return;
		
	fDelimiter = c;
	
	if(LockLooper())
	{
		DrawPreview();
		UnlockLooper();
	}
}

void
PreviewView::SetFieldNamesOnFirstRow(bool value)
{
	fFieldNamesOnFirstRow = value;


	if(LockLooper())
	{
		DrawPreview();
		UnlockLooper();
	}
}


int 
PreviewView::NumColumns()
{
	if (fColumnListView)
		return fColumnListView->CountColumns();
	else
		return 0;
}


std::vector<BString> 
PreviewView::ColumnNames()
{
//do an assert(fColumnListView) here instead?
//	if (fColumnListView)
//	{
		std::vector<BString> columns;

		if (fFieldNamesOnFirstRow)
		{
			for(int i = 0; i < fColumnListView->CountColumns(); i++)
			{
				CLVColumn* column = fColumnListView->ColumnAt(i);
				columns.push_back(column->GetLabel());
			}
		}
		else
		{
			for(int i = 0; i < fColumnListView->CountColumns(); i++)
			{
				BString name("Field");
				name << i+1;
				columns.push_back(name);
			}
		}

		return columns;
//	}
//	else
//		return NULL;
}


void
PreviewView::DrawPreview()
{
	BRect bounds = Bounds();
	bounds.right -= B_V_SCROLL_BAR_WIDTH + 1;
	bounds.bottom -= B_H_SCROLL_BAR_HEIGHT + 1;

	if (fContainerView)
	{
		//Remove and delete the view
		RemoveChild(fContainerView);
		delete fContainerView;
		fContainerView = NULL;
	}

	//Create the new views
	BFont labelFont(be_fixed_font);
	labelFont.SetSize(10);
	
	fColumnListView = new ColumnListView(bounds, &fContainerView, NULL,
		B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW|B_FRAME_EVENTS|B_NAVIGABLE,
		B_SINGLE_SELECTION_LIST, false, true, true, true, B_NO_BORDER,
		&labelFont);

	fColumnListView->SetFont(be_fixed_font);
	fColumnListView->SetFontSize(10);
	
	for(uint rowIndex = 0; rowIndex < fPreviewLines.size(); rowIndex++)
	{
		BList line = Import::ParseLine(GetLine(rowIndex), fDelimiter);
		
		if (rowIndex == 0)
		{
			//Add Columns
			if (fFieldNamesOnFirstRow)
			{
				for(int i = 0; i < line.CountItems(); i++)
				{
					BString* name = (BString*)line.ItemAt(i);

					//if there's only 1 column, make the column take up all the 
					//remaining space.  Otherwise, default column lengths to 80.
					float length;
					if (line.CountItems() == 1)
						length = bounds.right;
					else
						length = 80;
					
					fColumnListView->AddColumn(new CLVColumn(name->String(), length));
				}

				//the first line had the column names, so we should skip to the next line
				continue;
			}
			else
			{
				//the first line does not have column names, so add the columns
				//with blank labels.
				for(int i = 0; i < line.CountItems(); i++)
				{
					//if there's only 1 column, make the column take up all the 
					//remaining space.  Otherwise, default column lengths to 80.
					float length;
					if (line.CountItems() == 1)
						length = bounds.right;
					else
						length = 80;
						
					fColumnListView->AddColumn(new CLVColumn("", length));
				}
			}
		}

		//Add rows
		CLVEasyItem* row = new CLVEasyItem();

		for(int colIndex = 0; colIndex < line.CountItems(); colIndex++)
		{
			BString* text = (BString*)line.ItemAt(colIndex);
			row->SetColumnContent(colIndex, text->String(), false);
		}

		fColumnListView->AddItem(row);
		
		Import::DeleteParsedLine(line);
	}
		
	//Add the container view
	AddChild(fContainerView);
}


BString
PreviewView::GetLine(int lineNum)
{
	return fPreviewLines[lineNum];
}




