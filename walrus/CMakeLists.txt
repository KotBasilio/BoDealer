SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lncurses")
file (GLOB  BODEALER_SOURCE 
	"*.h"
	"*.cpp"
	)

	
add_executable(bodealer walrus.cpp waMulti.cpp waAI.cpp waBitSuits.cpp waBitTwelve.cpp waDDSTest.cpp waInit.cpp 
waIO.cpp waOrb.cpp waScOpLead.cpp waSCumul.cpp waScore.cpp waSemBidding.cpp waSemLead.cpp waSemSingle.cpp waShuffle.cpp /dds-develop/src/ABsearch.cpp
/dds-develop/src/ABstats.cpp /dds-develop/src/CalcTables.cpp /dds-develop/src/dds.cpp /dds-develop/src/DealerPar.cpp /dds-develop/src/dump.cpp
/dds-develop/src/File.cpp /dds-develop/src/Init.cpp /dds-develop/src/LaterTricks.cpp /dds-develop/src/Memory.cpp /dds-develop/src/Moves.cpp
/dds-develop/src/Par.cpp /dds-develop/src/PBN.cpp  /dds-develop/src/PlayAnalyser.cpp /dds-develop/src/QuickTricks.cpp /dds-develop/src/Scheduler.cpp
/dds-develop/src/SolveBoard.cpp /dds-develop/src/SolverIF.cpp /dds-develop/src/System.cpp /dds-develop/src/ThreadMgr.cpp  /dds-develop/src/Timer.cpp
/dds-develop/src/TimerGroup.cpp /dds-develop/src/TimerList.cpp  /dds-develop/src/TimeStat.cpp  /dds-develop/src/TimeStatList.cpp  /dds-develop/src/TransTableL.cpp
/dds-develop/src/TransTableS.cpp
/dds-develop/examples/hands.cpp
)
