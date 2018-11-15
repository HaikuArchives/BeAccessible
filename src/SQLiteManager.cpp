#include <iostream>

#include <Alert.h>
#include <Locker.h>

#include "ImportUtilities.c"
#include "SQLiteManager.h"
#include "TableProperties.h"


SQLiteManager::SQLiteManager()
{
	fDatabase = NULL;
}


SQLiteManager::SQLiteManager(BString fileName)
{
	fDatabase = NULL;
	Open(fileName.String());
}

SQLiteManager::SQLiteManager(const SQLiteManager &copy)
{
	fDatabase = NULL;
	
	//Close this database if it's open
	if (IsOpen())
		Close();

	if (copy.IsOpen())
		Open(copy.fFileName.String());

	//if copy is not open, then there's nothing to be done
}

SQLiteManager::~SQLiteManager()
{
	std::cout << "~SQLiteManager() called(" << this << ")" << std::endl;
	
	if(IsOpen())
		Close();
		
	fDatabase = NULL;
}


bool
SQLiteManager::CreateNewDatabase(const char* fileName)
{
	//Note, sqlite will create a new database if you call open w/ a filename
	//that doesn't exist.
	return Open(fileName);		
}


//This executes a series of sql statements.  It should be used just for 
//insert/update/delete queries.
bool
SQLiteManager::Execute(const char* sql)
{
	BLocker lock;
	lock.Lock();

	char* message = 0;
	std::cout << "SQL = " << sql << std::endl;
	
	int rc = sqlite3_exec(fDatabase, sql, NULL, NULL, &message);

	if (rc != SQLITE_OK)
	{
		if (message)
			sqlite3_free(message);
		
		DisplaySQLiteError(fDatabase);
		lock.Unlock();
		return false;	
	}
	
	lock.Unlock();
	return true;
}



bool
SQLiteManager::Open(const char* filename)
{
	fFileName = filename;
	
	int rc = sqlite3_open(filename, &fDatabase);

	if (rc != SQLITE_OK)
	{
		DisplaySQLiteError(fDatabase);
		return false;
	}
	
	return true;
}


bool
SQLiteManager::Close()
{
	if (IsOpen())
	{
		int rc = sqlite3_close(fDatabase);
		
		if (rc != SQLITE_OK)
		{
			DisplaySQLiteError(fDatabase);
			return false;
		}
		
	   	fDatabase = NULL;
	   	fFileName.SetTo("");
	   	return true;
	}
	
	//no database was open
	return false;
}


int32 
SQLiteManager::GetLastInsertRowId()
{
	return sqlite3_last_insert_rowid(fDatabase);
}


bool
SQLiteManager::TableExists(BString tableName)
{
	bool exists = false;
	
	char* sql = sqlite3_mprintf("SELECT name FROM sqlite_master WHERE type = 'table' "
								"AND name = %Q;", tableName.String());
	
	RecordSet* rs = OpenRecordSet(sql);
	rs->Step();
	
	if (rs->ColumnText(0) != NULL)
		exists = true;
	else
		exists = false;
	
	rs->Close();
	sqlite3_free(sql);

	return exists;
}


bool
SQLiteManager::IsOpen() const
{
	if (fDatabase)
		return true;
	else
		return false;
}

void
SQLiteManager::Compact()
{
	Execute("VACUUM;");
}


bool
SQLiteManager::DeleteTable(BString& table)
{
	//This is not wrapped inside a transaction b/c the calling code
	//may already be in one.
	BString sql;
	sql << "DROP TABLE [" << table << "]; " <<
		"DELETE FROM beaccessible_master where name = '" << table << 
		"' and type = 'table_preferences';";

	return Execute(sql.String());;
}


bool
SQLiteManager::CopyTableData(BString sourceTable, BString newTable)
{
	BString sql("BEGIN EXCLUSIVE; ");
	
	TableProperties props(sourceTable);
	sql += props.CreateTableSQL(newTable);
	
	sql << "INSERT INTO [" << newTable << "] SELECT * FROM [" << sourceTable << "]; ";
	sql << "COMMIT;";
	
	bool rc = Execute(sql.String());
	
	if (!rc)
		Execute("COMMIT;");
	
	return rc;
}


bool
SQLiteManager::CopyTableStructure(BString sourceTable, BString newTable)
{
	TableProperties props(sourceTable);
	return Execute(props.CreateTableSQL(newTable).String());
}

