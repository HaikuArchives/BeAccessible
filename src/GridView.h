#ifndef GridView_h
#define GridView_h

#include "ColumnListView.h"
#include "GridConstants.h"
class ContainerView;
class GridRow;


class GridView : public ColumnListView
{
	public:
		GridView(BRect bounds, CLVContainerView** contView, const char* name);
		virtual void KeyDown(const char* bytes, int32 numBytes);
		virtual void MessageReceived(BMessage* msg);
 		
 		void MouseDown(BPoint point);
		bool DeselectAll();
		void GridSelect(RowState state, int32 index, bool extend = false);
		void GridSelect(int32 start, int32 finish, bool extend = false);
		
		virtual void AddLastRow();
		void DeleteAllRows();
		void SelectAllRows();

		virtual bool GoToRow(int32 rowNumber, int32 column);
		int32 CountSelectedItems();
		void SetClickedItem(GridRow* item);
		bool InitiateDrag(BPoint point, int32 index, bool wasSelected);
		
		float GetWidth();


	protected:
		virtual void InitializeIcons();
		void AdjustVerticalScrollBar(int32 rowNumber, int32 oldRowNumber);
		virtual void CopySelection();

		
		GridRow* clicked_item;
		PrefilledBitmap** fIconList;
		int32 fPreviousMouseMovedIndex;
		int32 fCurrentColumn;
};



// Helper class to parse plain text being pasted 
//class PasteParser
//{
//	public:
//		PasteParser(const har* pastedText);
//		int32 CountColumns();
//		int32 CountRows();
//		ColumnAt(int32 row, int32 index);
//	private:
//		BString fText;
//		BList fRows;
//};
//
#endif



