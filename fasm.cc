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
#include "fasm.h"

struct label
{
    char Name[8];
    u32 Location;
};

typedef u64 instruction;

enum opcode
{
    HLT = 0, //0 arguments
    
    STR, //2 arguments
    LDR,
    CMP,
    MOV,
    MVN,
    
    ADD, //3 arguments
    SUB,
    AND,
    ORR,
    EOR,
    LSL,
    LSR,
    
    BEQ, //1 argument
    BNE,
    BLT,
    BGT,
    JMP,
};

global char* RegisterArray[] = 
{
    "eax", "ebx", "ecx", "edx", "eex",
};

global char* OpCodeArray[] = 
{
    "HLT",
    "STR","LDR","CMP","MOV", "MVN",
    "ADD","SUB","AND","ORR", "EOR", "LSL", "LSR",
    "BEQ", "BNE", "BLT", "BGT", "JMP"
};

internal opcode
GetOpCode(char* OpCode)
{
    for (u32 Op = 0; Op < ArrayCount(OpCodeArray); Op++)
    {
        if (strcmp(OpCodeArray[Op], OpCode) == 0)
        {
            return((opcode)Op);
        }
    }
}

internal u32
GetRegister(char* Register)
{
    for (u32 Reg = 0; Reg < ArrayCount(RegisterArray); Reg++)
    {
        if(strcmp(RegisterArray[Reg], Register)==0)
        {
            return(Reg);
        }
    }
}
internal void
LoadRegister(char* Register, s32 Value)
{
    u32 Index = GetRegister(Register);
    
}

internal instruction 
HarvardToVonNeumann(u32 OpCode, u32 Operand)
{
    u64 result = ((u64)OpCode) << 32 | Operand;
    return(result);
}

internal u32 
FindLabelIndex(char *Name, label *Labels, u32 LabelCount)
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

internal u32
ExtractOpcode(instruction Instruction)
{
    u32 result = (u32)(Instruction>>32);
    return(result);
}

internal u32
ExtractOperand(instruction Instruction)
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
    s64 Registers[5];
    s64 Accumulator = 0;
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
        char OperandStr[16];
        char OpCodeStr[4];
        char RegisterStoreStr[4];
        char RegisterInputStr[4];
        
        u32 OperandInt;
        u32 OpCodeInt;
        
        opcode OpCode;
        
        if(strchr(Line, ':') == NULL)
        {
            sscanf(Line, "%s",&OpCodeStr[0]);
            OpCodeInt = GetOpCode(OpCodeStr);
            switch(OpCodeInt)
            {
                case HLT:
                sscanf(Line, "%s", &OpCodeStr[0]);
                break;
                
                case STR:
                case LDR:
                case CMP:
                case MOV:
                case MVN:
                sscanf(Line, "%s %s %s", &OpCodeStr[0], &RegisterInputStr[0], &OperandStr[0]);
                break;
                
                case ADD:
                case SUB:
                case AND:
                case ORR:
                case EOR:
                case LSL:
                case LSR:
                sscanf(Line, "%s %s %s %s", 
                       &OpCodeStr[0], &RegisterInputStr[0], &RegisterStoreStr[0], &OperandStr[0]);
                break;
                
                case BEQ:
                case BNE:
                case BLT:
                case BGT:
                case JMP:
                sscanf(Line, "%s %s", &OpCodeStr[0], &OperandStr[0]);
                break;
            }
            
            if(OpCodeInt >= BEQ && OpCodeInt <= JMP)
            {
                u32 LabelIndex = FindLabelIndex(OperandStr, Labels, ArrayCount(Labels));
                OperandInt = Labels[LabelIndex].Location;
            }
            
            if(OperandStr[0] == '#')
            {
                char Literal[16];
                strncpy(Literal, OperandStr +1, 15);
                OperandInt = atoi(Literal);
            }
            Memory[InstructionIndex] = HarvardToVonNeumann(OpCodeInt, OperandInt);
            InstructionIndex++;
        }
    }
    u8 KeyUp = 0;
    u32 OpCode, Operand, CurrentInstructionRegister, RegisterStore, RegisterInput;
    printf("Press the down key to start");
    
    do
    {
        if(fgetc(stdin) == 'n')
        {
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
                
                case MVN:
                Accumulator = ~Accumulator;
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
                
                if(!(Source == ProgramCounter))
                {
                    printf("  %s %d\n", OpCodeArray[SourceOpCode], SourceOperand);
                }
                else
                {
                    printf("> %s %d\n", OpCodeArray[SourceOpCode], SourceOperand);
                }
                
            }
            printf("Accumulator: %d\n", Accumulator);
            ProgramCounter++;
            
        }
        
        
        
    }while(OpCode != HLT);
    
    fclose(File);
    
    return(0);
}