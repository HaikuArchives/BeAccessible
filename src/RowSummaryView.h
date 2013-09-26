#include <View.h>
#include <PictureButton.h>
#include <Bitmap.h>
#include <TextView.h>
class GridView;
class GridRow;
class RowNumberDisplay;


class RowSummaryView : public BView
{
	public:
		RowSummaryView(BRect frame, const char* name, uint32 resizingMode,
		               uint32 flags);
		virtual void Draw(BRect updateRect);
		virtual void AllAttached();
		virtual void MouseDown(BPoint where);
		void SetGridView(GridView* gridView);
		void SetRowNumber(int32 rowNumber);
		void SetFiltered(bool filter);

	private:
		void SetButtonStatus(int32 rowNumber);
	
		BPictureButton* fFirstRowButton;
		BPictureButton* fPreviousRowButton;
		BPictureButton* fNextRowButton;
		BPictureButton* fLastRowButton;
		BPictureButton* fNewRowButton;
		RowNumberDisplay* fRowNumberDisplay;
		GridView* fGridView;
		bool fNewRowSelected;
		bool fIsFiltered;
		//BRect fRowTotalBox;
};


class RowNumberDisplay : public BTextView
{
	public:
		RowNumberDisplay(BRect frame, const char* name, BRect textRect,
		                        uint32 resizingMode, uint32 flags);
		virtual void KeyDown(const char* bytes, int32 numbytes);
		void SetTargetForMessages(BView* target);
		virtual void Draw(BRect updateRect);
	
	private:
		BView* fTarget;
};
