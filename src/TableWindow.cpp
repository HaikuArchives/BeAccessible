#include <iostream>

#include <Alert.h>
#include <Application.h>
#include <Bitmap.h>
#include <CheckBox.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Picture.h>
#include <PictureButton.h>
#include <ScrollView.h>
#include <String.h>
#include <TextControl.h>

#include "Bitmaps.h"
#include "CLVColumn.h"
#include "ColumnDesignView.h"
#include "ColumnProperties.h"
#include "Constants.h"
#include "GridView.h"
#include "RowSummaryView.h"
#include "SQLiteManager.h"
#include "TableDataRow.h"
#include "TableDataView.h"
#include "TableDesignRow.h"
#include "TableDesignView.h"
#include "TableWindow.h"
#include "TextEntryAlert.h"

extern SQLiteManager* GlobalSQLMgr;


TableWindow::TableWindow(int view, const char* tableName, bool newTable)
: BWindow(BRect(50,50,550,450), tableName, B_DOCUMENT_WINDOW, 0)
{
	fNewTable = newTable;

	fName = new BString(tableName);
	
	fCurrentView = -1;
	fContainerView = NULL;
	fColumnDesign = NULL;
	fRowSummary = NULL;
	fDesignTopView = NULL;
	
	if (!fNewTable)
		fProps = new TableProperties(*fName);
	else
		fProps = new TableProperties();
		
	fOnPrimaryKey = NULL;
	fOffPrimaryKey = NULL;
	
	//Create MenuBar
	BRect rect = Bounds();
	rect.bottom = MENU_BAR_HEIGHT;
	
	menuBar = new BMenuBar(rect, "MenuBar");
	AddChild(menuBar);

	
	//Create Menus
	fileMenu	= new BMenu("File");
	editMenu = new BMenu("Edit");
	viewMenu = new BMenu("View");
	menuBar->AddItem(fileMenu);
	menuBar->AddItem(editMenu);
	menuBar->AddItem(viewMenu);
	
	//Create MenuItems
	BMenuItem* save = new BMenuItem ("Save Table", new BMessage(MENU_SAVE_TABLE_MSG), 'S');
	BMenuItem* saveAs = new BMenuItem("Save As/Export", new BMessage(MENU_FILE_EXPORT_MSG), 'S', B_SHIFT_KEY);
	BMenuItem* close = new BMenuItem("Close", new BMessage(MENU_FILE_CLOSE_MSG), 'W');
	BMenuItem* designView = new BMenuItem("Design", new BMessage(MENU_VIEW_DESIGN_MSG));
	BMenuItem* dataView = new BMenuItem("Data", new BMessage(MENU_VIEW_DATA_MSG));
	BMenuItem* selectAll = new BMenuItem("Select All", new BMessage(SELECT_ALL_ROWS_MSG), 'A');
	
	fileMenu->AddItem(save);
	fileMenu->AddItem(saveAs);
	fileMenu->AddSeparatorItem();
	fileMenu->AddItem(close);
	viewMenu->AddItem(designView);
	viewMenu->AddItem(dataView);	
	viewMenu->SetRadioMode(true);
	editMenu->AddItem(selectAll);

  menuBar->FindItem("Save As/Export")->SetEnabled(false);

	if (view == DATA_VIEW)
	{
		dataView->SetMarked(true);
		CreateDataView();
	}
	else if (view == DESIGN_VIEW)
	{
		designView->SetMarked(true);
		CreateDesignView();
	}
}



void TableWindow::Quit()
{
	Hide();
	
	fGridView->DeleteAllRows();

	delete fName;
	delete fProps;
	
	if (fOnPrimaryKey)
		delete fOnPrimaryKey;
		
	if (fOffPrimaryKey)
		delete fOffPrimaryKey;
	
	BWindow::Quit();
}

bool
TableWindow::QuitRequested()
{
	if (CheckForSave())
		return true;
	else
		return false;
}


