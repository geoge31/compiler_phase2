#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define SIZE 10


/* Enum Types */
typedef enum SymbolType {
    GLOBALV,LOCALV,FORMAL,USERFUNC,LIBFUNC
}symtype;




/* Symbol Table Struct */
typedef struct SymbolTableEntry{
    unsigned int scope;                                     /* scope of symbol */
    unsigned int line;                                      /* line of symbol (yylineno)*/
    char *name;                                             /* name of symbol */
    symtype type;                                           /* type of symbol */
    int isActive;                                           /* if the symbol is not hide */
    struct SymbolTableEntry *next;                          /* pointer to store the next symbol */
    struct SymbolTableEntry *nextScope;                     /* pointer to store the next symbol with the same scope */
}symtable;




/* Hash Table Struct */
typedef struct HashTable{
    symtable **entries;                                     /* An array of pointers to symbol table entries*/
    int max_scope;
}hashtable;




/* Global Pointer to Hash Table*/
hashtable *Header;
/* A Global Pointer to store all scopes in a list */
symtable *scopeList=NULL;




/* Hashing Function */
int hash_Function(int key){
    return (key%SIZE);
}




/* check if the given name is a function */
int checkForFunction(unsigned int bscope,char *bname){

    /* declarations */
    symtable *cf;

    /* assignments */
    cf=Header->entries[1];

    /* code */
    while(cf){
        if(strcmp(cf->name,bname)==0 && cf->isActive==1){                      /* if the given name (bname) is equal to a node in the lsit */
            if(cf->type==4 || cf->type==3){                 /* if the type of the node is function */
                return 1;                                   /* then return that it is a function  */
            }   
        }
        cf=cf->next;
    }

 
    return 0; 
}





/* Create a new hash table */
hashtable* hash_Table(){

        /* declarations */
        int i;
        hashtable *nt;

        /* memory alloc*/                             
        nt=malloc(sizeof(hashtable));
        nt->entries=malloc(sizeof(symtable)*SIZE);
        nt->max_scope=-1;

        /* code*/
        for(i=0;i<SIZE;i++){
            nt->entries[i]=NULL;                            /* initialise all buckets to point NULL*/
        }
        return nt;
}




/* lookup new */
int lookUp_inglobal(unsigned int nscope, char *cname){
    
    /* declarations */
    int scope;
    symtable *p;
  

    /* assignments */
    scope=nscope;
   

    /* code */
    while(scope>=0){   
        p=Header->entries[0];                                                                                           /* while the scope is not equal trace the first bucket of hash table which contains all id's */
        if(p){
            while(p){
                if(strcmp(p->name,cname)==0 && p->isActive==1){                                                         /* if the given name is equal to an entry name it means that you have found globally this symbol */
                        printf("OK,%s found and refers to: #%d,\"%s\"(%d) \n",cname,p->line,p->name,p->scope);
                        return 1;                                                                                       /* return that you found it and print in which symbol refers */
                }
                p=p->next;
            }
        }
        scope--;
    }


    return 0;

}




/* Look up in inner scope*/
int lookUp_inscope(unsigned int nscope,char *cname){
    
    /* declarations */
    symtable *lp;
    int i;

    /* assignments */
    i=0;

    /* code */
    while(i<SIZE){                              /* trace the whole hash table */
        lp=Header->entries[i];
        if(lp){
            while(lp){
                if(lp->scope==nscope && lp->isActive==1){
                    if(strcmp(lp->name,cname)==0 && lp->type!=4){
                        //ok found
                        printf("OK, %s found and refers to: #%d,\"%s\"(%d) \n",cname,lp->line,lp->name,lp->scope);
                        return 1;
                    }
                }
                lp=lp->next;
            }
        }
        i++;
    }

    
    
    /* else if not found return 0*/
    return 0;
}



