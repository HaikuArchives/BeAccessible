#include "Constants.h"
#include <ctype.h>

class SQLiteManager;
SQLiteManager* GlobalSQLMgr;



//unsigned DebugLevel;
//
//void ReadDebugLevel()
//{
//  ifstream inputFile("/boot/home/src/BeAccessible/debuglevel.txt", ios::in);
//  
//  if (!inputFile)
//  {
//  	cerr << "debuglevel.txt could not be opened" << endl;
//	exit(1);
//  	DebugLevel = 0;
//  }
//
//	inputFile >> DebugLevel;
//}
//
//
//void Debug_Info(int level, BString methodName, BString text)
//{
//	if (DebugLevel & level)
//	{
//		switch (level)
//		{
//			case METHOD_ENTRY:
//				cerr << "METHOD_ENTRY: ";
//				break;
//			case METHOD_EXIT:
//				cerr << "METHOD_EXIT: ";
//				break;
//			case GENERAL:
//				cerr << "GENERAL: ";
//				break;
//			case SQL:
//				cerr << "SQL: ";
//				break;
//		}
//				
//		cerr << methodName.String() << ": " << text.String() << endl;
//	}
//}


void trim(BString* string)
{
	if (string->Length() == 0)
		return;

	//Remove white space at beginning
	while (isspace((*string)[0]))
	{
		string->Remove(0, 1);
	}
	
	//Remove white space at end
	while (isspace((*string)[string->Length()-1]))
	{
		string->Remove(string->Length()-1, 1);
	}
}

