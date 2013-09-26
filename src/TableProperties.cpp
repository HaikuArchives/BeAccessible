#include <stdlib.h>
#include <ctype.h>
#include <Alert.h>
#include <Debug.h>
#include <Message.h>

#include "TableProperties.h"
#include "ColumnProperties.h"
#include "IndexProperties.h"
#include "Constants.h"
#include "SQLiteManager.h"

extern SQLiteManager* GlobalSQLMgr;

enum {COLUMN_NUMBER, NAME, DATA_TYPE, REQUIRED, DEFAULT_VALUE, PRIMARY_KEY};


TableProperties::TableProperties()
: fTableName(""),
  fColumns()
{

}

TableProperties::TableProperties(BString tableName) 
: fTableName(tableName),
  fColumns()
{
	GetColumnInfo();
	GetIndexList();
	
	//The column types returned by SQLite could be anything; since it's typeless,
	//it places no restriction on what the column types are.  Thus, we'll try to
	//convert the column types returned by SQLite into one of the predefined types
	//used by BeAccessible (which currently are AutoNumber, Number, DateTime, Boolean,
	//and Text.
	ConvertDataTypes();

	//Create a mapping between column names and their column properties structure
	for(int i = 0; i < fColumns.CountItems(); i++)
	{
		ColumnProperties* col = static_cast<ColumnProperties*>(fColumns.ItemAt(i));
		fColumnMapping.fMapping[col->Name()] = col;
	}

	//Get the table_preferences XML for the given table
	GetTableXML();
}



// This is the copy constructor.  We can't just use the BList's copy constructor
// here, because it only copies the pointers, not the data they point to.  So here
// we create a new BList and copy the data into this new BList.

TableProperties::TableProperties(const TableProperties &copy)
{
	fTableName = copy.fTableName;

	for(int i = 0; i < copy.fColumns.CountItems(); i++)
	{
		ColumnProperties* copyColumn = (ColumnProperties*)copy.fColumns.ItemAt(i);
		ColumnProperties* newColumn = new ColumnProperties(*copyColumn);
		fColumns.AddItem((void*)newColumn);
	}
}


TableProperties::~TableProperties()
{
	ColumnProperties* item;
	for(int32 k = 0; (item = (ColumnProperties*)fColumns.ItemAt(k)); k++)
		delete item;
}


bool
TableProperties::operator==(const TableProperties &right) const
{
	//FYI, we don't compare the table names between the two objects here, just the
	//column properties.  Why?  Consider the case where a user is creating a new table.
	//The table name may not have been entered yet, so all we can compare is the 
	//column properties.

	if (fColumns.CountItems() != right.fColumns.CountItems())
		return false;
  
	for(int i = 0; i < fColumns.CountItems(); i++)
	{
		ColumnProperties* leftColumn = (ColumnProperties*)fColumns.ItemAt(i);
		ColumnProperties* rightColumn = (ColumnProperties*)right.fColumns.ItemAt(i);
  	
  		if (*leftColumn != *rightColumn)
  			return false;
	}
  
  return true;
}


void
TableProperties::ConvertDataTypes()
{
	for(int32 i = 0; i < fColumns.CountItems(); i++)
	{
		ColumnProperties* column = (ColumnProperties*)fColumns.ItemAt(i);
		
		if ((column->Type().IFindFirst("TEXT") != B_ERROR) ||
		    (column->Type().IFindFirst("TEXT") != B_ERROR))
		{
			column->SetType("Text");
		}
		else if ((column->Type().IFindFirst("DATE") != B_ERROR) ||
	 		     (column->Type().IFindFirst("TIME") != B_ERROR))
		{
			column->SetType("DateTime");
		}
		else if (column->Type().IFindFirst("BOOL") != B_ERROR)
		{
			column->SetType("Boolean");
		}
		else if (column->Type() != "")
		{
			//Any other data type will be considered either a Number or Autonumber.
			if (column->AutoNumber())
			{
				column->SetType("AutoNumber");
			}
			else
			{
				column->SetType("Number");
			}
		}
	} // end for loop
}


