#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NUM_ERRORS 28
#define MAX_NAME_LENGTH 11
#define MAX_SYMBOL_TABLE_SIZE 100
#define NUM_TOKENS 33

char* tokenCode[NUM_TOKENS+1] = {"error", "nulsym", "identsym", "numbersym", "plussym", "minussym", "multsym", " slashsym", "oddsym", " eqlsym", "neqsym", "lessym", "leqsym",
                                 "gtrsym", "geqsym", "lparentsym", "rparentsym", "commasym", "semicolonsym", "periodsym", "becomessym", "beginsym", "endsym", "ifsym", "thensym", "whilesym", "dosym", "callsym",
                                 "constsym", "varsym", "procsym", "writesym", "readsym", "elsesym"
                                };

typedef struct symbol
{
    int kind; 		// const = 1, var = 2, proc = 3
    char name[12];	// name up to 11 chars
    int val; 		// number (ASCII value)
    int level; 	// L level
    int addr; 		// M address
} symbol;

enum opCodes {LIT = 1, OPR = 2, LOD = 3, STO = 4, CAL = 5, INC = 6, JMP = 7, JPC = 8, SIOout = 9, SIOin = 10, SIOhalt = 11};
enum oprCodes {RET = 0, NEG = 1, ADD = 2, SUB = 3, MUL = 4, DIV = 5, ODD = 6, MOD = 7, EQL = 8, NEQ = 9, LSS = 10, LEQ = 11, GTR = 12, GEQ = 13};

symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];

int symTableCounter = 0;
int token;
char tokenVar[MAX_NAME_LENGTH];
instruction codeWrite[1000];
int codeCounter = 0;
int addrBase = 4;
int addrLocal;
int procLevel = 0;
int curEnvironment = -1;

char *errors[NUM_ERRORS] =
{
    "Use = instead of :=.",
    "= must be followed by a number.",
    "Identifier must be followed by =.",
    "const, var, procedure must be followed by identifier.",
    "Semicolon or comma missing.",
    "Incorrect symbol after procedure declaration.",
    "Statement expected.",
    "Incorrect symbol after statement part in block.",
    "Period expected.",
    "Semicolon between statements missing.",
    "Undeclared identifier.",
    "Assignment to constant or procedure is not allowed.",
    "Assignment operator expected.",
    "call must be followed by an identifier.",
    "Call of a constant or variable is meaningless.",
    "then expected.",
    "Semicolon or } expected.",
    "do expected.",
    "Incorrect symbol following statement.",
    "Relational operator expected.",
    "Expression must not contain a procedure identifier.",
    "Right parenthesis missing.",
    "The preceding factor cannot begin with this symbol.",
    "An expression cannot begin with this symbol.",
    "This number is too large.",
    "Read must be followed by an identifier.",
    "Write must be followed by an identifier.",
    "Maximum lexical level exceeded."
};

void factor(FILE *ifp);
void term(FILE *ifp);
void expression(FILE *ifp);

void emit(int op, int l, int m)
{
    codeWrite[codeCounter].op = op; 	//opcode
    codeWrite[codeCounter].l = l;	    // lexicographical level
    codeWrite[codeCounter].m = m;	    // offset

    codeCounter++;
}

//Gets the next token
void getToken(FILE* ifp)
{
    token = -1;
    if (!feof(ifp) && fscanf(ifp, "%d", &token) == 1)
    {
        //Gets rid of the extra space after the number/token
        fgetc(ifp);
    }
}

//Gets the name of the variable
void getVar(FILE* ifp)
{
    memset(tokenVar, '\0', sizeof(tokenVar));
    if (!feof(ifp))
    {
        while(!feof(ifp) && !isspace(tokenVar[strlen(tokenVar)] = fgetc(ifp)));
        tokenVar[strlen(tokenVar)-1] = '\0';
    }

}

//Gets the name of the constant
void getConst(FILE* ifp)
{
    memset(tokenVar, '\0', sizeof(tokenVar));
    if (!feof(ifp))
    {
        while(!feof(ifp) && !isspace(tokenVar[strlen(tokenVar)] = fgetc(ifp)));
        tokenVar[strlen(tokenVar)-1] = '\0';
    }

}

//Prints out the error message, and exits the program
void printError(int errorCode)
{
    printf("%s", errors[errorCode]);
    exit(errorCode);
}

//Places values into the symbol table
void setTable(int kind, char name[], int val)
{
    symbol_table[symTableCounter].kind = kind;
    strcpy(symbol_table[symTableCounter].name, name);
    symbol_table[symTableCounter].level = procLevel;

    switch (kind)
    {
    case 1:
        symbol_table[symTableCounter].val = val;
        break;
    case 2:
        symbol_table[symTableCounter].addr = addrLocal+addrBase;
        addrLocal++;
        break;
    case 3:
        symbol_table[symTableCounter].val = 0;
        symbol_table[symTableCounter].addr = codeCounter;
        break;
    default:
        break;
    }
    symTableCounter++;
}

