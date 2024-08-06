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

struct NullFilter {
   MicroFunc func;
   const char* name;
   NullFilter(MicroFunc _func, const char* _name)
      : func(_func), name(_name) 
   {}
};

struct HandFilter : public NullFilter {
   int countArgs;
   HandFilter(MicroFunc _func, const char* _name, int _count)
      : NullFilter(_func, _name)
      , countArgs(_count)
   {}
};

#define DESCRIBE_BRACKET(NAME)     {&WaFilter::NAME, #NAME}
#define DESCRIBE_FILTER(NAME, NUM) {&WaFilter::NAME, #NAME, NUM}

static NullFilter OpeningBrackets[] =
{
   DESCRIBE_BRACKET(AnyInListBelow),
   DESCRIBE_BRACKET(ExcludeCombination)
};

static HandFilter ExportedHandFilters[] = {
   DESCRIBE_FILTER(PointsRange, 2),
   DESCRIBE_FILTER(ControlsRange, 2),
   DESCRIBE_FILTER(PointsAtLeast, 1),
   DESCRIBE_FILTER(KeyCardsRange, 2),

   DESCRIBE_FILTER(ExactShape, 4),
   DESCRIBE_FILTER(ModelShape, 4),

   DESCRIBE_FILTER(SpadesLen, 2),
   DESCRIBE_FILTER(HeartsLen, 2),
   DESCRIBE_FILTER(DiamondsLen, 2),
   DESCRIBE_FILTER(ClubsLen, 2),

   DESCRIBE_FILTER(LineControlsRange, 3),
   DESCRIBE_FILTER(LineAcesRange, 3),
   DESCRIBE_FILTER(LineKeyCardsSpade, 3),
   DESCRIBE_FILTER(LinePointsRange, 3),

   DESCRIBE_FILTER(PointsSuitLimit, 2),
   DESCRIBE_FILTER(PointsSuitAtLeast, 2),
   DESCRIBE_FILTER(PointsSuitLessSuit, 2),
   DESCRIBE_FILTER(PointsSuitLEqSuit, 2),

   DESCRIBE_FILTER(SpadesNatural, 0),
   DESCRIBE_FILTER(HeartsNatural, 0),
   DESCRIBE_FILTER(DiamondsNatural, 0),
   DESCRIBE_FILTER(NoMajorFit, 0),

   DESCRIBE_FILTER(PenaltyDoubleSuit, 1),
   DESCRIBE_FILTER(PenaltyDoubleDiamonds, 0),

   DESCRIBE_FILTER(NoOvcOn1LevOpen, 0),
   DESCRIBE_FILTER(NoOvercall, 0),
   DESCRIBE_FILTER(NoPrecision2C, 0),
   DESCRIBE_FILTER(No7Plus, 0),
   DESCRIBE_FILTER(No2SuiterAntiSpade, 0),
   DESCRIBE_FILTER(No2SuitsAntiHeart, 0),
   DESCRIBE_FILTER(No2SuitsMinors, 0),
   DESCRIBE_FILTER(TakeoutOfClubs, 0),
};