//Returns a comma separated list of the columns in the table
BString
TableProperties::ColumnNames()
{
	BString names;

	for (int i = 0; i < fColumns.CountItems(); i++)
	{
		names += "[";
		names += ((ColumnProperties*)fColumns.ItemAt(i))->Name();
		names += "]";
		
		if (i+1 < fColumns.CountItems())
			names += ", ";
	}
	
	return names;
}




int
TableProperties::CountColumns()
{
	return fColumns.CountItems();
}

void
TableProperties::Print()
{
	for(int i = 0; i < fColumns.CountItems(); i++)
	{
		ColumnProperties* column = (ColumnProperties*)fColumns.ItemAt(i);
		column->Print();
	}
}


BString
TableProperties::CreateTableSQL(BString tableName, bool isTemp = false)
{
	BString sql("CREATE ");
	char* charToEscape = "'";
	
	if (isTemp)
		sql += "TEMP ";
	
	sql += "TABLE [";
	sql += tableName;
	sql += "] (";
	
	for (int i = 0; i < fColumns.CountItems(); i++)
	{
		ColumnProperties* column = (ColumnProperties*)fColumns.ItemAt(i);
		
		sql += "[";
		sql += column->Name().CharacterEscape(charToEscape, '\'');
		sql += "] ";
		
		if (column->AutoNumber())
		{
			sql += "INTEGER PRIMARY KEY";
		}
		else
		{
			sql += column->Type().CharacterEscape(charToEscape, '\'');
			
			if (column->PrimaryKey())
				sql += " PRIMARY KEY";
		}
	
		if (column->HasIndex() && column->UniqueIndex())
			sql += " UNIQUE";
		
		if (column->Required())
			sql += " NOT NULL";
		
		if (column->DefaultValue().Length() > 0)
		{
			sql += " DEFAULT '";
			sql += column->DefaultValue().CharacterEscape(charToEscape, '\'');
			sql += "'";
		}
		
		if (i+1 < fColumns.CountItems())
			sql += ", ";
	}

	sql += "); ";
	
	//Construct the CREATE INDEX statement for any non-unique index.
	for (int i = 0; i < fColumns.CountItems(); i++)
	{
		ColumnProperties* column = (ColumnProperties*)fColumns.ItemAt(i);
		
		if (column->HasIndex() && !column->UniqueIndex())
		{
			sql += "CREATE INDEX [";
			sql += tableName;
			sql += "_";
			sql += column->Name().CharacterEscape(charToEscape, '\'');
			sql += "] ON [";
			sql += tableName;
			sql += "] ([";
			sql += column->Name().CharacterEscape(charToEscape, '\'');
			sql += "]); ";
		}		
	}

	return sql;
}


void
TableProperties::AddColumnProperties(ColumnProperties* columnProps)
{
	ColumnProperties* column = new ColumnProperties(*columnProps);
	fColumns.AddItem((void*)column);
}


BString**
TableProperties::DefaultValues()
{
	BString** values = new BString*[CountColumns()];
	
	for (int i = 0; i < CountColumns(); i++)
	{
		ColumnProperties* column = GetColumnProperties(i);
		values[i] = new BString(column->DefaultValue());
	}
	
	return values;
}


void 
TableProperties::GetColumnInfo()
{
	char* sql = sqlite3_mprintf("PRAGMA table_info([%q]);", fTableName.String());

	RecordSet* rs = GlobalSQLMgr->OpenRecordSet(sql);

	while(rs->Step())
	{
		ColumnProperties* column = new ColumnProperties();
	
		column->SetName(rs->ColumnText(NAME));
		column->SetType(rs->ColumnText(DATA_TYPE));
		column->SetRequired(strcmp(rs->ColumnText(REQUIRED), "0") ? true : false);
		column->SetDefaultValue(rs->ColumnText(DEFAULT_VALUE));
		column->SetPrimaryKey(strcmp(rs->ColumnText(PRIMARY_KEY),"0") ? true : false);

		//Primary key + Integer = AutoNumber
		if (column->PrimaryKey() && column->Type().ICompare("INTEGER") == 0)
			column->SetType("AutoNumber");

		fColumns.AddItem(column);
	}

	rs->Close();
	sqlite3_free(sql);
}


