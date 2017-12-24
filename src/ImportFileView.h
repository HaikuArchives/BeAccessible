#include <View.h>
#include <Box.h>
#include <List.h>
#include <Button.h>
#include <String.h>
#include <CheckBox.h>

class DelimiterView;
class TableView;
class PreviewView;

class ImportFileView : public BView
{
	public:
		ImportFileView(BRect frame, BString file);
		virtual void MessageReceived(BMessage* msg);
		virtual void AllAttached();

	private:
		bool CheckForMissingData();
		
		bool fHasFieldNames;
		BString fFile;
		BBox* fDelimiterBox;
		DelimiterView* fDelimiterView;
		BBox* fTableBox;
		TableView* fTableView;
		BBox* fColumnView;
		BCheckBox* fColumnsCheckBox;
		BBox* fPreviewBox;
		PreviewView* fPreviewView;
		BButton* fImportButton;
		BButton* fCancelButton;
};




