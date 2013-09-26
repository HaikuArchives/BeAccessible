#include <View.h>
#include <String.h>
#include <TextControl.h>
#include <RadioButton.h>
#include <Button.h>

enum { STRUCTURE_ONLY, STRUCTURE_AND_DATA };

class PasteTableView : public BView
{
	public:
		PasteTableView(BRect frame);
		virtual void Draw(BRect rect);
		virtual void MessageReceived(BMessage* msg);
		virtual void AllAttached();
		BString GetNewTableName();
		int GetPasteType();

	private:
		BTextControl* fNewTextField;
		BRadioButton* fStructOnlyButton;
		BRadioButton* fStructDataButton;
		BButton* fOkButton;
		BButton* fCancelButton;
		BBox* fBox;
};

