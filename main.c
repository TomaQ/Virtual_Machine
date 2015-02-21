
#include "vm.h"
#include "lex.h"
#include "parser.h"

int main(int argc, char *argv[])
{
    int i = 0;
    int a = 0, l = 0, v = 0;


    for (i=0; i<argc; i++)
    {


        if (argc != 1 && strcmp(argv[0], "./compiler") == 0)
        {

            //Call the VM function
            if (strcmp(argv[i],"-v")==0)
            {
                v = 1;
            }
            //Call the Lex WFunction
            else if(strcmp(argv[i],"-l")==0)
            {
                l = 1;
            }
            else if (strcmp(argv[i], "-a")==0)
            {
                a = 1;
            }
            else if (strcmp(argv[i], "./compiler")==0 && i == 0)
                continue;
            else
            {
                printf("Unknown Directive: %s\n", argv[i]);
                return 0;
            }
        }

        else if (argc == 1 && strcmp(argv[i], "./compiler") == 0)
        {
            continue;
        }

        else
        {
            printf("Unknown Directive: %s\n", argv[i]);
            return 0;
        }
    }

    lex_Cycle(l);
    parser_Cycle(a);
    vm_Cycle(v);

    return 0;
}
