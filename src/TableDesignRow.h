#ifndef TableDesignRow_h
#define TableDesignRow_h

#include <String.h>
#include "ColumnListView.h"
#include "CLVColumn.h"

#include "GridRow.h"
#include "PrefilledBitmap.h"
#include "List.h"
#include "PopUpMenu.h"
#include "TableProperties.h"

class GridCell;
class MyPopUpMenu;

class TableDesignRow : public GridRow
{
	public:
		//The ColumnProperties structure is passed by-value.  This is b/c the properties
		//will be changed by the user, which will be reflected in the local properties
		//structure in this class, not in original properties structure in the Window class.
		TableDesignRow(GridView* view, ColumnProperties props, PrefilledBitmap** IconList, 
		               BString** args, int32* fCurrentColumn, bool last = false);
		~TableDesignRow();
		void DrawItemColumn(BView* owner, BRect item_column_rect, int32 column_index, 
		                    bool complete);

		virtual void Exit(bool doInvalidate);
		void PrintToStream();
		void PreviousField(int32 currentColumn);
		void NextField(int32 currentColumn);
		bool SaveChanges();
		void NextRow(int32 column);
		void PreviousRow(int32 column);
		void AddRow(int32 column, bool makeFocus = true);
		void RightClickMenu();		
		void SaveColumnProperties();
		void DeleteMenuBar();
		ColumnProperties* fColumnProps;
		virtual BString ToString();
		
	private:
		void PopulateDataTypeMenu();
		void DrawIcon(BRect item_column_rect, BView* owner);

		BMenuBar* fMenuBar;
		BPopUpMenu* fPopUpMenu;
};
#endif
