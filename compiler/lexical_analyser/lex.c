/***************************************************
*
*参照了lcc的cpp实现
*
*Copyright(C)
*FileName:
*Author:
*Version:
*Date:
*Description:
*Others:
*Function List:
*History:
*	1.Date:
*	  Author:
*	  Modification:
date:2016-09-20
author:heqq
****************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#define ACTION_STATE 31
#define ACT(token_type,state) ((token_type<<7) | state)
#define GETACT(state) (state>>7 & 0x1ff)
#define MARKER 0100

//total state:42
enum STATE {
	//each line contains ten states
	START,ID1,NUM1,NUM2,NUM3,DOT1,STR1,STR2,STR3,CC1,
	CC2,WS1,PLUS1,MINUS1,COM1,COM2,COM3,COM4,STAR1,PERSENT1,
	GT1,GT2,LT1,LT2,OR1,AND1,NOT1,CIRC1,ASG1,SHARP1,
	// each line contains five states
	END=ACTION_STATE,END_FORWARD,S_NL,S_EOF,STR_EOF,
	STR_NL,S_COMNL,S_EOFCOM,S_COMMENT,S_WS,
	S_NAME,S_EOB
};

//define four `char' type
#define C_ALL	 	0
#define C_LETTER 	1
#define C_DIGIT 	2
#define C_WS 		3

typedef unsigned char uchar;
//define two special char:end of buffer & end of file
#define EOB	0xfe  
#define EOFC 0xff
//total type: 54
enum TOKEN_TYPE{
	//each line contains ten type
	NAME,NUMBER,ELLIPS,STRING,CHAR_CONST,NL,WS,DOT,PLUS,D_PLUS,
	ASSIGN_PLUS,MINUS,D_MINUS,ASSIGN_MINUS,ARROW,STAR,ASSIGN_STAR,SLASH,ASSIGN_SLASH,PERSENT,
	ASSIGN_PERSENT,GT,GEQ,LT,LEQ,RSH,ASSIGN_RSH,LSH,ASSIGN_LSH,OR,
	ASSIGN_OR,LOR,AND,ASSIGN_AND,LAND,NOT,NEQ,TITDE,ASSIGN,EQ,
	CIRC,ASSIGN_CIRC,SHARP,D_SHARP,LEFT_BRAKET,RIGHT_BRAKET,LEFT_PARENTHESIS,RIGHT_PARENTHESIS,LEFT_BRACE,RIGHT_BRACE,
	COMMA,COLON,SEMIC,QUEST
};
#ifdef _DEBUG
char *token_string[]={
	"id",	"number",	"...",	"string",	"char",		"new line",		"whitespace",	".",	"+",	"++",
	"+=",	"-",		"--",	"-=",		"->",		"*",			"*=",			"/",	"/=",	"%",
	"%=",	">",		">=",	"<",		"<=",		">>",			">>=",			"<<",	"<<=",	"|",
	"|=",	"||",		"&",	"&=",		"&&",		"!",			"!=",			"~",	"=",	"==",
	"^",	"^=",		"#",	"##",		"[",		"]",			"(",			")",	"{",	"}",
	",",	":",		";",	"?"
};
#define DEBUG(token_type)  printf("%s",token_string[token_type]);
#endif

//define `finite state machine'
struct fsm{
	int state;
	uchar ch;
	int next_state;
};

struct fsm fsm[]={
	START,	C_ALL,		START,

	// see `letter' or `_', maybe a ID 
	START,	C_LETTER,	ID1,
	START,	'_',		ID1,

	START,	C_DIGIT,	NUM1,
	START,	'.',		NUM2,

	START,	'L',		STR1,
	START,	'"',		STR2,

	START,	' ',		WS1,
	START,	'\t',		WS1,
	START,	'\v',		WS1,

	START,	'+',		PLUS1,
	START,	'-',		MINUS1,
	START,	'/',		COM1,
	START,	'*',		STAR1,
	START,	'%',		PERSENT1,

	START,	'>',		GT1,
	START,	'<',		LT1,
	START,	'|',		OR1,
	START,	'&',		AND,
	START,	'!',		NOT1,
	START,	'~',		ACT(TITDE,END_FORWARD),
	START,	'=',		ASG1,
	START,	'^',		CIRC1,

	START,	'#',		SHARP1,
	START,	'[',		ACT(LEFT_BRAKET,END_FORWARD),
	START,	']',		ACT(RIGHT_BRAKET,END_FORWARD),
	START,	'(',		ACT(LEFT_PARENTHESIS,END_FORWARD),
	START,	')',		ACT(RIGHT_PARENTHESIS,END_FORWARD),
	START,	'{',		ACT(LEFT_BRACE,END_FORWARD),
	START,	'}',		ACT(RIGHT_BRACE,END_FORWARD),
	START,	',',		ACT(COMMA,END_FORWARD),
	START,	':',		ACT(COLON,END_FORWARD),
	START,	';',		ACT(SEMIC,END_FORWARD),
	START,	'?',		ACT(QUEST,END_FORWARD),
	START,	'\n',		S_NL,
	START,	EOFC,		S_EOF,

	// id consists of 'digit' 'letter' '_ ' and starts with `letter' or `_'
	ID1,	C_ALL,		ACT(NAME,END),
	ID1,	C_LETTER,	ID1,
	ID1,	C_DIGIT,	ID1,
	ID1,	'_',		ID1,

	// number consists of `letter' `digit','e|E|+|-'
	// example:0xff,	.3155,	3.2e+5, 2.6e-0xff
	// see a `digit' 
	NUM1,	C_ALL,		ACT(NUMBER,END),
	NUM1,	C_DIGIT,	NUM1,
	NUM1,	C_LETTER,	NUM1,
	NUM1,	'.',		NUM1,
	NUM1,	'E',		NUM3,
	NUM1,	'e',		NUM3,

	// see a `.',maybe `...'
	NUM2,	C_ALL,		ACT(DOT,END),
	NUM2,	C_DIGIT,	NUM1,
	NUM2,	'.',		DOT1,

	//see a `E' 
	NUM3,	C_ALL,		ACT(NUMBER,END),
	NUM3,	'+',		NUM1,
	NUM3,	'-',		NUM1,
	NUM3,	C_DIGIT,	NUM1,
	NUM3,	C_LETTER,	NUM1,

	// starts with L, maybe a string or a char
	STR1,	C_ALL,		ACT(NAME,END),
	STR1,	'"',		STR2,
	STR1,	'\'',		CC1,

	//see a string maybe a multi-line 
	STR2,	C_ALL,		STR2,
	STR2,	'"',		ACT(STRING,END_FORWARD),
	STR2,	'\n',		STR_NL,
	STR2,	EOFC,		STR_EOF,
	STR2,	'\\',		STR3,
	
	STR3,	C_ALL,		STR2,
	STR3,	'\n',		STR_NL,
	STR3,	EOFC,		STR_EOF,

	//see a char. maybe foldline
	CC1,	C_ALL,		CC1,
	CC1,	'\'',		ACT(CHAR_CONST,END_FORWARD),
	CC1,	'\\',		CC2,
	CC1,	'\n',		STR_NL,
	CC1,	EOFC,		STR_EOF,

	CC2,	C_ALL,		CC1,
	CC2,	'\n',		STR_NL,
	CC2,	EOFC,		STR_EOF,

	//eat up ws
	WS1,	C_ALL,		S_WS,
	WS1,	' ',		WS1,
	WS1,	'\t',		WS1,
	WS1,	'\v',		WS1,

	// see +, ++, +=
	PLUS1,	C_ALL,		ACT(PLUS,END),
	PLUS1,	'+',		ACT(D_PLUS,END_FORWARD),
	PLUS1,	'=',		ACT(ASSIGN_PLUS,END_FORWARD),

	//see -, --, -=, ->
	MINUS1,	C_ALL,		ACT(MINUS,END),
	MINUS1,	'-',		ACT(D_MINUS,END_FORWARD),
	MINUS1,	'=',		ACT(ASSIGN_MINUS,END_FORWARD),
	MINUS1,	'>',		ACT(ARROW,END_FORWARD),

	//see /, /=, //, /*
	COM1,	C_ALL,		ACT(SLASH,END),
	COM1,	'=',		ACT(ASSIGN_SLASH,END_FORWARD),
	COM1,	'*',		COM2,
	COM1,	'/',		COM4,

	//comments start with `/*'
	COM2,	C_ALL,		COM2,
	COM2,	'*',		COM3,
	COM2,	'\n',		S_COMNL,
	COM2,	EOFC,		S_EOFCOM,

	//maybe end of comment: `*/'
	COM3,	C_ALL,		COM2,
	COM3,	'/',		S_COMMENT,
	COM3,	'\n',		S_COMNL,
	COM3,	EOFC,		S_EOFCOM,

	//comment starts with `//'
	COM4,	C_ALL,		COM4,
	COM4,	'\n',		S_COMNL,
	COM4,	EOFC,		S_EOFCOM,

	//starts with `*',`*='
	STAR1,	C_ALL,		ACT(STAR,END),
	STAR1,	'=',		ACT(ASSIGN_STAR,END_FORWARD),

	//starts with `%', `%='
	PERSENT1,	C_ALL,	ACT(PERSENT,END),
	PERSENT1,	'=',	ACT(ASSIGN_PERSENT,END_FORWARD),

	//see  >,>=, >>, >>=
	GT1,	C_ALL,		ACT(GT,END),
	GT1,	'=',		ACT(GEQ,END_FORWARD),
	GT1,	'>',		GT2,

	GT2,	C_ALL,		ACT(RSH,END),
	GT2,	'=',		ACT(ASSIGN_RSH,END_FORWARD),

	//see <, <=, <<, <<=
	LT1,	C_ALL,		ACT(LT,END),
	LT1,	'=',		ACT(LEQ,END_FORWARD),
	LT1,	'<',		LT2,

	LT2,	C_ALL,		ACT(LSH,END),
	LT2,	'=',		ACT(ASSIGN_LSH,END_FORWARD),

	//see |,|=, ||
	OR1,	C_ALL,		ACT(OR,END),
	OR1,	'=',		ACT(ASSIGN_OR,END_FORWARD),
	OR1,	'|',		ACT(LOR,END_FORWARD),

	// see &,&=,&&
	AND1,	C_ALL,		ACT(AND,END),
	AND1,	'=',		ACT(ASSIGN_AND,END_FORWARD),
	AND1,	'&',		ACT(LAND,END_FORWARD),

	//see !, !=		
	NOT1,	C_ALL,		ACT(NOT,END),
	NOT1,	'=',		ACT(NEQ,END_FORWARD),

	//see =, ==
	ASG1,	C_ALL,		ACT(ASSIGN,END),
	ASG1,	'=',		ACT(EQ,END_FORWARD),

	//see ^, ^=
	CIRC1,	C_ALL,		ACT(CIRC,END),
	CIRC1, 	'=',		ACT(ASSIGN_CIRC,END_FORWARD),

	//see # ##
	SHARP1,	C_ALL,		ACT(SHARP,END),
	SHARP1,	'#',		ACT(D_SHARP,END_FORWARD),

	-1
};

