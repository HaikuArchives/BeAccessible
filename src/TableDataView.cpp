#include <Window.h>
#include <Alert.h>
#include <OS.h>
//#include <StopWatch.h>

#include "TableWindow.h"
#include "ColumnProperties.h"
#include "TableDataView.h"
#include "TableDataRow.h"
#include "ColumnListView.h"
#include "Constants.h"
#include "GridConstants.h"
#include "RowSummaryView.h"
#include "SQLiteManager.h"
#include "TextEntryAlert.h"
#include "Filter.h"

extern SQLiteManager* GlobalSQLMgr;

TableDataView::TableDataView(BRect bounds, CLVContainerView** contView, 
const char* name, BString sql, TableProperties* props) 
: GridView(bounds, contView, name),
  fSQLMgr(GlobalSQLMgr->Clone())
{
	fSQL = sql;
	fFilter = "";
	fSort = "";
	fProps = props;
	fRowSummary = NULL;
	
	// Add columns to the GridView
	BString limitSQL("select * from [");
	limitSQL.Append(name);
	limitSQL.Append("] LIMIT 0;");
	
	bool columnsAdded = AddColumns(limitSQL);
	
	if (!columnsAdded)
	{
		Window()->Quit();
	}

	// Add rows to the GridView
	bool rowsAdded = AddRows(fSQL);
	
	if (rowsAdded)
	{
		AddLastRow();
		//BScrollBar* vBar = fContainerView->ScrollBar(B_VERTICAL);
		//vBar->SetRange(float(11.4), float(14*10));
	}
	else
	{
		Window()->Quit();
	}
}

TableDataView::~TableDataView()
{
	DeleteAllRows();
	
	//kill the getrowids thread, if it exists.
//	thread_id tid = find_thread("getrowids");
//	if (tid > B_OK)
//	{
//		kill_thread(tid);
//	}
//	
	//delete all the rowids stored in the BList, if any.
//	for (int i = 0; i < fRowIdList.CountItems(); i++)
//	{
//		int* rowid = static_cast<int*>(fRowIdList.ItemAt(i));
//		delete rowid;
//	}
//	fRowIdList.MakeEmpty();
}


void
TableDataView::AllAttached()
{
//	status_t exitValue;
//	wait_for_thread(fRowIdThreadNumber, &exitValue);

//	BRect currentRect = Bounds();
//	cout << "BRect(" << cr.left << ", " << cr.top << ", " << cr.right << ", " << 
//		cr.bottom << ");" << endl;
//	BScrollBar* sb = fScrollView->ScrollBar(B_VERTICAL);
//	float factor = CountItems() / fRowIdList.CountItems();
//	sb->SetProportion(factor);
//	currentRect.bottom = currentRect.bottom * factor;
//	fScrollView->SetDataRect(currentRect);
//	currentRect.PrintToStream();
}

