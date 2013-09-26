//	DBTableTabView.h

#ifndef DBTableTabView_H
#define DBTableTabView_H

#ifndef _VIEW_H
#include <View.h>
#endif
#include <Button.h>

#include "TableWindow.h"
#include "Constants.h"
#include "DBTabView.h"

class DBTableTabView : public DBTabView 
{
 public:
	DBTableTabView(BRect frame, const char *name); 
//	virtual void AttachedToWindow();
//	virtual void MouseDown(BPoint point);
//	virtual void MessageReceived(BMessage* message);

//	BListView* fListView;



};

#endif //BeAccessibleView_H
