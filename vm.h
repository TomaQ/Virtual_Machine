#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

typedef struct instruction
{
    int op; // opcode
    int l;  // L
    int m;  // M
} instruction;

int stack[MAX_STACK_HEIGHT], SP = 0, BP = 1, PC = 0, halt = 1, bpStack[MAX_STACK_HEIGHT], bpCounter = 0;
instruction code[MAX_CODE_LENGTH], IR = {0, 0, 0};
char *ops[11] =  {"LIT", "OPR", "LOD", "STO", "CAL", "INC", "JMP", "JPC", "SIO", "SIO", "SIO"};


int base(int l, int base) // l stand for L in the instruction format
{
    int b1 = base; //find base L levels down
    while (l > 0)
    {
        b1 = stack[b1 + 1];
        l--;
    }
    return b1;
}

void fetch(FILE* ofp, FILE* ofpMain, FILE* tmp)
{
    IR = code[PC];
    fprintf(ofp, "%3d\t%s\t%d\t%d", PC, ops[IR.op-1], IR.l, IR.m);
    fprintf(ofpMain, "%3d\t%s\t%d\t%d", PC, ops[IR.op-1], IR.l, IR.m);
    fprintf(tmp, "%3d\t%s\t%d\t%d", PC, ops[IR.op-1], IR.l, IR.m);
    PC++;

}

void execute(FILE* ofp, FILE* ofpMain, FILE* tmp)
{


    switch(IR.op)
    {
    case 1: //LIT
        SP++;
        stack[SP] = IR.m;
        break;
    case 2: //OPR
        switch (IR.m)
        {
        case 0: //REG
            SP = BP-1;
            PC = stack[SP+4];
            BP = stack[SP+3];
            bpCounter--;
            bpStack[bpCounter] = 0;
            break;
        case 1: //NEG
            stack[SP] *= -1;
            break;
        case 2: //ADD
            SP--;
            stack[SP] += stack[SP+1];
            break;
        case 3: //SUB
            SP--;
            stack[SP] -= stack[SP+1];
            break;
        case 4: //MUL
            SP--;
            stack[SP] *= stack[SP+1];
            break;
        case 5: //DIV
            SP--;
            stack[SP] /= stack[SP+1];
            break;
        case 6: //ODD
            stack[SP] %= 2;
            break;
        case 7: //MOD
            SP--;
            stack[SP] %= stack[SP+1];
            break;
        case 8: //EQL
            SP--;
            stack[SP] = (stack[SP] == stack[SP+1]);
            break;
        case 9: //NEQ
            SP--;
            stack[SP] = (stack[SP] != stack[SP+1]);
            break;
        case 10: //LES
            SP--;
            stack[SP] = (stack[SP] < stack[SP+1]);
            break;
        case 11: //LEQ
            SP--;
            stack[SP] = (stack[SP] <= stack[SP+1]);
            break;
        case 12: //GTR
            SP--;
            stack[SP] = (stack[SP] > stack[SP+1]);
            break;
        case 13: //GEQ
            SP--;
            stack[SP] = (stack[SP] >= stack[SP+1]);
            break;
        default:
            break;
        }
        break;
    case 3: //LOD
        SP += 1;
        stack[SP] = stack[base(IR.l, BP) + IR.m];
        break;
    case 4: //STO
        stack[base(IR.l, BP) + IR.m] = stack[SP];
        SP -= 1;
        break;
    case 5: //CAL
        stack[SP+1] = 0;
        stack[SP+2] = base(IR.l, BP);
        stack[SP+3] = BP;
        stack[SP+4] = PC;
        BP = SP + 1;
        PC = IR.m;
        bpStack[bpCounter] = SP;
        bpCounter++;
        break;
    case 6: //INC
        SP += IR.m;
        break;
    case 7: // JMP
        PC = IR.m;
        break;
    case 8: //JPC
        if (stack[SP] == 0)
            PC = IR.m;
        SP = SP-1;
        break;
    case 9: //SIO Print
        printf("[OUT]: %d\n", stack[SP--]);
        break;
    case 10: //SIO Input
        printf("[IN]: ");
        scanf("%d", &stack[++SP]);
        break;
    case 11: //SIO Halt
        halt = 0;
        break;
    default: //Unrecognized instruction
        BP = 0;
        break;
    }

    fprintf(ofp, "\t%d\t%d\t%d\t", PC, BP, SP);
    fprintf(ofpMain, "\t%d\t%d\t%d\t", PC, BP, SP);
    fprintf(tmp, "\t%d\t%d\t%d\t", PC, BP, SP);
    int iterator, i;
    for (iterator = 1; iterator<=SP; iterator++)
    {
        i=0;

        //If there are any new action registers, place them in the output in the appropriate locations
        while (bpStack[i] != 0)
        {
            if (iterator-1 == bpStack[i])
            {
                fprintf(ofp, "| ");
                fprintf(ofpMain, "| ");
                fprintf(tmp, "| ");
            }
            i++;
        }

        fprintf(ofp, "%d ", stack[iterator]);
        fprintf(ofpMain, "%d ", stack[iterator]);
        fprintf(tmp, "%d ", stack[iterator]);
    }


    fprintf(ofp, "\n");
    fprintf(ofpMain, "\n");
    fprintf(tmp, "\n");


}


void vm_Cycle(int print)
{

    FILE* ifp = fopen("mcode.txt", "r");
    FILE* ofp = fopen("stacktrace.txt", "w");
    FILE* ofpMain = fopen("compiler_output.txt", "a");
    FILE*tmp = fopen("temporary.txt", "w");


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

    //Check that the files exist
    if (ifp == NULL || ofp == NULL)
    {
        printf("File error.");
        exit(0);
    }

    int counter = 0;



    //Place text file input into code
    for(;;)
    {

        if (fscanf(ifp, "%d%d%d", &code[counter].op, &code[counter].l, &code[counter].m) != 3)
            break;

        if (counter == 0)
            fprintf(ofp, "%s", "Line\tOP\tL\tM\n");
        fprintf(ofp, "%3d\t%s\t%d\t%d\n", counter, ops[code[counter].op-1], code[counter].l, code[counter].m);
        counter++;

    }



    fprintf(ofp, "\n\t\t\t\tPC\tBP\tSP\tstack\n\n");
    fprintf(ofp, "Initial values\t\t\t%d\t%d\t%d\n", PC, BP, SP);

    fprintf(ofpMain, "\nStack Trace:\n");
    fprintf(ofpMain, "\n\t\t\t\tPC\tBP\tSP\tstack\n\n");
    fprintf(ofpMain, "Initial values\t\t\t%d\t%d\t%d\n", PC, BP, SP);
    fprintf(tmp, "\nStack Trace:\n");
    fprintf(tmp, "\n\t\t\t\tPC\tBP\tSP\tstack\n\n");
    fprintf(tmp, "Initial values\t\t\t%d\t%d\t%d\n", PC, BP, SP);

    //Exit code if halt or BP are false
    while (halt != 0 && BP != 0)
    {
        //Fetch Cycle
        fetch(ofp, ofpMain, tmp);
        //Execute Cycle
        execute(ofp, ofpMain, tmp);
    }

    fclose(ifp);
    fclose(ofp);
    fclose(ofpMain);
    fclose(tmp);
    if (doPrint)
    {
        tmp = fopen("temporary.txt", "r");
        for (;;)
        {
            hold = fgetc(tmp);
            if (!feof(tmp))
                printf("%c", hold);
            else
            {
                printf("\n\n");
                break;
            }
        }
        fclose(tmp);
    }

    return;


}