void TableWindow::CreateDataView()
{
	if (fCurrentView == DATA_VIEW)
		return;
	else
		DestroyDesignView();
	
	//Establish rectangle for the ColumnListView
	BRect bounds = Bounds();
	bounds.top += menuBar->Bounds().Height() + 1;
	//bounds.left += 2;
	bounds.right -= B_V_SCROLL_BAR_WIDTH;
	bounds.bottom -= B_H_SCROLL_BAR_HEIGHT;

	BString sql;

	if (fNewTable)
		sql.SetTo("");
	else
	{
		sql.SetTo("SELECT ROWID, * FROM [");
		sql.Append(*fName);
		sql.Append("]");
	}

	// Create the RowSummaryView.  It has to be created first b/c otherwise the buttons
	// won't function when the GridView is created next to it...
	BRect frame;
	frame.bottom = bounds.bottom + B_H_SCROLL_BAR_HEIGHT;
	frame.right = 299;
	frame.left = 0;
	frame.top = bounds.bottom + 1;
	
	fRowSummary = new RowSummaryView(frame, "rowsum", B_FOLLOW_BOTTOM, B_WILL_DRAW);
	AddChild(fRowSummary);

	
	fGridView = new TableDataView(bounds,&fContainerView,fName->String(), sql, fProps);

	// Adjust horizontal scrollbar to make room for the RowSummaryView
	BScrollBar* h_scrollbar = fContainerView->ScrollBar(B_HORIZONTAL);
	if (h_scrollbar)
	{
		h_scrollbar->ResizeBy(-300.0,0.0);
		h_scrollbar->MoveBy(300.0,0.0);
	}
	
	
	AddChild(fContainerView);

	fGridView->MakeFocus();
	fRowSummary->SetViewColor(216,216,216);
	fGridView->GoToRow(0, 1);
	Show();
	
	// Now that the GridView is created, pass a pointer to it to the RowSummary
	fRowSummary->SetGridView(fGridView);
	// Similarly, notify the GridView of the RowSummary's existence
	((TableDataView*)fGridView)->SetRowSummary(fRowSummary);

}


void
TableWindow::DestroyDesignView()
{
	if (fContainerView)
	{
		fGridView->DeleteAllRows();
		fDesignTopView->RemoveChild(fContainerView);
		delete fContainerView;
		fContainerView = NULL;
	}
	
	if (fColumnDesign)
	{
		fDesignTopView->RemoveChild(fColumnDesign);
		delete fColumnDesign;
		fColumnDesign = NULL;
	}

	if (fDesignTopView)
	{
		RemoveChild(fDesignTopView);
		delete fDesignTopView;
		fDesignTopView = NULL;
	}
	fCurrentView = DATA_VIEW;
}





void TableWindow::CreateDesignView()
{
	if (fCurrentView == DESIGN_VIEW)
		return;
	else
		DestroyDataView();
	
	// Prep the DesignView's top view
	BRect bounds = Bounds();
	fDesignTopView = new BView(bounds, "TopView", B_FOLLOW_ALL_SIDES, B_FRAME_EVENTS);
	fDesignTopView->SetViewColor(216,216,216,0);
	AddChild(fDesignTopView);

	//Bitmaps used to hold the pictures for the Primay key picture button
	BRect rect(0,0,15,15);
	BBitmap onBitmap(rect, B_COLOR_8_BIT);
	BBitmap offBitmap(rect, B_COLOR_8_BIT);
	
	//fill 'em up
	onBitmap.SetBits(PrimaryKeyButton_On, 2048, 0, B_COLOR_8_BIT);
	offBitmap.SetBits(PrimaryKeyButton_Off, 2048, 0, B_COLOR_8_BIT);
	
	//TempView used to create BPicture
	BView *tempView = new BView(rect, "temp", B_FOLLOW_NONE, B_WILL_DRAW);
	AddChild(tempView);
	
	//Record BPicture for On
	tempView->BeginPicture(new BPicture);
	tempView->DrawBitmap(&onBitmap);
	fOnPrimaryKey = tempView->EndPicture();
	
	//Record BPicture for Off
	tempView->BeginPicture(new BPicture);
	tempView->DrawBitmap(&offBitmap);
	fOffPrimaryKey = tempView->EndPicture();
	
	RemoveChild(tempView);
	delete tempView;
		
	//Establish bounds for the TableDesignView
	//BRect bounds = Bounds();
	bounds.top += menuBar->Bounds().Height() + 1;
	bounds.right -= B_V_SCROLL_BAR_WIDTH;
	bounds.bottom -= B_H_SCROLL_BAR_HEIGHT;	
	bounds.bottom = bounds.Height() * 0.7;

	fGridView = new TableDesignView(bounds,&fContainerView,fName->String(), fProps);
	
	fDesignTopView->AddChild(fContainerView);
	
	BRect newBounds = Bounds();
	newBounds.top = bounds.bottom + B_H_SCROLL_BAR_HEIGHT;
	
	fColumnDesign = new ColumnDesignView(fGridView, newBounds, "ColumnDesignView", 
	                                     fOnPrimaryKey, fOffPrimaryKey);
	fDesignTopView->AddChild(fColumnDesign);
	fGridView->MakeFocus();
	fGridView->GoToRow(0, 1);
	Show();
}