//Checks the symbol table to see if a variable or constant exists
symbol checkSymTable(int codeVar, FILE* ifp)
{
    int i;
    //get the name of the variable
    getVar(ifp);

    if (codeVar == -1)
    {
        for (i = symTableCounter-1; i >= 0; i--)
        {
            if (strcmp(tokenVar, symbol_table[i].name) == 0 && (symbol_table[i].kind == 1 || symbol_table[i].kind == 2))
            {

                return symbol_table[i];
            }
        }
    }

    else
    {
        for (i = symTableCounter-1; i >= 0; i--)
        {
            if (strcmp(tokenVar, symbol_table[i].name) == 0 && codeVar == symbol_table[i].kind)
            {
                return symbol_table[i];
            }
            else if (strcmp(tokenVar, symbol_table[i].name) == 0 && codeVar != symbol_table[i].kind)
                printError(11);
        }
    }
    //After traversing the symbol table, it's determined that the symbol does not exist
    printError(10);

}

int relation(int tokenIn)
{
    switch(tokenIn)
    {
    case eqlsym:
        return EQL;
    case neqsym:
        return NEQ;
    case lessym:
        return LSS;
    case leqsym:
        return LEQ;
    case gtrsym:
        return GTR;
    case geqsym:
        return GEQ;
    default:
        printError(19);
    }
}

void factor(FILE* ifp)
{
    if (token == identsym)
    {
        symbol tempSym = checkSymTable(-1, ifp);
        switch(tempSym.kind) {
            case 1: emit(LIT, 0, tempSym.val);
            break;
            case 2: emit(LOD, procLevel - tempSym.level, tempSym.addr);
            break;
            default: printError(20);
            break;

        }
        getToken(ifp);
    }
    else if (token == numbersym)
    {
        //Gets the next number, instead of getNumber()
        getToken(ifp);
        emit(LIT, 0, token);
        getToken(ifp);
    }
    else if (token == lparentsym)
    {
        getToken(ifp);
        expression(ifp);
        if (token != rparentsym)
            printError(21);
        getToken(ifp);
    }
    else
        printError(10);
}

void term(FILE* ifp)
{
    factor(ifp);
    while (token == multsym || token == slashsym)
    {
        int tmpToken = token;
        getToken(ifp);
        factor(ifp);
        switch(tmpToken)
        {
        case multsym:
            emit(OPR, 0, MUL);
            break;
        case slashsym:
            emit(OPR, 0, DIV);
            break;
        default:
            break;
        }
    }
}

void expression(FILE* ifp)
{
    if (token == plussym || token == minussym)
        getToken(ifp);
    term(ifp);
    while (token == plussym || token == minussym)
    {
        int tmpToken = token;
        getToken(ifp);
        term(ifp);
        switch(tmpToken)
        {
        case plussym:
            emit(OPR, 0, ADD);
            break;
        case minussym:
            emit(OPR, 0, SUB);
            break;
        default:
            break;
        }
    }
}

void condition(FILE* ifp)
{
    if (token == oddsym)
    {
        getToken(ifp);
        expression(ifp);
        emit(OPR, 0, ODD);
    }
    else
    {
        expression(ifp);
        int operand = relation(token);
        getToken(ifp);
        expression(ifp);
        emit(OPR, 0, operand);
    }
}

