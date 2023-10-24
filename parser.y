%{
    #include"parser.h"
    #include"SymbolTable.h"
    #include<stdio.h>
    #include<stdlib.h>

    #define YY_DECL int alpha_yylex (void* ylval)

    int yyerror(char* yaccProvidedMessage);
    int yylex(void);
    int gscope=0;
    int funName=1;
    int maxScope=0;



    extern int yylineno;
    extern char* yytext;
    extern FILE* yyin;   

%}

%start program

%union{
    int intVal;
    char* stringVal;
    double realVal;
}


%token  IF
%token  ELSE 
%token  WHILE
%token  FOR
%token  FUNCTION
%token  RETURN
%token  BREAK
%token  CONTINUE
%token  AND
%token  NOT
%token  OR
%token  LOCAL
%token  TRUE
%token  FALSE
%token  NIL
%token  COMMENT
%token  COMMENT2
%token  COMMENT3
%token  ASSIGN
%token  PLUS
%token  MINUS
%token  MUL
%token  DIVIDE
%token  MOD
%token  EQUAL
%token  NOT_EQUAL
%token  PLUS_PLUS
%token  MINUS_MINUS
%token  BIGGER
%token  LESS
%token  BIGGER_EQUAL
%token  LESS_EQUAL
%token  LEFT_BRACE 
%token  RIGHT_BRACE
%token  LEFT_BRACKET
%token  RIGHT_BRACKET
%token  LEFT_PARENTHESIS
%token  RIGHT_PARENTHESIS
%token  SEMICOLON
%token  COMMA
%token  COLON
%token  DOUBLE_COLON
%token  DOT
%token  DOUBLE_DOT
%token  STRING
%token  <stringVal> ID 
%token  <intVal> INTEGER
%token  <realVal> REAL_CONSTANT


%right      ASSIGN
%left       OR 
%left       AND
%nonassoc   EQUAL NOT_EQUAL
%nonassoc   BIGGER BIGGER_EQUAL LESS LESS_EQUAL
%left       PLUS MINUS
%left       MUL DIVIDE MOD
%right      NOT  PLUS_PLUS MINUS_MINUS  
%nonassoc   UMINUS
%left       DOT DOUBLE_DOT
%left       LEFT_BRACE RIGHT_BRACE
%left       LEFT_BRACKET RIGHT_BRACKET
%left       LEFT_PARENTHESIS RIGHT_PARENTHESIS

%type   <stringVal> lvalue

%%

program:            stmts
                    ;

stmts:               stmt stmts 
                    |
                    ;

stmt:               expr SEMICOLON
                    |ifstmt
                    |whilestmt
                    |forstmt
                    |returnstmt
                    |BREAK SEMICOLON
                    |CONTINUE SEMICOLON
                    |block
                    |funcdef
                    |SEMICOLON
                    ;

expr:               assignexpr   
                    |expr PLUS expr
                    |expr MUL expr
                    |expr DIVIDE expr 
                    |expr MOD expr
                    |expr EQUAL expr
                    |expr BIGGER expr
                    |expr BIGGER_EQUAL expr
                    |expr LESS expr
                    |expr LESS_EQUAL expr
                    |expr NOT_EQUAL expr
                    |expr AND expr
                    |expr OR expr 
                    |term
                    ;

term:               LEFT_PARENTHESIS expr RIGHT_PARENTHESIS 
                    |UMINUS expr
                    |NOT expr 
                    |PLUS_PLUS lvalue {
                        int isF=checkForFunction(gscope,$2);
                        if(isF==1){
                            yyerror("This Symbol is a Function and you cannot increment it!");
                        }
                    }
                    |lvalue PLUS_PLUS {
                        int isF=checkForFunction(gscope,$1);
                        if(isF==1){
                            yyerror("This Symbol is a Function and you cannot increment it!");
                        }
                    }
                    |MINUS_MINUS lvalue {
                        int isF=checkForFunction(gscope,$2);
                        if(isF==1){
                            yyerror("This Symbol is a Function and you cannot increment it!");
                        }
                    }
                    |lvalue MINUS_MINUS {
                        int isF=checkForFunction(gscope,$1);
                        if(isF==1){
                            yyerror("This Symbol is a Function and you cannot increment it!");
                        }
                    }
                    |primary

assignexpr:         lvalue {
                        int isF=checkForFunction(gscope,$1);
                        if(isF==1){
                            yyerror("This Symbol is a Function and you cannot assign it");
                        }
                    } ASSIGN expr 
                    ;


primary:            lvalue
                    |call
                    |objectdef
                    |LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS    
                    |const
                    ;

lvalue:             ID {
                        int chf=checkForFunction(gscope,$1);
                        if(chf==1){
                            yyerror("Symbol is a function");
                        }else{
                            int lkp=lookUp_inglobal(gscope,$1);
                            if(lkp==0){
                                if(gscope==0){
                                    set_Entry(gscope,yylineno,$1,GLOBALV,10);
                                }else{
                                    set_Entry(gscope,yylineno,$1,LOCALV,10);
                                }
                            }
                        }
                       

                    }
                    |LOCAL ID  {
                        int chc=check_collisions($2);
                        int chf=checkForFunction(gscope,$2);
                        if(chc==1) {
                            yyerror("Collision with library function name");
                        }else if(chf==1){
                            yyerror("Symbol is a function");
                        }else{
                            int lkp=lookUp_inscope(gscope,$2); 
                            if(lkp==0) {
                                if(gscope==0){
                                    set_Entry(gscope,yylineno,$2,GLOBALV,10);
                                }
                                else set_Entry(gscope,yylineno,$2,LOCALV,10);
                            }
                        }     
                    } 
                    |DOUBLE_COLON ID {int lkp=lookUp_namespace(0,$2); if(lkp==0) yyerror("Symbol doesn't exist");}
                    |member
                    ;

