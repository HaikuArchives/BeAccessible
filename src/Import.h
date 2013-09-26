#include <String.h>
#include <List.h>

#include "SQLiteManager.h"

class Import
{
	public:
		Import();
		static bool ImportFile(BString file, char delimiter,  BString tableName, 
			bool isNewTable, int numColumns, BList* columnNames = NULL);
		
		static BList ParseLine(BString line, BString delimiter);
		static void DeleteParsedLine(BList line);
		
	private:
		static bool CreateTable(BString name, int numCols, BList* colNames);
};