std::vector<BString>
SQLiteManager::TableList()
{
	std::vector<BString> tables;
	
	const char* sql = "select name from sqlite_master where type = 'table' "
		              "except select 'beaccessible_master' order by name asc;";

	RecordSet* rs = OpenRecordSet(sql);
	
	while(rs->Step())
	{
		tables.push_back(rs->ColumnText(0));
	}	

	rs->Close();

	return tables;
}


bool
SQLiteManager::ImportTable(BString file, const char* delimiter,  BString tableName, 
	std::vector<BString> &columnNames, bool ignoreFirstRow)
{
	char separator[20];
	strcpy(separator, delimiter);
	const char *zFile = file.String();     /* The file from which to extract data */
	sqlite3_stmt *pStmt;        /* A statement */
	int rc;                     /* Result code */
	int nCol;                   /* Number of columns in the table */
	int i;                   /* Loop counters */
	int nSep;                   /* Number of bytes in separator[] */
	BString zSQL;                 /* An SQL statement */
	char *zLine;                /* A single line of input from the file */
	char **azCol;               /* zLine[] broken up into columns */
	char *zCommit;              /* How to commit changes */   
	FILE *in;                   /* The input file */
	int lineno = 0;             /* Line number of input file */
	
	nSep = strlen(separator);
	if( nSep==0 ){
	  fprintf(stderr, "non-null separator required for import\n");
	  return false;
	}
	nCol = columnNames.size();
	if( nCol==0 ) return false;

	zSQL << "INSERT INTO [" << tableName << "] ([" << columnNames[0] << "]";
	
	for(int i = 1; i < nCol; i++)
		zSQL << ", [" << columnNames[i] << "]";
	
	zSQL << ") VALUES (?";
	
	for(int i = 1; i < nCol; i++)
		zSQL << ", ?";
	
	zSQL << ")";

	rc = sqlite3_prepare(fDatabase, zSQL.String(), 0, &pStmt, 0);
	if( rc ){
	  DisplaySQLiteError(fDatabase);
	  sqlite3_finalize(pStmt);
	  return false;
	}
	in = fopen(zFile, "rb");
	if( in==0 ){
	  fprintf(stderr, "cannot open file: %s\n", zFile);
	  sqlite3_finalize(pStmt);
	  return false;
	}
	azCol = (char**)malloc( sizeof(azCol[0])*(nCol+1) );
	if( azCol==0 ) return 0;
	sqlite3_exec(fDatabase, "BEGIN", 0, 0, 0);
	zCommit = "COMMIT";
	while( (zLine = local_getline(0, in))!=0 ){
	  char *z;
	  i = 0;
	  lineno++;
	  if (lineno == 1 && ignoreFirstRow)
	  	continue;
	  azCol[0] = zLine;
	  for(i=0, z=zLine; *z && *z!='\n' && *z!='\r'; z++){
	    if( *z==separator[0] && strncmp(z, separator, nSep)==0 ){
	      *z = 0;
	      i++;
	      if( i<nCol ){
	        azCol[i] = &z[nSep];
	        z += nSep-1;
	      }
	    }
	  }
	  if( i+1!=nCol ){
	  	//display error
	  	BString errorMsg;
	  	errorMsg << zFile << " line " << lineno << ": expected " << nCol << 
	  		" columns of data but found " << i+1;
	  	(new BAlert("Error", errorMsg.String(), "Ok"))->Go();
	    zCommit = "ROLLBACK";
	    break;
	  }
	  for(i=0; i<nCol; i++){
	    sqlite3_bind_text(pStmt, i+1, azCol[i], -1, SQLITE_STATIC);
	  }
	  sqlite3_step(pStmt);
	  rc = sqlite3_reset(pStmt);
	  free(zLine);
	  if( rc!=SQLITE_OK ){
	    fprintf(stderr,"Error: %s\n", sqlite3_errmsg(fDatabase));
	    zCommit = "ROLLBACK";
	    break;
	  }
	}
	free(azCol);
	fclose(in);
	sqlite3_finalize(pStmt);
	sqlite3_exec(fDatabase, zCommit, 0, 0, 0);

	if (strcmp(zCommit, "ROLLBACK") == 0)
		return false;
	else
		return true;
}



RecordSet*
SQLiteManager::OpenRecordSet(const char* sql)
{
	return new RecordSet(fDatabase, sql);
}

SQLiteManager
SQLiteManager::Clone()
{
	return *this;
}


/*
**
**  Functions used to display SQLite error messages
**
*/

