/************************************************************
* Walrus project                                        2023
* Configuration
*
************************************************************/
#define  _CRT_SECURE_NO_WARNINGS
#include "waCrossPlatform.h"
#include HEADER_SLEEP
#include HEADER_CURSES
#include "walrus.h"

/*
#include <iostream>
#include <cstring>

int main() {
// Define a string to tokenize
char str[] = "Hello, world! This is a sample string.";

// Define the delimiter characters


return 0;
}
*/

void Semantics::MiniLink(std::vector<MicroFilter>& filters)
{
   // resolve any-in-list-below block
   uint retAddr;
   for (uint ip = 0; ip < filters.size(); ip++) {
      // notice one
      auto& mic = filters[ip];
      if (!IsListStart(mic)) {
         continue;
      }

      // detect return address, done on zero depth or array end
      int depth = 1;
      for (retAddr = ip+1; retAddr < filters.size() && depth; retAddr++) {
         // notice nested blocks 
         const auto& scanRet = filters[retAddr];
         if (IsListStart(scanRet)) {
            depth++;
         }
         if (scanRet.func == &WaFilter::EndList) {
            depth--;
         }
      }

      // place it in the instruction
      mic.params[1] = retAddr - 1;
   }
}

enum ECompilerError {
   E_SUCCESS,
   E_POSITION,
   E_FILTER,
   E_ARGUMENTS
};

struct CompilerContext
{
   int idxLine = 0;
   char* line = nullptr;
   ECompilerError err;
   std::vector<MicroFilter>& filters;

   CompilerContext(const char* sourceCode, size_t size, std::vector<MicroFilter>& _filters)
      : err(E_SUCCESS)
      , filters(_filters)
   {

   }
};

bool Semantics::Compile(const char* sourceCode, size_t size, std::vector<MicroFilter>& filters)
{
   // no code => no problem
   if (!sourceCode || !sourceCode[0]) {
      return true;
   }

   // prepare
   CompilerContext ctx(sourceCode, size, filters);
   char* bufCopy = (char *)malloc(size);
   if (!bufCopy) {
      printf("Compile alloc failed %llu bytes\n", size);
      PLATFORM_GETCH();
      exit(0);
   }
   strcpy_s(bufCopy, size, sourceCode);

   // scan lines
   char* curLine = bufCopy;
   char* posEndl = bufCopy;
   for (int idxLine = 0; posEndl && ((size_t)(curLine - bufCopy) <= size); idxLine++) {
      posEndl = strchr(curLine, '\n');
      if (posEndl) {
         *posEndl = 0;
      }
      printf("Line %d: %s\n", idxLine, curLine);
      curLine = posEndl + 1;
   }

   // cleanup
   free(bufCopy);
   return true;
}

enum ECompilerState {
   S_IDLE,
   S_POSITION,
   S_FILTER,
   S_ARGUMENTS
};

bool Semantics::CompileOneLine(CompilerContext &ctx)
{
   static const char* delimiters = " ,.!:;";
   ECompilerState fsmState = S_IDLE;

   // Use strtok to get the first token
   for ( char* token = std::strtok(ctx.line, delimiters); token; token = std::strtok(nullptr, delimiters) ) {
      // Print each token
      printf( "%s\n", token);
   }

   return false;
}

