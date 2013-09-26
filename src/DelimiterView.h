#include <RadioButton.h>
#include <TextView.h>
#include <TextControl.h>

#include "View.h"

class PreviewView;

class DelimiterView : public BView
{
	public:
		DelimiterView(BRect frame, PreviewView* previewView);
		virtual void MessageReceived(BMessage* msg);
		virtual void AllAttached();
		const char* Delimiter();
		
	private:
		BRadioButton* fComma;
		BRadioButton* fTab;
		BRadioButton* fPipe;
		BRadioButton* fSemiColon;
		BRadioButton* fOther;
		BTextControl* fOtherText;
		PreviewView* fPreviewView;
};