int bigmap[256][ACTION_STATE];

//try to read file into in_buffer in once
//out buffer align with 4k
#define IN_BUFFER 	32768
#define OUT_BUFFER	4096
#define new(t) (t*)do_malloc(sizeof(t))

typedef struct source {
	char *FileName;
	int line;
	int lineinc;
	uchar *in_buffer;
	uchar *in_current;
	uchar *in_last;
	FILE *fp;
	struct source *next;
}Source;

Source *current_source=NULL;

typedef struct token{
	uchar *first;
	int len;
	int type;
	struct token *next;
}Token;

typedef struct token_row{
	Token *t_first;
	Token *t_current;
	Token *t_last;
}TokenRow;

//fill in the bigmap
void expand_lex(){
	struct fsm *tmp;
	int next_state,i,j;
	for(tmp=fsm;tmp->state>=0;tmp++){
		next_state = tmp->next_state;
		if(tmp->next_state >= ACTION_STATE)
			// state lt zero which means special action need
			next_state = ~tmp->next_state;
		switch(tmp->ch){

			case C_ALL:	
				for(i=0;i<256;i++)
					bigmap[i][tmp->state] = next_state;
				break;
			case C_LETTER:
				for(i=0;i<256;i++)
					if(('a'<=i && i<='z') || ('A'<=i && i<='Z'))
						bigmap[i][tmp->state] = next_state;
				break;
			case C_DIGIT:
				for(i=0;i<256;i++)
					if('0'<=i && i<= '9')
						bigmap[i][tmp->state] = next_state;
				break;
			default:
				bigmap[tmp->ch][tmp->state] = next_state;
				break;
		}
	}
	//set special flag for `foldline', `trigraph'
	for(j=0;j<ACTION_STATE;j++){
		for(i=0;i<256;i++){
			if(i == '\\' || i == '?'){
				if(bigmap[i][j]>0)
					bigmap[i][j] = ~bigmap[i][j];
				bigmap[i][j] |= MARKER;
			}
		}
		// beacuse fsm has already set some EOF .
		// there needs to be gt zero.
		
		if(bigmap[EOFC][j] > 0)
			bigmap[EOFC][j] = ~S_EOF;
		//set EOB for special action
		bigmap[EOB][j] = ~S_EOB;
	}
}
void fillbuf(Source *s){
	/*	always read 4k into buffer
		we can't read 32k into buffer at once
		because token_row points some token 
	*/
	int nr=IN_BUFFER/8,n;
	if(s->in_last+nr > s->in_buffer + IN_BUFFER)
		error(FATAL,"buffer overflow");
	if(s->fp ==NULL || (n=fread(s->in_last,1,nr,s->fp)) < 0)
		n=0;
	//adjust the last pointer;
	s->in_last += n;
	s->in_last[0] = s->in_last[1] = EOB;
	//when approach the end of file set the sentinel
	if(n==0){
		s->in_last[0] = s->in_last[1] = EOFC;
		return EOF;
	}
	return 0;
}
void put_token(TokenRow *tk_row){
	Token *tmp = tk_row->t_first;
	int i;
	for(;tmp != tk_row->t_last;tmp=tmp->next){
		for(i=0;i<tmp->len;i++)
			printf("%c",tmp->first[i]);
		printf("\ttype:%d\n",tmp->type);
	}
}
void get_token(TokenRow *tk_row){
	Source *s = current_source;
	uchar *ip,ch;
	int state,old_state;
	Token *tp;
	ip = s->in_current;
	for(;;){
continue2:
		put_token(tk_row);
		tp = tk_row->t_current;
		if(tp >= tk_row->t_last){
			// attach a new Token at the last
			// last Token is blank
			tk_row->t_last = new(Token);
			tp->next = tk_row->t_last;
		}
		tp->first = ip;
		tp->len = 0;
		//set state to `START' begin fsm
		state = START;
		for(;;){
			ch = *ip;
			//存储当前状态，在语句EOB的时候用于恢复
			old_state = state;
			state = bigmap[ch][state];
			if(state > 0){
				// see next char
				ip++;
				continue;
			}
			// when state lt zero which means special action
			// the last seven bit stores state;
			state = ~state;
			//低7位存储的状态
			switch(state & 0177){
				// needs eat up next ch
				case END_FORWARD:
					ip++;
				case END:
					tp->len=ip - tp->first;
					tp->type = GETACT(state);
					//当前指针往后移
					tk_row->t_current = tp->next;
					goto continue2;
				case S_EOF:
					return ;
				case S_NL:
					break;
				case STR_EOF:
					error(FATAL,"EOF in string or const char");
					break;
				case STR_NL:
					error(FATAL,"Unterminated string or char const");
					break;
				case S_COMNL:
					state = COM2;
					ip++;
					continue;
				case S_EOFCOM:
					error(WARNING,"EOF in comment");
					break;
				case S_COMMENT:
					//eat up `/'
					ip++;
					state = START;
					continue;
				case S_WS:
					state = START;
					// skip ws
					tp->first = ip;
					continue;
				case S_NAME:
					break;
				case S_EOB:
					fillbuf(s);
					state = old_state;
					continue;
				default:
					if((state & MARKER) ==0 ){
						ip++;
						continue;
					}
					state 
					printf("NOT implemented!");
					break;
			}
			break;
		}
		ip++;
	}
}

