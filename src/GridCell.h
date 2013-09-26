#ifndef GridCell_h
#define Gridcell_h

#include <TextView.h>
class GridRow;


class GridCell : public BTextView   
{
	public:
		GridCell(BRect frame, const char* text, GridRow* row, int32 colNum);
		virtual void InsertText(const char* text, int32 length, int32 offset,
                                const text_run_array* runs);
		void DeleteText(int32 start, int32 finish);
		virtual void KeyDown(const char* bytes, int32 numBytes);
		virtual void MouseDown(BPoint where);
		virtual bool Modified();
		virtual void Modified(bool m);
	
	protected:
		bool fModified;
		bool fInitializeText;
		GridRow* fRow;
		int32 fColumnNumber;
};
#endif

