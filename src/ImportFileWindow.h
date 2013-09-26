#include <Window.h>
#include <String.h>

class ImportFileView;

class ImportFileWindow : public BWindow
{
	public:
		ImportFileWindow(BRect frame, BString file);
		virtual void MessageReceived(BMessage* msg);

	private:
		ImportFileView* fImportFileView;
		BString fFile;	
};

