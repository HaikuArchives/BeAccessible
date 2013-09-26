//  BeAccessibleView.cpp

#ifndef BeAccessibleView_H
#include "BeAccessibleView.h"
#endif
#include "Beep.h"
#include <TextControl.h>
//#include "TestView.h"

BRect frame1(110.0, 50.0, 210.0, 67.0);
BRect frame2(212.0, 50.0, 310.0, 67.0);
BRect frame3(312.0, 50.0, 410.0, 67.0);
#define TEXT_INSET 1.0
#define BORDER_INSET -1.0

BeAccessibleView::BeAccessibleView(BRect rect, const char *name)
	   	   : BView(rect, name, B_FOLLOW_ALL, B_WILL_DRAW)
{
}

void BeAccessibleView::AttachedToWindow()
{

}

void BeAccessibleView::Draw(BRect updateRect)
{

}

void BeAccessibleView::MouseDown(BPoint point)
{
	//beep();
}


BRect BeAccessibleView::GetTextBounds(BRect frame)
{
	BRect textBounds;

	textBounds.left = TEXT_INSET;
	textBounds.right = frame.right - frame.left - TEXT_INSET;
	textBounds.top = TEXT_INSET;
	textBounds.bottom = frame.bottom - frame.top - TEXT_INSET;

	return (textBounds);
}
