#include <iostream>
#include <vector>

#include <Alert.h>
#include <Application.h>
#include <CheckBox.h>
#include <Message.h>
#include <Window.h>

#include "Constants.h"
#include "DelimiterView.h"
#include "Import.h"
#include "ImportFileView.h"
#include "PreviewView.h"
#include "SQLiteManager.h"
#include "TableView.h"

extern SQLiteManager* GlobalSQLMgr;

#define COLUMN_NAMES_MSG 'coln'
#define IMPORT_BUTTON_MSG 'imbt'
#define CANCEL_IMPORT_MSG 'cnbt'


ImportFileView::ImportFileView(BRect frame, BString file)
: BView(frame, "ImportFileView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW),
  fHasFieldNames(false),
  fFile(file)
{
	SetViewColor(216,216,216,0);

	BRect bounds = Bounds();
	BRect boxFrame, viewFrame;

	//Create the PreviewView first, since a pointer to it needs to be passed to the 
	//DelimiterView
	viewFrame.Set(15,20,bounds.right-35,125);
	fPreviewView = new PreviewView(viewFrame, file);

	//DelimiterView
	boxFrame.Set(10,10,bounds.right-10,100);
	fDelimiterBox = new BBox(boxFrame);
	fDelimiterBox->SetLabel("Delimiter");

	viewFrame.Set(10,10,bounds.right-30,85);
	fDelimiterView = new DelimiterView(viewFrame, fPreviewView);

	//ColumnView
	boxFrame.Set(10,120,bounds.right-10,170);
	fColumnView = new BBox(boxFrame);
	fColumnView->SetLabel("Columns");
	
	viewFrame.Set(15,18,bounds.right-100,40);
	fColumnsCheckBox = new BCheckBox(viewFrame, "Columns", 
		"First Row Contains Column Names", new BMessage(COLUMN_NAMES_MSG));
	
	//TableView
	boxFrame.Set(10, 190, bounds.right-10, 280);
	fTableBox = new BBox(boxFrame);
	fTableBox->SetLabel("Table");
	
	viewFrame.Set(10,10,bounds.right-30,85);
	fTableView = new TableView(viewFrame);

	//Preview
	boxFrame.Set(10,300,bounds.right-10,440);
	fPreviewBox = new BBox(boxFrame);
	fPreviewBox->SetLabel("Preview");

	//Buttons
	fImportButton = new BButton(BRect(bounds.right-78,455,bounds.right-12,480), 
		"ImportButton", "Import", new BMessage(IMPORT_BUTTON_MSG));
	fImportButton->MakeDefault(true);
	fCancelButton = new BButton(BRect(bounds.right-159,455,bounds.right-93,380), 
		"CancelButton", "Cancel", new BMessage(CANCEL_IMPORT_MSG));

	AddChild(fDelimiterBox);
	fDelimiterBox->AddChild(fDelimiterView);

	AddChild(fTableBox);
	fTableBox->AddChild(fTableView);

	AddChild(fColumnView);
	fColumnView->AddChild(fColumnsCheckBox);

	AddChild(fPreviewBox);	
	fPreviewBox->AddChild(fPreviewView);

	AddChild(fImportButton);
	AddChild(fCancelButton);
}


void
ImportFileView::AllAttached()
{
	fColumnsCheckBox->SetTarget(this);
	fImportButton->SetTarget(this);
	fCancelButton->SetTarget(this);
}


void
ImportFileView::MessageReceived(BMessage* msg)
{
	switch(msg->what)
	{
		case COLUMN_NAMES_MSG:
		{
			if (fColumnsCheckBox->Value() == B_CONTROL_ON)
			{
				fHasFieldNames = true;
				fPreviewView->SetFieldNamesOnFirstRow(true);
			}
			else
			{
				fHasFieldNames = false;
				fPreviewView->SetFieldNamesOnFirstRow(false);				
			}
			break;
		}
		
		case CANCEL_IMPORT_MSG:
		{
			Window()->Quit();
			break;
		}
		
		case IMPORT_BUTTON_MSG:
		{
			//Import, baby!
			if(CheckForMissingData())
			{
				std::vector<BString> colNames = fPreviewView->ColumnNames();
				BString tableName = fTableView->Table();
				
				if (fTableView->IsNewTable())
				{
					//create table if it doesn't exist
					BString sql("create table [");
					sql << tableName << "] ([" << colNames[0] << "] text";
					
					for (uint i=1; i < colNames.size(); i++)
						sql << ", [" << colNames[i] << "] text";
					
					sql << ");";
					
					bool rc = GlobalSQLMgr->Execute(sql.String());
					if (!rc)
						break;
				}
				
				bool ignoreFirstColumn = fColumnsCheckBox->Value() ? true : false;
				
				bool rc = GlobalSQLMgr->ImportTable(fFile, fDelimiterView->Delimiter(), 
					fTableView->Table(), colNames, ignoreFirstColumn);
				
				if (rc)
				{
					(new BAlert("Import", "File successfully imported", "Ok"))->Go();
					
					//Add new table name to table list
					if (fTableView->IsNewTable())
					{
						BMessage msg(ADD_TABLE_TO_LIST_MSG);
						msg.AddString("table", tableName);
						BMessenger(be_app).SendMessage(&msg);
					}
					
					Window()->Quit();
				}
				else
				{
					//import failed; drop the table if we just created it
					if (fTableView->IsNewTable())
						GlobalSQLMgr->DeleteTable(tableName);
					
					(new BAlert("Error", "An error occurred during the import.  "
						"The file was not imported.", "Ok"))->Go();
				}
			}
			break;
		}
		
		default:
		{
			BView::MessageReceived(msg);
			break;
		}
	
	}
}


bool
ImportFileView::CheckForMissingData()
{
	const char* delimiter = fDelimiterView->Delimiter();
	
	if (delimiter[0] == '\0')
	{
		(new BAlert("DelimAlert", "Please select a delimiter.", "Ok"))->Go();
		return false;
	}
	
	BString table = fTableView->Table();
	
	if (table.Length() == 0)
	{
		(new BAlert("TableAlert", "Please enter a name for the table.", "Ok"))->Go();
		return false;
	}

	return true;
}



