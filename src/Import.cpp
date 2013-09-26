#include <stdio.h>
#include <stdlib.h>

#include "Import.h"
#include "ImportUtilities.c"

extern SQLiteManager* GlobalSQLMgr;



bool
Import::ImportFile(BString file, char delimiter,  BString tableName, 
	bool isNewTable, int numColumns, BList* columnNames = NULL)
{
	if (isNewTable)
	{
		bool result = CreateTable(tableName, numColumns, columnNames);

		if (!result)
			return false;
	}

	file.CharacterEscape("'", '\'');
	tableName.CharacterEscape("'", '\'');

	BString sql("copy [");
	sql << tableName << "] from [" << file << "] using delimiters '" << delimiter << "';";
	
	SQLiteManager db = GlobalSQLMgr->Clone();
	db.Execute(sql.String());
	
	db.Close();
}

bool
Import::CreateTable(BString name, int numColumns, BList* columns)
{
	SQLiteManager db = GlobalSQLMgr->Clone();

	if (columns)
	{
		if (columns->CountItems() == 0)
			return false;
		
		//create table w/ given column names
		BString sql("create table [");
		sql << name << "] ([" << *((BString*)columns->ItemAt(0)) << "] text";
		
		for(int i = 1; i < columns->CountItems(); i++)
			sql << ", [" << *((BString*)columns->ItemAt(i)) << "] text";

		sql << ");";
		
		db.Execute(sql.String());
	}
	else
	{
		if (numColumns == 0)
			return false;
		
		//create table w/ default column names
		BString sql("create table [");
		sql << name << "] ([field1] text";
		
		for(int i = 1; i < numColumns; i++)
			sql << ", [field" << i+1 << "] text";
		
		sql << "}";
		
		db.Execute(sql.String());
	}
}



BList
Import::ParseLine(BString line, BString delimiter)
{
	int32 index;
	int32 prevIndex = 0;
	BList parsedLine;
	
	if (delimiter.Length() == 0)
	{
		BString* text = new BString();
		line.CopyInto(*text, 0, line.CountChars());
		parsedLine.AddItem((void*)text);
		return parsedLine;
	}
	
	
	while((index = line.FindFirst(delimiter, prevIndex)) != B_ERROR)
	{
		BString* colName = new BString();
		line.CopyInto(*colName, prevIndex, index - prevIndex);
		colName->RemoveAll(delimiter);
		
		parsedLine.AddItem((void*)colName);
			
		prevIndex = index+1;
	}
	
	//There should be 1 column name left now: between the last delimiter and
	//the newline.  Parse it out and add it as a column
	BString* colName = new BString();
	line.CopyInto(*colName, prevIndex, line.CountChars() - prevIndex);
	parsedLine.AddItem((void*)colName);

	return parsedLine;
}

void
Import::DeleteParsedLine(BList line)
{
	for(int i = 0; i < line.CountItems(); i++)
	{
		BString* text = (BString*)line.ItemAt(i);
		delete text;
	}
}