void
TableDataView::MessageReceived(BMessage* msg)
{
	//cout << "TableDataView::MessageReceived()" << endl;
	switch(msg->what)
	{
		case GRID_DELETE_ROW_MSG:
		{
			BString transactionSQL("BEGIN EXCLUSIVE; ");

			transactionSQL << "DELETE FROM [";
			transactionSQL << Name();
			transactionSQL << "] WHERE ROWID IN (";
			
			//Used later to establish current row after delete is completed.
			int32 firstSelectedRow = CurrentSelection();
			
			TableDataRow* row;
			bool newRowSelected = false;
			int32 selected = 0, i = 0;
			while ((selected = CurrentSelection(i)) >= 0)
			{
				row = (TableDataRow*)ItemAt(selected);
				
				if (!row->IsLast())
				{
					if (i != 0)
					{
						transactionSQL += ", ";
					}

					transactionSQL += row->GetRowID();
				}
				else
				{
					newRowSelected = true;
				}
				
				i++;
			}
			
			if (newRowSelected) 
				i--;
			
			transactionSQL += "); END TRANSACTION;";
			
			//Display warning message
			BString msg("You are about to delete ");
			msg << i;
			msg += " row(s).\n\nThis operation cannot be undone.  Are you sure you want"
			       " to continue?";
			BAlert* alert = new BAlert("Title", msg.String(), "Cancel", "No", "Yes",
			                   B_WIDTH_AS_USUAL, B_OFFSET_SPACING, B_WARNING_ALERT);
			alert->SetShortcut(0, B_ESCAPE);
			int32 button_index = alert->Go();
			
			if (button_index == 2)
			{
				//User clicked Ok
				bool sqlSuccess = fSQLMgr.Execute(transactionSQL.String());

				if (!sqlSuccess)
				{
					fSQLMgr.Execute("ROLLBACK;");
					break;
				}

				RequeryDataView(firstSelectedRow);
			}
			
			break;
		}
	
		case MSG_FILTER_BY:
		case MSG_FILTER_EXCL:
		{
			BString text;
			msg->FindString("filter", &text);
			
			int32 columnNumber;
			msg->FindInt32("columnNumber", &columnNumber);
			
			BString colName(ColumnAt(columnNumber)->GetLabel());
			
			if (fFilter.Length() == 0)
			{
				fFilter.SetTo(" WHERE [");
			}
			else
			{
				fFilter.Append(" AND [");
			}
			
			fFilter.Append(colName);
			fFilter.Append("] ");
			fFilter.Append(text);
						
			RequeryDataView();
			
			if (fRowSummary)
				fRowSummary->SetFiltered(true);
			break;
		}
		
		case MSG_FILTER_FOR:
		{
			BString filter;
			msg->FindString("filter", &filter);
			
			BString colName;
			msg->FindString("column", &colName);
			
			if (fFilter.Length() == 0)
				fFilter.SetTo(" WHERE ");
			else
				fFilter.Append(" AND ");

			colName.Prepend("[");
			colName.Append("]");
						
			Filter f(colName, filter);
			
			fFilter.Append(f.Parse());
			
			RequeryDataView();
			
			if (fRowSummary)
				fRowSummary->SetFiltered(true);
			break;
		}
		
		case MSG_ASK_FOR_FILTER:
		{
			int32 columnNumber;
			msg->FindInt32("columnNumber", &columnNumber);
			BPoint point;
			msg->FindPoint("point", &point);
			
			BString colName(ColumnAt(columnNumber)->GetLabel());
			
			FilterForWindow* filterFor;
			filterFor = new FilterForWindow(point, colName, this);
			break;
		}
		
		case MSG_REMOVE_FILTER_SORT:
		{
			fFilter.SetTo("");
			fSort.SetTo("");
			
			RequeryDataView();
			if (fRowSummary)
				fRowSummary->SetFiltered(false);
			break;
		}
		
		case MSG_SORT_ASCENDING:
		{
			int32 columnNumber;
			msg->FindInt32("columnNumber", &columnNumber);
		
			fSort.SetTo(" ORDER BY [");
			fSort.Append(ColumnAt(columnNumber)->GetLabel());
			fSort.Append("] ASC");

			RequeryDataView();
			break;
		}
		
		case MSG_SORT_DESCENDING:
		{
			int32 columnNumber;
			msg->FindInt32("columnNumber", &columnNumber);
		
			fSort.SetTo(" ORDER BY [");
			fSort.Append(ColumnAt(columnNumber)->GetLabel());
			fSort.Append("] DESC");
		
			RequeryDataView();
			break;
		}
	
		default:
		{
			GridView::MessageReceived(msg);
			break;
		}
	}
}


void TableDataView::MouseDown(BPoint where)
{
	GridView::MouseDown(where);
}


bool TableDataView::AddColumns(BString sql)
{
	//Add first column for icons
	AddColumn(new CLVColumn(NULL,20.0,CLV_LOCK_AT_BEGINNING|CLV_NOT_MOVABLE|CLV_NOT_RESIZABLE));

	RecordSet* rs = fSQLMgr.OpenRecordSet(sql.String());
	
	//There's no need to call rs->Step() here.  The column count and column names
	//are available after opening the recordset.

	//Add columns for all fields in the SQL query
	for(int i = 0; i < rs->ColumnCount(); i++)
	{
		AddColumn(new CLVColumn(rs->ColumnName(i),
				fProps->GetColumnProperties(i)->Size(),
				CLV_TELL_ITEMS_WIDTH|CLV_HEADER_TRUNCATE));
	}

	rs->Close();

	return true;
}




bool TableDataView::AddRows(BString sql)
{
	RecordSet* rs = fSQLMgr.OpenRecordSet(sql.String());
	int numCols = rs->ColumnCount();
	
	while(rs->Step())
	{
		//the rowid is the first column; it will be passed to the row class 
		//separate from the cache
		BString rowId(rs->ColumnText(0));
		
		//create the cache (sans rowid) to be passed to the row class
		BString** cache = new BString*[numCols-1];
		for(int i = 1; i < numCols; i++)
			cache[i-1] = new BString(rs->ColumnText(i));
		
		AddItem(new TableDataRow(this, numCols-1, cache, fIconList, rowId, &fCurrentColumn));
	}	

	rs->Close();
	
	return true;
}



void
TableDataView::AddLastRow()
{
	int32 numColumns = CountColumns() - 1;
	
	//Add last row for a new record
	AddItem(new TableDataRow(this,numColumns, fProps->DefaultValues(), fIconList, "", 
	                         &fCurrentColumn, true));
}


