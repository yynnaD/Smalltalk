//
//  main.cpp
//  smalltalk
//
//  Created by Danny Gletner on 10/28/20.
//  Copyright © 2020 Danny Gletner. All rights reserved.
//

#include<iostream>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
//#include<conio.h>

using namespace std;

/*****************************************************************
 *                     DECLARATIONS                              *
 *****************************************************************/
typedef int NUMBER;
typedef int NAME;
const int  NAMELENG = 25;      /* Maximum length of a name */
const int  MAXNAMES = 500;     /* Maximum number of different names */
const int  MAXINPUT = 40000;     /* Maximum length of an input */
const char*   PROMPT = "-> ";
const char*   PROMPT2 = "> ";
const char  COMMENTCHAR = ';';
const int   TABCODE = 9;        /* in ASCII */

struct EXPLISTREC;
typedef EXPLISTREC* EXPLIST;

enum EXPTYPE {VALEXP,VAREXP,APEXP};

struct STVALUEREC;
typedef STVALUEREC* STVALUE;

struct CLASSREC;
typedef CLASSREC* CLASS;


struct EXPREC
{
    EXPTYPE etype; //what type of expression
    STVALUE valu;
    NAME varble;
    NAME optr;
    EXPLIST args;
};
typedef EXPREC* EXP;

struct EXPLISTREC
{
    EXP head;
    EXPLIST tail;
};


struct VALUELISTREC
{
    STVALUE  head;
    VALUELISTREC*  tail;
};

typedef VALUELISTREC* VALUELIST;

struct NAMELISTREC
{
    NAME   head;
    NAMELISTREC* tail;
};
typedef NAMELISTREC* NAMELIST;


struct  ENVREC
{
    NAMELIST vars;
    VALUELIST values;
};

typedef ENVREC* ENV;

enum STVALUETYPE  {INT,SYM,USER};

struct  STVALUEREC
{
    CLASS owner;
    STVALUETYPE vtype;
    NUMBER intval;  //if its INT
    NAME symval; //if its SYM
    ENV userval; //if its USER
};


struct  FUNDEFREC
{
    NAME  funname;
    NAMELIST  formals;
    EXP  body;
    FUNDEFREC*  nextfundef;
};
typedef FUNDEFREC* FUNDEF;

struct   CLASSREC
{
    NAME clname;
    CLASS clsuper;
    NAMELIST clrep;
    FUNDEF exported;
    CLASS nextclass;
};

FUNDEF  fundefs;         //pointer to global functions
ENV     globalEnv;       //pointer to global enviornment
EXP     currentExp;      //pointer to the expression to be eval
char userinput[MAXINPUT];//array to store the user input
int  inputleng, pos;
char*  printNames[MAXNAMES]; //symbol table
int   numNames, numBuiltins,numCtrlOps;
NAME  SELF;   //location of self in the symbol table
CLASS OBJECTCLASS; //pointer to the abstract object class
CLASS classes;     //pointer to all the classes
STVALUE objectInst;//pointer to the instance of object class
STVALUE trueValue, falseValue;
int   quittingtime;

/*****************************************************************
 *                     DATA STRUCTURE OP'S                       *
 *****************************************************************/

/* mkVALEXP - return an EXP of type VALEXP with num n            */

EXP mkVALEXP ( STVALUE v)
{
    EXP e;
    e = new EXPREC;
    e->etype = VALEXP;
    e->valu = v;
    return e;
    // do this
    
}/* mkVALEXP */

/* mkVAREXP - return and EXP of type VAREXP with NAME nm */

EXP mkVAREXP(NAME nm)
{
    EXP e;
    e = new EXPREC;
    e->etype = VAREXP;
    e->varble = nm;
    return e;
}/* mkVAREXP */

/* mkAPEXP - return EXP of type APEXP w/ optr op and args el     */

