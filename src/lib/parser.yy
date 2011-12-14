%lex

Digits [0-9]+
Oct_Digit [0-7]
Hex_Digit [a-fA-F0-9]
Int_Literal [1-9][0-9]*
Oct_Literal '0'{Oct_Digit}*
Hex_Literal '0'[xX]{Hex_Digit}*

%%

[ \t]*'#'[^\n]*\n {}
[\n][\t] { return 'STATEMENTOPEN' }
"true" { return 'TRUE' }
"false" { return 'FALSE' }
" ? " { return 'QUESTION' }
" = " { return 'ASSIGNMENT' }
"|>" { return 'CONSTDEFINE' }
"%>" { return 'ENUMDEFINE' }
"$>" { return 'STRUCTDEFINE' }
"?>" { return 'CONDITIONDEFINE' }
"->" { return 'SERIALDEFINE' }
"=>" { return 'PARALLELDEFINE' }
">>" { return 'FUNCDEFINE' }

"@" { return 'EXPORTS' }
"requires " { return 'STARTREQUIRE' }
"(" {return 'LEFT_PARANTHESIS'}
")" {return 'RIGHT_PARANTHESIS'}
"[" {return 'LEFT_SQUARE_BRACKET'}
"]" {return 'RIGHT_SQUARE_BRACKET'}
"." { return 'DOT' }
"," { return 'COMMA' }
"'" { return 'SINGLEQUOTE' }
" " { return 'SPACE' }
"*" { return 'VALUEOF' }
\\ { return 'BACKSLASH' }
[\n]+ { return 'TABLEEND' }
\n<<EOF>> { return 'TABLEEND' }

[a-zA-Z_][a-zA-Z0-9_]* {return 'IDENTIFIER'}
{Int_Literal} {return 'INTEGER_LITERAL'}
{Hex_Literal} {return 'INTEGER_LITERAL'}
{Oct_Literal} {return 'INTEGER_LITERAL'}

%%

/lex

%%

clarity
	: requires TABLEEND tables { return [].concat($1, $3);}
	| tables {return $1;}
	;

requires
	: require {$$ = [$1];}
	| requires require {$$ = [].concat($1, $2);}
	;

require
	: require_item {$1.scope ='internal'; $$ = $1;}
	| EXPORTS require_item {$2.scope ='external'; $$ = $2;}
	;

require_item
	: STARTREQUIRE IDENTIFIER TABLEEND {$$ = {type:'require', value:$3};}
	;

tables
	: table {$$ = [$1];}
	| table tables {$$ = [].concat($1, $2);}
	;

table
	: EXPORTS IDENTIFIER SPACE define_table {$4.name = $2; $4.scope = 'external'; $$ = $4;}
	| IDENTIFIER SPACE define_table {$2.name = $1; $2.scope = 'internal'; $$ = $2;}
	;

define_table
	: CONSTDEFINE const_statement TABLEEND {$$ = {type:'const', body:$2};}
	| ENUMDEFINE enum_statements TABLEEND {$$ = {type:'enum', body:$2};}
	| STRUCTDEFINE struct_statements TABLEEND {$$ = {type:'struct', body:$2};}
	| execution_table {$$ = $1;}
	;

execution_table
	: define_params SPACE execution_table_item {$3.input=$1;$$ = $3;}
	;

execution_table_item
	: PARALLELDEFINE parallel_statements TABLEEND {$$ = {type:'parallel', body:$2};}
	| SERIALDEFINE serial_statements TABLEEND {$$ = {type:'serial', body:$2};}
	| CONDITIONDEFINE condition_statements TABLEEND {$$ = {type:'condition', body:$2};}
	| FUNCDEFINE TABLEEND {$$ = {type:'signature'};}
	;

define_params
	: define_param {$$ = [$1];}
	| define_params SPACE define_param {$$ = [].concat($1, $3);}
	;

define_param
	: define_type IDENTIFIER {$$ = {type:$1, name:$2};}
	;

define_type
	: BACKSLASH IDENTIFIER BACKSLASH {$$ = $2;}
	;

enum_statements
	: enum_statements enum_statement {$$ = [].concat($1, $2);}
	| enum_statement {$$ = [$1];}
	;

struct_statements
	: struct_statements struct_statement {$$ = $1.concat($2);}
	| struct_statement {$$ = [$1];}
	;

parallel_statements
	: parallel_statements parallel_statement {$$ = $1.concat($2);}
	| parallel_statement {$$ = [$1];}
	;

serial_statements
	: serial_statements serial_statement {$$ = $1.concat($2);}
	| serial_statement {$$ = [$1];}
	;

condition_statements
	: condition_statements condition_statement {$$ = $1.concat($2);}
	| condition_statement {$$ = [$1];}
	;

const_statement
	: STATEMENTOPEN BACKSLASH IDENTIFIER BACKSLASH primary_expression {$$ = {type:$3, value:$5};}
	;

enum_statement
	: STATEMENTOPEN IDENTIFIER {$$ = {name:$2};}
	| STATEMENTOPEN IDENTIFIER SPACE INTEGER_LITERAL {$$ = {name:$2, value:$4};}
	;

struct_statement
	: STATEMENTOPEN BACKSLASH IDENTIFIER BACKSLASH IDENTIFIER {$$ = {type:$3, name:$5};}
	| STATEMENTOPEN BACKSLASH IDENTIFIER LEFT_SQUARE_BRACKET INTEGER_LITERAL RIGHT_SQUARE_BRACKET BACKSLASH IDENTIFIER {$$ = {type:$3, name:$8, length:$5};}
	;

parallel_statement
	: STATEMENTOPEN statement {$$ = $2;}
	;

serial_statement
	: STATEMENTOPEN statement {$$ = $2;}
	| STATEMENTOPEN declaration {$$ = $2;}
	;

condition_statement
	: STATEMENTOPEN statement QUESTION statement
	;

statement
	: IDENTIFIER {$$ = {type:'statement', name:$1};}
	| IDENTIFIER SPACE params {$$ = {type:'statement', name:$1, params:$3};}
	;

params
	: primary_expression {$$ = [$1];}
	| params COMMA SPACE primary_expression {$$ = $1.concat($4);}
	;

declaration
	: BACKSLASH IDENTIFIER BACKSLASH IDENTIFIER {$$ = {type:'declaration', declaration:{type:$2, name:$4}};}
	;

primary_expression
	: INTEGER_LITERAL {$$ = $1;}
	| TRUE {$$ = $1;}
	| FALSE {$$ = $1;}
	;

%%
