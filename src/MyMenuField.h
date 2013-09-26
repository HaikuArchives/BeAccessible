#include "MenuField.h"


class MyMenuField : public BMenuField
{
	public:
		MyMenuField(BRect frame, const char* name, const char* label,
		            BMenu* menu, bool fixedSize, uint32 resizingMode,
		            uint32 flags);
		virtual void Draw(BRect updateRect);
};

