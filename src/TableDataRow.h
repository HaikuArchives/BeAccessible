#ifndef TableDataRow_h
#define TableDataRow_h
#include <String.h>
#include "TableDataView.h"
#include "CLVColumn.h"
#include "GridRow.h"
#include "PrefilledBitmap.h"
#include "List.h"
class GridCell;

class TableDataRow : public GridRow
{
	public:
		TableDataRow(GridView* view, int32 numArgs, BString** args, PrefilledBitmap** IconList, 
		             BString rowId, int32* currentColumn, bool last = false);
		BString GetRowID();
		
	protected:
		virtual void DrawItemColumn(BView* owner, BRect item_column_rect, int32 column_index, 
		                    bool complete);
//		virtual void DrawText(BView* owner, BRect item_column_rect, int32 column_index);
//		virtual void DrawTextBoxes(BView* owner, BRect item_column_rect, int32 column_index);

		virtual bool SaveChanges();
		virtual void Restore();
		
		BString fRowID; 			//The ROWID of the record in the database
};
#endif
