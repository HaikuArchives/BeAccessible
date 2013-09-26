#ifndef TableDesignView_h
#define TableDesignView_h
#include <List.h>

#include "GridView.h"
class TableProperties;

class TableDesignView : public GridView
{
	public:
		TableDesignView(BRect bounds, CLVContainerView** contView, 
		              const char* name, TableProperties* props);
		~TableDesignView();
		virtual void MessageReceived(BMessage* msg);
		TableProperties* GetTableProperties();
		void SetTableProperties(TableProperties* props);
		BList* GetOriginalColumnOrder();
				
	private:
		TableProperties* fProps;
		// Used to keep track of the original placement of columns in a table, in case
		// the user reorders the columns.
		BList* fOrigColumnOrder;
};
#endif