void DisplaySQLiteError(sqlite3* db)
{
	BString errorMsg;

	errorMsg << sqlite3_errmsg(db) << " (" << GetErrorCodeString(db) << ":" <<
		sqlite3_errcode(db) << ")";
	
	(new BAlert("Error", errorMsg.String(), "Ok"))->Go();	
}


BString GetErrorCodeString(sqlite3* db)
{
	switch(sqlite3_errcode(db))
	{
		case SQLITE_OK:
			return "SQLITE_OK";
			break;
			
		case SQLITE_ERROR:
			return "SQLITE_ERROR";
			break;
			
		case SQLITE_INTERNAL:
			return "SQLITE_INTERNAL";
			break;
			
		case SQLITE_PERM:
			return "SQLITE_PERM";
			break;
			
		case SQLITE_ABORT:
			return "SQLITE_ABORT";
			break;
			
		case SQLITE_BUSY:
			return "SQLITE_BUSY";
			break;
			
		case SQLITE_LOCKED:
			return "SQLITE_LOCKED";
			break;
			
		case SQLITE_NOMEM:
			return "SQLITE_NOMEM";
			break;
			
		case SQLITE_READONLY:
			return "SQLITE_READONLY";
			break;
			
		case SQLITE_INTERRUPT:
			return "SQLITE_INTERRUPT";
			break;
			
		case SQLITE_IOERR:
			return "SQLITE_IOERR";
			break;
			
		case SQLITE_CORRUPT:
			return "SQLITE_CORRUPT";
			break;
			
		case SQLITE_NOTFOUND:
			return "SQLITE_NOTFOUND";
			break;
			
		case SQLITE_FULL:
			return "SQLITE_FULL";
			break;
			
		case SQLITE_CANTOPEN:
			return "SQLITE_CANTOPEN";
			break;
			
		case SQLITE_PROTOCOL:
			return "SQLITE_PROTOCOL";
			break;
			
		case SQLITE_EMPTY:
			return "SQLITE_EMPTY";
			break;
			
		case SQLITE_SCHEMA:
			return "SQLITE_SCHEMA";
			break;
			
		case SQLITE_TOOBIG:
			return "SQLITE_TOOBIG";
			break;
			
		case SQLITE_CONSTRAINT:
			return "SQLITE_CONSTRAINT";
			break;
			
		case SQLITE_MISMATCH:
			return "SQLITE_MISMATCH";
			break;
			
		case SQLITE_MISUSE:
			return "SQLITE_MISUSE";
			break;
			
		case SQLITE_NOLFS:
			return "SQLITE_NOLFS";
			break;
			
		case SQLITE_AUTH:
			return "SQLITE_AUTH";
			break;
			
		case SQLITE_ROW:
			return "SQLITE_ROW";
			break;
			
		case SQLITE_DONE:
			return "SQLITE_DONE";
			break;
		default:
			return "Unrecognized Error";
			break;
	}
}


/*
**
**  RecordSet class
**
*/


RecordSet::RecordSet(sqlite3* db, const char* sql)
: fSQL(sql)
{
	fStmt = NULL;
	fDB = db;
	
	Prepare();
}

RecordSet::~RecordSet()
{

}

bool
RecordSet::Prepare()
{
	int rc = sqlite3_prepare(fDB, fSQL.String(), fSQL.Length(), &fStmt, NULL);
	
	std::cout << "SQL = " << fSQL.String() << std::endl;
	
	if (rc != SQLITE_OK)
	{
		DisplaySQLiteError(fDB);
		return false;
	}
	
	return true;
}


bool
RecordSet::Step()
{
	int rc = sqlite3_step(fStmt);
	
	if (rc == SQLITE_ROW)
		return true;
	else if (rc == SQLITE_DONE)
		return false;
	else 
	{
		if (strcmp(sqlite3_errmsg(fDB), "database schema has changed") == 0)
		{
			Prepare();
			return Step();
		}
		else
		{
			DisplaySQLiteError(fDB);
			return false;
		}
	}
}

const char*
RecordSet::ColumnText(int col)
{
	if (sqlite3_column_type(fStmt, col) == SQLITE_NULL)
		return NULL;
	else
		return (const char*)sqlite3_column_text(fStmt, col);
}

const char*
RecordSet::ColumnName(int col)
{
	return sqlite3_column_name(fStmt, col);
}

int
RecordSet::ColumnCount()
{
	return sqlite3_column_count(fStmt);
}

void
RecordSet::Close()
{
	int rc = sqlite3_finalize(fStmt);

	if (rc != SQLITE_OK)
	{
		DisplaySQLiteError(fDB);
	}
	
	delete this;
}

