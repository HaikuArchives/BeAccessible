#ifndef GridConstants_h
#define GridConstants_h


enum {CURRENT_RECORD, NEW_RECORD, EDIT_RECORD, PRIMARY_KEY, 
      SELECTED_PRIMARY_KEY};

enum RowState {NORMAL, TEXTBOX, HIGHLIGHTED};

#define GRID_APPEND_ROW_MSG		'gapr'
#define GRID_INSERT_ROW_MSG 	'ginr'
#define GRID_GOTO_ROW_MSG		'gtor'
#define GRID_GOTO_PREV_ROW_MSG 	'gtpr'
#define GRID_GOTO_NEXT_ROW_MSG  'gtnr'
#define GRID_DELETE_ROW_MSG 	'gdlr'
#define GRID_GOTO_LAST_ROW_MSG  'gltr'
#define GRID_GOTO_NEW_ROW_MSG   'gnwr'


#endif