void statement(FILE* ifp)
{
    int pr, qr;
    if (token == identsym)
    {
        //Verify the var exists in the symbol table
        symbol tempSym = checkSymTable(2, ifp);
        getToken(ifp);
        if (token != becomessym)
            printError(2);
        getToken(ifp);
        expression(ifp);
        emit(STO, procLevel-tempSym.level, tempSym.addr);
    }

    else if (token == callsym) {
        getToken(ifp);
        if (token != identsym)
            printError(13);
        symbol tempSymbol = checkSymTable(3, ifp);
        getToken(ifp);
        emit(CAL, procLevel - tempSymbol.level, tempSymbol.addr);
    }
    else if (token == beginsym)
    {
        getToken(ifp);
        statement(ifp);
        while (token == semicolonsym)
        {
            getToken(ifp);
            statement(ifp);
        }
        if (token != endsym)
        {
            printError(9);
        }
        getToken(ifp);
    }
    else if (token == ifsym)
    {
        getToken(ifp);
        condition(ifp);
        if (token != thensym)
            printError(15);
        getToken(ifp);
        pr= codeCounter;
        emit(JPC, 0, 0);
        statement(ifp);
        if (token == elsesym)
        {
            getToken(ifp);
            qr = codeCounter;
            emit(JMP, 0, 0);
            codeWrite[pr].m = codeCounter;
            statement(ifp);
            codeWrite[qr].m = codeCounter;
        }
        else
            codeWrite[pr].m = codeCounter;
    }
    else if (token == whilesym)
    {
        pr = codeCounter;
        getToken(ifp);
        condition(ifp);
        qr = codeCounter;
        emit(JPC, 0, 0);
        if (token != dosym)
            printError(17);
        getToken(ifp);
        statement(ifp);
        emit(JMP, 0, pr);
        codeWrite[qr].m = codeCounter;
    }
    else if (token == readsym)
    {
        getToken(ifp);
        if (token != identsym)
            printError(25);
        symbol tempSym = checkSymTable(2, ifp);
        getToken(ifp);
        emit(SIOin, 0, 2);
        emit(STO, 0, tempSym.addr);

    }
    else if (token == writesym)
    {
        getToken(ifp);
        if (token != identsym && token != numbersym)
            printError(26);
        expression(ifp);
        emit(SIOout, 0, 1);
    }

}


void block(FILE* ifp)
{
    if (token == constsym)
    {
        do
        {
            getToken(ifp);
            if (token != identsym)
                printError(3);
            getVar(ifp);
            getToken(ifp);
            if (token != eqlsym)
                printError(2);
            getToken(ifp);
            if (token != numbersym)
                printError(1);
            getToken(ifp);
            setTable(1, tokenVar, token);
            getToken(ifp);
        }
        while (token == commasym);
        if (token != semicolonsym)
            printError(4);
        getToken(ifp);
    }
    if (token == varsym)
    {
        do
        {
            getToken(ifp);
            if (token != identsym)
                printError(3);
            getVar(ifp);
            setTable(2, tokenVar, 0);
            getToken(ifp);
        }
        while (token == commasym);
        if (token != semicolonsym)
            printError(4);
        getToken(ifp);
    }
    int tmpLocal = addrLocal;
    int tmpbase = addrBase;
    emit(JMP, 0, codeCounter+1);
    int levelTemp = codeCounter-1;
    while (token == procsym) {
        getToken(ifp);
        if (token != identsym)
            printError(3);
        getVar(ifp);
        setTable(3, tokenVar, 0);
        getToken(ifp);
        procLevel++;
        if (procLevel > 3)
            printError(27);
        int prevEnrivonment = curEnvironment;
        curEnvironment = symTableCounter - 1;
        int curCount = symTableCounter;
        addrLocal = 0;
        if (token != semicolonsym)
            printError(4);
        getToken(ifp);
        block(ifp);
        procLevel--;
        symTableCounter = curCount;
        curEnvironment = prevEnrivonment;
        codeWrite[levelTemp].m = codeCounter;
        if (token != semicolonsym)
            printError(4);
        getToken(ifp);
    }
    emit(INC, 0, tmpbase + tmpLocal);
    statement(ifp);
    emit(OPR, 0, RET);
}

void program(FILE* ifp)
{
    getToken(ifp);
    addrLocal = 0;
    block(ifp);
    if (token != periodsym)
    {
        printError(8);
    }
    //emit(SIOhalt, 0, 3);
}

void parser_Cycle(int print)
{

    FILE *ifp = fopen("lexemelist.txt", "r");
    FILE *ofpMain = fopen("compiler_output.txt", "a");
    FILE *ofp = fopen("mcode.txt", "w");

    int doPrint = -1;
    char hold;

    switch (print)
    {
    case 1:
        doPrint = 1;
        break;
    default:
        doPrint = 0;
        break;
    }

    program(ifp);


    int i = 0;

    fprintf(ofpMain, "\nCode Compiled Successfully\n\n");

    for (i; i < codeCounter; i++)
    {
        fprintf(ofpMain, "%d %d %d\n", codeWrite[i].op, codeWrite[i].l, codeWrite[i].m);
        fprintf(ofp, "%d %d %d\n", codeWrite[i].op, codeWrite[i].l, codeWrite[i].m);
    }

    fprintf(ofpMain, "\n");

    fclose(ifp);
    fclose(ofp);
    fclose(ofpMain);

    if (doPrint)
    {
        FILE* rfp = fopen("mcode.txt", "r");
        printf("\nCode Compiled Successfully\n\n");
        for (;;)
        {
            hold = fgetc(rfp);
            if (!feof(rfp))
                printf("%c", hold);
            else
            {
                printf("\n");
                break;
            }
        }
        fclose(rfp);
    }

return;


}
