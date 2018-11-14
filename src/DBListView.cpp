#include <iostream>
#include <PopUpMenu.h>
#include <Menu.h>
#include <MenuItem.h>
#include <Alert.h>
#include <Window.h>
#include <ClassInfo.h>
#include <Clipboard.h>

#include "BeAccessibleApp.h"
#include "DBListView.h"
#include "Constants.h"
#include "TextEntryAlert.h"
#include "TableProperties.h"
#include "TableWindow.h"
#include "SQLiteManager.h"
#include "PasteTableWindow.h"

extern SQLiteManager* GlobalSQLMgr;


DBListView::DBListView(BRect rect, const char* name, list_view_type type,
  uint32 resizingMode, uint32 flags) : BListView(rect, name, type, resizingMode, flags)
{
	fButtonStatus = true;
	fPasteWindow = NULL;
	
	BMessage* open = new BMessage(OPEN_BUTTON);
	open->AddString("TabName", BString(name));
	SetInvocationMessage(open);
	ClearClipboard();
}


DBListView::~DBListView()
{
	// Clear the clipboard to remove any table references since they
	// won't be valid if a different database is opened */
	ClearClipboard();
}


void
DBListView::ClearClipboard()
{
	if (TableOnClipboard())
	{
		if (be_clipboard->Lock())
		{
			be_clipboard->Clear();
			be_clipboard->Commit();
			be_clipboard->Unlock();
		}
	}
}



void 
DBListView::SelectionChanged()
{
	BWindow* window = Window();

	if (CurrentSelection() < 0)
	{
		BMessage* disable = new BMessage(DISABLE_OPEN_BUTTON);
		disable->AddString("ListName", BString(Name()));
		BMessenger(window).SendMessage(disable);
		fButtonStatus = false;
	}
	else
	{
		if (!fButtonStatus)
		{
			BMessage* enable = new BMessage(ENABLE_OPEN_BUTTON);
			enable->AddString("ListName", BString(Name()));
			BMessenger(window).SendMessage(enable);
			fButtonStatus = true;			
		}	
	}

}

void
DBListView::MouseDown(BPoint where)
{
	BMessage* message = Window()->CurrentMessage();
	int32 item_index = IndexOf(where);
	
	if (item_index >= 0)
		Select(item_index);
	else
		DeselectAll();
	
    if (message->FindInt32("buttons") == B_SECONDARY_MOUSE_BUTTON)
	{
		BPopUpMenu* menu = new BPopUpMenu("rightClickTable", false, false);
		BStringItem* tableName = (BStringItem*)ItemAt(item_index);
	
		if (tableName)
		{
			BMessage* renameMsg = new BMessage(RENAME_TABLE_MSG);
			BMessage* deleteMsg = new BMessage(DELETE_TABLE_MSG);
			
			renameMsg->AddString("table", tableName->Text());
			renameMsg->AddInt32("index", item_index);
			deleteMsg->AddString("table", tableName->Text());
			deleteMsg->AddInt32("index", item_index);
			
			menu->AddItem(new BMenuItem("Rename", renameMsg));
			menu->AddItem(new BMenuItem("Delete", deleteMsg));
			menu->AddSeparatorItem();
		}
			
		BMenuItem *cut, *copy, *paste, *import;
		cut = new BMenuItem("Cut", new BMessage(B_CUT), 'X');
		copy = new BMenuItem("Copy", new BMessage(B_COPY), 'C');
		paste = new BMenuItem("Paste", new BMessage(B_PASTE), 'V');
		import = new BMenuItem("Import Text File", new BMessage(IMPORT_FILE_REQUESTED_MSG));
		menu->AddItem(cut);
		menu->AddItem(copy);
		menu->AddItem(paste);
		menu->AddSeparatorItem();
		menu->AddItem(import);
	
		
		cut->SetEnabled(false);

		if (!tableName)
		{
			copy->SetEnabled(false);
		}
		
		if (TableOnClipboard())
			paste->SetEnabled(true);
		else
			paste->SetEnabled(false);

		BPoint point = where;
		ConvertToScreen(&point);
		menu->SetTargetForItems(this);  //the rule
		import->SetTarget(Window());    //the exception to the rule
	
		BRect openRect;
		openRect.top = point.y - 5;
		openRect.bottom = point.y + 5;
		openRect.left = point.x - 5;
		openRect.right = point.x + 5;
		
		menu->Go(point, true, false, openRect);
	   
		delete menu;
	}
	else
	{
		BListView::MouseDown(where);	
	}
}


