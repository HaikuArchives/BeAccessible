#include "Filter.h"

Filter::Filter(BString identifier, BString filter)
: fIdentifier(identifier),
  fOrigFilter(filter),
  fCurrentIndex(0)
{

}


BString
Filter::Parse()
{
	if (fOrigFilter.Length() == 0)
		return "";

	BString newFilter(fIdentifier);
	newFilter += " ";
	BString token, previousToken;
	
	while((token = GetNextToken()) != "")
	{
		if (token == ">" || token == ">=" || token == "<" || token == "<=")
			newFilter << token << " " << Quote(GetNextToken()) << " ";
		else if (token == "<>" || token == "=" || token == "!=")
			newFilter << token << " " << Quote(GetNextToken()) << " ";		
		else if (token == "and")
			newFilter << "and " << fIdentifier << " ";
		else if (token == "or")
			newFilter << "or " << fIdentifier << " ";
		else if (token == "not")
			newFilter << "not ";
		else if (token == "is")
			newFilter << "is " << GetNextToken() << " ";
		else if (token == "null")
		{
			if (previousToken == "not")
				newFilter << "null ";
			else
				newFilter << "is null ";
		}
		else if (token == "between")
		{
			BString beginRange(GetNextToken());
			BString andText(GetNextToken());
			BString endRange(GetNextToken());
			newFilter << "between " << Quote(beginRange) << " " << andText << " " 
			          << Quote(endRange) << " ";
		}
		else if (token == "in")
			newFilter << "in " << GetNextToken() << " ";
		else if (token == "like")
		{
			newFilter << "like ";
			BString pattern(GetNextToken());
			pattern.ReplaceAll('*', '%');
	
			newFilter << Quote(pattern) << " ";
		}
		else
		{
			if (token.FindFirst("*") < 0)
				newFilter << "= " << Quote(token) << " ";
			else
			{
				token.ReplaceAll('*', '%');
				newFilter << " like " << Quote(token) << " ";
			}
		}
		
		previousToken = token;
	}
	
	return newFilter;
}


BString
Filter::GetNextToken()
{
	if (fCurrentIndex >= fOrigFilter.Length())
		return "";

	char c = fOrigFilter[fCurrentIndex];
	
	if (IsOperator(c))
		return GetOperator();
	else if (IsGroup(c))
		return GetGroup();
	else if (IsWhiteSpace(c))
	{
		fCurrentIndex += 1;
		return GetNextToken();
	}
	else
		return GetText();
}


bool
Filter::IsOperator(char c)
{
	if (c == '>' || c == '<' || c == '=' || c == '!')
		return true;
	else
		return false;
}


bool
Filter::IsGroup(char c)
{
	if (c == '\'' || c == '"' || c == '(')
		return true;
	else
		return false;
}

bool
Filter::IsWhiteSpace(char c)
{
	if (c == ' ' || c == '\t')
		return true;
	else
		return false;
}

BString
Filter::GetOperator()
{
	if (fOrigFilter[fCurrentIndex] == '>')
	{
		if (fOrigFilter[fCurrentIndex+1] == '=')
		{
			fCurrentIndex += 2;
			return ">=";
		}
		else
		{
			fCurrentIndex += 1;
			return ">";
		}
	}
	else if (fOrigFilter[fCurrentIndex] == '<')
	{
		if (fOrigFilter[fCurrentIndex+1] == '=')
		{
			fCurrentIndex += 2;
			return "<=";
		}
		else if (fOrigFilter[fCurrentIndex+1] == '>')
		{
			fCurrentIndex += 2;
			return "<>";
		}
		else
		{
			fCurrentIndex += 1;
			return "<";
		}
	}
	else if (fOrigFilter[fCurrentIndex] == '=')
	{
		fCurrentIndex += 1;
		return "=";	
	}
	else if (fOrigFilter[fCurrentIndex] == '!')
	{
		if (fOrigFilter[fCurrentIndex+1] == '=')
		{
			fCurrentIndex += 2;
			return "!=";
		}
	}
	
	return "";
}

BString
Filter::GetGroup()
{
	if (fOrigFilter[fCurrentIndex] == '\'')
	{
		int32 endQuoteIndex = fOrigFilter.FindFirst('\'', fCurrentIndex+1);

		BString destination;
		fOrigFilter.CopyInto(destination, fCurrentIndex,
			endQuoteIndex - fCurrentIndex + 1);

		fCurrentIndex += endQuoteIndex - fCurrentIndex + 1;
		return destination;
	}
	else if (fOrigFilter[fCurrentIndex] == '"')
	{
		int32 endQuoteIndex = fOrigFilter.FindFirst('"', fCurrentIndex+1);

		BString destination;
		fOrigFilter.CopyInto(destination, fCurrentIndex,
			endQuoteIndex - fCurrentIndex + 1);

		fCurrentIndex += endQuoteIndex - fCurrentIndex + 1;
		return destination;
	}
	else // '('
	{
		int32 endQuoteIndex = fOrigFilter.FindFirst(')', fCurrentIndex+1);

		BString destination;
		fOrigFilter.CopyInto(destination, fCurrentIndex,
			endQuoteIndex - fCurrentIndex + 1);

		fCurrentIndex += endQuoteIndex - fCurrentIndex + 1;
		return destination;
	}
}

BString
Filter::GetText()
{
	int32 whiteSpaceIndex = fOrigFilter.FindFirst(' ', fCurrentIndex+1);
	if (whiteSpaceIndex < 0)
		whiteSpaceIndex = fOrigFilter.Length();

	BString destination;
	fOrigFilter.CopyInto(destination, fCurrentIndex,
		whiteSpaceIndex - fCurrentIndex);

	fCurrentIndex += whiteSpaceIndex - fCurrentIndex + 1;
	return destination;
}


//Add quotes around the text if it doesn't have any
BString
Filter::Quote(BString text)
{
	BString newText;
	int32 lastIndex = text.Length()-1;
	
	if ((text[0] == '\'' && text[lastIndex] == '\'') ||
	    (text[0] == '"' && text[lastIndex] == '"'))
	{
		return text;
	}
	else
	{
		newText << "'" << text << "'";	
		return newText;
	}
}
