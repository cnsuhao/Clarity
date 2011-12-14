# Overview
Clarity is a programming language with the following attributes:

* Compiled
* Statically typed
* Real memory addressing
* C-compatible
* Syntactically script-like
* Whitespace aware
* Event driven runtime
* Condition less (but has condition tables)
* Loop less (but has loop tables)
* Made for parallel execution, but without threads
* "Table" based (definition tables, execution tables)
* "Tight", very few keywords, very few brackets, parenthesis etc
* "Consistent", indentation/line breaks/spacing is relevant

Clarity is a "C-clone", made for development in environments 
where a VM would be inappropriate, such as OS/driver/embedded 
development. The purpose of the language is to bring some of 
the syntactical niceties lately added to more modern dynamic 
languages to the hardware aware environments.  
Besides being a syntactically different C environment, Clarity 
has a different approach to looping and branching, forcing 
the program to be more divided into blocks. It also has a small 
runtime, adding features such as array handling, event handling 
etc.
Clarity converts into C-code, and is fully compatible with C. 
C can call Clarity functions, and Clarity can call C functions.

# Tables
The Clarity syntax consists of a set of "tables", either 
declaring types, or listing execution instructions. The basic 
format of a table is:

	[exports.]<tablename>[<parameters>]= <tabletype>
		<body>

For example, this is how a serial execution table is defined:

	functionName = (/Uint16/param) ->
		otherFunction param

This code is basically the same as a normal C-function:

	static void functionName(Uint16 param)
	{
		otherFunction(param);
	}

Tables are implicitly "private". To make them visible to other 
files the "export." prefix needs to be used:

	export.functionName = (/Uint16/param) ->
		otherFunction param

This is equivalent to the following in C:

	void functionName(Uint16 param)
	{
		otherFunction(param);
	}

Another example is an enum definition table:

	enumName = %>
		ONE
		TWO
		THREE

This is similar to a C enum:

	typedef {
		ONE,
		TWO,
		THREE
	} enumName;


# Definition Tables
Since Clarity is a statically typed language, we need to be able to define 
types. This is done using the definition tables. There are four types of 
definition tables, function pointers, consts, enums and structs.

## >>  -  Function pointer
A function pointer table is declared in the following way:

	FuncTypeName = (\ParamType\param1, \Paramtype2\param2) >>

Function pointer tables has no body.

## |>  -  Const
A const table is declared in the following way:

	CONST_NAME = |>
		\Uint16\264

## %>  -  Enum
An enum table is declared in the following way:
	EnumType = %>
		ONE
		TWO
		THREE

## $>  -  Struct
A struct table is declared in the following way:

	StructType = $>
		\Bool\someBool
		\UInt16\someInt
		\SomeType\someType

# Execution Tables
## ->  -  Serial
## =>  -  Parallel
## ->  -  Conditional
## />  -  Map