/*
 * ColumnListView
 * Copyright (c) 1997-2000 Brian Tietz
 *
 * ColumnListView, its associated  classes and  source code, and the other components of
 * Santa's Gift Bag are being  made publicly available  and free to use  in freeware and
 * shareware products with a price under $25 (I believe that shareware should be cheap).
 * For  overpriced  shareware  (hehehe) or  commercial products, please  contact  me  to
 * negotiate a fee for use. After all, I did work hard on this class and  invested a lot
 * of time into it. That being  said, DON'T WORRY I don't  want much. It totally depends
 * on the sort of project you're working on and how much you  expect to  make off it. If
 * someone makes money off my work, I'd like to get at least a little something.  If any
 * of  the  components of  Santa's Gift Bag  are  is  used in a shareware or  commercial
 * product, I get a free copy. The source is  made available so that you can improve and
 * extend it as you need. In general it is best to customize your ColumnListView through
 * inheritance, so that you  can take advantage of enhancements  and bug  fixes as  they
 * become  available. Feel  free  to distribute  the  ColumnListView  source,  including
 * modified versions, but keep this documentation and license with it.
 *
 * Original code: Brian Tietz
 * Modified code: Ramshankar (Ram), Jeremy Friesner (jaf)
 *
 */

#ifndef _CLV_COLUMN_LIST_VIEW_H_
#define _CLV_COLUMN_LIST_VIEW_H_


//******************************************************************************************************
//**** SYSTEM HEADER FILES
//******************************************************************************************************
#include <ListView.h>


//******************************************************************************************************
//**** PROJECT HEADER FILES AND CLASS NAME DECLARATIONS
//******************************************************************************************************
#include "Colors.h"
#include "CLVColumn.h"
#include "CLVListItem.h"
class CLVColumnLabelView;
class CLVFillerView;
class CLVContainerView;
#include "PrefilledBitmap.h"
#include "BetterScrollView.h"


//******************************************************************************************************
//**** CONSTANTS AND TYPE DEFINITIONS
//******************************************************************************************************
typedef int (*CLVCompareFuncPtr)(const CLVListItem* item1, const CLVListItem* item2, int32 sort_key);


//******************************************************************************************************
//**** ColumnListView CLASS DECLARATION
//******************************************************************************************************
class ColumnListView : public BListView
{
	public:
		//Constructor and destructor
		ColumnListView(	BRect Frame,
						CLVContainerView** ContainerView,	//Used to get back a pointer to the container
															//view that will hold the ColumnListView, the
															//the CLVColumnLabelView, and the scrollbars.
															//If no scroll bars or border are asked for,
															//this will act like a plain BView container.
						const char* Name = NULL,
						uint32 ResizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
						uint32 flags = B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE,
						list_view_type Type = B_SINGLE_SELECTION_LIST,
						bool hierarchical = false,
						bool horizontal = true,					//Which scroll bars should I add, if any
						bool vertical = true,
						bool scroll_view_corner = true,
						border_style border = B_NO_BORDER,		//What type of border to add, if any
						const BFont* LabelFont = be_plain_font);
		virtual ~ColumnListView();

		//Archival stuff
			/*** Not implemented yet
		ColumnListView(BMessage* archive);
		static ColumnListView* Instantiate(BMessage* data);
		virtual	status_t Archive(BMessage* data, bool deep = true) const;
			***/

		// hacked in by jaf
		CLVColumnLabelView * GetColumnLabelView() {return fColumnLabelView;}