EXP mkAPEXP (NAME op, EXPLIST el)
{
    EXP e;
    e = new EXPREC;
    e->etype = APEXP;
    e->optr = op;
    e->args = el;
    return e;
    //do this
}/* mkAPEXP */



/* mkINT - return an STVALUE with integer value n    */

STVALUE mkINT (int n )
{
    STVALUE newval;
    newval = new STVALUEREC;
    newval->owner = OBJECTCLASS;
    newval->vtype = INT;
    newval->intval = n;
    return (newval);
} /* mkINT */

/* mkSYM - return an STVALUE with symbol value s   */

STVALUE mkSYM ( NAME s)
{
    STVALUE newval;
    newval = new STVALUEREC;
    newval->owner = OBJECTCLASS;
    newval->vtype = SYM;
    newval->symval = s;
    return newval;
} /* mkSYM */

/* mkUSER - return a USER-type STVALUE    */

STVALUE mkUSER (ENV rho, CLASS ownr)
{
    STVALUE newval;
    newval = new STVALUEREC;
    newval->vtype = USER;
    newval->userval = rho;
    newval->owner = ownr;
    return newval;
} /* mkUSER */


/* mkExplist - return an EXPLIST with head e and tail el         */

EXPLIST mkExplist (EXP e, EXPLIST el)
{
    EXPLIST newel;
    newel = new EXPLISTREC;
    newel->head = e;
    newel->tail = el;
    return newel;
    //do this
}/* mkExplist */

/* mkNamelist - return a NAMELIST with head n and tail nl        */

NAMELIST mkNamelist ( NAME nm, NAMELIST nl)
{
    NAMELIST newnl;
    newnl = new NAMELISTREC;
    newnl->head = nm;
    newnl->tail = nl;
    return newnl;
}/* mkNamelist */

/* mkValuelist - return an VALUELIST with head n and tail vl     */

VALUELIST mkValuelist (STVALUE n,  VALUELIST vl)
{
    VALUELIST newvl;
    newvl = new VALUELISTREC;
    newvl->head = n;
    newvl->tail = vl;
    return newvl;
}/* mkValuelist */

/* mkEnv - return an ENV with vars nl and values vl              */

ENV mkEnv ( NAMELIST nl, VALUELIST vl)
{
    ENV rho;
    rho = new ENVREC;
    rho->vars = nl;
    rho->values = vl;
    return rho;
}/* mkEnv */

/* lengthVL - return length of VALUELIST vl      */

int lengthVL ( VALUELIST vl)
{
    int i = 0;
    while (vl != 0)
    {
        i++;
        vl = vl->tail;
    }
    return i;
}/* lengthVL */

/* lengthNL - return length of NAMELIST nl    */

int lengthNL ( NAMELIST nl)
{
    int i = 0;
    while( nl !=0 )
    {
        ++i;
        nl = nl->tail;
    }
    return i;
}/* lengthNL */


/*****************************************************************
 *                     NAME MANAGEMENT                           *
 *****************************************************************/

// fetchFun - get function definition of NAME fname from fenv

FUNDEF fetchFun (NAME fname, FUNDEF fenv)
{
    FUNDEF f = fenv;
    while (f != 0) {
        if (f->funname == fname)
            return f;
        f = f->nextfundef;
    }
    return 0;
} // fetchFun

/* fetchClass - get class definition of NAME cname   */

CLASS fetchClass (NAME cname)
{
    CLASS c = classes;
    while(c != 0){
        if(c->clname == cname)
            return c;
        c = c->nextclass;
    }
    return 0;
}/* fetchClass */

/* newClass - add new class cname to classes      */

CLASS newClass (NAME cname, CLASS super)
{
    CLASS cl = new CLASSREC;
    cl->clname = cname;
    cl->clsuper = super;
    cl->nextclass = classes;
    classes = cl;
    return cl;
}//* newClass

//  newFunDef - add new function fname to fenv

