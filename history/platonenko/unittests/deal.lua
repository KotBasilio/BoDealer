conf = {
   W="9.84.A8743.AJ543",    -- fix W hand
   num=1                  -- finish after accepting 100 deals
}

-- for each accepted deal, calculate number of tricks in NT by S:
function stats()
    note("5 diamonds doubled made should be 750 was ", scoreBoard(5, 5, "D", 1, 1))
    note("5 diamonds doubled +1 should be 950 was ", scoreBoard(5, 6, "D", 1, 1))
    note("5 diamonds doubled +2 should be 1150 was ", scoreBoard(5, 7, "D", 1, 1))

    note("5 spades doubled made should be 850 was ", scoreBoard(5, 5, "S", 1, 1))
    note("5 spades doubled +1 should be 1050 was ", scoreBoard(5, 6, "S", 1, 1))
    note("5 spades doubled +2 should be 1250 was ", scoreBoard(5, 7, "S", 1, 1))

    note("1 no trumps made should be 90 was ", scoreBoard(1, 1, "NT", 1, 0))
    note("1 no trumps +1 should be 120 was ", scoreBoard(1, 2, "NT", 1, 0))
    note("1 no trumps +2 should be 150 was ", scoreBoard(1, 3, "NT", 1, 0))
    note("1 no trumps +3 should be 180 was ", scoreBoard(1, 4, "NT", 1, 0))

    note("is intervantion AK872 ", interventionSuit(C.new("AK872...")))
    note("is intervantion A8732 ", interventionSuit(C.new("A8732...")))
    note("is intervantion Q8732 ", interventionSuit(C.new("Q8732...")))
    note("is intervantion J8732 ", interventionSuit(C.new("J8732...")))
    note("is intervantion J9732 ", interventionSuit(C.new("J9732...")))
local hand = C.new("7.Q654.KQ98.JT63")
    note("is intervantion ", isIntervention(hand, 8))

end


