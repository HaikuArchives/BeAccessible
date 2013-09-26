#ifndef GridRow_h
#define GridRow_h

#include <String.h>
#include "GridView.h"
#include "CLVColumn.h"
#include "CLVListItem.h"

#include "PrefilledBitmap.h"
#include "List.h"
class GridCell;
class GridView;


class GridRow : public CLVListItem
{
	public:
		GridRow(GridView* view, int32 numArgs, BString** args, PrefilledBitmap** IconList, 
		        int32* currentColumn, bool last = false);
		virtual ~GridRow();
		virtual void Exit(bool doInvalidate);
		virtual bool ExitRequested();
		bool IsLast();
		void SetLast(bool last);
		void InvalidateRow();
		void SetColumnClicked(int32 column);
		void SetColumnClicked(BPoint where);		
		void SetModified(bool status);
		bool Modified();
		virtual void PreviousField(int32 currentColumn);
		virtual void NextField(int32 currentColumn);
		void NextRow(int32 column);
		void PreviousRow(int32 column);
		virtual void Restore();		
		virtual void RightClickMenu();		
		const char* GetContentAt(int32 column);
		int32 CurrentColumn();
		virtual BString ToString();
		BRect ColumnRect(int column);
		void SetState(RowState newState);
		RowState State();
		GridView* Parent();
		
	protected:
		void DrawItemColumn(BView* owner, BRect item_column_rect, int32 column_index, 
		                    bool complete);
		void InitializeCache(int32 numArgs, BString** args);
		void ColumnWidthChanged(int32 column_index, float column_width, ColumnListView *the_view);
		void PrintToStream();
		virtual bool SaveChanges();
		void AddRow(int32 column, bool makeFocus = true);
		BRect GetRowSize();
		void SetIcon(int icon);		

	
		bool fModified;
		int32 fCurrentIcon;
		GridView* fView;
		BString** fText;
		GridCell** fCell;
		int32 fNumColumns;
		float fTextOffset;
		PrefilledBitmap** fIconList;
		BList fCachedRects;
		bool fLast;
		bool fHighlightText;
		int32* fCurrentColumn;
		BPoint fPointClicked;
		RowState fState;
};
#endif
