#ifndef TableWindow_h
#define TableWindow_h

#include <Window.h>
#include <Menu.h>
//#include "ColumnListView.h"
class CLVContainerView;
class TableProperties;
class ColumnDesignView;
class GridView;
class RowSummaryView;


class TableWindow : public BWindow
{
//	friend ColumnListView;

	public:
		TableWindow(int view, const char* tableName, bool newTable = false);
		virtual	void Quit();
		virtual bool QuitRequested();
		virtual void MessageReceived(BMessage* msg);
		ColumnDesignView* GetColumnDesignView();
		void RemoveContainerView();
		void ReattachContainerView();
		BString TableName();
		
		int fCurrentView;
		ColumnDesignView* fColumnDesign;
		
	private:
		void CreateDataView();
		void DestroyDataView();
		
		void CreateDesignView();
		void DestroyDesignView();

		TableProperties BuildNewTableProperties();

		bool SaveTableDesign();
		bool CreateNewTable();
		void SaveTableLayout();
		
		bool CheckForSave();
		bool PromptForTableName();
		void DisplayNoFieldsError();

		TableProperties* fProps;		
		GridView* fGridView;
		BString* fName;
		CLVContainerView* fContainerView;
		RowSummaryView* fRowSummary;
		BMenuBar* menuBar;
		BMenu* fileMenu;
		BMenu* editMenu;
		BMenu* viewMenu;
		BView* fDesignTopView;	// Top view used for DesignView b/c DesignView doesn't take
		                        // up the whole window.  DataView does, so it doesn't have
		                        // a top view
		BPicture* fOnPrimaryKey;
		BPicture* fOffPrimaryKey;
		bool fNewTable;
};
#endif
