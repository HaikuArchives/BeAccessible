//	DBWindow.cpp

#ifndef _APPLICATION_H
#include <Application.h>
#endif
#ifndef DBWindow_H
#include "DBWindow.h"
#endif
#ifndef DBTableTabView_H
#include "DBTableTabView.h"
#endif
#include <ListView.h>

#include "DBQueryTabView.h"
#include "BeAccessibleApp.h"
#include "TabView.h"
#include "sqlite.h"
#include "Constants.h"

#define TABLES 0
#define QUERIES 1

DBWindow::DBWindow(BRect frame, const char* title)
				: BWindow(frame, title, B_FLOATING_WINDOW_LOOK, 
						  B_FLOATING_SUBSET_WINDOW_FEEL, 0)
{

}

bool DBWindow::QuitRequested()
{
	be_app->PostMessage(CLOSE_DB_REQUESTED);
	return (true);
}
