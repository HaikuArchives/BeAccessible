#ifndef SQLITEMANAGER_H
#define SQLITEMANAGER_H

#include <String.h>
#include <List.h>
#include <vector>

#include "sqlite3.h"

class RecordSet;

//Functions used to handle SQLite errors
void DisplaySQLiteError(sqlite3* db);
BString GetErrorCodeString(sqlite3* db);

class SQLiteManager
{
	public:
		SQLiteManager();
		SQLiteManager(const SQLiteManager &);
		SQLiteManager(BString fileName);
		~SQLiteManager();
		bool Execute(const char* sql);
		bool Open(const char* filename);
		bool Close();
		int32 GetLastInsertRowId();
		bool TableExists(BString tableName);
		bool CreateNewDatabase(const char* fileName);
		bool IsOpen() const;
		bool DeleteTable(BString& table);
		void Compact();
		bool CopyTableData(BString sourceTable, BString newTable);
		bool CopyTableStructure(BString sourceTable, BString newTable);
		SQLiteManager Clone();
		std::vector<BString> TableList();
		bool ImportTable(BString file, const char* delimiter,  BString tableName, 
			std::vector<BString> &columnNames, bool ignoreFirstRow);
		RecordSet* OpenRecordSet(const char* sql);
		
	private:
		sqlite3* fDatabase;
		BString fFileName;
};

class RecordSet
{
	public:
		RecordSet(sqlite3* db, const char* sql);
		~RecordSet();
		bool Prepare();
		bool Step();
		const char* ColumnText(int col);
		const char* ColumnName(int col);
		int ColumnCount();
		void Close();	//deletes itself
		
	private:
		BString fSQL;
		sqlite3* fDB;
		sqlite3_stmt* fStmt;
};
#endif
