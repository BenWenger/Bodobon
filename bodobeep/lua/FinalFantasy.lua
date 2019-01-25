
bodo_driver = {
    ["system"]={"nes"},
    ["channels"]={"pulse1", "pulse2", "triangle"}
}
    



chanDuties = {
    [0] = 1,
    [1] = 0
}

--[[
    chanStatus has:
        - envCounter        (current envelope counter)
        ... and I guess that's all I need?
]]--
chanNames = {"pulse1", "pulse2", "triangle"}
chanStatus = {}
for k,v in pairs(chanNames) do
    chanStatus[v] = {}
end

bodo_startPlayback = function()
    for k,v in chanNames do
        chanStatus[v].envCounter = 0
    end
end


bodo_getLength = function(tone)
    return bodo.host.lengthTable[tone.length]
end

bodo_startTone = function(chanObj, chanId, tone)    
    if tone.pitch >= 0 then             -- play an actual tone
        local fVal = bodo.host.freqTable[tone.pitch]
        chanObj.setPitch(fVal)
        
        if chanId == "Triangle" then
            chanObj.setVolume( 1 )
        else
            chanStatus[chanId].envCounter = 0
            chanObj.setVolume( bodo.host.envTables[tone.env][0] )
            chanObj.setDuty( chanDuties[chanId] )
        end
    else                                -- rest / sustain
        if chanId == "Triangle" then    -- triangle is a rest
            chanObj.setVolume( 0 )
        end
    end 
end


bodo_updateTone = function(chanObj, chanId, tone)
    -- Triangle doesn't need any update work
    if chanId ~= "Triangle" then
        local e = chanStatus[chanId].envCounter
        e = e + bodo.host.speedTable[ tone.envSpeed ]
        if e > 255 then e = 255 end
        
        chanStatus[chanId].envCounter = e
        e = e >> 3
        
        chanObj.setVolume( bodo.host.envTables[ tone.env ][ e ] )
    end
end