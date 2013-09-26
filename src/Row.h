#ifndef ROW_H
#define ROW_H
#include "CLVEasyItem.h"


class Row : public CLVEasyItem
{
	public:
		Row(uint32 level, bool superitem, bool expanded, float minheight);
		virtual void NextField(int32) = 0;
		virtual void NextRow(int32) = 0;
		virtual void PreviousRow(int32) = 0;
		virtual void PreviousField(int32) = 0;
		virtual void Restore() = 0;
		virtual void Invalidate() = 0;
		virtual void SetModified(bool) = 0;
		virtual bool Modified() = 0;
		virtual void SetIcon(int icon) = 0;
		virtual ~Row();
};
#endif

