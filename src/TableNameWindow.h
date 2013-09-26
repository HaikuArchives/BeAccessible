//	TableNameWindow.h

#ifndef TableNameWindow_H
#define TableNameWindow_H

#ifndef _WINDOW_H
#include <Window.h>
#endif


class TableNameWindow : public BWindow 
{
	public:
		TableNameWindow(const char* title, const char* table, int mode, BWindow* parent); 
		virtual bool QuitRequested();
};

#endif //TableNameWindow_H