void
TableWindow::DestroyDataView()
{
	if (fContainerView && Lock())
	{
		fGridView->DeleteAllRows();
		RemoveChild(fContainerView);
		delete fContainerView;
		fContainerView = NULL;
		Unlock();
	}

	if (fRowSummary && Lock())
	{
		RemoveChild(fRowSummary);
		delete fRowSummary;
		fRowSummary = NULL;
		Unlock();
	}

	fCurrentView = DESIGN_VIEW;	
}

//
//void 
//TableWindow::KeyDown(const char* bytes, int32 numBytes)
//{
//	if (numBytes == 1)
//	{
//		switch(bytes[0])
//		{
//		
//			default:
//			{
//				//BWindow::KeyDown(bytes, numBytes);
//				break;
//			}		
//		}
//	}
//}
//


void
TableWindow::MessageReceived(BMessage* msg)
{
	switch(msg->what)
	{
		case SELECT_ALL_ROWS_MSG:
		{
			fGridView->SelectAllRows();
			break;
		}

		case MENU_FILE_CLOSE_MSG:
		{
			if (CheckForSave())
			{
				Quit();
			}
		
			break;
		}
		
		case MENU_VIEW_DATA_MSG:
		{
			if (fNewTable)
			{
				BString msg("You must first save the table.\n\nDo you want to save the table now?");
				BAlert* alert = new BAlert("Title", msg.String(), "Cancel", "No", "Yes",
				                   B_WIDTH_AS_USUAL, B_OFFSET_SPACING, B_WARNING_ALERT);
				alert->SetShortcut(0, B_ESCAPE);
				int32 button_index = alert->Go();
			
				if (button_index == 2)	// Yes
				{
					if (CreateNewTable())
						CreateDataView();
					else
						viewMenu->FindItem("Design")->SetMarked(true);
				}
				else
				{
					//Change the menu back to design view
					viewMenu->FindItem("Design")->SetMarked(true);
				}
			}
			else if (CheckForSave())
			{
				CreateDataView();
			}
			else
			{
				//Change the menu back to design view
				viewMenu->FindItem("Design")->SetMarked(true);
			}
		
			break;
		}
		
		case MENU_VIEW_DESIGN_MSG:
		{
			CreateDesignView();
			break;
		}
		
		case PRIMARY_KEY_BUTTON:
		{
			if (fColumnDesign)
			{
				TableDesignRow* row;
				int32 selected = 0;

				selected = fGridView->CurrentSelection(0);
				row = (TableDesignRow*)fGridView->ItemAt(selected);
				
				//Change the PrimaryKey flag in the row's column properties structure, and
				//redraw the selected row
				void* button;
				msg->FindPointer("source", &button);
				
				if (((BPictureButton*)button)->Value() == B_CONTROL_ON)
				{
					row->fColumnProps->SetPrimaryKey(true);
				}
				else
				{
					row->fColumnProps->SetPrimaryKey(false);
				}
				row->SaveColumnProperties();	
				row->InvalidateRow();
			}//end if

			break;	
		}


		case MENU_SAVE_TABLE_MSG:
		{
			if (fCurrentView == DESIGN_VIEW)
			{
				if (fNewTable)
					CreateNewTable();
				else
					SaveTableDesign();
			}
			else if (fCurrentView == DATA_VIEW)
			{
				SaveTableLayout();
			}
			
			break;
		}
		
		default:
			BWindow::MessageReceived(msg);
			break;
		
	}//end switch
}

