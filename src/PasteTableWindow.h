#include <Window.h>

class PasteTableView;

class PasteTableWindow : public BWindow
{
	public:
		PasteTableWindow(BRect frame, BString table);
		virtual void MessageReceived(BMessage* msg);

	private:
		PasteTableView* fPasteView;
		BString fTable;	
};