FUNDEF newFunDef (NAME fname, FUNDEF& fenv)
{
    FUNDEF f = new FUNDEFREC;
    f->funname = fname;
    f->nextfundef = fenv;
    fenv = f;
    return f;
} // newFunDef

/* initNames - place all pre-defined names into printNames */

void initNames() //symtable
{
    int i = 0;
    fundefs = 0;
    printNames[i] = "if";    i++;   //0
    printNames[i] = "while"; i++;   //1
    printNames[i] = "set";   i++;   //2
    printNames[i] = "begin"; i++;   //3
    numCtrlOps    = i;
    printNames[i] = "new";i++;      //4
    printNames[i] = "+"; i++;       //5
    printNames[i] = "-"; i++;       //6
    printNames[i] = "*"; i++;       //7
    printNames[i] = "/"; i++;       //8
    printNames[i] = "="; i++;       //9
    printNames[i] = "<"; i++;       //10
    printNames[i] = ">"; i++;       //11
    printNames[i] = "print";i++;    //12
    printNames[i] = "self";         //13
    SELF = i;
    numNames = i;
    numBuiltins = i-1;
}//initNames

/* install - insert new name into printNames  */

NAME install ( char* nm)
{
    int i = 1;
    while (i <= numNames)
        if (strcmp( nm,printNames[i] ) == 0)
            break;
        else
            i++;
    if (i > numNames)
    {
        numNames = i;
        printNames[i] = new char[strlen(nm) + 1];
        strcpy(printNames[i], nm);
    }
    return i;
}// install

/* prName - print name nm              */

void prName ( NAME nm)
{
    cout << printNames[nm];
} //prName

/*****************************************************************
 *                        INPUT                                  *
 *****************************************************************/

/* isDelim - check if c is a delimiter   */

int isDelim (char c)
{
    return ( ( c == '(') || ( c == ')') ||( c == ' ')||( c== COMMENTCHAR) );
}

/* skipblanks - return next non-blank position in userinput */

int skipblanks (int p)
{
    while (userinput[p] == ' ')
        ++p;
    return p;
}


/* matches - check if string nm matches userinput[s .. s+leng]   */

int matches (int s, int leng,  char* nm)
{
    int i=0;
    while (i < leng )
    {
        if( userinput[s] != nm[i] )
            return 0;
        ++i;
        ++s;
    }
    if (!isDelim(userinput[s]) )
        return 0;
    return 1;
}/* matches */



/* nextchar - read next char - filter tabs and comments */

void nextchar (char& c)
{
    scanf("%c", &c);
    if (c == COMMENTCHAR )
    {
        while ( c != '\n' )
            scanf("%c",&c);
    }
}

/* readParens - read char's, ignoring newlines, to matching ')' */
void readParens()
{
    int parencnt; /* current depth of parentheses */
    char c;
    parencnt = 1; // '(' just read
    do
    {
        if (c == '\n')
            cout <<PROMPT2;
        nextchar(c);
        pos++;
        if (pos == MAXINPUT )
        {
            cout <<"User input too long\n";
            exit(1);
        }
        if (c == '\n' )
            userinput[pos] = ' ';
        else
            userinput[pos] = c;
        if (c == '(')
            ++parencnt;
        if (c == ')')
            parencnt--;
    }
    while (parencnt != 0 );
} //readParens

/* readInput - read char's into userinput */

void readInput()
{
    char  c;
    cout << PROMPT;
    pos = 0;
    do
    {
        ++pos ;
        if (pos == MAXINPUT )
        {
            cout << "User input too long\n";
            exit(1);
        }
        nextchar(c);
        if (c == '\n' )
            userinput[pos] = ' ';
        else
            userinput[pos] = c;
        if (userinput[pos] == '(' )
            readParens();
    }
    while (c != '\n');
    inputleng = pos;
    userinput[pos+1] = COMMENTCHAR; // sentinel
}


/* reader - read char's into userinput; be sure input not blank  */

