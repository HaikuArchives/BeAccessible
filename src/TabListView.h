#include "ListView.h"


class TabListView : public BListView
{
	public:
		TabListView(BRect rect, const char* name);
		virtual void MouseDown(BPoint point);
}