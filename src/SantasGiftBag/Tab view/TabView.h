//*** LICENSE ***
//ColumnListView, its associated classes and source code, and the other components of Santa's Gift Bag are
//being made publicly available and free to use in freeware and shareware products with a price under $25
//(I believe that shareware should be cheap). For overpriced shareware (hehehe) or commercial products,
//please contact me to negotiate a fee for use. After all, I did work hard on this class and invested a lot
//of time into it. That being said, DON'T WORRY I don't want much. It totally depends on the sort of project
//you're working on and how much you expect to make off it. If someone makes money off my work, I'd like to
//get at least a little something.  If any of the components of Santa's Gift Bag are is used in a shareware
//or commercial product, I get a free copy.  The source is made available so that you can improve and extend
//it as you need. In general it is best to customize your ColumnListView through inheritance, so that you
//can take advantage of enhancements and bug fixes as they become available. Feel free to distribute the 
//ColumnListView source, including modified versions, but keep this documentation and license with it.

//*** DESCRIPTION ***
//Tab view header file
#ifndef _SGB_TAB_VIEW_H_
#define _SGB_TAB_VIEW_H_


//******************************************************************************************************
//**** SYSTEM HEADER FILES
//******************************************************************************************************
#include <View.h>


//******************************************************************************************************
//**** ColumnListView CLASS DECLARATION
//******************************************************************************************************
class TabView : public BView
{
	public:
		//Constructor and destructor
		TabView(BRect frame, const char *name, const char** names, int32 number_of_views,
			uint32 resizingMode =  B_FOLLOW_LEFT | B_FOLLOW_TOP, uint32 flags = B_NAVIGABLE | B_WILL_DRAW |
			B_FRAME_EVENTS, const BFont *labelFont = be_plain_font);
		BRect GetContentArea();
		float GetTabsWidth();
		void AddViews(BView** tabbedViews);
		~TabView();
		virtual void Draw(BRect updateRect);
		virtual void MouseDown(BPoint point);
		virtual void FrameResized(float width, float height);
		virtual void SelectTab(int32 index);
		BView* GetCurrentTab();

	private:
		char** fNames;
		BView** fViews;
		int32 fNumberOfViews;
		float fTabViewBottom;
		BPoint* LabelPosition;
		BRect* LabelRect;
		int32 fCurrentlyShowing;
		BRect* fTabRect;
		BRect* fLabelRect;
		BPoint* fLabelPosition;
		BRect fBounds;
		BRect fContentArea;
};


#endif