// params: 	name -> filename
// 			fd -> file desc
// 			string -> when read from stdin represents the command arg
// 	if fd == NULL && string !=NULL read from string
// 	if fd != NULL && string == NULL read from file name
void set_source(char *name,FILE *fd,char *string){
	Source *s = new(Source);
	int len;
	s->FileName = name;
	s->line=1;
	s->lineinc=0;
	s->in_buffer=NULL;
	s->in_current=NULL;
	s->in_last=NULL;
	s->fp=fd;
	s->next=current_source;
	// set current_source points the first one
	current_source = s;
	// actual need 1 byte for set EOFC
	//from command arg
	if(fd == NULL && string != NULL){
		len=strlen(string);
		s->in_buffer = (char*)do_malloc(len+4);
		strncpy(s->in_buffer,string,len);
	}
	//from file 
	else {
		s->in_buffer = (char *)do_malloc(IN_BUFFER+4);
		len =0;
	} 
	s->in_current = s->in_buffer;
	s->in_last = s->in_buffer + len;
	s->in_last[0] = s->in_last[1] = EOB;
}

void usage(){
	char *usages[]={
		"usage: cpp [options] source",
		"options:	-v(version)",
		"		-V(verbose)",
		"		-+(Cplusplus)",
		"		-N(Not include)",
		"		-I(include)",
		"		-D(define)",
		"		-U(undefine)",
		"		-t(trigraphs)",
		NULL,	
	};
	char **help;
	for(help=usages;*help;*help++)
		printf("%s\n", *help);
}

int main(int argc,char **argv){
	TokenRow token_row;
	Token blank_token;
	token_row.t_first = token_row.t_current = token_row.t_last = &blank_token;
	if(argc == 1)
		usage();
	expand_lex();
	if(argc==2){
		FILE *fd;
		fd=fopen(argv[1],"r");
		if(fd == NULL){
			error(FATAL,"open file error");
		}
		set_source(argv[1],fd,NULL);
		get_token(&token_row);
	}
	return 0;
}