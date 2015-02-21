#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define MAX_NUM_Length 5
#define MAX_NAME_LENGTH 11
#define NUM_RES_WORDS 14
#define NUM_SPEC_SYMBOLS 13
#define NUM_POSS_SYMBOLS 16


typedef enum
{
    nulsym = 1, identsym, numbersym, plussym, minussym,
    multsym,  slashsym, oddsym, eqlsym, neqsym, lessym, leqsym,
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
    readsym , elsesym
} token_type;


char* errorCodes[4] =
{
    "The variable does not start with a letter.",
    "A number exceeds 5 digits in length.",
    "A variable name exceeds 11 characters in length.",
    "An invalid symbol was detected."
};

//An array of the reserved words and their respected value
char* resWords[NUM_RES_WORDS]= {"const", "var", "procedure", "call", "begin", "end", "if", "then", "else", "while" , "do", "read", "write", "odd"};
int resWordValues[NUM_RES_WORDS]= {constsym, varsym, procsym, callsym, beginsym, endsym, ifsym, thensym, elsesym, whilesym, dosym, readsym, writesym, oddsym};

//An array of all acceptable symbols
char specSymbols[NUM_SPEC_SYMBOLS]= {'+', '-', '*', '/', '(', ')' , '=', ',', '.', '<', '>', ';', ':'};

//An array of all possible valid symbols and their respected value
char* possibleSymbols[NUM_POSS_SYMBOLS]= {"+","-","*","/","=","<>","<","<=",">",">=","(",")",",",";",".",":="};
int possibleSymbolsValues[NUM_POSS_SYMBOLS]= {plussym, minussym, multsym, slashsym, eqlsym, neqsym, lessym, leqsym, gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym, periodsym, becomessym};


char input[MAX_NAME_LENGTH];



//Print out the error when encountered
void errorHandler(int code)
{
    printf("%s", errorCodes[code]);
    exit(0);

}


int checkComment(FILE* ifp)
{

    char b;
    b = fgetc(ifp);
    if (!feof(ifp))
    {
        if(b == '/')
        {
            while (!feof(ifp))
                if (fgetc(ifp) == '\n')
                    return 1;
        }
        else if(b == '*')
        {
            while (!feof(ifp))
            {
                if (fgetc(ifp) == '*' && fgetc(ifp) == '/')
                {
                    if(fgetc(ifp) == '\n')
                    {
                        ;
                    }
                    else
                        fseek(ifp, -1, SEEK_CUR);
                    return 1;
                }
            }
        }

        else
        {
            fseek(ifp, -1, SEEK_CUR);
            return 0;
        }
    }

    return 0;
}




//Checks if it's a valid symbol
int isSymbol(char input)
{
    int i;
    for(i=0; i<NUM_SPEC_SYMBOLS; i++)
        if(specSymbols[i]==input)
            return 1;
    errorHandler(3);
    return 0;
}

int fetchReservedWord(char input[])
{
    int i;
    for(i=0; i<NUM_RES_WORDS; i++)
    {
        if(strcmp(resWords[i],input) == 0)
            return resWordValues[i];
    }

    return 0;
}

int fetchReservedSymbol(char input[])
{
    int i;
    for(i=0; i<NUM_POSS_SYMBOLS; i++)
    {
        if(strcmp(possibleSymbols[i],input) == 0)
            return possibleSymbolsValues[i];
    }

    return 0;
}