/* Look up for functin*/
int lookUp_inFunction(unsigned int lscope,char *lname){

    symtable *lf;
    int i;

    i=0;

    while(i<SIZE){
        lf=Header->entries[i];
        if(lf){
            while(lf){
                if(lf->scope==lscope && lf->isActive==1){
                    if(strcmp(lf->name,lname)==0){
                        return 1; // function's name found 
                    }
                }
                lf=lf->next;
            }
        }
        i++;
    }
    return 0;
}



/* Look up for namespace */
int lookUp_namespace(unsigned int nscope,char *sname){

    /* declarations */
    symtable *np;
    int i,sz;

    /* assignments */
    sz=0;
    i=0;

    /* code */
   while(sz<SIZE){
    np=Header->entries[i];
        if(np){
            while(np){
                    if(strcmp(np->name,sname)==0 && np->scope==nscope){
                        //ok found
                        printf("OK, %s found and refers to: #%d,\"%s\"(%d) \n",sname,np->line,np->name,np->scope);
                        return 1;
                    }
                
                np=np->next;
            }
        }
    sz++;
   }

    return 0;
}




/* check collisions */
int check_collisions(char *cname){

    /* declarations */
    symtable *cc;

    /* assignments */
    cc=Header->entries[1];

    /* code */
    while(cc){
        if(strcmp(cc->name,cname)==0 && cc->isActive==1){
            if(cc->type==4){
                return 1;
            }
        }
        cc=cc->next;
    }


    return 0;  
}




/* Insert a new entry */
void insert_HT(symtable *s,int index){
                                            
    if(Header->entries[index]==NULL){                        /* if pointer points to NULL add it there */
        Header->entries[index]=malloc(sizeof(symtable));     /* allocate memory for a symtable entry */
        Header->entries[index]=s;                            /* set the specific pointer (bucket of hash table) to point to the new entry */
        Header->entries[index]->next=NULL;
       
    }
    else{                                                    /* else if not NULL */
        s->next=Header->entries[index];                      /* add the new entry at the begginng of the hash table list */
        Header->entries[index]=s;
    }    
}




/* Insert an elemnt to the list */
void insert_List(symtable *hdr,symtable *elm,unsigned int nscope){

    /* declarations */
    symtable *trace;
    symtable *pos,*posn;
    symtable *headerlist;

    /* assignments */
    trace=NULL;                                                     /* pointer to trace the nexts */
    pos=NULL;                                                       /* pointer to store the address of the vertical list to be traced */
    posn=NULL;                                                      /* pointer store the address that global list pointer stores */
    headerlist=hdr;

    /* code */
    if(scopeList==NULL){                                           /* if the global list pointer points NULL */
        scopeList=malloc(sizeof(symtable));                        /* allocate memory */
        scopeList=elm;                                             /* store the address of element of the clone of global list pointer */     
        scopeList->next=NULL;                                      /* set its next to null */
        elm->nextScope=NULL;                                        /* set nextScope of elment to NULL */
        scopeList=elm;                                       /* set the global list pointer to point to the new element */
    }else{                                                          /* else if glp already points somewhere */
        trace=scopeList;                                           /* set trace pointer to point at the same address */
        
        while(trace!=NULL){                                         /* trace the list horizontally */
            if(trace->scope==nscope){                               /* if the given scope has been found */
                pos=trace;                                          /* add a tmp pointer to store this address that the given scope found */
                break;                                              /* break the loop */
            }
            trace=trace->next; 
        }
    }


    if(pos!=NULL && pos->next==NULL){                               /* if the while loop ends and given scope has not been found */
        elm->next=scopeList;                                       /* add the new element at the beggining of the list */
        scopeList=elm;                                             /* add the clone header to point at the new element */
                                             /* add the global list pointer to store the new element */
    }

    else{                                                           /* else if the given scope has been found start tracing the list vertically for the specific scopes*/
        while(pos!=NULL){                                           /* at the end of this while the pointer pos will store the address that the new element has to be stored */
        posn=pos;
        pos=pos->nextScope;
    }

        if(posn!=NULL){
            posn->nextScope=elm;                                       /* store at this address the new element */
        }

    }


    //free(treace);

}