bool Semantics::MiniLink(std::vector<MicroFilter>& filters)
{
   // checsk size
   if (filters.size() > IPR_COMPACTION * 4) {
      printf("Link ERROR: Insufficent hit array size for %llu filters\n", filters.size());
      return false;
   }

   // resolve all bracket-blocks
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

      // sanity check
      if (depth) {
         printf("Link ERROR: Unmatched control path bracket for filter: %s\n", mic.name);
         return false;
      }
   }

   return true;
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

   void AddText(const char* text) {
      strcat(fToBuild.name, text);
      strcat(fToBuild.name, " ");
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

static bool IsDigitsOnly(const char* str) 
{
   if (str == nullptr) {
      return false;
   }

   while (*str) {
      if (!isdigit(static_cast<unsigned char>(*str))) {
         return false;
      }
      ++str;
   }
   return true;
}

enum EParserState {
   PS_IDLE,
   PS_POSITION,
   PS_FILTER,
   PS_ARGUMENTS,
   PS_BRACKET
};

struct Parser
{
   char* token = nullptr;
   int argsLeftExpected = 0;

   Parser(CompilerContext& _ctx) 
      : ctx(_ctx)
      , posName()
      , backupLine()
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

   void NoticeTokenIsPosition()
   {
      strcpy_s(posName, sizeof(posName), token);
   }

   void AppendPosition()
   {
      ctx.AddText(posName);
   }

   bool Fail(const char* reason1, const char* r2 = "", const char* r3 = "")
   {
      printf("\n%s%s%s #%d: %s\n", reason1, r2, r3, ctx.idxLine + 1, backupLine);
      return false;
   }

   bool FailTok(const char* reason)
   {
      return Fail(reason, token, " in line");
   }

   bool AcceptOpeningBracket()
   {
      for (const auto& filter : OpeningBrackets) {
         if (IsToken(filter.name)) {
            ctx.fToBuild.func = filter.func;
            AppendPosition();
            ctx.AddText(filter.name);
            ctx.AddText(" {");
            argsLeftExpected = 0;
            return true;
         }
      }

      return false;
   }

   bool AcceptClosingBracket()
   {
      if (IsToken("}")) {
         ctx.fToBuild.func = &WaFilter::EndList;
         strcpy(ctx.fToBuild.name, "} ");
         return true;
      }

      return false;
   }

   bool AcceptHandFilter()
   {
      for (const auto& filter : ExportedHandFilters) {
         if (IsToken(filter.name)) {
            ctx.fToBuild.func = filter.func;
            ctx.AddText(filter.name);
            argsLeftExpected = filter.countArgs;
            return true;
         }
      }

      return false;
   }

   bool AcceptArgument()
   {
      if (!argsLeftExpected) {
         return false;
      }

      if (TryAcceptNumber()) {
         return true;
      }

      return false;
   }

private:
   char backupLine[64];
   char posName[10];
   CompilerContext& ctx;
   static const char* delimiters;

   bool TryAcceptNumber()
   {
      if (!IsDigitsOnly(token)) {
         return false;
      }

      int number = atoi(token);
      ctx.AddArg(number);
      ctx.AddText(token);
      argsLeftExpected--;

      return true;
   }
};
const char* Parser::delimiters = " ,.!:;(){";

#define ACCEPT_POS(NAME) if (parser.IsToken(#NAME)) {  \
            ctx.AddArg(NAME);                          \
            parser.NoticeTokenIsPosition();            \
            fsmState = PS_FILTER;                       \
         }

bool Semantics::CompileOneLine(CompilerContext &ctx)
{
   // prepare
   //printf("Line %2d: %s\n", ctx.idxLine + 1, ctx.line);
   EParserState fsmState = PS_IDLE;

   // parse all tokens
   Parser parser(ctx);
   for ( ; parser.token; ++parser ) {
      //printf( "%s\n", parser.token);

      switch (fsmState) {
         case PS_IDLE:
            if (parser.AcceptClosingBracket()) {
               fsmState = PS_BRACKET;
               break;
            }
            // merge down

         case PS_POSITION:
            ACCEPT_POS(SOUTH);
            ACCEPT_POS(WEST);
            ACCEPT_POS(NORTH);
            ACCEPT_POS(EAST);

            if (fsmState != PS_FILTER) {
               return parser.FailTok("Unrecognized position ");
            }
            break;

         case PS_FILTER:
            if (parser.AcceptOpeningBracket()) {
               fsmState = PS_BRACKET;
            } else if (parser.AcceptHandFilter()) {
               fsmState = PS_ARGUMENTS;
            } else {
               return parser.FailTok("Unrecognized hand filter ");
            }
            break;

         case PS_ARGUMENTS:
            if (!parser.AcceptArgument()) {
               return parser.FailTok("Unexpected argument ");
            }
            break;

         default:
            DEBUG_UNEXPECTED;
            return false;
      }
   }

   // analyze state where line has ended
   switch (fsmState) {
      case PS_IDLE:
         return true;

      case PS_ARGUMENTS:
         if (parser.argsLeftExpected) {
            return parser.Fail("Too few arguments in line");
         }
         parser.AppendPosition();
         ctx.DumpBuiltFilter();
         return true;

      case PS_BRACKET:
         ctx.DumpBuiltFilter();
         return true;
   }

   // not accepted => error
   return parser.Fail("Unexpected end of line");
}

//#define DBG_REPORT_SIZES

void WaConfig::BuildNewFilters(Walrus *walrus)
{
   if (!sizeSourceCode) {
      printf("No filters are found in the config.\n");
      return;
   }

   #ifdef DBG_REPORT_SIZES
      printf("A filters source code is found in the config. Passing to compiler, size is %llu of %llu.\n", 
         sizeSourceCode, sizeof(sourceCodeFilters));
   #else
      printf("Compiling filters...");
   #endif

   if (!walrus->sem.Compile(sourceCodeFilters, sizeSourceCode, filtersLoaded)) {
      isInitSuccess = false;
      printf("Config ERROR: Failed to compile filters.\n");
      return;
   }

   if (!walrus->sem.MiniLink(filtersLoaded)) {
      isInitSuccess = false;
      printf("Config ERROR: Failed to link filters.\n");
      return;
   }

   printf("Success on filters compiling and linking.\n");
}