void reader ()
{
    do
    {
        readInput();
        pos = skipblanks(1);
    }
    while( pos > inputleng); // ignore blank lines
}

/* parseName - return (installed) NAME starting at userinput[pos]*/

NAME parseName()
{
    char nm[25]; // array to accumulate characters
    int leng; // length of name
    leng = 0;
    while ( (pos <= inputleng) && !isDelim(userinput[pos]) )
    {
        ++leng;
        nm[leng-1] = userinput[pos];
        ++pos;
    }
    if (leng == 0)
    {
        cout<<"Error: expected name, instead read: "<< userinput[pos]<<endl;
        exit(1);
    }
    nm[leng] = '\0';
    pos = skipblanks(pos); // skip blanks after name
    return ( install(nm) );
}// parseName

/* isDigits - check if sequence of digits begins at pos   */

int isDigits (int pos)
{
    if ( ( userinput[pos] < '0' ) ||  ( userinput[pos] > '9' ) )
        return 0;
    while ( ( userinput[pos] >='0') && ( userinput[pos] <= '9') )
        ++pos;
    if (!isDelim(userinput[pos] ))
        return 0;
    return 1;
}// isDigits


/* isNumber - check if a number begins at pos  */

int isNumber (int pos)
{
    return ( isDigits(pos) || ((userinput[pos] == '-') &&
                               isDigits(pos+1)));
}// isNumber


/* parseInt - return number starting at userinput[pos]   */

int parseInt()
{
    int n, sign;
    n = 0; sign = 1;
    if ( userinput[pos] == '-' )
    {
        sign = -1;
        pos++;
    }
    while ( (userinput[pos] >= '0') && (userinput[pos] <= '9' ) )
    {
        n = 10*n + userinput[pos] - '0';
        pos++;
    }
    pos = skipblanks(pos); // skip blanks after number
    return (n*sign);
}// parseInt


int isValue (int pos)
{
    return ((userinput[pos] == '#') || isNumber(pos));
}// isValue

/* parseVal - return number starting at userinput[pos]   */

STVALUE parseVal()
{
    if (userinput[pos] == '#')
    {
        pos++;
        NAME nm = parseName();
        return mkSYM(nm);
        
    }
    return mkINT(parseInt());
}

EXPLIST parseEL();

/* parseExp - return EXP starting at userinput[pos]  */

EXP parseExp()
{
    NAME nm;
    EXPLIST el;
    pos = skipblanks(pos);
    if ( userinput[pos] == '(' )
    {// APEXP
        pos = skipblanks(pos+1); // skip '( ..'
        nm = parseName();
        el = parseEL();
        return ( mkAPEXP(nm, el));
    }
    else if (isValue(pos))
        return ( mkVALEXP(parseVal() ));  // VALEXP
    else
        return ( mkVAREXP(parseName() ) ); // VAREXP
}// parseExp

/* parseEL - return EXPLIST starting at userinput[pos]  */

EXPLIST parseEL()
{
    EXP e;
    EXPLIST el;
    pos = skipblanks(pos);
    if ( userinput[pos] == ')')
    {
        pos = skipblanks(pos+1); // skip ') ..'
        return 0;
    }
    e = parseExp();
    el = parseEL();
    return ( mkExplist(e, el));
}// parseEL


/* parseNL - return NAMELIST starting at userinput[pos]  */

NAMELIST parseNL()
{
    NAME nm;
    NAMELIST nl;
    if ( userinput[pos] == ')' )
    {
        pos = skipblanks(pos+1); // skip ') ..'
        return 0;
    }
    nm = parseName();
    nl = parseNL();
    return ( mkNamelist(nm, nl));
}// parseNL

/* parseDef - parse function definition at userinput[pos]   */

// parseDef - parse function definition at userinput[pos]

