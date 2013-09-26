//	BeAccessibleView.h

#ifndef BeAccessibleView_H
#define BeAccessibleView_H

#ifndef _VIEW_H
#include <View.h>
#endif
//#include "TestView.h"

class BeAccessibleView : public BView 
{
public:
	BeAccessibleView(BRect frame, const char *name); 
	virtual void AttachedToWindow();
	virtual void Draw(BRect updateRect);
	virtual void MouseDown(BPoint point);
	BTextView* a;
	BTextView* b;
	BTextView* c;
	BRect GetTextBounds(BRect frame);
};

#endif //BeAccessibleView_H