member:             lvalue DOT ID
                    |lvalue LEFT_BRACKET expr RIGHT_BRACKET
                    |call DOT ID
                    |call LEFT_BRACKET expr RIGHT_BRACKET
                    ;

call:               call  LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
                    |lvalue {
                        
                        int isF=checkForFunction(gscope,$1);
                        int lkp;
                        if(isF==0){
                            lkp=lookUp_inglobal(gscope,$1);
                            if(lkp==0){
                                if(gscope==0){
                                    set_Entry(gscope,yylineno,$1,GLOBALV,10);
                                }else{
                                    set_Entry(gscope,yylineno,$1,LOCALV,10);
                                }   
                            }
                        }
                        
                    }callsuffix
                    |LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
                    ;

callsuffix:         normcall
                    |methodcall
                    ;

normcall:           LEFT_PARENTHESIS elist RIGHT_PARENTHESIS   
                    ;

methodcall:         DOUBLE_DOT ID LEFT_PARENTHESIS elist RIGHT_PARENTHESIS // equivalent to lvalue.id(lvalue,elist)
                    ;

elist:              expr exprs
                    |
                    ;


exprs:              COMMA expr exprs
                    |
                    ;

objectdef:          LEFT_BRACKET elist RIGHT_BRACKET
                    |LEFT_BRACKET indexed RIGHT_BRACKET
                    ;

indexed:            indexedelem COMMA indexedelem
                    indexedelem
                    ;
 

indexedelem:        LEFT_BRACE expr COLON expr RIGHT_BRACE
                    ;

block:              LEFT_BRACE{gscope++;  if(gscope>=maxScope){maxScope=gscope;}} stmts RIGHT_BRACE {hide(gscope); gscope--;}
                    ;

funcdef:            FUNCTION ID{
                        int lkp=lookUp_inFunction(gscope,$2);
                        if(lkp==1) yyerror("Collision with name of an ID/FUNCTION");
                        else set_Entry(gscope,yylineno,$2,USERFUNC,11);
                    } LEFT_PARENTHESIS{gscope++;  if(gscope>=maxScope){maxScope=gscope;}} idlist RIGHT_PARENTHESIS {gscope--;} block
                    |FUNCTION {
                        char st1[50]="_f";
                        char st2[50];
                        sprintf(st2,"%d",funName);
                        strcat(st1,st2);  
                        set_Entry(gscope,yylineno,st1,USERFUNC,11);
                        funName++;
                    }LEFT_PARENTHESIS idlist RIGHT_PARENTHESIS block
                    ;

const:              INTEGER 
                    |STRING 
                    |NIL
                    |TRUE
                    |FALSE
                    ;

idlist:             ID COMMA ID {
                        set_Entry(gscope,yylineno,$1,FORMAL,10);
                        set_Entry(gscope,yylineno,$3,FORMAL,10);

                    }
                    |ID {set_Entry(gscope,yylineno,$1,FORMAL,10);}
                    |
                    ;

ifstmt:             IF LEFT_PARENTHESIS expr RIGHT_PARENTHESIS stmt ELSE stmt
                    |IF LEFT_PARENTHESIS expr RIGHT_PARENTHESIS stmt
                    ;

whilestmt:          WHILE LEFT_PARENTHESIS expr RIGHT_PARENTHESIS stmt
                    ;


forstmt:            FOR LEFT_PARENTHESIS elist SEMICOLON expr SEMICOLON elist SEMICOLON RIGHT_PARENTHESIS stmt
                    ;

returnstmt:         RETURN expr SEMICOLON
                    RETURN SEMICOLON
                    ;

%%

/* yyerror function */
int yyerror(char* yaccProvidedMessage){
    fprintf(stderr, "%s at line: %d, before token: %s,  INPUT NOT VALID\n",yaccProvidedMessage,yylineno,yytext);
}

/* main function */
int main(int argc,char** argv){
    
    if(argc>1){
        if(!(yyin=fopen(argv[1],"r"))){
            fprintf(stderr,"Cannot read file: %s\n",argv[1]);
          return 1;
        }
    }else {
        yyin=fopen( argv[1], "r" );
    }

    Header=hash_Table();                /* Create a new hash table and store its address to the global pointer Header */ 
    default_Libs();                     /* Set at scope 0 all default libraries */
    printf("\n\n\tGeneral Output\n\n");
    yyparse();                          /* Begin Parsing */
    printf("\n\n\n\tSymbol Table\n\n\n");
    print_HT(Header,maxScope);                   /* Print the table */
    printf("\n\n");
    free(scopeList);
    free_HT(Header);                    /* free memory */

    return 0;
}




