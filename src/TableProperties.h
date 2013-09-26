#ifndef TableProperties_h
#define TableProperties_h
#include <String.h>
#include <List.h>
#include "expat.h"
#include <map>

class ColumnProperties;

//The elements that will be encountered when parsing table_preferences XML
enum CurrentElement { NONE, ELEMENT_NAME, ELEMENT_SIZE };

//Structure used to encapsulate data passed to XML callback functions
class ColumnMapping
{
	public:
		//Name of current column being parsed
		BString fColumn;
		//Map of column names to ColumnProperties class
		map<BString, ColumnProperties*, less<BString> > fMapping;
		//Current element being parsed
		CurrentElement fCurrentElement;
};

class IndexProperties;

class TableProperties
{
	public:
		TableProperties();
		TableProperties(const TableProperties &);
		TableProperties(BString tableName);
		~TableProperties();
		int CountColumns();
		void Print();
		BString ColumnNames();
		BString CreateTableSQL(BString tableName, bool isTemp = false);
		BString** DefaultValues();
		ColumnProperties* GetColumnProperties(int32 index);
		void AddColumnProperties(ColumnProperties* columnProps);
		bool operator==(const TableProperties &) const;
		bool operator!=(const TableProperties &right) const
		  { return ! (*this == right); }


	private:
		void ConvertDataTypes();
		void SetAutoNumber(int32 index, bool newValue);

		void GetColumnInfo();
		void GetIndexList();
		void GetIndexInfo(const IndexProperties &index);
		void GetTableXML();
		
		//XML callbacks		                         
		static void StartElement(void* userData, const char* name, 
			const char** atts);
		static void EndElement(void* userData, const char* name);
		static void CharacterHandler(void* userData, const char* s, int len);		

		BString fTableName;
		
		//List of ColumnProperties pointers
		BList fColumns;

		//Structure passed to XML callback functions
		ColumnMapping fColumnMapping;
};



#endif
