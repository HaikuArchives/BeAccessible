#ifndef DBListView_h
#define DBListView_h

#include <Window.h>
#include <ListView.h>
class PasteTableWindow;

class DBListView : public BListView
{
	public:
		DBListView(BRect rect, const char* name, list_view_type type,
		           uint32 resizingMode, uint32 flags);
		~DBListView();
		virtual void SelectionChanged();
		virtual void MouseDown(BPoint where);
		virtual void MessageReceived(BMessage* msg);
		virtual void KeyDown(const char* bytes, int32 numBytes);
		void Sort();
		static int Compare(const void* firstItem, const void* secondItem);
		
	private:
		bool IsTableOpen(BString name);	
		bool RenameTable(const char* oldName, int32 index, char* newName);
		void CopyTable(const char* table);
		void PasteTable(BString table);
		bool TableOnClipboard();
		BString GetTableFromClipboard();
		void ClearClipboard();
				
		PasteTableWindow* fPasteWindow;
		bool fButtonStatus;
};
#endif

