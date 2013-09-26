#include <View.h>
#include <RadioButton.h>
#include <PopUpMenu.h>
#include <MenuField.h>
#include <TextControl.h>

class TableView : public BView
{
	public:
		TableView(BRect frame);
		virtual void MessageReceived(BMessage* msg);
		virtual void AllAttached();
		BString Table();
		bool IsNewTable();
		
	private:
		void AddTablesToMenu();
	
		BRadioButton* fExisting;
		BRadioButton* fNew;
		BMenuField* fExistingTableName;
		BTextControl* fNewTableName;
		BPopUpMenu* fMenu;
		BMenuField* fTableMenu;
};


