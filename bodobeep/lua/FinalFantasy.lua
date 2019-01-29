
bodo_driver = {
    ["system"]={"nes"},
    ["channels"]={"pulse1", "pulse2", "triangle"}
}

bodo_startPlay = function()
    for name,chan in pairs(bodo.channels) do
        chan.data.envCounter = 0
        chan.data.started = false
    end
    bodo.channels.pulse1.setDuty(1)
    bodo.channels.pulse2.setDuty(0)
end


bodo_getLength = function(tone)
    return bodo.host.lengthTable[tone.length]
end

bodo_startTone = function(tone, chan)
    if tone.pitch >= 0 then             -- play an actual tone
        chan.data.envCounter    = 0
        chan.data.started       = true
        local fVal = bodo.host.freqTable[tone.pitch]
        chan.setPitch(fVal, true)
        
        if chan.name == "triangle" then
            chan.setVolume( 1 )
        else
            chan.setVolume( bodo.host.envTables[tone.env][0] )
        end
    else                                -- rest / sustain
        if chan.name == "triangle" then -- triangle is a rest
            chan.setVolume( 0 )
        end
    end 
end


bodo_updateTone = function(tone, chan)
    -- Triangle doesn't need any update work
    if chan.name ~= "triangle" and chan.data.started then
        local e = chan.data.envCounter
        e = e + bodo.host.speedTable[ tone.envSpeed ]
        if e > 255 then e = 255 end
        
        chan.data.envCounter = e
        e = e >> 3
        
        chan.setVolume( bodo.host.envTables[ tone.env ][ e ] )
    end
end
