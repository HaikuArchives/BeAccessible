/*#include <String.h>
#include <List.h>

#include "GridView.h"
#include "TypedBList.h"
class SQLiteManager;

//Helper data structures
struct ColumnInfo
{
	BString fColumnName;
	BString fColumnAlias;
};

struct TableInfo
{
	BString fTableName;
	BString fTableAlias;
};

typedef TypedBList< TableInfo* > TableInfoList;
typedef TypedBList< ColumnInfo* > ColumnInfoList;

class QueryProperties
{
	public:
		QueryProperties();
		BString LimitZeroSQL();
		BString RowIdSQL();
		int NumTables();
		BString TableAt(int index);
	
		ColumnInfoList fColumns;
		TableInfoList fTables;
		BString fResultList;
		BString fTableList;
		BString fWhereExpr;
		BString fLimitExpr;
		BString fOrderByExpr;
		BString fOrderByType;
		BString fSQL;
};



class DataView : public GridView
{
	public:
		DataView(BRect bounds, CLVContainerView** contView, const char* name,
			QueryProperties queryProps);
		~DataView();

		//DataSet functions... perhaps make them a separate class?
		BString DataSetSQL();
		void AddItemToDataSet(void* item);
		int32 CountItemsInDataSet();
	
	private:
		void GetColumns();
		void AddColumn(BString name);
		void FetchCluster(int32 start);
		void AddRows(BString sql);
	
		QueryProperties* fQueryProps;
		SQLiteManager* fSQLMgr;
		BList fRowIdCache;
		int32 fRowIdThreadNumber;
};

//Functions used in a separate thread
int32 GetRowIds(void* data);
int GetRowIdsCallback(void *pArg, int argc, char **argv, char **columnNames);
*/