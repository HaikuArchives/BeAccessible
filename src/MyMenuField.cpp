#include "MyMenuField.h"

MyMenuField::MyMenuField(BRect frame, const char* name, const char* label,
   BMenu* menu, bool fixedSize, uint32 resizingMode, uint32 flags)
 : BMenuField(frame, name, label, menu, fixedSize, resizingMode, flags)
{
	SetDivider(0);

}

void MyMenuField::Draw(BRect updateRect)
{
	SetViewColor(255,255,255,0);
}
