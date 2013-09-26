//	StartWindow.h

#ifndef Start_Window_H
#define Start_Window_H

#ifndef _WINDOW_H
#include <Window.h>
#endif
#include <Path.h>
#include <RadioButton.h>
#include <ListItem.h>
#include <ListView.h>


class StartWindow : public BWindow 
{
	public:
		StartWindow(BRect frame, const BList* recentFiles); 
		virtual bool QuitRequested();
		void MessageReceived(BMessage* msg);
		BStringItem* CurrentSelection();
		
	private:
		BRadioButton* newDatabase;
		BRadioButton* existingDatabase;
		BListView* dbList;
		BScrollView* scrollView;
		BButton* cancel;
		BButton* select;
};

#endif //Start_Window_H
