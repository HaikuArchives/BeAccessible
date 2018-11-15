#include <iostream>

#include "ColumnProperties.h"
#include "Alert.h"

const float DEFAULT_COLUMN_SIZE = 80.0;

//Default constructor
ColumnProperties::ColumnProperties()
{
	//set default values
	fName = new BString("");
	fType = new BString("");
	fRequired = false;
	fPrimaryKey = false;
	fAutoNumber = false;
	fDefaultValue = new BString("");
	fHasIndex = false;
	fIndexName = new BString("");
	fUniqueIndex = false;
	fSize = DEFAULT_COLUMN_SIZE;
}

//Copy constructor
ColumnProperties::ColumnProperties(const ColumnProperties &copy)
{
	fName = new BString(*copy.fName);
	fType = new BString(*copy.fType);
	fDefaultValue = new BString(*copy.fDefaultValue);
	fIndexName = new BString(*copy.fIndexName);
	fRequired = copy.fRequired;
	fPrimaryKey = copy.fPrimaryKey;
	fAutoNumber = copy.fAutoNumber;
	fHasIndex = copy.fHasIndex;
	fUniqueIndex = copy.fUniqueIndex;
	fSize = copy.fSize;
}

bool
ColumnProperties::operator==(const ColumnProperties &right) const
{
	if (*fName != *right.fName)
		return false;
	if (*fType != *right.fType)
		return false;
	if (fRequired != right.fRequired)
		return false;
	if (fPrimaryKey != right.fPrimaryKey)
		return false;
	if (fAutoNumber != right.fAutoNumber)
		return false;
	if (*fDefaultValue != *right.fDefaultValue)
		return false;
	if (fHasIndex != right.fHasIndex)
		return false;
// The index name will be different for every index, so we shouldn't
// compare names.
//	if (*fIndexName != *right.fIndexName)
//		return false;
	if (fUniqueIndex != right.fUniqueIndex)
		return false;
	if (fSize != right.fSize)
		return false;

	return true;
}


BString ColumnProperties::Name()
{
	return *fName;
}

BString ColumnProperties::Type()
{
	return *fType;
}

BString ColumnProperties::DefaultValue()
{
	return *fDefaultValue;
}

BString ColumnProperties::IndexName()
{
	return *fIndexName;
}

bool ColumnProperties::Required()
{
	return fRequired;
}

bool ColumnProperties::PrimaryKey()
{
	return fPrimaryKey;
}

bool ColumnProperties::AutoNumber()
{
	return fAutoNumber;
}

bool ColumnProperties::HasIndex()
{
	return fHasIndex;
}

bool ColumnProperties::UniqueIndex()
{
	return fUniqueIndex;
}

float ColumnProperties::Size()
{
	return fSize;
}

void ColumnProperties::SetName(BString name)
{
	fName->SetTo(name);
}

void ColumnProperties::SetType(BString type)
{
	fType->SetTo(type);

	if (fType->ICompare("AutoNumber") == 0)
	{
		//The new type is AutoNumber, which means this must be a primary key
		fAutoNumber = true;
		fPrimaryKey = true;
	}
	else if (fAutoNumber)
	{
		//The new type is not AutoNumber, which means this cannot be a primary key
		fAutoNumber = false;
		fPrimaryKey = false;
	}
}

void ColumnProperties::SetDefaultValue(BString defaultValue)
{
	fDefaultValue->SetTo(defaultValue);
}

void ColumnProperties::SetIndex(BString indexName, bool uniqueIndex)
{
	fHasIndex = true;
	fUniqueIndex = uniqueIndex;
	fIndexName->SetTo(indexName);
}

void ColumnProperties::RemoveIndex()
{
	fHasIndex = false;
	fUniqueIndex = false;
	fIndexName->SetTo("");
}

void ColumnProperties::SetPrimaryKey(bool key)
{
	fPrimaryKey = key;

	if (!fPrimaryKey && fAutoNumber)
	{
		BString msg("AutoNumber fields are required to be primary keys.");
		BAlert* alert = new BAlert("Title", msg.String(), "Ok");
		alert->SetShortcut(0, B_ESCAPE);
		alert->Go();

		fPrimaryKey = true;
	}

}

void ColumnProperties::SetRequired(bool required)
{
	fRequired = required;
}


void ColumnProperties::SetSize(float size)
{
	fSize = size;
}

void
ColumnProperties::Print()
{
	std::cout << "name = '" << fName->String() << "'" << std::endl;
	std::cout << "\ttype = '" << fType->String() << "'" << std::endl;
	std::cout << "\tDefaultValue = '" << fDefaultValue->String() << "'" << std::endl;	
	std::cout << "\tIndexName = '" << fIndexName->String() << "'" << std::endl;
	std::cout << "\tUniqueIndex = " << fUniqueIndex << std::endl;
	std::cout << "\tHasIndex = " << fHasIndex << std::endl;
	std::cout << "\tRequired = " << fRequired << std::endl;
	std::cout << "\tPrimaryKey = " << fPrimaryKey << std::endl;
	std::cout << "\tAutoNumber = " << fAutoNumber << std::endl;
}


ColumnProperties::~ColumnProperties()
{
	if (fName)
		delete fName;
	if (fType)
		delete fType;
	if (fDefaultValue)
		delete fDefaultValue;
	if (fIndexName)
		delete fIndexName;
}
