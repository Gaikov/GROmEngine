// Copyright (c) 2003-2007, Gaikov Roman. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Parser.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_Parser_H_
#define	_Parser_H_

#define		PS_MAX_NAME_LEN	64
#define		PS_MAX_STR_LEN	256
#define		MAX_BLOCKS		16

struct script_state_t;

//init/free
script_state_t*	ps_begin( char *block, char prefix = '$' );		//set text block for parsing
void			ps_end( script_state_t *ss );					//release state and close all blocks

//block manipulation
bool			ps_block_begin( script_state_t *ss, const char *name );	//if name == NULL begin any block
void			ps_block_end( script_state_t *ss );					//stop parsing block
bool			ps_block_next( script_state_t *ss );				//go to next block and begin parsing

//-----------------------------------------------------
//  ps_block_get:  return text of current block
//-----------------------------------------------------
const char*		ps_block_get( script_state_t *ss, /*out*/ char *name, int nameSize );
const char*     ps_block_name( script_state_t *ss);

//variable parsing
//-----------------------------------------------------
//  ps_var_begin: start searching var
//-----------------------------------------------------
bool			ps_var_begin( script_state_t *ss, const char *name );
//-----------------------------------------------------
//  ps_var_next:  search next var placement if found call next functions
//-----------------------------------------------------
bool			ps_var_next( script_state_t *ss );				

// parse data from found variable position
float			ps_var_f( script_state_t *ss );
bool			ps_var_2f( script_state_t *ss, float val[2] );
bool			ps_var_3f( script_state_t *ss, float val[3] );
bool			ps_var_4f( script_state_t *ss, float val[4] );
bool			ps_var_str( script_state_t *ss, char *str, int len );
char*			ps_var_str( script_state_t *ss );

// ������� ��� �������������� �������� ����������
const char*		ps_var_fmt( script_state_t *ss, const char *fmt );
const char*		ps_var_ptr( script_state_t *ss );

/*=============== ������ ������������� ==================
	float	f[3];
	char *fmt = ps_var_fmt( ss, "A: %f, B: %f, C: %f" );
	char *ptr = ps_var_ptr( ss );
	if ( fmt && ptr )
		sscanf( ptr, fmt, &f[0], &f[1], &f[3] );
========================================================*/

// ��������������� �������
// ��� ��������� ������ ���������� ���������� �� ����� 
// (�� ����� �������� ps_var_begin/ps_var_next)
float			ps_get_f( script_state_t *ss, const char *name, float defVal = 0 );
bool			ps_get_2f( script_state_t *ss, const char *name, float val[2], const float defVal[2] = 0 );
bool			ps_get_3f( script_state_t *ss, const char *name, float val[3], const float defVal[3] = 0 );
bool			ps_get_4f( script_state_t *ss, const char *name, float val[4], const float defVal[4] = 0 );
bool			ps_get_color( script_state_t *ss, const char *name, float val[4] );
const char*		ps_get_str( script_state_t *ss, const char *name, const char *defVal = 0 );

//test 
//void	ps_test( char *data );

#endif	//_Parser_H_