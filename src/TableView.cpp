#include <MenuItem.h>

#include "TableView.h"
#include "SQLiteManager.h"

#define EXISTING_TABLE_MSG 'extb'
#define EXISTING_TABLE_SELECTED_MSG 'exsl'
#define NEW_TABLE_MSG 'nwtb'
#define NEW_TABLENAME_INVOKE_MSG  'ntbi'
#define NEW_TABLENAME_MODIFY_MSG  'ntbm'

extern SQLiteManager* GlobalSQLMgr;

TableView::TableView(BRect frame)
: BView(frame, "TableView", B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW)
{
	SetViewColor(216,216,216,0);
	
	fExisting = new BRadioButton(BRect(5,13,100,38), "ExistingTable", "Existing", new BMessage(EXISTING_TABLE_MSG));
	fNew = new BRadioButton(BRect(5,42,100,67), "NewTable", "New", new BMessage(NEW_TABLE_MSG));

	fMenu = new BPopUpMenu("Tables");
	AddTablesToMenu();
	//Set first table as default menu item
	BMenuItem* item;
	if ((item = fMenu->ItemAt(0)) != NULL)
	{
		item->SetMarked(true);
	}

	//Make the existing option the default
	fExisting->SetValue(1);
		
	fTableMenu = new BMenuField(BRect(90,10,350,35), "TableMenuField", NULL, fMenu);

	fNewTableName = new BTextControl(BRect(90,42,250,67), "NewTableNameText", NULL, NULL, 
		new BMessage(NEW_TABLENAME_INVOKE_MSG));
	fNewTableName->SetModificationMessage(new BMessage(NEW_TABLENAME_MODIFY_MSG));
	
	AddChild(fExisting);
	AddChild(fNew);
	AddChild(fTableMenu);
	AddChild(fNewTableName);
}



void
TableView::AllAttached()
{
	fExisting->SetTarget(this);
	fNew->SetTarget(this);
	fMenu->SetTargetForItems(this);
	fNewTableName->SetTarget(this);
}


void
TableView::MessageReceived(BMessage* msg)
{
	switch(msg->what)
	{
		case EXISTING_TABLE_MSG:
		{
			fNewTableName->MakeFocus(false);
			break;
		}

		case EXISTING_TABLE_SELECTED_MSG:
		{
			fExisting->SetValue(1);
			fNewTableName->MakeFocus(false);
			break;
		}
		
		case NEW_TABLE_MSG:
		{
			fNewTableName->MakeFocus(true);
			break;
		}
		
		case NEW_TABLENAME_MODIFY_MSG:
		{
			fNew->SetValue(1);
			break;
		}
		
		default:
		{
			BView::MessageReceived(msg);
			break;
		}
	}
}


void
TableView::AddTablesToMenu()
{
	vector<BString> tables = GlobalSQLMgr->TableList();

	for(uint i = 0; i < tables.size(); i++)
	{
		fMenu->AddItem(new BMenuItem(tables[i].String(), 
			new BMessage(EXISTING_TABLE_SELECTED_MSG)));
	}
}


BString
TableView::Table()
{
	if (fNew->Value())
		return fNewTableName->Text();
	else
		return fMenu->FindMarked()->Label();
}

bool
TableView::IsNewTable()
{
	if (fNew->Value())
		return true;
	else
		return false;
}



