//*** LICENSE ***
//ColumnListView, its associated classes and source code, and the other components of Santa's Gift Bag are
//being made publicly available and free to use in freeware and shareware products with a price under $25
//(I believe that shareware should be cheap). For overpriced shareware (hehehe) or commercial products,
//please contact me to negotiate a fee for use. After all, I did work hard on this class and invested a lot
//of time into it. That being said, DON'T WORRY I don't want much. It totally depends on the sort of project
//you're working on and how much you expect to make off it. If someone makes money off my work, I'd like to
//get at least a little something.  If any of the components of Santa's Gift Bag are is used in a shareware
//or commercial product, I get a free copy.  The source is made available so that you can improve and extend
//it as you need. In general it is best to customize your ColumnListView through inheritance, so that you
//can take advantage of enhancements and bug fixes as they become available. Feel free to distribute the 
//ColumnListView source, including modified versions, but keep this documentation and license with it.

//*** DESCRIPTION ***
//Tab view source file


//******************************************************************************************************
//**** SYSTEM HEADER FILES
//******************************************************************************************************
#include <string.h>
#include <Region.h>


//******************************************************************************************************
//**** PROJECT HEADER FILES
//******************************************************************************************************
#include "TabView.h"
#include "Colors.h"


//******************************************************************************************************
//**** TabView CLASS DEFINITION
//******************************************************************************************************
TabView::TabView(BRect frame, const char *name, const char** names, int32 number_of_views,
	uint32 resizingMode, uint32 flags, const BFont *labelFont)
: BView(frame, name, resizingMode, flags)
{
	fNames = new char*[number_of_views];
	fViews = new BView*[number_of_views];
	int32 Counter;
	for(Counter = 0; Counter < number_of_views; Counter++)
	{
		fNames[Counter] = new char[strlen(names[Counter])+1];
		strcpy(fNames[Counter],names[Counter]);
		fViews[Counter] = NULL;
	}
	fNumberOfViews = number_of_views;
	SetFont(labelFont);
	SetViewColor(BeBackgroundGrey);
	font_height FontAttributes;
	labelFont->GetHeight(&FontAttributes);
	float LabelFontAscent = ceil(FontAttributes.ascent);
	float LabelFontHeight = LabelFontAscent + ceil(FontAttributes.descent);
	float TabViewBottom = 5.0+LabelFontHeight+5.0;
	fTabRect = new BRect[fNumberOfViews];
	fLabelRect = new BRect[fNumberOfViews];
	fLabelPosition = new BPoint[fNumberOfViews];
	float* Widths = new float[fNumberOfViews];
	int32* LengthArray = new int32[fNumberOfViews];
	for(Counter = 0; Counter < fNumberOfViews; Counter++)
		LengthArray[Counter] = strlen(fNames[Counter]);
	labelFont->GetStringWidths((const char**)fNames,LengthArray,fNumberOfViews,Widths);
	delete[] LengthArray;
	float MaxTabsWidth = 0.0;
	for(Counter = 0; Counter < fNumberOfViews; Counter++)
		if(Widths[Counter]+18.0 > MaxTabsWidth)
			MaxTabsWidth = Widths[Counter]+18.0;
	for(Counter = 0; Counter < fNumberOfViews; Counter++)
	{
		fTabRect[Counter].Set((MaxTabsWidth+2.0)*Counter,0.0,(MaxTabsWidth+2.0)*(Counter+1)-1.0,
			TabViewBottom);
		float Offset = floor((MaxTabsWidth-Widths[Counter])/2.0);
		fLabelRect[Counter].Set(fTabRect[Counter].left+Offset,fTabRect[Counter].top+5.0,
			fTabRect[Counter].left+Offset+Widths[Counter],fTabRect[Counter].top+5.0+LabelFontHeight);
		fLabelPosition[Counter].Set(fLabelRect[Counter].left,fLabelRect[Counter].top+LabelFontAscent);
	}
	fCurrentlyShowing = 0;
	fBounds = Bounds();
	fContentArea.Set(fBounds.left+1.0,TabViewBottom+1.0,fBounds.right-1.0,fBounds.bottom-1.0);
	delete[] Widths;
}