void
DBListView::KeyDown(const char* bytes, int32 numBytes)
{
	if (numBytes == 1)
	{
		switch(bytes[0])
		{
			case B_DELETE:
			{
				int32 currentIndex = CurrentSelection();
				if (currentIndex >= 0)
				{
					BStringItem* item = (BStringItem*)ItemAt(currentIndex);
					
					BMessage msg(DELETE_TABLE_MSG);
					msg.AddString("table", item->Text());
					msg.AddInt32("index", currentIndex);
					MessageReceived(&msg);
				}
			}
			
			
			default:
			{
				BListView::KeyDown(bytes, numBytes);
			}
			
		}
	
	}
}


void
DBListView::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case RENAME_TABLE_MSG:
		{
			const char* table;
			msg->FindString("table", &table);
			int32 index;
			msg->FindInt32("index", &index);

			if (IsTableOpen(table))
			{
				BString msg("You can't rename a table while it's open.\n\n");
				msg += "Close the table first and then rename it.";
				BAlert* alert = new BAlert("Title", msg.String(), "Ok");
				alert->SetShortcut(0, B_ESCAPE);
				alert->Go();
				break;
			}

			TextEntryAlert* alert = new TextEntryAlert("Rename Table", "Table Name: ", 
			   table, "Cancel", "Ok", true);
			   
			char* buffer = new char[50];
			int32 button_index = alert->Go(buffer, 50);

			if (button_index == 1)
			{
				RenameTable(table, index, buffer);
				Sort();
				ClearClipboard();  //the old tablename may have been copied
			}
		
			break;
		}
		
		case DELETE_TABLE_MSG:
		{
			const char* table;
			msg->FindString("table", &table);
			int32 index;
			msg->FindInt32("index", &index);

			if (IsTableOpen(table))
			{
				BString msg("You can't delete a table while it's open.\n\n");
				msg += "Close the table first and then delete it.";
				BAlert* alert = new BAlert("Title", msg.String(), "Ok");
				alert->SetShortcut(0, B_ESCAPE);
				alert->Go();
				break;
			}

			BString msg("Are you sure you want to delete the table \"");
			msg << table;
			msg += "\"?";
			BAlert* alert = new BAlert("Title", msg.String(), "Cancel", "No", "Yes",  
			                    B_WIDTH_AS_USUAL, B_OFFSET_SPACING, B_WARNING_ALERT);
			alert->SetShortcut(0, B_ESCAPE);
			int32 button_index = alert->Go();
	
			if (button_index == 2)
			{
				BString tblName(table);
				bool sqlSuccess = GlobalSQLMgr->DeleteTable(tblName);
				
				if (sqlSuccess)
				{
					RemoveItem(index);
					ClearClipboard();   //the old tablename may have been copied
				}
			}
			break;
		}
		
		case B_COPY:
		{
			int32 index = CurrentSelection();
			if (index >= 0)
			{
				BStringItem* item = (BStringItem*)ItemAt(index);
				if (item)
				{
					CopyTable(item->Text());
				}
			}
			
			break;
		}
		
		case B_PASTE:
		{
			PasteTable(GetTableFromClipboard());
			break;
		}
	}
}