//Checks if the symbol or combination of symbols are valid
int checkSymbol(FILE* ifp,  FILE* ofpCO, FILE* ofpLT, FILE* ofpLL, char input[], char inputChar)
{

    switch (inputChar)
    {
    case '+':
        fprintf(ofpCO, "%c", inputChar);
        fprintf(ofpLT, "%c\t\t%d\n", inputChar, fetchReservedSymbol(input));
        fprintf(ofpLL, "%d ", fetchReservedSymbol(input));
        return 1;
    case '-':
        fprintf(ofpCO, "%c", inputChar);
        fprintf(ofpLT, "%c\t\t%d\n", inputChar, fetchReservedSymbol(input));
        fprintf(ofpLL, "%d ", fetchReservedSymbol(input));
        return 1;
    case '*':
        fprintf(ofpCO, "%c", inputChar);
        fprintf(ofpLT, "%c\t\t%d\n", inputChar, fetchReservedSymbol(input));
        fprintf(ofpLL, "%d ", fetchReservedSymbol(input));
        return 1;
    case '/':
        if (!checkComment(ifp))
        {
            fprintf(ofpCO, "%c", inputChar);
            fprintf(ofpLT, "%c\t\t%d\n", inputChar, fetchReservedSymbol(input));
            fprintf(ofpLL, "%d ", fetchReservedSymbol(input));
        }
        return 1;
    case '=':
        fprintf(ofpCO, "%c", inputChar);
        fprintf(ofpLT, "%c\t\t%d\n", inputChar, fetchReservedSymbol(input));
        fprintf(ofpLL, "%d ", fetchReservedSymbol(input));
        return 1;
    case '(':
        fprintf(ofpCO, "%c", inputChar);
        fprintf(ofpLT, "%c\t\t%d\n", inputChar, fetchReservedSymbol(input));
        fprintf(ofpLL, "%d ", fetchReservedSymbol(input));
        return 1;
    case ')':
        fprintf(ofpCO, "%c", inputChar);
        fprintf(ofpLT, "%c\t\t%d\n", inputChar, fetchReservedSymbol(input));
        fprintf(ofpLL, "%d ", fetchReservedSymbol(input));
        return 1;
    case ',':
        fprintf(ofpCO, "%c", inputChar);
        fprintf(ofpLT, "%c\t\t%d\n", inputChar, fetchReservedSymbol(input));
        fprintf(ofpLL, "%d ", fetchReservedSymbol(input));
        return 1;
    case ';':
        fprintf(ofpCO, "%c", inputChar);
        fprintf(ofpLT, "%c\t\t%d\n", inputChar, fetchReservedSymbol(input));
        fprintf(ofpLL, "%d ", fetchReservedSymbol(input));
        return 1;
    case '.':
        fprintf(ofpCO, "%c", inputChar);
        fprintf(ofpLT, "%c\t%d\n", inputChar, fetchReservedSymbol(input));
        fprintf(ofpLL, "%d ", fetchReservedSymbol(input));
        return 1;
    case ':':
        if ((input[strlen(input)] = fgetc(ifp)) != '=')
            errorHandler(2);
        else
        {
            fprintf(ofpCO, "%s", input);
            fprintf(ofpLT, "%s\t\t%d\n", input, fetchReservedSymbol(input));
            fprintf(ofpLL, "%d ", fetchReservedSymbol(input));
            return 1;
        }
    case '<':
        input[strlen(input)] = fgetc(ifp);
        if (input[strlen(input)-1] == '=')
        {
            fprintf(ofpCO, "%s", input);
            fprintf(ofpLT, "%s\t\t%d\n", input, fetchReservedSymbol(input));
            fprintf(ofpLL, "%d ", fetchReservedSymbol(input));
            return 1;
        }
        else if (input[strlen(input)-1] ==  '>')
        {
            fprintf(ofpCO, "%s", input);
            fprintf(ofpLT, "%s\t\t%d\n", input, fetchReservedSymbol(input));
            fprintf(ofpLL, "%d ", fetchReservedSymbol(input));
            return 1;
        }
        else
        {
            input[strlen(input)-1] = '\0';
            fprintf(ofpCO, "%c", inputChar);
            fprintf(ofpLT, "%c\t\t%d\n", inputChar, fetchReservedSymbol(input));
            fprintf(ofpLL, "%d ", fetchReservedSymbol(input));
            fseek(ifp, -1, SEEK_CUR);
            return 1;
        }
        break;
    case '>':
        input[strlen(input)] = fgetc(ifp);
        if (input[strlen(input)-1] == '=')
        {
            fprintf(ofpCO, "%s", input);
            fprintf(ofpLT, "%s\t\t%d\n", input, fetchReservedSymbol(input));
            fprintf(ofpLL, "%d ", fetchReservedSymbol(input));
            return 1;
        }
        else
        {
            input[strlen(input)-1] = '\0';
            fprintf(ofpCO, "%c", inputChar);
            fprintf(ofpLT, "%c\t\t%d\n", inputChar, fetchReservedSymbol(input));
            fprintf(ofpLL, "%d ", fetchReservedSymbol(input));
            fseek(ifp, -1, SEEK_CUR);
            return 1;
        }
        break;
    default:
        break;
    }
    return 0;

}


int checkReservedWord(FILE* ifp, char input[])
{
    int i;
    char a = fgetc(ifp);
    for(i=0; i<NUM_RES_WORDS; i++)
    {
        if((strcmp(resWords[i],input) == 0) && (isspace(a) || isSymbol(a)))
        {
            fseek(ifp, -1, SEEK_CUR);
            return 1;
        }
    }

    fseek(ifp, -1, SEEK_CUR);
    return 0;
}


