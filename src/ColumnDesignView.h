// ColumnDesignView.h

#ifndef ColumnDesignView_H
#define ColumnDesignView_H

#ifndef _VIEW_H
#include <View.h>
#endif
#include <Button.h>
#include <MenuField.h>
#include <TextControl.h>
#include <Picture.h>
#include <PictureButton.h>
#include <PopUpMenu.h>

#include "Constants.h"
class DBListView;
class ColumnProperties;
class ColumnListView;
class MyTextControl;

#define TEXT 1
#define NUMBER 2
#define AUTONUMBER 3
#define DATETIME 4
#define BOOLEAN 5

class ColumnDesignView : public BView 
{
  public:
	ColumnDesignView(ColumnListView* fColumnListView, BRect frame, const char *name, 
	                 BPicture* on, BPicture* off); 
	virtual void Draw(BRect rect);
	void DisplayColumnProperties(int32 columnNumber, bool forceRedraw = false);
	BString DefaultValue();
	bool HasIndex();
	bool UniqueIndex();
	bool Required();
	
  private:
  	ColumnListView* fColumnListView;
	int32 fCurrentColumnNumber;
	BPopUpMenu* fIndexMenu;
  	BMenuField* fIndexMenuField;
	BMenuItem* fNoIndex;
	BMenuItem* fDuplicateIndex;
	BMenuItem* fUniqueIndex;
	BPicture* fOnPrimaryKey;
	BPicture* fOffPrimaryKey;
	BPictureButton* fPrimaryKeyButton;
  	BTextControl* fDefaultValueBox;
	BPopUpMenu* fRequiredMenu;
	BMenuField* fRequiredMenuField;
	BMenuItem* fRequired;
	BMenuItem* fNotRequired;
	
	int GetConstant(BString* type);  
	void AddPrimaryKeyOptions(ColumnProperties* columnProps);
	void AddIndexOptions(ColumnProperties* columnProps);
	void AddDefaultValueBox(ColumnProperties* columnProps);
	void AddRequiredOptions(ColumnProperties* columnProps);
	void DrawTextOptions();
	void DrawNumberOptions();
	void DrawBooleanOptions();
	void DrawDateTimeOptions();

};

//
//class MyTextControl : public BTextControl
//{
//	public:
//		MyTextControl(BRect box, const char *name, const char *label, const char *text,
//	    	          BMessage* message);
//		status_t Invoke(BMessage* message = NULL);
//		void Draw(BRect updateRect);
//		void SetCurrentColumn(int32 column);
//		
//	
//	private:
//		int32 fCurrentColumn;	
//};
//
#endif
