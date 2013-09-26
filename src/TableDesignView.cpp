#include <Alert.h>
#include "TableWindow.h"
#include "TableDesignView.h"
#include "TableProperties.h"
#include "ColumnProperties.h"
#include "TableDesignRow.h"
#include "ColumnDesignView.h"
#include "GridConstants.h"


TableDesignView::TableDesignView(BRect bounds, CLVContainerView** contView,
const char* name, TableProperties* props)
: GridView(bounds, contView, name)
{
	fProps = props;

	//Design view has 3 columns:  icon, field name, and data type
	AddColumn(new CLVColumn(NULL,20.0,CLV_LOCK_AT_BEGINNING |
						   CLV_NOT_MOVABLE | CLV_NOT_RESIZABLE));
	AddColumn(new CLVColumn("Field Name",220.0,
						   CLV_NOT_MOVABLE | CLV_NOT_RESIZABLE));
	AddColumn(new CLVColumn("Data Type",100.0,
						   CLV_NOT_MOVABLE | CLV_NOT_RESIZABLE));																		

	TableDesignRow* row;
	fOrigColumnOrder = new BList();

	//For each column, add a row to the ColumnListView
	for (int i = 0; i < fProps->CountColumns(); i++)
	{
		ColumnProperties colProps = *(fProps->GetColumnProperties(i));
		
		BString** args = new BString*[1];
		args[0] = new BString(colProps.Name());
		
		row = new TableDesignRow(this, colProps, fIconList, args, &fCurrentColumn);
		AddItem(row);
		fOrigColumnOrder->AddItem((void*)row);	                                                   
	}
	
	ColumnProperties blankProps;

	//Add 20 blank rows after the last column	
	for (int i = 0; i < 20; i++)
	{
	    AddItem(new TableDesignRow(this, blankProps, fIconList, NULL, &fCurrentColumn));
	}
}


TableDesignView::~TableDesignView()
{
	DeleteAllRows();

	if (fOrigColumnOrder)
	{
		delete fOrigColumnOrder;
		fOrigColumnOrder = NULL;
	}
}

void
TableDesignView::MessageReceived(BMessage* msg)
{
	switch(msg->what)
	{	
		case GRID_DELETE_ROW_MSG:
		{
			BString msg("Are you sure you want to delete the selected field(s)?");
			BAlert* alert = new BAlert("Title", msg.String(), "Cancel", "No", "Yes",
			                   B_WIDTH_AS_USUAL, B_OFFSET_SPACING, B_WARNING_ALERT);
			alert->SetShortcut(0, B_ESCAPE);
			int32 button_index = alert->Go();
				
			TableDesignRow* row;
			int32 selected = 0, i = 0, lastSelectedItem = 0;
				
			if (button_index == 2) // Yes clicked
			{
				while ((selected = CurrentSelection(i)) >= 0)
				{
					lastSelectedItem = selected;
						
					row = (TableDesignRow*)RemoveItem(selected);
					
					if (row)
						delete row;
				}// end while
					
				//The row just deleted is the last clicked item.  Since it no longer
				//exists, we should set the last clicked item to the next row.
				SetClickedItem(NULL);
				GoToRow(lastSelectedItem, 1);
				ColumnDesignView* columnDesign = ((TableWindow*)Window())->GetColumnDesignView();
				columnDesign->DisplayColumnProperties(lastSelectedItem, true);

				//Force the field type popup menus to be redrawn by deleting them (they'll
				//be recreated when the row is redrawn).
				for (int32 k = lastSelectedItem; k < CountItems(); k++)
				{
					TableDesignRow* row = (TableDesignRow*)ItemAt(k);
					row->DeleteMenuBar();
				}
			}
			break;
		}

		case GRID_INSERT_ROW_MSG:
		{
			int32 selected = 0, i = 0, lastSelectedItem = 0;

			while ((selected = CurrentSelection(i)) >= 0)
			{	
				lastSelectedItem = selected;

				ColumnProperties blankProps;
	    		AddItem(new TableDesignRow(this, blankProps, fIconList, NULL, &fCurrentColumn), selected);
				i++;
			}

			for (int32 k = lastSelectedItem+1; k < CountItems(); k++)
			{
				TableDesignRow* row = (TableDesignRow*)ItemAt(k);
				row->DeleteMenuBar();
			}
			
			GoToRow(lastSelectedItem, 1);
			
			ColumnDesignView* columnDesign = ((TableWindow*)Window())->GetColumnDesignView();
			columnDesign->DisplayColumnProperties(lastSelectedItem, true);
			break;
		}
	
		case DATA_TYPE_CHANGED:
		{
			void* voidRow;
			msg->FindPointer("row", &voidRow);
			TableDesignRow* row = (TableDesignRow*)voidRow;
			
			int32 rowIndex = IndexOf(row);
			GoToRow(rowIndex, int32(2));
			row->SaveColumnProperties();
			row->InvalidateRow();
			break;		
		}
	
		default:
		{
			GridView::MessageReceived(msg);
			break;
		}
	}
}

TableProperties*
TableDesignView::GetTableProperties()
{
	return fProps;
}

void
TableDesignView::SetTableProperties(TableProperties* props)
{
	fProps = props;
}

BList*
TableDesignView::GetOriginalColumnOrder()
{
	return fOrigColumnOrder;
}