float TabView::GetTabsWidth()
{
	return fTabRect[fNumberOfViews-1].right + 3.0;
}


BRect TabView::GetContentArea()
{
	return fContentArea;
}


void TabView::AddViews(BView** tabbed_views)
{
	for(int32 Counter = 0; Counter < fNumberOfViews; Counter++)
	{
		fViews[Counter] = tabbed_views[Counter];
		if(Counter != fCurrentlyShowing)
			fViews[Counter]->Hide();
		AddChild(fViews[Counter]);
	}
}


TabView::~TabView()
{
	for(int32 Counter = 0; Counter < fNumberOfViews; Counter++)
		delete[] fNames[Counter];
	delete[] fNames;
	delete[] fViews;
	delete[] fTabRect;
	delete[] fLabelRect;
	delete[] fLabelPosition;
}


void TabView::Draw(BRect updateRect)
{
	BRegion ClippingRegion;
	GetClippingRegion(&ClippingRegion);
	//Redraw the tabs
	float TabsBottom = fTabRect[0].bottom;
	int32 Counter;
	for(Counter = 0; Counter < fNumberOfViews; Counter++)
	{
		if(ClippingRegion.Intersects(fTabRect[Counter]))
		{
			rgb_color BackgroundColor = ViewColor();
			if(Counter != fCurrentlyShowing)
			{
				rgb_color UnselectedColor;
				UnselectedColor.red = uint8((uint32(BackgroundColor.red) + uint32(BeShadow.red)) / 2);
				UnselectedColor.green = uint8((uint32(BackgroundColor.green) + uint32(BeShadow.green)) / 2);
				UnselectedColor.blue = uint8((uint32(BackgroundColor.blue) + uint32(BeShadow.blue)) / 2);
				UnselectedColor.alpha = 255;
				SetHighColor(UnselectedColor);
				SetLowColor(UnselectedColor);
			}
			else
			{
				SetHighColor(ViewColor());
				SetLowColor(ViewColor());
			}
			FillRect(BRect(fTabRect[Counter].left+1.0,fTabRect[Counter].top+1.0,
				fTabRect[Counter].right-2.0,TabsBottom - (Counter!=fCurrentlyShowing?1.0:0.0)));
			if(ClippingRegion.Intersects(fLabelRect[Counter]))
			{
				SetHighColor(Black);
				DrawString(fNames[Counter],fLabelPosition[Counter]);
			}
			SetHighColor(BeHighlight);
			MovePenTo(BPoint(fTabRect[Counter].left,TabsBottom-(Counter!=0?1.0:0.0)));
			StrokeLine(BPoint(fTabRect[Counter].left,fTabRect[Counter].top+2.0));
			StrokeLine(BPoint(fTabRect[Counter].left+2.0,fTabRect[Counter].top));
			StrokeLine(BPoint(fTabRect[Counter].right-4.0,fTabRect[Counter].top));
			const rgb_color MiddleColor = {190,190,190,255};
			SetHighColor(MiddleColor);
			MovePenTo(BPoint(fTabRect[Counter].right-3.0,fTabRect[Counter].top));
			StrokeLine(BPoint(fTabRect[Counter].right-2.0,fTabRect[Counter].top+1.0));
			SetHighColor(BeShadow);
			MovePenTo(BPoint(fTabRect[Counter].right-1.0,fTabRect[Counter].top+2.0));
			StrokeLine(BPoint(fTabRect[Counter].right-1.0,TabsBottom-1.0));
		}
	}

	//Draw the main view area left border
	SetHighColor(BeHighlight);
	if(updateRect.left <= fBounds.left)
	{
		MovePenTo(fBounds.LeftBottom());
		StrokeLine(BPoint(fBounds.left,TabsBottom+1.0));
	}

	//Draw the top border
	if(updateRect.top <= TabsBottom && updateRect.bottom >= TabsBottom)
	{
		if(fCurrentlyShowing > 0 && fTabRect[fCurrentlyShowing].left >= updateRect.left)
		{
			MovePenTo(BPoint(fBounds.left,TabsBottom));
			StrokeLine(BPoint(fTabRect[fCurrentlyShowing].left,TabsBottom));
		}
		if(fTabRect[fCurrentlyShowing].right <= updateRect.right &&
			fTabRect[fCurrentlyShowing].right-1.0 <= fBounds.right)
		{
			MovePenTo(BPoint(fTabRect[fCurrentlyShowing].right-1.0,TabsBottom));
			StrokeLine(BPoint(fBounds.right,TabsBottom));
		}
	}
	
	//Draw the right border
	SetHighColor(BeShadow);
	if(updateRect.right >= fBounds.right)
	{
		if(fBounds.right >= fTabRect[fNumberOfViews-1].right)
			MovePenTo(BPoint(fBounds.right,TabsBottom+1.0));
		else
			//Figure out where to start drawing right border (top or bottom of tab)
			for(Counter = 0; Counter < fNumberOfViews; Counter++)
			{
				if(fBounds.right <= fTabRect[Counter].right)
				{
					//Position cursor for right boundary
					if(fBounds.right == fTabRect[Counter].left)
						MovePenTo(BPoint(fBounds.right,TabsBottom+1.0));
					else if(fBounds.right == fTabRect[Counter].left+1.0)
						MovePenTo(BPoint(fBounds.right,fTabRect[0].top+2.0));
					else if(fBounds.right <= fTabRect[Counter].right-3.0)
						MovePenTo(BPoint(fBounds.right,fTabRect[0].top+1.0));
					else if(fBounds.right <= fTabRect[Counter].right-2.0)
						MovePenTo(BPoint(fBounds.right,fTabRect[0].top+2.0));
					else if(fBounds.right <= fTabRect[Counter].right-1.0)
						MovePenTo(BPoint(fBounds.right,TabsBottom+1.0));
					else
						MovePenTo(BPoint(fBounds.right,TabsBottom+1.0));
					break;
				}
			}
		StrokeLine(fBounds.RightBottom());
	}
	else if(updateRect.bottom >= fBounds.bottom)
		MovePenTo(fBounds.RightBottom());

	//Draw the bottom border
	if(updateRect.bottom >= fBounds.bottom)
		StrokeLine(BPoint(fBounds.left+1.0,fBounds.bottom));
}