/* Set attributes of inserting element */
void set_Entry(unsigned int nscope,unsigned int nline,char *nname,symtype ntype,int toHash){

    /* declarations */
    int index;
    symtable *se;                                           /* pointer se(set_entry) which stores the new element*/
    
    /* memory alloc */
    se=malloc(sizeof(symtable));

    /* assignments */                                       /* set attributes */
    se->scope=nscope;
    se->line=nline;
    se->name=nname;
    se->type=ntype;
    se->isActive=1;
    se->next=NULL;
    se->nextScope=NULL;

    /* code */                                              /* if we are here we have to insert the element in bot list and table */
   // insert_List(scopeList,se,nscope);                       /* insert the element at the list */

    index=hash_Function(toHash);                            /* call the hash_Function to access a position */
    insert_HT(se,index);                                   /* insert the new elemente into the hash table */


}




/* insert default libraries at scope 0*/
void default_Libs(){
    
    /* code */
    set_Entry(0,0,"print",LIBFUNC,11);
    set_Entry(0,0,"input",LIBFUNC,11);
    set_Entry(0,0,"objectmemberkeys",LIBFUNC,11);
    set_Entry(0,0,"objjecttotalmembers",LIBFUNC,11);
    set_Entry(0,0,"objectcopy",LIBFUNC,11);
    set_Entry(0,0,"totalarguments",LIBFUNC,11);
    set_Entry(0,0,"argument",LIBFUNC,11);
    set_Entry(0,0,"typeof",LIBFUNC,11);
    set_Entry(0,0,"strtonum",LIBFUNC,11);
    set_Entry(0,0,"sqrt",LIBFUNC,11);
    set_Entry(0,0,"cos",LIBFUNC,11);
    set_Entry(0,0,"sin",LIBFUNC,11);
}




/* print the elements of symbol table */
void print_HT(hashtable *pr,int nscope){

    symtable *p;
    int min_scope,max_scope;
    int i;
    int bucket;


    min_scope=0;
    max_scope=nscope;
    bucket=0;
    
    while(min_scope<=max_scope){
        printf("---------   Scope #%d   ----------\n",bucket);
        i=0;


        while(i<2){
            p=Header->entries[i];
            while(p){
                if(p->scope==bucket){
                    if(p->type==0){
                    printf("\"%s\"  [global variable]    (line %d)   (scope %d)\n",p->name,p->line,p->scope);

                    }if(p->type==1){
                    printf("\"%s\"  [local variable]    (line %d)   (scope %d)\n",p->name,p->line,p->scope);

                    }if(p->type==2){
                    printf("\"%s\"  [formal arguments]    (line %d)   (scope %d)\n",p->name,p->line,p->scope);

                    }if(p->type==3){
                    printf("\"%s\"  [user function]    (line %d)   (scope %d)\n",p->name,p->line,p->scope);

                    }if(p->type==4 ){
                    printf("\"%s\"  [library function]    (line %d)   (scope %d)\n",p->name,p->line,p->scope);
                    }
                }
                
                p=p->next;
            }
            i++;
        }
            
            bucket++;
            
        
        min_scope++;
        
  }

}




/* print the List  */
void print_L(){

    symtable *pr;
    pr=scopeList;

    printf("\n");

    while(pr!=NULL){
        printf("[scope:%d, name:%s] ",pr->scope,pr->name);
    pr=pr->next;
    } 
}




/* hide symbols */
void hide(unsigned int hscope){

    symtable *hd;
    int i=0;

    while(i<SIZE){
        hd=Header->entries[i];
        if(hd){
            while(hd){
                if(hd->scope==hscope){
                    hd->isActive=0;
                }
                hd=hd->next;
            }
        }
        i++;
    }
}
    



/* free list*/
void free_L(symtable *fr){
    
    free(fr);
}



/* free table */
void free_HT(hashtable *fr){

    free(fr);
}