		//Column setup functions
		virtual bool AddColumn(CLVColumn* Column);			//Note that a column may only be added to
															//one ColumnListView at a time, and may not
															//be added more than once to the same
															//ColumnListView without removing it
															//inbetween
		virtual bool AddColumnList(BList* NewColumns);
		virtual bool RemoveColumn(CLVColumn* Column);
		virtual bool RemoveColumns(CLVColumn* Column, int32 Count);	//Finds Column in ColumnList
																	//and removes Count columns and
																	//their data from the view
																	//and its items
		int32 CountColumns() const;
		int32 IndexOfColumn(CLVColumn* column) const;
		CLVColumn* ColumnAt(int32 column_index) const;
		virtual bool SetDisplayOrder(const int32* Order);
			//Sets the display order: each int32 in the Order list specifies the column index of the
			//next column to display.  Note that this DOES NOT get called if the user drags a
			//column, so overriding it will not inform you of user changes.  If you need that info,
			//override DisplayOrderChanged instead.  Also note that SetDisplayOrder does call
			//DisplayOrderChanged(false).
		void GetDisplayOrder(int32* order) const;
			//Fills a caller-provided array with the display order of the columns in the same format
			//as used in calling SetDisplayOrder.  The order pointer should point to an array 
			//int32 order[n] where n is the number of columns in the ColumnListView.
		virtual void ColumnWidthChanged(int32 ColumnIndex, float NewWidth);
		virtual void DisplayOrderChanged(const int32* order);
			//Override this if you want to find out when the display order changed.
		virtual void SetSortKey(int32 ColumnIndex);
			//Set it to -1 to remove the sort key.
		virtual void AddSortKey(int32 ColumnIndex);
		void ReverseSortMode(int32 ColumnIndex);
		virtual void SetSortMode(int32 ColumnIndex,CLVSortMode Mode);
			//Override this to filter the modes
		int32 GetSorting(int32* SortKeys, CLVSortMode* SortModes) const;
			//Returns the number of used sort keys, and fills the provided arrays with the sort keys
			//by column index and sort modes, in priority order.  The pointers should point to an array 
			//int32 SortKeys[n], and an array CLVSortMode SortModes[n] where n is the number of sortable
			//columns in the ColumnListView.  Note: sorting will only occur if the key column is shown.
		void SetSorting(int32 NumberOfKeys, int32* SortKeys, CLVSortMode* SortModes);
			//Sets the sorting parameters using the same format returned by Sorting
		virtual void SortingChanged();	//Override this to detect changes to the sorting keys or modes.

		inline void SetItemSelectColor(bool window_active, rgb_color color)
		{
			if(window_active) fSelectedItemColorWindowActive = color;
			else fSelectedItemColorWindowInactive = color;
		}
		inline rgb_color ItemSelectColor(bool window_active) const
		{
			if(window_active) return fSelectedItemColorWindowActive;
			return fSelectedItemColorWindowInactive;
		}
		inline rgb_color ItemSelectColor() const
		{
			if(fWindowActive) return fSelectedItemColorWindowActive;
			return fSelectedItemColorWindowInactive;
		}

		//BView overrides
		virtual void KeyDown(const char* bytes, int32 numBytes);
		virtual void FrameResized(float Width, float Height);
 		virtual void ScrollTo(BPoint point);
 		virtual void MouseDown(BPoint point);
		virtual void MouseUp(BPoint where);
		virtual void MouseMoved(BPoint where, uint32 code, const BMessage* message);
		virtual void WindowActivated(bool active);

