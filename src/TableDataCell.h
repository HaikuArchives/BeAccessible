#ifndef TableDataCell_h
#define TableDataCell_h
#include "GridCell.h"
#include <PopUpMenu.h>
#include <MenuItem.h>

class TableDataCell : public GridCell
{
	public:
		TableDataCell(BRect frame, const char* text, GridRow* row, 
		              int32 colNum);
		virtual void MouseDown(BPoint where);
};


#endif


