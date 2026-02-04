function contractResult(declarer, level, suit, doubled, play)
    if (play == nil) then
        play = ""
    end    
    if (doubled == nil) then
        doubled = 0
    end    
    local vulnarable = conf.NSvul
    if (declarer == E or declarer == W) then vulnarable = conf.EWvul end
    local made = tricks(declarer, suit, play)-6
    if (doubled == 0 and made == 7 and (level == 5 or ((suit == "S" or suit == "H") and level == 4)))
    then level = 6 end
    return scoreBoard(level, made, suit, vulnarable, doubled)
end

function scoreBoard(declared, made, suit, vulnarable, doubled)
    if made >= declared
    then
        return declaredValue(suit, declared, vulnarable, doubled) + (made - declared) * suitTrickValue(suit, vulnarable, doubled) 
    else
        return -penalty(declared - made, vulnarable, doubled)
    end
end

function suitTrickValue(suit, vulnarable, doubled)
    if doubled > 0 then return doubled * 100 * (vulnarable + 1) end
    if suit == "C" or suit == "D" then return 20 else return 30 end
end

function declaredValue(suit, level, vulnarable, doubled)
    local declaredTricksValue = 0;
    if suit == "NT" then declaredTricksValue = 10 end
    declaredTricksValue = declaredTricksValue  + suitTrickValue(suit, 1, 0) * level;
    declaredTricksValue = declaredTricksValue * (doubled + 1)
    local result = declaredTricksValue + doubled * 50
    if level == 7 then 
        if vulnarable == 1 then return result + 2000 else return result + 1300 end
    end
    if level == 6 then 
        if vulnarable == 1 then return result + 1250 else return result + 800 end
    end
    if declaredTricksValue >= 100 then 
        if vulnarable == 1 then return result + 500 else return result + 300 end
    end
    return result + 50;
end

function penalty(down, vulnarable, doubled)
    if doubled ~= 1 then return down * 50 * (vulnarable + 1) end
    if vulnarable == 1 then return down * 300 - 100 end
    if down <= 3 then return down * 200 - 100 else  return down * 300 - 400 end
end
