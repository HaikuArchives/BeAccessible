#ifndef TableDataView_h
#define TableDataView_h

#include "GridView.h"
#include "TableProperties.h"
#include "SQLiteManager.h"

class CLVContainerView;
class RowSummaryView;

class TableDataView : public GridView
{
	public:
		TableDataView(BRect bounds, CLVContainerView** contView, 
		              const char* name, BString sql, TableProperties* props);
		~TableDataView();
		virtual void MessageReceived(BMessage* msg);
		TableProperties* GetTableProperties();
		void SetTableProperties(TableProperties* props);
		void SetRowSummary(RowSummaryView* rowSum);
		virtual void MouseDown(BPoint where);
		virtual bool GoToRow(int32 rowNumber, int32 column);
		virtual void SelectionChanged();
		virtual void AllAttached();
	
	private:
		void AddLastRow();
		bool AddColumns(BString sql);
		bool AddRows(BString sql);
		static int AddRowsCallback(void *pArg, int argc, char **argv, char **columnNames);
		void RequeryDataView(int32 currentRow = 0);
		BString BuildSQL();
		
		BString fSQL;
		BString fFilter;
		BString fSort;
		TableProperties* fProps;
		RowSummaryView* fRowSummary;
		SQLiteManager fSQLMgr;

};


/*
**
**  Classes for Filtering
**
*/

#include <TextView.h>
class FilterForView;
class FilterForTextView;

class FilterForWindow : public BWindow
{
	public:
		FilterForWindow(BPoint point, BString column, TableDataView* parent);
		virtual void MessageReceived(BMessage* msg);

	private:
		BString fColumn;
		BView* fParent;
		FilterForView* fFilterView;
};


class FilterForView : public BView
{
	public:
		FilterForView(BRect frame);
		virtual void AllAttached();
		virtual void Draw(BRect updateRect);
		BString GetFilter();
		
	private:
		FilterForTextView* fFilterText;
};


class FilterForTextView : public BTextView
{
	public:
		FilterForTextView(BRect frame, const char* name, BRect textRect,
		  uint32 resizingMode, uint32 flags);
		virtual void KeyDown(const char* bytes, int32 numbytes);
};

#endif


