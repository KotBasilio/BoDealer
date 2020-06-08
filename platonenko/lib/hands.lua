function passedHand(hand)
    return hand:hcp() < 11 and not ((blockDistribution(hand) or twoSuiter(hand)) and hand:hcp_in_range(4, 10))
end

function is4SpadeOpen(hand)
    return is4MOpen(hand:S(), hand:diamonds() > 3 or hand:clubs() > 3)
end

function is4HeartOpen(hand)
    return is4MOpen(hand:H(), hand:diamonds() > 3 or hand:clubs() > 3)
end

function is4MOpen(suit, has4cardMinor)
    return (suit:count() == 7 and (suit:points(1, 1, 1, 1) > 1 or (suit:points(1, 1, 1) > 0 and has4cardMinor))) or suit:count() > 7
end

function isBlock(hand)
    return  (blockDistribution(hand) or twoSuiter(hand)) and hand:hcp_in_range(4, 10)
end

function blockDistribution(hand)
    return (blockSuit(hand:S()) and hand:hearts() < 4)
        or (blockSuit(hand:H()) and hand:spades() < 4)
        or blockInMinor(hand)
end

function multi(hand)
    return (semiBlockSuit(hand:S()) and hand:hearts() < 4 and hand:diamonds() < 4 and hand:clubs() < 4)
        or (semiBlockSuit(hand:H()) and hand:spades() < 4 and hand:diamonds() < 4 and hand:clubs() < 4)
end

function is2SpadeOpen(hand)
    return semiBlockSuit(hand:S()) and hand:hearts() < 4
end

function is2HeartOpen(hand)
    return semiBlockSuit(hand:H()) and hand:sapdes() < 4
end

function semiBlockSuit(suit)
    return suit:count() == 6 and (suit:points(1, 1, 1) > 0 or suit:points(0, 0, 0, 1, 1) == 2)
end

function blockSuit(suit)
    return (suit:count() > 5 and (suit:points(1, 1, 1) > 0 or suit:points(0, 0, 0, 1, 1) == 2)) or suit:count() > 7
end

function blockInMinor(hand)
    return (blockSuit(hand:D()) or blockSuit(hand:C())) and noFourCardMajors(hand)
end

function noFourCardMajors(hand)
    return hand:spades() < 4 or hand:hearts() < 4
end

function noFiveCardMajors(hand)
    return hand:spades() < 5 or hand:hearts() < 5
end

function twoSuiter(hand)
    return (hand:spades() > 4 or hand:hearts() > 4) and C.pattern(hand, function(a, b, c, d) return a == 5 and b == 5; end)
end

function oneClubNat(hand)
    return hand:hcp_in_range(11, 21) and oneClubDistribution(hand) and not(oneNoTrump(hand) or twoNoTrump(hand))
end

function oneDiamondNat(hand)
    return hand:hcp_in_range(11, 21) and oneDiamondDistribution(hand) and not(oneNoTrump(hand) or twoNoTrump(hand))
end

function oneHeartNat(hand)
    return hand:hcp_in_range(11, 21) and oneHeartDistribution(hand) and not(oneNoTrump(hand) or twoNoTrump(hand))
end

function oneSpadeNat(hand)
    return hand:hcp_in_range(11, 21) and oneSpadeDistribution(hand) and not(oneNoTrump(hand) or twoNoTrump(hand))
end

function oneClubDistribution(hand)
    return not(oneSpadeDistribution(hand)) and not(oneHeartDistribution(hand)) and not(oneDiamondDistribution(hand))
end

function oneDiamondDistribution(hand)
    return not(oneSpadeDistribution(hand)) and not(oneHeartDistribution(hand)) and hand:diamonds() >= hand:clubs()
end

function oneHeartDistribution(hand)
    return hand:hearts() > 4 and not(oneSpadeDistribution(hand))
end

function oneSpadeDistribution(hand)
    return hand:spades() > 4
end

function oneNoTrump(hand)
    return hand:balanced() and hand:hcp_in_range(15, 17)
end

function twoNoTrump(hand)
    return hand:balanced() and hand:hcp_in_range(20, 21)
end

function oneTwoDiamondNat(hand)
    return oneDiamondDistribution(hand) and hand:hcp_in_range(11, 15) and hand:diamonds() > 5 and hand:diamonds() < 8 
       and hand:clubs() < 5 and not(hand:clubs() == 4 and hand:hcp_in_range(14, 15))
end

function oneTwoClubNat(hand)
    return oneClubDistribution(hand) and hand:hcp_in_range(11, 15) and hand:clubs() > 5 and hand:clubs() < 8 
end

function oneSpadeOverOneMinor(hand)
    return oneSpadeOverOneMinorDistribution(hand) and (hand:hcp() > 5 or hand:points(1, 0, 0, 0) == 1 or (hand:points(0, 1, 0, 0) == 1 and hand:hcp() == 5))
end

function oneSpadeOverOneMinorDistribution(hand)
    return hand:spades() > 4 or (hand:spades() == 4 and hand:spades() > hand:hearts())
end

function interventionSuit(suit)
    return suit:count() > 4 and (suit:points(1, 1, 1) > 0 or (suit:points(0, 0, 0, 1) == 1 and suit:points(0, 0, 0, 0, 1, 1) > 0))
end

function solidSuit(suit)
    return suit:points(1, 1, 1, 1) > 2
end

function isIntervention(hand, hcp)
    return (hand:hcp() >= hcp and (interventionSuit(hand:S()) or interventionSuit(hand:H()) or interventionSuit(hand:D())))
    or solidSuit(hand:S()) or solidSuit(hand:H()) or solidSuit(hand:D()) or solidSuit(hand:C())
end

function ThreeNoTrumpOverOne(hand)
   return (hand:spades() < 4 and hand:hearts() < 4 and hand:balanced() and hand:hcp_in_range(9, 14))
   or (hand:spades() < 4 and hand:hearts() < 4 and (hand:clubs() >= 5 or hand:diamonds() >= 5) and E:hcp_in_range(6, 12))
   or ((hand:spades() == 4 and hand:hearts() == 3) or (hand:spades() == 3 and hand:hearts() == 4) and hand:clubs() == 3 and hand:diamonds() == 3 and hand:hcp_in_range(9, 14))
end