		//List functions
		virtual bool AddUnder(BListItem* item, BListItem* superitem);
		virtual bool AddItem(BListItem* item, int32 fullListIndex);
		virtual bool AddItem(BListItem* item);
		virtual bool AddList(BList* newItems);						//This must be a BList of
																	//CLVListItem*'s, NOT BListItem*'s
		virtual bool AddList(BList* newItems, int32 fullListIndex);	//This must be a BList of
																	//CLVListItem*'s, NOT BListItem*'s
		virtual bool RemoveItem(BListItem* item);
		virtual BListItem* RemoveItem(int32 fullListIndex);			//Actually returns CLVListItem
		virtual bool RemoveItems(int32 fullListIndex, int32 count);
		virtual void MakeEmpty();
		CLVListItem* FullListItemAt(int32 fullListIndex)  const;
		int32 FullListIndexOf(const CLVListItem* item) const;
		int32 FullListIndexOf(BPoint point) const;
		CLVListItem* FullListFirstItem() const;
		CLVListItem* FullListLastItem() const;
		bool FullListHasItem(const CLVListItem* item) const;
		int32 FullListCountItems() const;
		bool FullListIsEmpty() const;
		int32 FullListCurrentSelection(int32 index = 0) const;
		void FullListDoForEach(bool (*func)(CLVListItem*));
		void FullListDoForEach(bool (*func)(CLVListItem*, void*), void* arg2);
		CLVListItem* Superitem(const CLVListItem* item) const;
		int32 FullListNumberOfSubitems(const CLVListItem* item) const;
		virtual void Expand(CLVListItem* item);
		virtual void Collapse(CLVListItem* item);
		bool IsExpanded(int32 fullListIndex) const;
		void SetSortFunction(CLVCompareFuncPtr compare);
		void SortItems();
		virtual CLVContainerView* CreateContainer(bool horizontal, bool vertical, bool scroll_view_corner,
			border_style border, uint32 ResizingMode, uint32 flags);

		void SetTextColor(rgb_color color) { fTextColor = color; }
		rgb_color TextColor() const { return fTextColor; }

		void SetBgColor(rgb_color color) { fBgColor = color; }
		rgb_color BgColor() const { return fBgColor; }

	protected:
		friend class CLVMainView;
		friend class CLVColumn;
		friend class CLVColumnLabelView;
		friend class CLVListItem;
		

		int QuickSortCompareItems(const CLVListItem * item1, const CLVListItem * item2) const;
		static inline int QuickSortCompareItemsFunc(const void * i1, const void * i2)
		{
			const CLVListItem * item1 = (const CLVListItem *) i1;
			return item1->fSortingContextCLV->QuickSortCompareItems(item1, (const CLVListItem *)i2);
		}

		virtual void UpdateColumnSizesDataRectSizeScrollBars(bool scrolling_allowed = true);
		void ColumnsChanged();
		void EmbedInContainer(bool horizontal, bool vertical, bool scroll_view_corner, border_style border,
			uint32 ResizingMode, uint32 flags);
		void SortListArray(CLVListItem** SortArray, int32 NumberOfItems);
		void MakeEmptyPrivate();
		bool AddListPrivate(BList* newItems, int32 fullListIndex);
		bool AddItemPrivate(CLVListItem* item, int32 fullListIndex);
		void SortFullListSegment(int32 OriginalListStartIndex, int32 InsertionPoint, BList* NewList);
		BList* SortItemsInThisLevel(int32 OriginalListStartIndex);
		static int PlainBListSortFunc(BListItem** item1, BListItem** item2);
		static int HierarchicalBListSortFunc(BListItem** item1, BListItem** item2);
		void AssertWindowLocked() const;

		CLVColumnLabelView* fColumnLabelView;
		CLVContainerView* fScrollView;
		bool fHierarchical;
		BList fColumnList;
		BList fColumnDisplayList;
		BList fSortKeyList;		//List contains CLVColumn pointers
		BList fFullItemList;
		PrefilledBitmap fRightArrow;
		PrefilledBitmap fDownArrow;
		int32 fExpanderColumn;
		CLVCompareFuncPtr fCompare;
		bool fWatchingForDrag;
		BPoint fLastMouseDown;
		int32 fNoKeyMouseDownItemIndex;
		rgb_color fSelectedItemColorWindowActive;
		rgb_color fSelectedItemColorWindowInactive;
		bool fWindowActive;

		rgb_color fTextColor;
		rgb_color fBgColor;         
};


class CLVContainerView : public BetterScrollView
{
	public:
		CLVContainerView(ColumnListView* target, uint32 resizingMode, uint32 flags, bool horizontal, bool vertical,
			bool scroll_view_corner, border_style border);
		~CLVContainerView();
//		virtual void AttachedToWindow();

	private:
		friend class ColumnListView;
		bool IsBeingDestroyed;
};



#endif