void TabView::MouseDown(BPoint point)
{
	for(int32 Counter = 0; Counter < fNumberOfViews; Counter++)
		if(fTabRect[Counter].Contains(point))
		{
			if(Counter != fCurrentlyShowing)
				SelectTab(Counter);
			break;
		}
	MakeFocus(true);
}


void TabView::FrameResized(float width, float height)
{
	float XMin = fBounds.right;
	float XMax = fBounds.right;
	float YMin = fBounds.bottom;
	float YMax = fBounds.bottom;
	fBounds = Bounds();
	if(fBounds.right > XMax)
		XMax = fBounds.right;
	if(fBounds.right < XMin)
		XMin = fBounds.right;
	if(fBounds.bottom > YMax)
		YMax = fBounds.bottom;
	if(fBounds.bottom < YMin)
		YMin = fBounds.bottom;
	if(XMin != XMax)
		Invalidate(BRect(XMin,fBounds.top,XMax,fBounds.bottom));
	if(YMin != YMax)
		Invalidate(BRect(fBounds.left,YMin,fBounds.right,YMax));
	//Get rid of a warning
	width = 0;
	height = 0;
}


void TabView::SelectTab(int32 index)
{
	if(index != fCurrentlyShowing)
	{
		Invalidate(fTabRect[fCurrentlyShowing]);
		fViews[fCurrentlyShowing]->Hide();
		fCurrentlyShowing = index;
		Invalidate(fTabRect[fCurrentlyShowing]);
		fViews[fCurrentlyShowing]->Show();
	}
}

BView* TabView::GetCurrentTab()
{
	return(fViews[fCurrentlyShowing]);
}
