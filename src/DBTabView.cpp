//	DBTabView.cpp
#include <iostream>
#include <vector>

#ifndef DBTabView_H
#include "DBTabView.h"
#endif

#include <String.h>

#include "BetterScrollView.h"
#include "DBListView.h"
#include "SQLiteManager.h"

extern SQLiteManager* GlobalSQLMgr;


DBTabView::DBTabView(BRect rect, const char *name)
	   	   : BView(rect, name, B_FOLLOW_ALL_SIDES, B_FRAME_EVENTS)
{
	BRect listRect, buttonRect;
	SetViewColor(216,216,216,0);

	listRect.top += 1;
	listRect.left += 1;
	listRect.right = rect.Width() - 80 - B_V_SCROLL_BAR_WIDTH;
	listRect.bottom = rect.Height() - 1;

	BString listName(name);
	listName += "List";
	fListView = new DBListView(listRect, listName.String(), B_SINGLE_SELECTION_LIST,
	                          B_FOLLOW_ALL_SIDES, B_WILL_DRAW|B_FRAME_EVENTS|B_NAVIGABLE);
	
	buttonRect = listRect;
	buttonRect.left = buttonRect.right + 15 + B_V_SCROLL_BAR_WIDTH;
	buttonRect.right += (70 + B_V_SCROLL_BAR_WIDTH);
	buttonRect.bottom = buttonRect.top + 15;
	
	BMessage* openMsg = new BMessage(OPEN_BUTTON);
	openMsg->AddString("TabName", BString(name));
	fOpenButton = new BButton(buttonRect, NULL, "Open", openMsg,
								B_FOLLOW_RIGHT, B_WILL_DRAW|B_NAVIGABLE);

	BetterScrollView* scrollView = new BetterScrollView(NULL, fListView,
	    B_FOLLOW_ALL_SIDES, B_WILL_DRAW|B_FRAME_EVENTS, false, true, false);

	BRect newRect;
	newRect.top = 32;
	newRect.bottom = newRect.top + 15;
	newRect.right = rect.Width() - 10;
	newRect.left = rect.Width() - 65;

	BMessage* designMsg = new BMessage(DESIGN_BUTTON);
	designMsg->AddString("TabName", BString(name));
	fDesignButton = new BButton(newRect, NULL, "Design", designMsg,
								B_FOLLOW_RIGHT, B_WILL_DRAW|B_NAVIGABLE);


	newRect.top += newRect.top;

	BMessage* newMsg = new BMessage(NEW_BUTTON);
	newMsg->AddString("TabName", BString(name));
	fNewButton = new BButton(newRect, NULL, "New", newMsg,
								B_FOLLOW_RIGHT, B_WILL_DRAW|B_NAVIGABLE);

	AddChild(scrollView);
	AddChild(fOpenButton);
	AddChild(fDesignButton);		
	AddChild(fNewButton);	

	if (BString(name) == "Table")
	{
		std::vector<BString> tables = GlobalSQLMgr->TableList();
		
		for(uint i = 0; i < tables.size(); i++)
			fListView->AddItem(new BStringItem(tables[i].String()));
			
//		const char* sql = "SELECT name FROM sqlite_master WHERE type='table' ORDER BY lower(name);";
//		RecordSet* rs = GlobalSQLMgr->OpenRecordSet(sql);
//
//		while(rs->Step())
//		{
//			fListView->AddItem(new BStringItem(rs->ColumnText(0)));
//		}
//
//		rs->Close();
		
		fListView->AddItem(new BStringItem("sqlite_master"));
	}
	else if (BString(name) == "Query")
	{
		// select name from beaccessible_master where type='query' order by lower(name)
	}
	// ...
	
	fListView->MakeFocus(true);
}


void DBTabView::AttachedToWindow()
{
	SetFont(be_bold_font);
	SetFontSize(24);
}


