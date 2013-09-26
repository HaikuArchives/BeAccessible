//	DBQueryTabView.h

#ifndef DBQueryTabView_H
#define DBQueryTabView_H

#ifndef _VIEW_H
#include <View.h>
#endif
#include <Button.h>

#include "Constants.h"
#include "DBTabView.h"

class DBQueryTabView : public DBTabView 
{
public:
	DBQueryTabView(BRect frame, const char *name); 
//	virtual void AttachedToWindow();
//	virtual void MouseDown(BPoint point);
//	virtual void MessageReceived(BMessage* message);

//	BListView* fListView;
//	BButton* fOpenButton;
	BButton* fDesignButton;
	BButton* fNewButton;

//private:
//	static int populateQueries(void *pArg, int argc, char **argv, char **columnNames);

};

#endif //BeAccessibleView_H
