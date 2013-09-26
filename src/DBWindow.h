//	DBWindow.h

#ifndef DBWindow_H
#define DBWindow_H

#ifndef _WINDOW_H
#include <Window.h>
#endif


class DBWindow : public BWindow 
{
	public:
				DBWindow(BRect frame, const char* title); 
virtual	bool	QuitRequested();
};

#endif //BeAccessible_Window_H