NAME parseDef (FUNDEF& fenv)
{
    NAME fname;        // function name
    FUNDEF newfun;     // new FUNDEFREC
    pos = skipblanks(pos + 1); // skip  (
    pos = skipblanks(pos + 6); //skip define
    fname = parseName();
    pos = skipblanks(pos + 1); // skip  (
    NAMELIST formals = parseNL();
    EXP e = parseExp();
    pos = skipblanks(pos);
    pos = skipblanks(pos + 1); //skip over the last )
    newfun = newFunDef(fname, fenv);
    newfun->formals = formals;
    newfun->body = e;
    fenv = newfun;
    return fname;
    
    
} // parseDef


// parseClass - parse class definition at userinput[pos]

NAME parseClass()
{
    NAME cname, sname, fname;
    CLASS thisclass, superclass;
    NAMELIST rep;;
    FUNDEF cenv;
    pos = skipblanks(pos);
    pos = skipblanks(pos+1); // skip '( ..'
    pos = skipblanks(pos);
    pos = skipblanks(pos+5); // skip 'class ...'
    cname = parseName();
    sname = parseName();
    superclass = fetchClass(sname);
    
    thisclass = newClass(cname,superclass);
    pos = skipblanks(pos);
    pos = skipblanks(pos+1); // skip '( ...'
    rep = parseNL(); // class variable names
    
    //Now  link all the inhereited variables
    if (rep == 0)
        thisclass->clrep = superclass->clrep;
    else
    {
        thisclass->clrep = rep;
        while (rep ->tail != 0 )
            rep = rep->tail;
        rep->tail = superclass->clrep;
    }
    pos = skipblanks(pos);
    
    //now parse all the methods
    cenv = 0;
    while (userinput[pos]=='(')
    {
        pos = skipblanks(pos);
        fname = parseDef(cenv);
        prName(fname);
        cout<<endl;
    }
    thisclass->exported = cenv;
    
    pos = skipblanks(pos);
    pos = skipblanks(pos+1); // skip ' ..)'
    return cname;
}// parseClass


/*****************************************************************
 *                     ENVIRONMENTS                              *
 *****************************************************************/

/* emptyEnv - return an environment with no bindings */

ENV emptyEnv()
{
    return  mkEnv(0, 0);
}


// bindVar - bind variable nm to value n in environment rho

void bindVar ( NAME nm, STVALUE v, ENV rho)
{
    rho->vars = mkNamelist(nm, rho->vars);
    rho->values = mkValuelist(v, rho->values);
}// bindVar

/* findVar - look up nm in rho   */

VALUELIST findVar ( NAME nm, ENV rho)
{
    NAMELIST  nl;
    VALUELIST  vl;
    nl = rho->vars;
    vl = rho->values;
    while  (nl != 0)
        if ( nl->head == nm )
            break;
        else{
            nl = nl->tail;
            vl = vl->tail;
        };
    return vl;
}


/* assign - assign value n to variable nm in rho   */

void  assign (NAME nm,  STVALUE v, ENV rho)
{
    VALUELIST varloc;
    varloc = findVar(nm, rho);
    varloc->head = v;
}// assign

/* fetch - return number bound to nm in rho */

STVALUE fetch ( NAME nm, ENV rho)
{
    VALUELIST  vl;
    vl = findVar(nm, rho);
    return (vl->head);
}

/* isBound - check if nm is bound in rho  */

int isBound ( NAME nm, ENV rho)
{
    return ( findVar(nm, rho) != 0 );
}


/*****************************************************************
 *                     VALUES                                    *
 *****************************************************************/

// prValue - print value v

void prValue ( STVALUE v)
{
    if (v->vtype == INT )
        cout <<(v->intval);
    else if( v->vtype == SYM )
        prName(v->symval);
    else
        cout << "<userval>";
} // prValue


// isTrueVal - return true if v is true (non-zero) value

int isTrueVal ( STVALUE v)
{
    if ((v->vtype == USER) || (v->vtype == SYM))
        return 1;
    return (v->intval != 0 );
} //isTrueVal


