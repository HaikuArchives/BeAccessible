#include <Message.h>
#include <iostream.h>

#include "DelimiterView.h"
#include "PreviewView.h"

#define COMMA_DELIM_MSG 'cmdm'
#define TAB_DELIM_MSG 'tbdm'
#define PIPE_DELIM_MSG 'ppdm'
#define SEMICOLON_DELIM_MSG 'scdm'
#define OTHER_DELIM_MSG 'otdm'
#define OTHER_INVOKE_MSG 'otin'
#define OTHER_MODIFY_MSG 'otmd'

DelimiterView::DelimiterView(BRect frame, PreviewView* previewView)
: BView(frame, "DelimiterView", B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW)
{
	fPreviewView = previewView;
	
	SetViewColor(216,216,216,0);
	
	//dh = delimHeight
	//dw = delimWidth
	//ih = initialHeight
	//iw = initialWidth
	int dh = 25, dw = 85, ih=15, iw = 5;
	
	fComma = new BRadioButton(BRect(iw, ih, iw+dw, ih+dh), "CommaDelim", "Comma", new BMessage(COMMA_DELIM_MSG));
	fTab = new BRadioButton(BRect(iw, ih+dh, iw+dw, ih+dh*2), "TabDelim", "Tab", new BMessage(TAB_DELIM_MSG));
	fPipe = new BRadioButton(BRect(iw+dw, ih, iw+dw*2, ih+dh), "PipeDelim", "Pipe", new BMessage(PIPE_DELIM_MSG));
	fSemiColon = new BRadioButton(BRect(iw+dw, ih+dh, iw+dw*2, ih+dh), "SemiColonDelim", "Semi-Colon", new BMessage(SEMICOLON_DELIM_MSG));
	fOther = new BRadioButton(BRect(iw+dw*2, ih, iw+dw*2.7, ih+dh), "OtherDelim", "Other:", new BMessage(OTHER_DELIM_MSG));
	
	
	BRect rect(iw+dw*2.7, ih, iw+dw*2.9+3, ih+dh*.6);
	fOtherText = new BTextControl(rect, "OtherTextControl", NULL, NULL, new BMessage(OTHER_INVOKE_MSG));
	fOtherText->SetModificationMessage(new BMessage(OTHER_MODIFY_MSG));

	BTextView* textView = fOtherText->TextView();
	textView->SetMaxBytes(1);
	
	BRect textRect = textView->TextRect();
	textRect.left = 4; 
	textView->SetTextRect(textRect);

	AddChild(fComma);
	AddChild(fTab);
	AddChild(fPipe);
	AddChild(fSemiColon);
	AddChild(fOther);
	AddChild(fOtherText);
}


void
DelimiterView::AllAttached()
{
	fComma->SetTarget(this);
	fTab->SetTarget(this);
	fPipe->SetTarget(this);
	fSemiColon->SetTarget(this);
	fOther->SetTarget(this);
	fOtherText->SetTarget(this);
}


void
DelimiterView::MessageReceived(BMessage* msg)
{
	switch(msg->what)
	{
		case COMMA_DELIM_MSG:
		{
			fPreviewView->SetDelimiter(',');
			fOtherText->MakeFocus(false);
			break;
		}
		
		case TAB_DELIM_MSG:
		{
			fPreviewView->SetDelimiter('\t');
			fOtherText->MakeFocus(false);
			break;
		}
		
		case PIPE_DELIM_MSG:
		{
			fPreviewView->SetDelimiter('|');
			fOtherText->MakeFocus(false);
			break;
		}
		
		case SEMICOLON_DELIM_MSG:
		{
			fPreviewView->SetDelimiter(';');
			fOtherText->MakeFocus(false);
			break;
		}
	
		case OTHER_DELIM_MSG:
		{
			fOtherText->MakeFocus(true);
			break;
		}
		
		
		case OTHER_INVOKE_MSG:
		{
			cout << "OTHER_INVOKE_MSG rcvd" << endl;
			break;
		}
		
		case OTHER_MODIFY_MSG:
		{
			//Select the Other radio button when the user types something into
			//it's textbox
			fOther->SetValue(1);
			fPreviewView->SetDelimiter(fOtherText->Text()[0]);
			break;
		}
		
		default:
		{
			BView::MessageReceived(msg);
			break;
		}
	
	}
}


const char*
DelimiterView::Delimiter()
{
	if (fComma->Value())
		return ",";
	else if (fTab->Value())
		return "\t";
	else if (fPipe->Value())
		return "|";
	else if (fSemiColon->Value())
		return ";";
	else
		return fOtherText->Text();
}



