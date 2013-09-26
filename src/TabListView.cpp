#include "TabListView.h"



TabListView::TabListView(BRect rect, const char* name)
	: BListView(rect, name, B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES,
	            B_WILL_DRAW|B_FRAME_EVENTS)
{

}


void
TabListView::MouseDown(BPoint point)
{
	
}