bool
TableWindow::SaveTableDesign()
{
	//BeAccessibleApp* myApp = (BeAccessibleApp*)be_app;
	TableProperties table = BuildNewTableProperties();

	if (table.CountColumns() == 0)
	{
		DisplayNoFieldsError();
		return false;
	}
	
	BString backupTableName(*fName), sql;
	backupTableName += "_backup";
	
	//Start transaction
	bool sqlSuccess = GlobalSQLMgr->Execute("BEGIN EXCLUSIVE;");
	if (!sqlSuccess)
	{
		return false;
	}
	
	//Create backup of the original table
	sqlSuccess = GlobalSQLMgr->Execute(fProps->CreateTableSQL(backupTableName, true).String());
	if (!sqlSuccess)
	{
		GlobalSQLMgr->Execute("ROLLBACK;");
		return false;
	}
	
	//Copy the contents of the original table into the backup table
	sql.SetTo("INSERT INTO [");
	sql += backupTableName;
	sql += "] SELECT ";
	sql += fProps->ColumnNames();
	sql += " FROM [";
	sql += *fName;
	sql += "];";

	sqlSuccess = GlobalSQLMgr->Execute(sql.String());
	if (!sqlSuccess)
	{
		GlobalSQLMgr->Execute("ROLLBACK;");
		return false;
	}
	
	//Drop the original table
	sqlSuccess = GlobalSQLMgr->DeleteTable(*fName);
	if (!sqlSuccess)
	{
		GlobalSQLMgr->Execute("ROLLBACK;");
		return false;
	}
	
	//Recreate the original table, but this time with the new structure
	sqlSuccess = GlobalSQLMgr->Execute(table.CreateTableSQL(*fName).String());
	if (!sqlSuccess)
	{
		GlobalSQLMgr->Execute("ROLLBACK;");
		return false;
	}

	//Copy the contents of the backup table into the new table	
	sql.SetTo("INSERT INTO [");
	sql += *fName;
	sql += "] SELECT ";
	
	TableDesignView* tdView = (TableDesignView*)fGridView;
	BList* origColumnOrder = tdView->GetOriginalColumnOrder();
	
	for (int i = 0; i < table.CountColumns(); i++)
	{
		TableDesignRow* row = (TableDesignRow*)fGridView->ItemAt(i);
		int32 index = origColumnOrder->IndexOf((void*)row);
		
		if (index >= 0)
		{
			ColumnProperties* column = fProps->GetColumnProperties(index);
			sql += "[";
			sql += column->Name();
			sql += "]";
		}
		else
		{
			sql += " null";
		}
	
		if (i+1 < table.CountColumns())
			sql += ", ";
	}
	
	sql += " FROM [";
	sql += backupTableName;
	sql += "];";
	
	sqlSuccess = GlobalSQLMgr->Execute(sql.String());
	if (!sqlSuccess)
	{
		GlobalSQLMgr->Execute("ROLLBACK;");
		return false;
	}

	//Drop the backup table
	sqlSuccess = GlobalSQLMgr->DeleteTable(backupTableName);
	if (!sqlSuccess)
	{
		GlobalSQLMgr->Execute("ROLLBACK;");
		return false;
	}
	
	//So far, so good.  No errors were encountered, so commit the changes.
	sqlSuccess = GlobalSQLMgr->Execute("COMMIT;");
	if (!sqlSuccess)
	{
		GlobalSQLMgr->Execute("ROLLBACK;");
		return false;
	}
	
	delete fProps;
	fProps = new TableProperties(*fName);
	// FIXME - tabledesignview is needed
	// fGridView->SetTableProperties(fProps);
	return true;
}

bool
TableWindow::CreateNewTable()
{
	if (!PromptForTableName())
		return false;

	//BeAccessibleApp* myApp = (BeAccessibleApp*)be_app;
	TableProperties props = BuildNewTableProperties();

	if (props.CountColumns() == 0)
	{
		DisplayNoFieldsError();
		return false;
	}

	// start transaction
	bool sqlSuccess = GlobalSQLMgr->Execute("BEGIN EXCLUSIVE;");
	if (!sqlSuccess)
	{
		return false;
	}

	// create table
	sqlSuccess = GlobalSQLMgr->Execute(props.CreateTableSQL(*fName).String());
	if (!sqlSuccess)
	{
		GlobalSQLMgr->Execute("ROLLBACK;");
		return false;
	}

	// no errors; commit the transaction
	sqlSuccess = GlobalSQLMgr->Execute("COMMIT;");
	if (!sqlSuccess)
	{
		GlobalSQLMgr->Execute("ROLLBACK;");
		return false;
	}

	//Update properties structure
	delete fProps;
	fProps = new TableProperties(*fName);
	// FIXME tabledesignview is needed 
	// fGridView->SetTableProperties(fProps);

	//Update SQL string
//	BString sql("SELECT ROWID, * FROM [");
//	sql += *fName;
//	sql += "]";
//	fGridView->SetSQL(sql);
//	
	//No longer a new table
	fNewTable = false;
	SetTitle(fName->String());
	
	BMessage msg(ADD_TABLE_TO_LIST_MSG);
	msg.AddString("table", *fName);
	BMessenger(be_app).SendMessage(&msg);
	return true;
}


