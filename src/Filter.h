#include <String.h>

class Filter
{
	public:
		Filter(BString identifier, BString filter);
		BString Parse();
	private:
		BString GetNextToken();
		bool IsOperator(char c);
		bool IsGroup(char c);
		bool IsWhiteSpace(char c);
		BString GetOperator();
		BString GetGroup();
		BString GetText();
		BString Quote(BString text);
		
		BString fIdentifier;
		BString fOrigFilter;
		int32 fCurrentIndex;
};