bool
DBListView::RenameTable(const char* oldName, int32 index, char* newName)
{

	//BeAccessibleApp* myApp = (BeAccessibleApp*)be_app;

	BString newTableName(newName), oldTableName(oldName), sql;

	if (newTableName == oldTableName)
		return true;

	TableProperties props(oldTableName);
	
	//Start transaction
	bool sqlSuccess = GlobalSQLMgr->Execute("BEGIN EXCLUSIVE;");
	if (!sqlSuccess)
	{
		return false;
	}
	
	//Create backup of the original table
	sqlSuccess = GlobalSQLMgr->Execute(props.CreateTableSQL(newTableName).String());
	if (!sqlSuccess)
	{
		GlobalSQLMgr->Execute("ROLLBACK;");
		return false;
	}
	
	//Copy the contents of the original table into the backup table
	sql.SetTo("INSERT INTO [");
	sql += newTableName;
	sql += "] SELECT ";
	sql += props.ColumnNames();
	sql += " FROM [";
	sql += oldTableName;
	sql += "];";

	sqlSuccess = GlobalSQLMgr->Execute(sql.String());
	if (!sqlSuccess)
	{
		GlobalSQLMgr->Execute("ROLLBACK;");
		return false;
	}
	
	//Drop the original table
	sqlSuccess = GlobalSQLMgr->DeleteTable(oldTableName);
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
	
	BStringItem* item = (BStringItem*)ItemAt(index);
	item->SetText(newName);
	InvalidateItem(index);
	
	return true;
}


void
DBListView::Sort()
{
	SortItems(Compare);
}

int
DBListView::Compare(const void* firstItem, const void* secondItem)
{
	BString first(  (*(BStringItem**)((BListItem**)firstItem))->Text() );
	BString second( (*(BStringItem**)((BListItem**)secondItem))->Text() );
	
	return first.ICompare(second);
}

bool
DBListView::IsTableOpen(BString name)
{
	//Loop through all open windows
	for (int i = 0; i < be_app->CountWindows(); i++)
	{
		BWindow* window = be_app->WindowAt(i);
		
		if (is_instance_of(window, TableWindow))
		{
			TableWindow* table = (TableWindow*)window;
			if (table->TableName() == name)
				return true;
		}
	}
	
	return false;
}

void
DBListView::CopyTable(const char* copyTable)
{
	if (!copyTable)
		return;
	
	//Copy the table string
	BString table(copyTable);

	BMessage* clip;
	
	if (be_clipboard->Lock())
	{
		be_clipboard->Clear();
		if ((clip = be_clipboard->Data()))
		{
			clip->AddData("BeAccessibleTable", B_STRING_TYPE, table.String(), table.Length());
			be_clipboard->Commit();
		}
		be_clipboard->Unlock();
	}
}

void
DBListView::PasteTable(BString table)
{
	//Display the StartWindow	
   	BRect frame = Bounds();
   	BRect pasteRect(frame.Width()/2 - 120 + frame.left, 
   	                frame.Height()/2 - 70 + frame.top, 
   	                frame.Width()/2 + 120 + frame.left,
	   	            frame.Height()/2 + 70 + frame.top);
	   	         
	fPasteWindow = new PasteTableWindow(ConvertToScreen(pasteRect), table);
}


bool
DBListView::TableOnClipboard()
{
	const char* text;
	int32 textLength;
	BMessage* clip = (BMessage*)NULL;
	bool found = false;
		
	if (be_clipboard->Lock())
	{
		if ((clip = be_clipboard->Data()))
		{
			if (clip->FindData("BeAccessibleTable", 
			                   B_STRING_TYPE, (const void**)&text, (ssize_t*)&textLength) == B_OK)
			{
				found = true;
			}
			else
			{
				found = false;
			}
		}
		be_clipboard->Unlock();
	}

	return found;
}

BString
DBListView::GetTableFromClipboard()
{
	const char* text = NULL;
	int32 textLength;
	BMessage* clip;
		
	if (be_clipboard->Lock())
	{
		if ((clip = be_clipboard->Data()))
		{
			clip->FindData("BeAccessibleTable", 
			               B_STRING_TYPE, (const void**)&text, (ssize_t*)&textLength);
		}
		be_clipboard->Unlock();
	}

	return BString(text, textLength);
}

