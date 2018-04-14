#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>
#include "fasm.h"

struct label
{
    char Name[8];
    s32 Location;
};

struct instruction
{
    char OpCode[4];
    s32 Operand;
};

s32 FindLabelIndex(char *Name, label *Labels, s32 LabelCount)
{
    for(s32 Label = 0; Label < LabelCount; Label++)
    {
        if(strcmp(Labels[Label].Name,  Name) == 0)
        {
            return(Label);
        }
    }
    return(-1);
}

int main(int Argc, char** Argv)
{
    
    FILE *File = fopen(Argv[1], "r");
    s32 LineCount = 0;
    s32 Character = 0;
    instruction Memory[50];
    s32 Accumulator= 0;
    label Labels[50];
    
    char Line[50];
    s32 InstructionIndex = 0;
    s32 LabelIndex = 0;
    fgets(Line, sizeof(Line), File);
    if(!strcmp(Line, ".start"))
    {
        printf("You must define an entry point!");
        return(-1);
    }
    
    while(fgets(Line, sizeof(Line), File) != NULL)
    {
        if (strchr(Line, ':') != NULL)
        {
            char *Colon = strchr(Line, ':');
            s32 StringIndex = (s32)(Colon - Line);
            strncpy(Labels[LabelIndex].Name, Line,StringIndex);
            Labels[LabelIndex].Name[StringIndex] = '\0';
            Labels[LabelIndex].Location = InstructionIndex;
            LabelIndex++;
        }
        InstructionIndex++;
    }
    rewind(File);
    File++;
    InstructionIndex = 0;
    while(fgets(Line, sizeof(Line), File) != NULL)
    {
        char Operand[16];
        if(strchr(Line, ':') == NULL)
        {
            sscanf(Line, "%s %s",&Memory[InstructionIndex].OpCode, &Operand);
            if(!isdigit(Operand[0]))
            {
                Memory[InstructionIndex].Operand =Labels[FindLabelIndex(Operand, Labels, ArrayCount(Labels))].Location;
            }
            else
            {
                Memory[InstructionIndex].Operand = atoi(Operand);
            }
            InstructionIndex++;
        }
    }
    s8 KeyUp = 0;
    s32 ProgramCounter = 0;
    do
    {
        if(GetAsyncKeyState(VK_SHIFT) & 0x8000 && KeyUp == 0)
        {
            KeyUp = 1;
            system("cls");
            
            if(strcmp(Memory[ProgramCounter].OpCode, "STR") == 0)
            {
                Accumulator = Memory[ProgramCounter].Operand;
            }
            if(strcmp(Memory[ProgramCounter].OpCode, "ADD") == 0)
            {
                Accumulator += Memory[ProgramCounter].Operand;
            }
            if(strcmp(Memory[ProgramCounter].OpCode, "SUB") == 0)
            {
                Accumulator -= Memory[ProgramCounter].Operand;
            }
            if(strcmp(Memory[ProgramCounter].OpCode, "JMP") == 0)
            {
                ProgramCounter = Memory[ProgramCounter].Operand-1;
            }
            if(strcmp(Memory[ProgramCounter].OpCode, "BEQ") == 0)
            {
                if(Memory[ProgramCounter-1].Operand == Accumulator)
                {
                    ProgramCounter = Memory[ProgramCounter].Operand-1;
                }
            }
            if(strcmp(Memory[ProgramCounter].OpCode, "BNE") == 0)
            {
                if(Memory[ProgramCounter-1].Operand != Accumulator)
                {
                    ProgramCounter = Memory[ProgramCounter].Operand-1;
                }
            }
            if(strcmp(Memory[ProgramCounter].OpCode, "BLT") == 0)
            {
                if(Memory[ProgramCounter-1].Operand > Accumulator)
                {
                    ProgramCounter = Memory[ProgramCounter].Operand-1;
                }
            }
            if(strcmp(Memory[ProgramCounter].OpCode, "BGT") == 0)
            {
                if(Memory[ProgramCounter-1].Operand < Accumulator)
                {
                    ProgramCounter = Memory[ProgramCounter].Operand-1;
                }
            }
            
            for (s32 Source = 0; Source < InstructionIndex; Source++)
            {
                if(!(Source == ProgramCounter+1))
                {
                    printf("  %s %d\n", Memory[Source].OpCode, Memory[Source].Operand);
                }
                else
                {
                    printf("> %s %d\n", Memory[Source].OpCode, Memory[Source].Operand);
                }
                
            }
            printf("Accumulator: %d\n", Accumulator);
            ProgramCounter++;
            
        }
        else if(GetAsyncKeyState(VK_SHIFT) == 0)
        {
            KeyUp = 0;
        }
        
    }while(!(strcmp(Memory[ProgramCounter].OpCode, "HLT") == 0));
    
    fclose(File);
    
    return(0);
}