void
TableProperties::GetIndexList()
{
	char* sql = sqlite3_mprintf("PRAGMA index_list([%q]);", fTableName.String());

	RecordSet* rs = GlobalSQLMgr->OpenRecordSet(sql);
	
	while(rs->Step())
	{
		IndexProperties index;
		index.fName = BString(rs->ColumnText(1));
		index.fIsUnique = (strcmp(rs->ColumnText(2), "0") ? true : false);

		GetIndexInfo(index);
	}

	rs->Close();
	sqlite3_free(sql);
}


void 
TableProperties::GetIndexInfo(const IndexProperties &index)
{
	char* sql = sqlite3_mprintf("PRAGMA index_info(%Q);", index.fName.String());
	
	RecordSet* rs = GlobalSQLMgr->OpenRecordSet(sql);
	
	if(rs->Step())
	{
		int i = atoi(rs->ColumnText(1));
		ColumnProperties* column = (ColumnProperties*)fColumns.ItemAt(i);
		column->SetIndex(index.fName, index.fIsUnique);
	}

	rs->Close();
	sqlite3_free(sql);
}


void 
TableProperties::GetTableXML()
{
	char* sql = sqlite3_mprintf("select value from beaccessible_master where "
		"name = %Q and type = 'table_preferences' limit 1;", fTableName.String());

	RecordSet* rs = GlobalSQLMgr->OpenRecordSet(sql);

	if (rs->Step())
	{	
		//Set up XML parser
		XML_Parser parser = XML_ParserCreate(NULL);
		XML_SetUserData(parser, (void*)&fColumnMapping);
		XML_SetElementHandler(parser, StartElement, EndElement);
		XML_SetCharacterDataHandler(parser, CharacterHandler);
	
		//Parse
		if (!XML_Parse(parser, rs->ColumnText(0), strlen(rs->ColumnText(0)), true))
			(new BAlert("Error", "XML_Parse had a fatal error", "Ok"))->Go();
	
		//Done
		XML_ParserFree(parser);
	}
	
	rs->Close();
	sqlite3_free(sql);
}



ColumnProperties*
TableProperties::GetColumnProperties(int32 index)
{
	return (ColumnProperties*)fColumns.ItemAt(index);
}


void
TableProperties::StartElement(void* userData, const char* name, const char** atts)
{
	ColumnMapping* colMap = static_cast<ColumnMapping*>(userData);

	if (strcmp(name, "name") == 0)
		colMap->fCurrentElement = ELEMENT_NAME;
	else if (strcmp(name, "size") == 0)
		colMap->fCurrentElement = ELEMENT_SIZE;
	else
		colMap->fCurrentElement = NONE;
}

void
TableProperties::EndElement(void* userData, const char* name)
{
	ColumnMapping* colMap = static_cast<ColumnMapping*>(userData);
	colMap->fCurrentElement = NONE;
}

void 
TableProperties::CharacterHandler(void* userData, const char* s, int len)
{
	ColumnMapping* colMap = static_cast<ColumnMapping*>(userData);
	
	switch(colMap->fCurrentElement)
	{
		case ELEMENT_NAME:
		{
			//name of column currently being parsed
			colMap->fColumn.SetTo(s, len);			
			break;
		}
		
		case ELEMENT_SIZE:
		{
			BString c(s, len);
			float size = atof(c.String());
			
			ColumnProperties* col = NULL;
			col = colMap->fMapping[colMap->fColumn];
	
			if (col)
				col->SetSize(size);
			break;
		}
		
		case NONE:
		{
			break;
		}
	}
}


