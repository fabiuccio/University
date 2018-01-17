import java_cup.runtime.*;

%%

/* JFlex produrra' in uscita una classe dal nome Lexer.class */
%class Lexer
/* Per la compatibilita'  con Cup */
%cup
/* Viene settata la variabile yyline con il numero di linea attualente letta dallo scanner:
tale variabile potra' essere passata allo scanner assieme ai Token riconosciuti              */
%line
/* Opzionale e abbastanza inutile in questo esercizio: serve per fare in modo che JFlex
setti in yycolumn il numero di colonna dell'ultimo carattere riconosciuto dallo scanner */
%column
/* Stato di tipo esclusivo comment */
%state comment


%{
  private Symbol symbol(int type) {
    return new Symbol(type, yyline, yycolumn);
  }
  private Symbol symbol(int type, Object value) {
    return new Symbol(type, yyline, yycolumn, value);
	
  }
%}

nl 		= 	\r | \n | \r\n
ws 		= 	[ \t]+
numero 	= 	[0-9][0-9]* /* Tra numero e atomo, in una string composta da soli numeri, sceglie il numero. */
atomo 	= 	[a-z0-9][A-Za-z0-9_]*
my_type 	= 	[A-Z][A-Za-z0-9_]*
type		=	String|Integer|int|[Bb]oolean|[Ff]loat|[Dd]ouble|[Ss]hort|[Cc]har|[Ll]ong|[Bb]yte|provide|request|listen|send
keyword	=	Society|Agent|Service|Role|Communication|Documentation|Code|attrib|name|type|priviledge|receiver|task|protocol|ontology|content|id
plain_text	=	[_][^}]*

%%

{nl}|{ws}	{;}


"{" 			{return symbol(sym.GRAFFAPERTA);}
"}" 			{return symbol(sym.GRAFFACHIUSA);}
"[" 			{return symbol(sym.QUADRAPERTA);}
"]" 			{return symbol(sym.QUADRACHIUSA);}
"|" 			{return symbol(sym.OR);}
"." 			{return symbol(sym.PT);}
";" 			{return symbol(sym.PTVIR);}
"," 			{return symbol(sym.VIR);}
"::" 			{return symbol(sym.ATTRIBUZIONE, new String("::"));}
{numero} 		{return symbol(sym.NUMERO);}
{keyword}		{return symbol(sym.KEYWORD, new String(yytext()));}
{type}		{return symbol(sym.TYPE, new String(yytext()));}
{my_type}		{return symbol(sym.MY_TYPE, new String(yytext()));}
{atomo} 		{return symbol(sym.ATOMO, new String(yytext()));}
{plain_text}	{return symbol(sym.PLAIN_TEXT, new String(yytext()));}




/* Si ricorda che questa regola (atomo) deve comparire dopo le
altre regole, in particolare quelle relative alle parole
chiave, altrimenti a causa delle regole di risoluzione
delle ambiguita' tutte le keyword verrebbero erroneamente
riconosciute come identificatori */

"/*" 			{yybegin(comment);}
<comment>{
	[^*]* 	{;}
	"*"+[^*/]* 	{;}
	"*"+"/" 	{yybegin(YYINITIAL);}
}


