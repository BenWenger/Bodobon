
bodo_driver = {
    ["system"]={"nes"},
    ["channels"]={"pulse1", "pulse2", "triangle"}
}

chanDutyOr = {
    ["pulse1"]= 1,
    ["pulse2"]= 0
}

bodo_playStartSong = function()
    for name,chan in pairs(bodo.channels) do
        chan.data.envCounter = 0
        chan.data.started = false
    end
end

bodo_getPitchRange = function()
    return 12*4, "rest"
end

bodo_getLength = function(tone)
    return bodo.host.lengthTable[tone.length]
end

applyEnvelope = function( chan, env, pos )
    v = bodo.host.envTables[env][pos]
    
    chan.setVolume( v & 0x0F )
    v = (v >> 4) & 3
    chan.setDuty( v | chanDutyOr[chan.name] )
end

bodo_playStartTone = function(tone, chan)
    if type(tone.pitch) == "number" then             -- play an actual tone
        chan.data.envCounter    = 0
        chan.data.started       = true
        local fVal = bodo.host.freqTable[tone.pitch]
        chan.setPitch(fVal, true)
        
        if chan.name == "triangle" then
            chan.setVolume( 1 )
        else
            applyEnvelope(chan, tone.env, 0)
        end
    else                                -- rest / sustain
        if chan.name == "triangle" then -- triangle is a rest
            chan.setVolume( 0 )
        end
    end 
end


bodo_playUpdateTone = function(tone, chan)
    -- Triangle doesn't need any update work
    if chan.name ~= "triangle" and chan.data.started then
        local e = chan.data.envCounter
        e = e + bodo.host.speedTable[ tone.envSpeed ]
        if e > 0xFF then e = 0xFF end
        
        chan.data.envCounter = e
        e = e >> 3
        
        applyEnvelope(chan, tone.env, e)
    end
end

bodo_getToneEditDetails = function(chan, song)
    return {
        ["env"] = {
            {0,16}
        },
        ["envSpeed"] = {
            {0,16}
        },
        ["pitch"] = {
            "rest",
            {0,12 * 4}
        },
        ["length"] = {
            {0,16 * 6}
        }
    }
end