void
TableWindow::SaveTableLayout()
{

	//Build the xml to store the column names and sizes
	BString xml = "<table_preferences>";
	
	//Cycle through columns, starting with the 2nd (the first is the column of icons,
	//and it can't be resized so it's ignored here)
	for(int i = 1; i < fGridView->CountColumns(); i++)
	{
		CLVColumn* column = fGridView->ColumnAt(i);
		xml += "<column>";
		xml << "<name>" << column->GetLabel() << "</name>";
		xml << "<size>" << column->Width() << "</size>";
		xml += "</column>";
	}
	
	xml += "</table_preferences>";

	//Build the sql to insert/update the xml into the master table
	BString sql = "insert or replace into beaccessible_master (name, type, value) select ";
	
	xml.CharacterEscape("\'", '\'');
	BString tableName = *fName;
	tableName.CharacterEscape("\'", '\'');
	
	sql << "'" << tableName << "', 'table_preferences', '" << xml << "';";
	GlobalSQLMgr->Execute(sql.String());
}


ColumnDesignView*
TableWindow::GetColumnDesignView()
{
	return fColumnDesign;
}


TableProperties
TableWindow::BuildNewTableProperties()
{
	//We need to call Exit on the current row so that any changes just
	//made on that row get applied before we save the table.
	int32 index = fGridView->CurrentSelection(int32(0));
	
	if (index >= 0)
	{
		TableDesignRow* row = (TableDesignRow*)fGridView->ItemAt(index);
		row->Exit(true);
	}

	//Create a blank TableProperties structure, which we'll fill with the ColumnProperties
	//from each row in the Design view.  This will then be compared to the original
	//ColumnProperties to see if anything has changed.
	TableProperties newTable;

	for(int i = 0; i < fGridView->CountItems(); i++)
	{
		TableDesignRow* row = (TableDesignRow*)fGridView->ItemAt(i);

		//Since the type of a column is required (and enforced), it's a good indication
		//that this row is actually a column.  Add it to the new TableProperties structure.
		if (row->fColumnProps->Type() != "")
			newTable.AddColumnProperties(row->fColumnProps);

	}

	return newTable;
}


bool
TableWindow::CheckForSave()
{
	if (fCurrentView == DATA_VIEW)
		return true;
	
	TableProperties newProps = BuildNewTableProperties();
	
	if (!fNewTable && (*fProps == newProps))
	{
		return true;		// No changes have been made to an existing table
	}
	else if (fNewTable && (newProps.CountColumns() == 0))
	{
		// A new table does not have any fields defined.  Close the window
		// without saving.
		return true;
	}

	BString msg("Do you want to save changes to the design of table ");
	msg += *fName;
	msg += "?";
	
	BAlert* alert = new BAlert("Title", msg.String(), "Cancel", "No", "Yes",
	                   B_WIDTH_AS_USUAL, B_OFFSET_SPACING, B_WARNING_ALERT);
	alert->SetShortcut(0, B_ESCAPE);
	int32 button_index = alert->Go();

	if (button_index == 0)  // Cancel
	{
		return false;
	}
	else if (button_index == 1)  // No
	{
		return true;
	}
	else if (button_index == 2)	// Yes
	{
		if (fNewTable)
		{
			return CreateNewTable();
		}
		else
		{
			return SaveTableDesign();
		}
	}

	//Shouldn't ever reach here
	return false;
}


bool
TableWindow::PromptForTableName()
{
	TableProperties newProps = BuildNewTableProperties();

	if (newProps.CountColumns() == 0)
	{
		DisplayNoFieldsError();
		return false;
	}

	TextEntryAlert* alert = new TextEntryAlert("Save Table", "Table Name: ", 
	   fName->String(),	"Cancel", "Ok", true);
	   
	char* buffer = new char[50];
	int32 index = alert->Go(buffer, 50);

	if (index == 1)	 // Ok
	{
		fName->SetTo(buffer);
		delete[] buffer;
		return true;
	}
	else   // Cancel
	{
		delete[] buffer;
		return false;	
	}
}

void
TableWindow::DisplayNoFieldsError()
{
		BString msg("The table cannot be saved because there are no fields defined.\n\n");
		msg += "You must define at least one field in order to save the table.";
		BAlert* alert = new BAlert("Title", msg.String(), "Ok");
		alert->SetShortcut(0, B_ESCAPE);
		alert->Go();
}


void
TableWindow::RemoveContainerView()
{
	if (fContainerView)
		RemoveChild(fContainerView);
}

void
TableWindow::ReattachContainerView()
{
	if (fContainerView)
		AddChild(fContainerView);
}


BString
TableWindow::TableName()
{
	return *fName;
}

