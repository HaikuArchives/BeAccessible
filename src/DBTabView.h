//	DBTabView.h

#ifndef DBTabView_H
#define DBTabView_H

#ifndef _VIEW_H
#include <View.h>
#endif
#include <Button.h>
#include <ListItem.h>

#include "Constants.h"
class DBListView;

class DBTabView : public BView 
{
 public:
	DBTabView(BRect frame, const char *name); 
	virtual void AttachedToWindow();
	
	DBListView* fListView;
	BButton* fOpenButton;
	BButton* fDesignButton;
	BButton* fNewButton;
};

#endif //BeAccessibleView_H