void lex_Read(FILE* ifp, FILE* ofpCO, FILE* ofpLT, FILE* ofpLL)
{
    int handle = -1, position = strlen(input)-1;

    if (!isalpha(input[position]) && !isspace(input[position]) && !isdigit(input[position]) && !isSymbol(input[position]))
        errorHandler(3);

    else if (isalpha(input[position]))
        handle = 0;
    else if (isspace(input[position]))
        handle = 1;
    else if (isdigit(input[position]))
        handle = 2;
    else
        handle = 3;

    switch (handle)
    {

    //If the next char is a letter, handle appropriately
    case 0:
        while (!feof(ifp))
        {
            //Check if the number is too long
            if (strlen(input) > 11)
                errorHandler(2);

            position = strlen(input);
            input[position] = fgetc(ifp);
            if (!feof(ifp))
            {
                //Check if the new char is a space. If the input up til now hasn't been a reserved word, then it is a variable
                if (isspace(input[position]))
                {
                    fprintf(ofpCO, "%s", input);
                    input[strlen(input)-1] = '\0';
                    fprintf(ofpLT, "%s\t\t%d\n", input, identsym);
                    fprintf(ofpLL, "%d %s ", identsym, input);
                    memset(input, '\0', sizeof(input));
                    return;
                }

                else if (isalpha(input[position]))
                {
                    if (checkReservedWord(ifp, input))
                    {
                        fprintf(ofpCO, "%s", input);
                        fprintf(ofpLT, "%s\t\t%d\n", input, fetchReservedWord(input));
                        fprintf(ofpLL, "%d ", fetchReservedWord(input));
                        char hold = fgetc(ifp);
                        if (!isalpha(hold) && !isdigit(hold))
                        {
                            memset(input, '\0', sizeof(input));
                            fseek(ifp, -1, SEEK_CUR);
                            return;
                        }
                    }
                }

                else if (isdigit(input[position]))
                    continue;

                else if (isSymbol(input[position]))
                {
                    input[strlen(input)-1] = '\0';
                    fprintf(ofpCO, "%s", input);
                    fprintf(ofpLT, "%s\t\t%d\n", input, identsym);
                    fprintf(ofpLL, "%d %s ", identsym, input);
                    memset(input, '\0', sizeof(input));
                    fseek(ifp, -1, SEEK_CUR);
                    return;

                }
            }
        }
        break;

    case 1:
        fprintf(ofpCO, "%c", input[position]);
        memset(input, '\0', sizeof(input));
        return;
        break;

    case 2:
        while (!feof(ifp))
        {
            //Check if the number is too long
            if (strlen(input) > 5)
            {
                errorHandler(1);
            }
            position = strlen(input);
            input[position] = fgetc(ifp);

            if (!feof(ifp))
            {
                if (isspace(input[position]))
                {
                    fprintf(ofpCO, "%s", input);
                    input[strlen(input)-1] = '\0';
                    fprintf(ofpLT, "%s\t\t%d\n", input, numbersym);
                    fprintf(ofpLL, "%d %s ", numbersym, input);
                    memset(input, '\0', sizeof(input));
                    return;
                }

                else if (isdigit(input[position]))
                    continue;

                //Check if there is a letter in the number
                else if (isalpha(input[position]))
                    errorHandler(0);

                else if (isSymbol(input[position]))
                {
                    input[strlen(input)-1] = '\0';
                    fprintf(ofpCO, "%s", input);
                    fprintf(ofpLT, "%s\t\t%d\n", input, numbersym);
                    fprintf(ofpLL, "%d %s ", numbersym, input);
                    memset(input, '\0', sizeof(input));
                    fseek(ifp, -1, SEEK_CUR);
                    return;

                }
            }
        }

        break;

    case 3:
        if(checkSymbol(ifp, ofpCO, ofpLT, ofpLL, input, input[position]))
            memset(input, '\0', sizeof(input));
        return;

    default:
        break;
    }
}



void lex_Cycle(int print)
{

    FILE *ifp = fopen("input.txt", "r");
    FILE *ofpCO = fopen("cleaninput.txt", "w"), *ofpLT = fopen("lexemetable.txt", "w"), *ofpLL = fopen("lexemelist.txt", "w"), *ofpMain = fopen("compiler_output.txt", "w");

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


    if (ifp == NULL || ofpCO == NULL || ofpLT == NULL || ofpLL == NULL)
    {
        printf("File error.");
        exit(0);
    }

    fprintf(ofpLT, "lexeme\t\ttoken type\n");

    for(;;)
    {
        input[strlen(input)] = fgetc(ifp);
        if (!feof(ifp))
        {
            lex_Read(ifp, ofpCO, ofpLT, ofpLL);
        }
        else
        {
            fclose(ofpCO);
            fclose(ofpLT);
            fclose(ofpLL);
            rewind(ifp);
            for (;;)
            {
                hold = fgetc(ifp);
                if (!feof(ifp))
                    fprintf(ofpMain, "%c", hold);
                else
                {
                    fprintf(ofpMain, "\n\n");
                    break;
                }
            }
            fclose(ifp);
            fclose(ofpMain);

            if (doPrint)
            {
                FILE* rfp = fopen("lexemelist.txt", "r");
                 printf("\n\n");
                for (;;)
                {
                    hold = fgetc(ifp);
                    if (!feof(rfp))
                        printf("%c", hold);
                    else
                    {
                        printf("\n\n");
                        break;
                    }
                }
                fclose(rfp);
            }
            return;
        }

    }

    fclose(ifp);
    fclose(ofpCO);
    fclose(ofpLT);
    fclose(ofpLL);
    return;


}
