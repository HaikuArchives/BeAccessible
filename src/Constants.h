#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <iostream>
#include <fstream>
#include <String.h>
#include "PrefilledBitmap.h"

#define MENU_BAR_HEIGHT  18.0

//Message constants
#define MENU_FILE_OPEN_MSG   'open'
#define MENU_FILE_NEW_MSG    'cnew'
#define MENU_FILE_CLOSE_MSG  'clse'
#define IMPORT_FILE_MSG      'impt'
#define IMPORT_FILE_REQUESTED_MSG 'ifrq'
#define MENU_FILE_QUIT_MSG   'quit'
#define MENU_FILE_EXPORT_MSG 'expt'
#define MENU_SAVE_TABLE_MSG  'svtb'
#define NEW_FILE_REQUESTED   'nreq'
#define CLOSE_DB_REQUESTED   'cldb'

#define GO_TO_ROW_MSG			   'gtrw'  // keep
#define ADD_ROW              'arow'  // replace with GRID_APPEND_ROW_MSG
#define INSERT_DESIGN_ROW_MSG 'indr' // replace with GRID_INSERT_ROW_MSG
#define DELETE_DESIGN_ROW_MSG 'dedr' // replace with GRID_DELETE_ROW_MSG
#define DELETE_DATA_ROW_MSG  'delr'  // replace with GRID_DELETE_ROW_MSG
#define GOTO_NEW_RECORD_MSG  'gonw'  // remove
#define INSERT_NEW_RECORD_MSG 'insr' // replace with GRID_INSERT_ROW_MSG

#define SELECT_ALL_ROWS_MSG  'sall'
#define PASTE_TABLE_AS_MSG	 'ptas'
#define CANCEL_PASTE_TABLE_MSG 'cpta'
#define OPEN_DATABASE_MSG		 'opdb'
#define COMPACT_DATABASE_MSG 'cpct'
#define OPEN_BUTTON    			 'opnb'
#define DESIGN_BUTTON				 'dsgb'
#define NEW_BUTTON					 'nwbt'
#define DISABLE_OPEN_BUTTON	 'dopb'
#define ENABLE_OPEN_BUTTON	 'eopb'
#define MENU_VIEW_DESIGN_MSG 'desn'
#define MENU_VIEW_DATA_MSG   'data'
#define PRIMARY_KEY_BUTTON    'pkbt'
#define NO_INDEX							'nndx'
#define DUP_INDEX							'dndx'
#define UNIQUE_INDEX					'undx'
#define DEFAULT_VALUE_MSG			'dfvl'
#define DATA_TYPE_CHANGED     'dtch'
#define NEW_DATABASE_MSG      'ndbm'
#define EXISTING_DATABASE_MSG 'exdb'
#define CANCEL_START_WINDOW   'cnst'
#define SELECT_DATABASE       'slct'
#define RENAME_TABLE_MSG			'rntb'
#define DELETE_TABLE_MSG			'dltb'
#define ADD_TABLE_TO_LIST_MSG	'adnt'
#define MSG_SORT_ASCENDING    'sasc'
#define MSG_SORT_DESCENDING   'sdes'
#define MSG_FILTER_BY         'flby'
#define MSG_FILTER_EXCL       'fexc'
#define MSG_REMOVE_FILTER_SORT 'rmfs'
#define MSG_FILTER_FOR        'ffor'
#define MSG_ASK_FOR_FILTER    'askf'

//Icon types

//Debug Levels
#define METHOD_ENTRY 1
#define METHOD_EXIT 2
#define GENERAL 4
#define SQL 8

//View Types
#define DESIGN_VIEW 0
#define DATA_VIEW 1
#define SQL_VIEW 2


//void ReadDebugLevel();
//void Debug_Info(int level, BString methodName, BString text);
//void trim(BString* string);
#endif
