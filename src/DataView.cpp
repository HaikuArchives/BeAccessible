/*#include "DataView.h"
#include "SQLiteManager.h"

extern SQLiteManager* GlobalSQLMgr;


QueryProperties::QueryProperties() 
{

}


int
QueryProperties::NumTables()
{
	return fTables.CountItems();
}

BString
QueryProperties::TableAt(int index)
{
	//if no alias exists, return the table name
	//if (fTables.ItemAt(index)->fTableAlias.Length() == 0)
		return fTables.ItemAt(index)->fTableName;
	//otherwise return the table alias
	//else  
	//	return fTables.ItemAt(index)->fTableAlias;
}


BString
QueryProperties::RowIdSQL()
{
	BString sql("select ");

	//get rowid of each table
	int numTables = NumTables();
	for(int i=0; i < numTables; i++)
	{
		sql << TableAt(i) << ".rowid";
		
		if (i < numTables-1)
			sql << ", ";
	}
	
	sql << fTableList << " " << fWhereExpr << " " << fLimitExpr << " " <<
	       fOrderByExpr << " " << fOrderByType << ";";
	
	return sql;
}



BString
QueryProperties::LimitZeroSQL()
{
	BString sql("select * from (");
	sql << fSQL << ") as t limit 0";
	
	return sql;
}



DataView::DataView(BRect bounds, CLVContainerView** contView, const char* name,
QueryProperties queryProps)
: GridView(bounds, contView, name)
{
	fSQLMgr = GlobalSQLMgr->Clone();
	
	GetColumns();	
}

DataView::~DataView()
{

}


void
DataView::GetColumns()
{
	//Run sql with limit 0 to get the column names
	sqlite_vm* vm = fSQLMgr->Prepare(fQueryProps->LimitZeroSQL().String());
	
	int numCols, rc;
	const char** data;
	const char** columns;
	
	while ((rc = sqlite_step(vm, &numCols, &data, &columns)) != SQLITE_DONE)
	{
		if (rc == SQLITE_BUSY)
			snooze(100);
		else if (rc == SQLITE_ERROR || rc == SQLITE_MISUSE)
			break;
 		else if (rc == SQLITE_ROW)
		{
			//add each column from the results to the gridview
			for (int i=0; i < numCols; i++)
				AddColumn(columns[i]);
		}
		else
			break;
	}

	fSQLMgr->Finalize();
}


void
DataView::AddColumn(BString name)
{
	
}


//This is a callback function used by SQLite.  A row is added to the GridView
//every time this function is invoked.
//int DataView::AddRowsCallback(void *pArg, int argc, char **argv, char **columnNames)
//{
//  DataView* gridView = static_cast<DataView*>(pArg);
//	
//  if (argv)
//  {
	// Extract the ROWID.  It will be passed to DataRow separately from the
	// grid's text
//  	BString rowId(argv[0]);
//  	BString** cache = gridView->CreateCache(argc, argv);
//  	gridView->AddItem(new TableDataRow(gridView, argc-1, cache,	gridView->fIconList,rowId, &(gridView->fCurrentColumn)));
//  }
//	
//	return(0);
//}


void
DataView::FetchCluster(int32 start)
{
	const int clusterSize = 100;
	
	for(int i=0; i < fQueryProps->NumTables(); i++)
	{
		BString sql("select * from [");
		sql << fQueryProps->TableAt(i) << "] limit 100 offset 0;";
	}

//	BString sql = RowIdSQL();
//	
//	const int clusterSize = 100;
//	
//	//grab the rowid for the first clusterSize rows beginning at start, and append them
//	//to the sql statement
//	for (int i = start; i < clusterSize && i < fRowIdList.CountItems(); i++)
//	{
//		int32 rowid = *(static_cast<int*>(fRowIdList.ItemAt(i)));
//		
//		if (i == clusterSize+start-1)	//no comma
//			sql << rowid;
//		else
//			sql << rowid << ", ";
//	}
//
//	sql << ") order by rowid asc;";
//
//	fSQLMgr->Execute(sql.String(), static_cast<void*>(this), AddRowsCallback);
}


void
DataView::AddRows(BString sql)
{
	sqlite_vm* vm = fSQLMgr->Prepare(sql);
	
	int numCols, rc;
	const char** data;
	const char** columns;
	
	while ((rc = sqlite_step(vm, &numCols, &data, &columns)) != SQLITE_DONE)
	{
		if (rc == SQLITE_BUSY)
			snooze(100);
		else if (rc == SQLITE_ERROR || rc == SQLITE_MISUSE)
			break;
 		else if (rc == SQLITE_ROW)
		{
			//something really useful should go here...
		}
		else
			break;
	}

	Finalize();
}

void
DataView::AddItemToDataSet(void* item)
{
	fRowIdList.AddItem(item);
}


BString
DataView::DataSetSQL()
{
	BString sql("select ROWID from [");
	sql.Append(fTableName);
	sql.Append("];");
	
	return sql;
}

int32
DataView::CountItemsInDataSet()
{
	if (find_thread("getrowids") > B_OK)
		return -1;
	
	return fRowIdList.CountItems();
}

//
//Begin functions used in separate thread to get rowids
//

//the thread's main function
int32 
GetRowIds(void* data)
{
	SQLiteManager* sqlMgr = GlobalSQLMgr->Clone();

	TableDataView* dataSet = static_cast<TableDataView*>(data);
	
	sqlMgr->Execute(dataSet->DataSetSQL().String(), data, GetRowIdsCallback);
	
	cout << dataSet->CountItemsInDataSet() << " rowids were found." << endl;
	return 0;
}


//Callback used by sqlite
int
GetRowIdsCallback(void *pArg, int argc, char **argv, char **columnNames)
{
	TableDataView* dataSet = static_cast<TableDataView*>(pArg);

	if(argv && (argc == 1))
	{
		int32* rowid = new int32(atol(argv[0]));
		dataSet->AddItemToDataSet(static_cast<void*>(rowid));
	}
	
	return 0;
}
*/

