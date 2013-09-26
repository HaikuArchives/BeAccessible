#include "String.h"

class ColumnProperties
{
	public:
		ColumnProperties();
		ColumnProperties(const ColumnProperties &);
		~ColumnProperties();
		bool operator==(const ColumnProperties &) const;
		bool operator!=(const ColumnProperties &right) const
		  { return ! (*this == right); }
			
		void Print();
			
		//Get
		BString Name();
		BString Type();
		BString DefaultValue();
		BString IndexName();
		bool Required();
		bool PrimaryKey();
		bool AutoNumber();
		bool HasIndex();
		bool UniqueIndex();
		float Size();
		
		//Set
		void SetName(BString name);
		void SetType(BString type);
		void SetDefaultValue(BString defaultValue);
		void SetIndex(BString indexName, bool uniqueIndex);
		void RemoveIndex();
		void SetPrimaryKey(bool key);
		void SetRequired(bool allow);
		void SetSize(float size);

	private:	
		BString* fName;
		BString* fType;
		bool fRequired;
		bool fPrimaryKey;
		bool fAutoNumber;
		BString* fDefaultValue;
		bool fHasIndex;
		BString* fIndexName;
		bool fUniqueIndex;
		float fSize;
};