/* arity - return number of arguments expected by op  */

int arity ( int op)
{
    if ( ( op > 4) && (op < 12) )
        return 2;
    return  1;
}// arity

/* applyValueOp - apply VALUEOP op to arguments in VALUELIST vl */

STVALUE applyValueOp ( int op,  VALUELIST vl) //todo / done?
{
    STVALUE s1 = vl->head;
    STVALUE s2;
    if(vl->tail != NULL)
        s2 = vl->tail->head;
    
    if(op == 9){ // =
        if(s1->vtype == s2->vtype){
            if(s1->vtype == INT && s1->intval == s2->intval)
                return trueValue;
            if(s1->vtype == SYM && s1->symval == s2->symval)
                return trueValue;
        }
        return falseValue;
    }
    if(op == 12) //print
    {
        prValue(s1);
        return s1;
    }
    if (op == 5)  // +
        return mkINT(s1->intval + s2->intval);
    if(op == 6) // -
        return mkINT(s1->intval - s2->intval);
    if(op == 7) // *
        return mkINT(s1->intval * s2->intval);
    if(op == 8) // /
        return mkINT(s1->intval / s2->intval);
    if(op == 10) // <
    {
        if(s1->intval < s2->intval)
            return trueValue;
        else return falseValue;
    }
    if(op == 11) // >
    {
        if(s1->intval > s2->intval)
            return trueValue;
        else return falseValue;
    }
    
    exit(1);
} // applyValueOp



/*****************************************************************
 *                     EVALUATION                                *
 *****************************************************************/

/* evalList - evaluate each expression in el */
STVALUE eval ( EXP e,  ENV rho, STVALUE rcvr);

VALUELIST evalList ( EXPLIST el, ENV rho, STVALUE rcvr)//todo / done?
{
    if(el == 0) return 0;
    STVALUE v = eval(el->head, rho, rcvr);
    VALUELIST vl = evalList(el->tail, rho, rcvr);
    return mkValuelist(v, vl);
        
}// evalList

// applyGlobalFun - apply function defined at top level
STVALUE applyGlobalFun (NAME optr, VALUELIST actuals, STVALUE rcvr) //todo, done?
{
    FUNDEF f = fetchFun(optr, fundefs);
    ENV rho = mkEnv(f->formals, actuals);
    return eval(f->body, rho, rcvr);
}// applyGlobalFun


// methodSearch - find class of optr, if any, starting at cl
FUNDEF methodSearch (NAME optr,  CLASS cl)
{
    FUNDEF f = 0;
    while(cl != 0)
    {
        f = fetchFun(optr, cl->exported);
        if(f != 0)
            return f;
        cl = cl->clsuper;
    }
    return 0;
}// methodSearch


// applyMethod - apply method f to actuals
STVALUE applyMethod (FUNDEF f, VALUELIST actuals)
{
    ENV rho = mkEnv(f->formals, actuals->tail);
    return eval(f->body, rho, actuals->head);
}// applyMethod

// mkRepFor - make list of all zeros of same length as nl

VALUELIST mkRepFor ( NAMELIST nl)
{
    if (nl == 0)
        return 0;
    return mkValuelist(falseValue, mkRepFor(nl->tail));
}// mkRepFor


// applyCtrlOp - apply CONTROLOP op to args in rho

