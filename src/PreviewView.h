#include <View.h>
#include <String.h>
#include <List.h>
#include <vector>

#include "ColumnListView.h"
#include "CLVColumn.h"
#include "CLVEasyItem.h"

class PreviewView : public BView
{
	public:
		PreviewView(BRect frame, BString file);
		virtual void Draw(BRect updateRect);
		void SetDelimiter(char delim);
		void SetFieldNamesOnFirstRow(bool value);
		
		int NumColumns();
		std::vector<BString> ColumnNames();
				
	private:
		void DrawPreview();
		BString GetLine(int lineNum);
	
		std::vector<BString> fPreviewLines;
		BString fDelimiter;
		bool fFieldNamesOnFirstRow;
		ColumnListView* fColumnListView;
		CLVContainerView* fContainerView;
		
};


