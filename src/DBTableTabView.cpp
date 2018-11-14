//	DBTableTabView.cpp

#ifndef DBTableTabView_H
#include "DBTableTabView.h"
#endif
#include <ListView.h>
#include <String.h>
#include <iostream>
#include "Beep.h"

#include "BeAccessibleApp.h"
#include "Constants.h"

DBTableTabView::DBTableTabView(BRect rect, const char *name)
	   	   : DBTabView(rect, name, "SELECT name FROM sqlite_master WHERE "
	   	                           "type='table' ORDER BY lower(name);")
{
	//Debug_Info(METHOD_ENTRY, "DBTableTabView::DBTableTabView", NULL);
	
					
								
//	BRect newRect;
//	SetViewColor(216,216,216,0);
//
//	newRect.right = rect.Width() - 80;
//	newRect.bottom = rect.Height();
//
//	fListView = new BListView(newRect, NULL, B_SINGLE_SELECTION_LIST,
//	                          B_FOLLOW_ALL_SIDES, B_WILL_DRAW|B_FRAME_EVENTS);
//
//	newRect.left = newRect.right + 15;
//	newRect.right += 70;
//	newRect.bottom = newRect.top + 15;
//	
//	fOpenButton = new BButton(newRect, NULL, "Open", new BMessage(OPEN_TABLE_BUTTON),
//								B_FOLLOW_RIGHT, B_WILL_DRAW|B_FULL_UPDATE_ON_RESIZE);
//
//	AddChild(fListView);
//	AddChild(fOpenButton);
//
//	const char* sql = "SELECT name FROM _BSysTables;";
//	((BeAccessibleApp*)be_app)->ExecuteCommand(sql, (void*)fListView, populateTables);

}

//void DBTableTabView::AttachedToWindow()
//{
//	SetFont(be_bold_font);
//	SetFontSize(24);
//}
//
//void DBTableTabView::MouseDown(BPoint point)
//{
//	beep();
//}
//
//
//void DBTableTabView::MessageReceived(BMessage* message)
//{
//	switch(message->what)
//	{
//		default:
//			break;
//	}
//}
//
//
//int DBTableTabView::populateTables(void *pArg, int argc, char **argv, 
//                                   char **columnNames)
//{
//	if (argv)
//	{
//		BListView *listView = (BListView*)pArg;
//		listView->AddItem(new BStringItem(argv[0]));
//	}
//	return(0);
//}