STVALUE applyCtrlOp (int op, EXPLIST args, ENV rho, STVALUE rcvr) //todo / done?
{
    STVALUE v;
    NAME nm;
    if(op == 0) //if (e1 e2 e3)
    {
        if(isTrueVal(eval(args->head, rho, rcvr)))
            return eval(args->tail->head, rho, rcvr);
        return eval(args->tail->tail->head, rho, rcvr);
    }
    if(op == 1) //while
    {
        v = eval(args->head, rho, rcvr);
        while (isTrueVal(v)) {
            v = eval(args->tail->head, rho, rcvr);
            v = eval(args->head, rho, rcvr);
        }
        return v;
    }
    if(op == 2) //set
    {
        v = eval(args->tail->head, rho, rcvr);
        nm = args->head->varble;
        if(isBound(nm, rho))
            assign(nm, v, rho);
        else if(isBound(nm, rcvr->userval))
            assign(nm, v, rcvr->userval);
        else if(isBound(nm, globalEnv))
            assign(nm, v, globalEnv);
        else
            bindVar(nm, v, globalEnv); //add it to globalEnv
    }
    if(op == 3) //begin
    {
        while (args != NULL) {
            v = eval(args->head, rho, rcvr);
            args = args->tail;
        }
        return v;
    }
    if(op == 4) //new
    {
        CLASS cl = fetchClass(args->head->varble);
        VALUELIST vl = mkRepFor(cl->clrep);
        ENV rho = mkEnv(cl->clrep, vl);
        STVALUE newVal = mkUSER(rho, cl);
        assign(SELF, newVal, newVal->userval);
        return newVal;
    }
    exit(1);
}// applyCtrlOp

// eval - return value of e in environment rho, receiver rcvr

STVALUE eval (EXP e, ENV rho, STVALUE rcvr) //todo / done?
{
    VALUELIST vl;
    FUNDEF f;
    switch( e->etype )
    {
        case VALEXP : return(e->valu);
            
        case VAREXP : //you do this
            if(isBound(e->varble, rho))
                return fetch(e->varble, rho);
            if(isBound(e->varble, rcvr->userval))
               return fetch(e->varble, rcvr->userval);
            if(isBound(e->varble, globalEnv))
                return fetch(e->varble, globalEnv);
            break;
            
        case APEXP :
            if(e->optr <= numCtrlOps)
                return applyCtrlOp(e->optr, e->args, rho, rcvr);
            
            vl = evalList(e->args, rho, rcvr);
            if(vl == 0)
                return applyGlobalFun(e->optr, vl, rcvr);
            
            FUNDEF f = methodSearch(e->optr, vl->head->owner);
            if(f != 0)
                return applyMethod(f, vl);
            
            if(e->optr <= numBuiltins)
                return applyValueOp(e->optr, vl);
            return applyGlobalFun(e->optr, vl, rcvr);
            //you do this
            break;
    }//switch
    return 0;
} // eval


/*****************************************************************
 *                     READ-EVAL-PRINT LOOP                      *
 *****************************************************************/

// initHierarchy - allocate class Object and create an instance

void initHierarchy()
{
    classes = 0;
    OBJECTCLASS = newClass(install("Object"), 0);
    OBJECTCLASS->exported = 0;
    OBJECTCLASS->clrep = mkNamelist(SELF, 0);
    objectInst = mkUSER(mkEnv(OBJECTCLASS->clrep, mkValuelist(mkINT(0), 0)),
                        OBJECTCLASS);
}// initHierarchy

int main()
{
    initNames();
    initHierarchy();
    globalEnv = emptyEnv();
    
    trueValue = mkINT(1);
    falseValue = mkINT(0);
    
    quittingtime = 0;
    
    while (!quittingtime)
    {
        reader();
        if (matches(pos, 4, "quit"))
            quittingtime =     1;
        else if ((userinput[pos] == '(') &&
                 (matches(skipblanks(pos+1), 6,"define")) )
        {
            prName(parseDef(fundefs));
            cout <<endl;
        }
        else if ( (userinput[pos]=='(') &&
                 ( matches(skipblanks(pos+1),5,"class")))
        {
            prName(parseClass());
            cout << endl;
        }
        else
        {
            
            currentExp = parseExp();
            prValue(eval(currentExp, emptyEnv(), objectInst));
            
            cout <<endl;
        }
    }// while
    return 0;
} // smalltalk
