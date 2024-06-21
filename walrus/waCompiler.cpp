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
//#include <cstring>

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
   // parsing
   int idxLine = 0;
   char* line = nullptr;
   size_t size;

   // out 
   MicroFilter fToBuild;
   int idxArg = 0;
   std::vector<MicroFilter>& out;
   ECompilerError err;

   CompilerContext(const char* sourceCode, size_t _size, std::vector<MicroFilter>& _filters)
      : err(E_SUCCESS)
      , size(_size)
      , out(_filters)
      , fToBuild()
   {
      bufCopy = (char *)malloc(size);
      if (!bufCopy) {
         printf("Compile alloc failed %llu bytes\n", size);
         PLATFORM_GETCH();
         exit(0);
      }
      strcpy_s(bufCopy, size, sourceCode);
      line = bufCopy;
   }

   ~CompilerContext()
   {
      if (bufCopy) {
         free(bufCopy);
      }
   }

   char* Buf() { return bufCopy; }

   bool IsLineInRange()
   {
      return ((size_t)(line - bufCopy) <= size);
   }

   void NextLineAt(char* pos)
   {
      idxLine++;
      line = pos;
      fToBuild = MicroFilter();
      idxArg = 0;
   }

   void DumpBuiltFilter()
   {
      out.push_back(fToBuild);
      fToBuild = MicroFilter();
   }

   void AddArg(int arg)
   {
      fToBuild.params[idxArg++] = arg;
   }

private:
   char* bufCopy;
};

bool Semantics::Compile(const char* sourceCode, size_t size, std::vector<MicroFilter>& filters)
{
   // no code => no problem
   if (!sourceCode || !sourceCode[0]) {
      return true;
   }

   // scan via context
   CompilerContext ctx(sourceCode, size, filters);
   char* posEndl(ctx.Buf());
   for (; posEndl && ctx.IsLineInRange(); ctx.NextLineAt(posEndl + 1)) {
      // detach line
      posEndl = strchr(ctx.line, '\n');
      if (posEndl) {
         *posEndl = 0;
      }

      // pass
      if (!CompileOneLine(ctx)) {
         return false;
      }
   }

   return true;
}

enum ECompilerState {
   S_IDLE,
   S_POSITION,
   S_FILTER,
   S_ARGUMENTS
};

//uint ExactShape(twContext* lay, const uint* par);
//uint ModelShape(twContext* lay, const uint *par);
//uint PointsRange(twContext* lay, const uint *par);
//uint PointsLimit(twContext* lay, const uint *par);
//uint PointsAtLeast(twContext* lay, const uint *par);
//uint ControlsRange(twContext* lay, const uint* par);
//uint KeyCardsRange(twContext* lay, const uint* par);
//// -
//uint LineControlsRange(twContext* lay, const uint* par);
//uint LineAcesRange(twContext* lay, const uint* par);
//uint LineKeyCardsSpade(twContext* lay, const uint* par);
//uint LinePointsRange(twContext* lay, const uint *par);
//// -
//uint PointsSuitLimit(twContext* lay, const uint* par);
//uint PointsSuitAtLeast(twContext* lay, const uint* par);
//uint PointsSuitLessSuit(twContext* lay, const uint* par);
//uint PointsSuitLEqSuit(twContext* lay, const uint* par);
//// -
//uint SpadesNatural(twContext* lay, const uint* par);
//uint HeartsNatural(twContext* lay, const uint* par);
//uint DiamondsNatural(twContext* lay, const uint* par);
//uint NoMajorFit(twContext* lay, const uint *par);
//// -
//uint SpadesLen(twContext* lay, const uint *par);
//uint HeartsLen(twContext* lay, const uint *par);
//uint DiamondsLen(twContext* lay, const uint *par);
//uint ClubsLen(twContext* lay, const uint *par);
//// -
//uint NoOvcOn1LevOpen(twContext* lay, const uint *par);
//uint NoOvercall(twContext* lay, const uint *par);
//uint NoPrecision2C(twContext* lay, const uint *par);
//uint No7Plus(twContext* lay, const uint *par);
//uint No2SuiterAntiSpade(twContext* lay, const uint *par);
//uint No2SuitsAntiHeart(twContext* lay, const uint *par);
//uint No2SuitsMinors(twContext* lay, const uint* par);
//uint TakeoutOfClubs(twContext* lay, const uint* par);
//// -- branching
//uint AnyInListBelow(twContext* lay, const uint *par);
//uint ExcludeCombination(twContext* lay, const uint *par);

//sem.vecFilters.push_back( MicroFilter(&WaFilter::NAME, #NAME" "#P2" "#P3" "#HAND, HAND, P2, P3)          )

struct Parser
{
   char* token = nullptr;

   Parser(CompilerContext& _ctx) : ctx(_ctx)
   {
      strcpy_s(backupLine, sizeof(backupLine), ctx.line);
      token = std::strtok(ctx.line, delimiters);
   }

   void operator++()
   {
      token = std::strtok(nullptr, delimiters);
   }

   bool IsToken(const char* key)
   {
      return 0 == strcmp(token, key);
   }

   void Fail(const char* reason1, const char* r2 = "", const char* r3 = "")
   {
      printf("%s%s%s #%d: %s\n", reason1, r2, r3, ctx.idxLine + 1, backupLine);
   }

private:
   char backupLine[64];
   CompilerContext& ctx;
   static const char* delimiters;
};
const char* Parser::delimiters = " ,.!:;()";

#define ACCEPT_POS(NAME) if (parser.IsToken(#NAME)) {  \
            ctx.AddArg(NAME);                            \
            fsmState = S_FILTER;                         \
         }

bool Semantics::CompileOneLine(CompilerContext &ctx)
{
   // prepare
   printf("Line %d: %s\n", ctx.idxLine, ctx.line);
   ECompilerState fsmState = S_IDLE;

   // parse all tokens
   Parser parser(ctx);
   for ( ; parser.token; ++parser ) {
      // Print each token
      printf( "%s\n", parser.token);
      switch (fsmState) {
         case S_IDLE:
            // opening brackets
            // no break;

         case S_POSITION:
            ACCEPT_POS(SOUTH);
            ACCEPT_POS(WEST);
            ACCEPT_POS(NORTH);
            ACCEPT_POS(EAST);

            if (fsmState == S_POSITION) {
               parser.Fail("Unrecognized position ", parser.token, " in line");
               return false;
            }
            break;

         case S_FILTER:
            ctx.fToBuild.func = &WaFilter::PointsRange;
            strcpy(ctx.fToBuild.name, "SOUTH");
            fsmState = S_ARGUMENTS;
            break;

         case S_ARGUMENTS:
            ctx.AddArg(14);
            ctx.AddArg(15);

            ctx.DumpBuiltFilter();
            return true;
            break;

         default:
            DEBUG_UNEXPECTED;
            return false;
      }
   }

   // only in idle state we can accept empty token
   if (fsmState == S_IDLE) {
      return true;
   }

   parser.Fail("Unexpected end of line");
   return false;
}
