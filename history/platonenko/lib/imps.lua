function imps(totalPoints)
   return absImps(sign(totalPoints) * totalPoints) * sign(totalPoints)
end

function absImps(number)
    if (number  <  20) then return  0 end
    if (number <=  40) then return  1 end
    if (number <=  80) then return  2 end
    if (number <= 120) then return  3 end
    if (number <= 160) then return  4 end
    if (number <= 210) then return  5 end
    if (number <= 260) then return  6 end
    if (number <= 310) then return  7 end
    if (number <= 360) then return  8 end
    if (number <= 420) then return  9 end
    if (number <= 490) then return 10 end
    if (number <= 590) then return 11 end
    if (number <= 740) then return 12 end
    if (number <= 890) then return 13 end
    return 14
end
