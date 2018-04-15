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
    u32 Location;
};

typedef u64 instruction;

enum opcode
{
    HLT = 0,
    STR,
    ADD,
    SUB,
    CMP,
    BEQ,
    BNE,
    BLT,
    BGT,
    JMP,
    opcode_end,
};
char* OpCodeArray[] = {"HLT", "STR", "ADD", "SUB", "CMP", "BEQ", "BNE", "BLT", "BGT", "JMP"};

opcode GetOpCodeEnum(char* OpCode)
{
    for (u32 Op = 0; Op < ArrayCount(OpCodeArray); Op++)
    {
        if (strcmp(OpCodeArray[Op], OpCode) == 0)
        {
            return((opcode)Op);
        }
    }
}

instruction HarvardToVonNeumann(u32 OpCode, u32 Operand)
{
    u64 result = ((u64)OpCode) << 32 | Operand;
    return(result);
}

u32 FindLabelIndex(char *Name, label *Labels, u32 LabelCount)
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

u32 ExtractOpcode(instruction Instruction)
{
    u32 result = (u32)(Instruction>>32);
    return(result);
}

u32 ExtractOperand(instruction Instruction)
{
    u32 result = (u32)Instruction;
    return(result);
}

int main(int Argc, char** Argv)
{
    
    FILE *File = fopen(Argv[1], "r");
    u32 LineCount = 0;
    u32 Character = 0;
    instruction Memory[50];
    label Labels[50];
    
    u64 Accumulator = 0;
    u64 ProgramCounter = 0;
    
    char Line[50];
    u32 InstructionIndex = 0;
    u32 LabelIndex = 0;
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
    
    InstructionIndex = 0;
    while(fgets(Line, sizeof(Line), File) != NULL)
    {
        char OperandChar[16];
        char OpCodeChar[4];
        u32 OperandInt;
        u32 OpCodeInt;
        if(strchr(Line, ':') == NULL)
        {
            sscanf(Line, "%s %s",&OpCodeChar[0], &OperandChar[0]);
            OpCodeInt = GetOpCodeEnum(OpCodeChar);
            if(!isdigit(OperandChar[0]))
            {
                OperandInt =Labels[FindLabelIndex(OperandChar, Labels, ArrayCount(Labels))].Location;
            }
            else
            {
                OperandInt = atoi(OperandChar);
            }
            Memory[InstructionIndex] = HarvardToVonNeumann(OpCodeInt, OperandInt);
            InstructionIndex++;
        }
    }
    u8 KeyUp = 0;
    u32 OpCode, Operand, CurrentInstructionRegister;
    
    printf("Press the down key to start");
    
    do
    {
        if(GetAsyncKeyState(VK_DOWN) & 0x8000 && KeyUp == 0)
        {
            KeyUp = 1;
            system("cls");
            
            OpCode = ExtractOpcode(Memory[ProgramCounter]);
            CurrentInstructionRegister= ExtractOperand(Memory[ProgramCounter-1]);
            Operand = ExtractOperand(Memory[ProgramCounter]);
            switch((opcode)OpCode)
            {
                case STR:
                Accumulator = Operand;
                break;
                
                case ADD:
                Accumulator += Operand;
                break;
                
                case SUB:
                Accumulator -= Operand;
                break;
                
                case JMP:
                ProgramCounter = Operand-1;
                break;
                
                case BEQ:
                if(CurrentInstructionRegister == Accumulator)
                {
                    ProgramCounter = Operand-1;
                }
                break;
                
                case BNE:
                if(CurrentInstructionRegister != Accumulator)
                {
                    ProgramCounter = Operand-1;
                }
                break;
                
                case BLT:
                if(CurrentInstructionRegister < Accumulator)
                {
                    ProgramCounter = Operand-1;
                }
                break;
                
                case BGT:
                if(CurrentInstructionRegister > Accumulator)
                {
                    ProgramCounter = Operand-1;
                }
                break;
                
            }
            
            for (u32 Source = 0; Source < InstructionIndex; Source++)
            {
                u32 SourceOpCode = ExtractOpcode(Memory[Source]);
                u32 SourceOperand = ExtractOperand(Memory[Source]);
                
                if(!(Source == ProgramCounter+1))
                {
                    printf("%s %d\n", OpCodeArray[SourceOpCode], SourceOperand);
                }
                else
                {
                    printf("> %s %d\n", OpCodeArray[SourceOpCode], SourceOperand);
                }
                
            }
            printf("Accumulator: %d\n", Accumulator);
            ProgramCounter++;
            
        }
        else if(GetAsyncKeyState(VK_DOWN) == 0)
        {
            KeyUp = 0;
        }
        
    }while(OpCode != HLT);
    
    fclose(File);
    
    return(0);
}