void
TableDataView::RequeryDataView(int32 currentRow = 0)
{
	// If this line is inside the RemoveContainerView/ReattachContainerView calls
	// below, it will crash on R5.
	DeleteAllRows();

	TableWindow* tw = (TableWindow*)Window();
	if (tw)
		tw->RemoveContainerView();

	SetClickedItem(NULL);
	bool rowsAdded = AddRows(BuildSQL().String());

	if (rowsAdded)
		AddLastRow();					

	if (tw)
		tw->ReattachContainerView();

	GoToRow(currentRow,fCurrentColumn);
}

BString
TableDataView::BuildSQL()
{
	BString sql;
	// Start with the initial SQL
	sql.Append(fSQL);
	// Add on the WHERE clause
	sql.Append(fFilter);
	// Add on the ORDER BY clause
	sql.Append(fSort);
	// End the statement
	sql.Append(";");
	
	return sql;
}



TableProperties*
TableDataView::GetTableProperties()
{
	return fProps;
}

void
TableDataView::SetTableProperties(TableProperties* props)
{
	fProps = props;
}

void
TableDataView::SetRowSummary(RowSummaryView* rowSum)
{
	fRowSummary = rowSum;
	fRowSummary->SetRowNumber(1);
}


bool 
TableDataView::GoToRow(int32 rowNumber, int32 column)
{
	bool rc = GridView::GoToRow(rowNumber, column);
	if (rc && fRowSummary)
		fRowSummary->SetRowNumber(rowNumber+1);
	return rc;
}


void
TableDataView::SelectionChanged()
{
	int32 numSelected = CountSelectedItems();
	if (numSelected > 1 && fRowSummary)
		fRowSummary->SetRowNumber(0);
	else if (numSelected == 1 && fRowSummary)
		fRowSummary->SetRowNumber(CurrentSelection()+1);
}


//
//void
//TableDataView::AddItemToTempList(BListItem* row)
//{
//	fTempList->AddItem((void*)row);
//}
//
//void
//TableDataView::ClearTempList()
//{
//	delete fTempList;
//	fTempList = new BList();
//}

//void
//TableDataView::AddTempListToView()
//{

//	BStopWatch* watch = new BStopWatch("AddList Timer");
//	AddList(fTempList);
//	delete watch;

//	ClearTempList();
//}


FilterForWindow::FilterForWindow(BPoint point, BString column, TableDataView* parent)
: BWindow(BRect(point.x, point.y, point.x+200, point.y+30), "Filter For",
  B_FLOATING_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL, B_NOT_ZOOMABLE|B_NOT_RESIZABLE),
  fColumn(column),
  fParent(parent)
{
	//Change title to include the name of the column
	BString title("Filter on \"");
	title += column;
	title += "\"";
	SetTitle(title.String());
	
	fFilterView = new FilterForView(Bounds());
	AddChild(fFilterView);
	Show();
}

void
FilterForWindow::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case MSG_FILTER_FOR:
		{
			BMessage msg(MSG_FILTER_FOR);
			msg.AddString("filter", fFilterView->GetFilter());
			msg.AddString("column", fColumn);
			BMessenger(fParent).SendMessage(&msg);
			Quit();
			break;
		}
		
		default:
		{
			BWindow::MessageReceived(msg);
		}
	}
	
}

//
//

FilterForView::FilterForView(BRect frame)
: BView(frame, "FilterForView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW)
{
	SetViewColor(216,216,216,0);
	BRect bounds(60,6,190,23);
	fFilterText = new FilterForTextView(bounds, "FilterText", BRect(1,1,99,16), 
	  B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	
	AddChild(fFilterText);
}

void
FilterForView::AllAttached()
{
	fFilterText->MakeFocus(true);
	fFilterText->SetWordWrap(false);
	fFilterText->MakeResizable(true);
	BView::AllAttached();
}

BString
FilterForView::GetFilter()
{
	BString filter(fFilterText->Text());
	return filter;
}

void
FilterForView::Draw(BRect updateRect)
{
	DrawString("Filter For: ", BPoint(5,18));
}

//
//

FilterForTextView::FilterForTextView(BRect frame, const char* name, BRect textRect,
uint32 resizingMode, uint32 flags)
: BTextView(frame, name, textRect, resizingMode, flags)
{

}

void
FilterForTextView::KeyDown(const char* bytes, int32 numbytes)
{
	switch(bytes[0])
	{
		case B_ESCAPE:
		{
			Window()->Quit();
			break;
		}
		
		case B_ENTER:
		{
			BWindow* window = Window();
			BMessenger(window).SendMessage(MSG_FILTER_FOR);
			break;
		}
		
		default:
		{
			BTextView::KeyDown(bytes, numbytes);
			break;
		}
	}
}

