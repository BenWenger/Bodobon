--
--  Define the actual opcodes and business
--

mnemonicTable = { [0]=
--         x0      x1      x2      x3       x4      x5      x6      x7        x8      x9      xA      xB       xC      xD      xE      xF
--[[0x]] "nop"  ,"tcall","set1" ,"bbs"  , "or"   ,"or"   ,"or"   ,"or"   ,  "or"   ,"or"   ,"or1"  ,"asl"  , "asl"  ,"push" ,"tset1","brk"  ,  --[[0x]]
--[[1x]] "bpl"  ,nil    ,"clr1" ,"bbc"  , "or"   ,"or"   ,"or"   ,"or"   ,  "or"   ,"or"   ,"decw" ,"asl"  , "asl"  ,"dec"  ,"cmp"  ,"jmp"  ,  --[[1x]]
--[[2x]] "clrp" ,nil    ,nil    ,nil    , "and"  ,"and"  ,"and"  ,"and"  ,  "and"  ,"and"  ,"or1"  ,"rol"  , "rol"  ,"push" ,"cbne" ,"bra"  ,  --[[2x]]
--[[3x]] "bmi"  ,nil    ,nil    ,nil    , "and"  ,"and"  ,"and"  ,"and"  ,  "and"  ,"and"  ,"incw" ,"rol"  , "rol"  ,"inc"  ,"cmp"  ,"call" ,  --[[3x]]
--[[4x]] "setp" ,nil    ,nil    ,nil    , "eor"  ,"eor"  ,"eor"  ,"eor"  ,  "eor"  ,"eor"  ,"and1" ,"inc"  , "inc"  ,"push" ,"tclr1","pcall",  --[[4x]]
--[[5x]] "bvc"  ,nil    ,nil    ,nil    , "eor"  ,"eor"  ,"eor"  ,"eor"  ,  "eor"  ,"eor"  ,"cmpw" ,"inc"  , "inc"  ,"mov"  ,"cmp"  ,"jmp"  ,  --[[5x]]
--[[6x]] "clrc" ,nil    ,nil    ,nil    , "cmp"  ,"cmp"  ,"cmp"  ,"cmp"  ,  "cmp"  ,"cmp"  ,"and1" ,"ror"  , "ror"  ,"push" ,"dbnz" ,"ret"  ,  --[[6x]]
--[[7x]] "bvs"  ,nil    ,nil    ,nil    , "cmp"  ,"cmp"  ,"cmp"  ,"cmp"  ,  "cmp"  ,"cmp"  ,"addw" ,"ror"  , "ror"  ,"mov"  ,"cmp"  ,"ret1" ,  --[[7x]]
--         x0      x1      x2      x3       x4      x5      x6      x7        x8      x9      xA      xB       xC      xD      xE      xF
--[[8x]] "setc" ,nil    ,nil    ,nil    , "adc"  ,"adc"  ,"adc"  ,"adc"  ,  "adc"  ,"adc"  ,"eor1" ,"dec"  , "dec"  ,"mov"  ,"pop"  ,"mov"  ,  --[[8x]]
--[[9x]] "bcc"  ,nil    ,nil    ,nil    , "adc"  ,"adc"  ,"adc"  ,"adc"  ,  "adc"  ,"adc"  ,"subw" ,"dec"  , "dec"  ,"mov"  ,"div"  ,"xcn"  ,  --[[9x]]
--[[Ax]] "ei"   ,nil    ,nil    ,nil    , "sbc"  ,"sbc"  ,"sbc"  ,"sbc"  ,  "sbc"  ,"sbc"  ,"mov1" ,"inc"  , "inc"  ,"cmp"  ,"pop"  ,"mov"  ,  --[[Ax]]
--[[Bx]] "bcs"  ,nil    ,nil    ,nil    , "sbc"  ,"sbc"  ,"sbc"  ,"sbc"  ,  "sbc"  ,"sbc"  ,"movw" ,"inc"  , "inc"  ,"mov"  ,"das"  ,"mov"  ,  --[[Bx]]
--[[Cx]] "di"   ,nil    ,nil    ,nil    , "mov"  ,"mov"  ,"mov"  ,"mov"  ,  "cmp"  ,"mov"  ,"mov1" ,"mov"  , "mov"  ,"mov"  ,"pop"  ,"mul"  ,  --[[Cx]]
--[[Dx]] "bne"  ,nil    ,nil    ,nil    , "mov"  ,"mov"  ,"mov"  ,"mov"  ,  "mov"  ,"mov"  ,"movw" ,"mov"  , "dec"  ,"mov"  ,"cbne" ,"daa"  ,  --[[Dx]]
--[[Ex]] "clrv" ,nil    ,nil    ,nil    , "mov"  ,"mov"  ,"mov"  ,"mov"  ,  "mov"  ,"mov"  ,"not1" ,"mov"  , "mov"  ,"notc" ,"pop"  ,"sleep",  --[[Ex]]
--[[Fx]] "beq"  ,nil    ,nil    ,nil    , "mov"  ,"mov"  ,"mov"  ,"mov"  ,  "mov"  ,"mov"  ,"mov"  ,"mov"  , "inc"  ,"mov"  ,"dbnz" ,"stop"    --[[Fx]]
--         x0      x1      x2      x3       x4      x5      x6      x7        x8      x9      xA      xB       xC      xD      xE      xF
}


-- First arg in addr mode
--   Note d* (as in:  "SET1 $10.3") is special because the value on the right of the dot determines
--     the opcode.  So only record the first opcode, and calculate the others based on that value
--
--   TCALL is also weird in that there is a different opcode for each value.  Same idea, only opcode
--   $01 is recorded here, and others will be calculated by value
addrModeTable_1 = { [0]=
--        x0   x1   x2   x3    x4   x5   x6   x7      x8   x9   xA   xB    xC   xD   xE   xF
--[[0x]] "ip","tc","d*","d*", "ac","ac","ac","ac",   "ac","dp","cc","dp", "ab","st","ab","ip",  --[[0x]]
--[[1x]] "rl",nil ,"d*","d*", "ac","ac","ac","ac",   "dp","xi","dp","dx", "ac","xx","xx","ij",  --[[1x]]
--[[2x]] "ip",nil ,nil ,nil , "ac","ac","ac","ac",   "ac","dp","cc","dp", "ab","ac","dp","rl",  --[[2x]]
--[[3x]] "rl",nil ,nil ,nil , "ac","ac","ac","ac",   "dp","xi","dp","dx", "ac","xx","xx","ab",  --[[3x]]
--[[4x]] "ip",nil ,nil ,nil , "ac","ac","ac","ac",   "ac","dp","cc","dp", "ab","xx","ab","dp",  --[[4x]]
--[[5x]] "rl",nil ,nil ,nil , "ac","ac","ac","ac",   "dp","xi","ya","dx", "ac","xx","yy","ab",  --[[5x]]
--[[6x]] "ip",nil ,nil ,nil , "ac","ac","ac","ac",   "ac","dp","cc","dp", "ab","yy","dp","ip",  --[[6x]]
--[[7x]] "rl",nil ,nil ,nil , "ac","ac","ac","ac",   "dp","xi","ya","dx", "ac","ac","yy","ip",  --[[7x]]
--        x0   x1   x2   x3    x4   x5   x6   x7      x8   x9   xA   xB    xC   xD   xE   xF
--[[8x]] "ip",nil ,nil ,nil , "ac","ac","ac","ac",   "ac","dp","cc","dp", "ab","yy","st","dp",  --[[8x]]
--[[9x]] "rl",nil ,nil ,nil , "ac","ac","ac","ac",   "dp","xi","ya","dx", "ac","xx","ya","ac",  --[[9x]]
--[[Ax]] "ip",nil ,nil ,nil , "ac","ac","ac","ac",   "ac","dp","cc","dp", "ab","yy","ac","xp",  --[[Ax]]
--[[Bx]] "rl",nil ,nil ,nil , "ac","ac","ac","ac",   "dp","xi","ya","dx", "ac","sp","ac","ac",  --[[Bx]]
--[[Cx]] "ip",nil ,nil ,nil , "dp","ab","xi","ix",   "xx","ab","mb","dp", "ab","xx","xx","ya",  --[[Cx]]
--[[Dx]] "rl",nil ,nil ,nil , "dx","ax","ay","iy",   "dp","dy","dp","dx", "yy","ac","dx","ac",  --[[Dx]]
--[[Ex]] "ip",nil ,nil ,nil , "ac","ac","ac","ac",   "ac","xx","mb","yy", "yy","ip","yy","ip",  --[[Ex]]
--[[Fx]] "rl",nil ,nil ,nil , "ac","ac","ac","ac",   "xx","xx","dp","yy", "yy","yy","yy","ip"   --[[Fx]]
--        x0   x1   x2   x3    x4   x5   x6   x7      x8   x9   xA   xB    xC   xD   xE   xF
}


-- Second arg in addr mode
addrModeTable_2 = { [0]=
--        x0   x1   x2   x3    x4   x5   x6   x7      x8   x9   xA   xB    xC   xD   xE   xF
--[[0x]] "--","--","--","rl", "dp","ab","xi","ix",   "im","dp","mb","--", "--","--","--","--",  --[[0x]]
--[[1x]] "--",nil ,"--","rl", "dx","ax","ay","iy",   "im","yi","--","--", "--","--","ab","--",  --[[1x]]
--[[2x]] "--",nil ,nil ,nil , "dp","ab","xi","ix",   "im","dp","nb","--", "--","--","rl","--",  --[[2x]]
--[[3x]] "--",nil ,nil ,nil , "dx","ax","ay","iy",   "im","yi","--","--", "--","--","dp","--",  --[[3x]]
--[[4x]] "--",nil ,nil ,nil , "dp","ab","xi","ix",   "im","dp","mb","--", "--","--","--","--",  --[[4x]]
--[[5x]] "--",nil ,nil ,nil , "dx","ax","ay","iy",   "im","yi","dp","--", "--","ac","ab","--",  --[[5x]]
--[[6x]] "--",nil ,nil ,nil , "dp","ab","xi","ix",   "im","dp","nb","--", "--","--","rl","--",  --[[6x]]
--[[7x]] "--",nil ,nil ,nil , "dx","ax","ay","iy",   "im","yi","dp","--", "--","xx","dp","--",  --[[7x]]
--        x0   x1   x2   x3    x4   x5   x6   x7      x8   x9   xA   xB    xC   xD   xE   xF
--[[8x]] "--",nil ,nil ,nil , "dp","ab","xi","ix",   "im","dp","mb","--", "--","im","--","im",  --[[8x]]
--[[9x]] "--",nil ,nil ,nil , "dx","ax","ay","iy",   "im","yi","dp","--", "--","sp","xx","--",  --[[9x]]
--[[Ax]] "--",nil ,nil ,nil , "dp","ab","xi","ix",   "im","dp","mb","--", "--","im","--","ac",  --[[Ax]]
--[[Bx]] "--",nil ,nil ,nil , "dx","ax","ay","iy",   "im","yi","dp","--", "--","xx","--","xp",  --[[Bx]]
--[[Cx]] "--",nil ,nil ,nil , "ac","ac","ac","ac",   "im","xx","cc","yy", "yy","im","--","--",  --[[Cx]]
--[[Dx]] "--",nil ,nil ,nil , "ac","ac","ac","ac",   "xx","xx","ya","yy", "--","yy","rl","--",  --[[Dx]]
--[[Ex]] "--",nil ,nil ,nil , "dp","ab","xi","ix",   "im","ab","--","dp", "ab","--","--","--",  --[[Ex]]
--[[Fx]] "--",nil ,nil ,nil , "dx","ax","ay","iy",   "dp","dy","dp","dx", "--","ac","rl","--"   --[[Fx]]
--        x0   x1   x2   x3    x4   x5   x6   x7      x8   x9   xA   xB    xC   xD   xE   xF
}

baseModePatterns = {
    ["ip"]= {},
    ["ac"]= {"a"},
    ["cc"]= {"c"},
    ["xx"]= {"x"},
    ["yy"]= {"y"},
    ["ya"]= {"ya"},
    ["sp"]= {"sp"},
    ["st"]= {"psw"},
    ["xi"]= {"(", "x", ")"},
    ["xp"]= {"(", "x", ")", "+"},
    ["yi"]= {"(", "y", ")"},
    ["im"]= {"#", 1},
    ["dp"]= {1},
    ["dx"]= {1, "+", "x"},
    ["dy"]= {1, "+", "y"},
    ["ab"]= {1},
    ["ax"]= {1, "+", "x"},
    ["ay"]= {1, "+", "y"},
    ["ix"]= {"[", 1, "+", "x", "]"},
    ["iy"]= {"[", 1, "]", "+", "y"},
    ["mb"]= {1, ".", 1},
    ["nb"]= {"/", 1, ".", 1},
    ["rl"]= {1},
    ["d*"]= {1, ".", 1},
    ["ij"]= {"[", 1, "+", "x", "]"},
    ["tc"]= {1}
}

baseModeSizes = {               -- does not include the opcode, or the other param
    ["ip"]= 0,
    ["ac"]= 0,
    ["cc"]= 0,
    ["xx"]= 0,
    ["yy"]= 0,
    ["ya"]= 0,
    ["sp"]= 0,
    ["st"]= 0,
    ["xi"]= 0,
    ["xp"]= 0,
    ["yi"]= 0,
    ["im"]= 1,
    ["dp"]= 1,
    ["dx"]= 1,
    ["dy"]= 1,
    ["ab"]= 2,
    ["ax"]= 2,
    ["ay"]= 2,
    ["ix"]= 1,
    ["iy"]= 1,
    ["mb"]= 2,
    ["nb"]= 2,
    ["rl"]= 1,
    ["d*"]= 1,
    ["ij"]= 2,
    ["tc"]= 0,
    ["--"]= 0
}

-- Support function
function tblConcat(t1,t2)
    for i=1,#t2 do
        t1[#t1+1] = t2[i]
    end
    return t1
end

--
--  Transform the above tables into something that can be used by the assembler, and where
--     we can look up the opcode by mnemonic+addrmode rather than the other way around
--

opcodeLookup = {}           -- used to look up an opcode from a mnemonic/addrmode pair
asmMnemonics = {}           -- the mnemonic table to give to the assembler
addrModePatterns = {}       -- the table that combines baseModePatterns with itself so we have 2 parameters

suffixes = {
    [".b"]= {},
    [".w"]= {}
}

modeSizes = {}              -- total size of an instruction with the given mode (both operands, plus opcode)

addOpcodeConfig = function(opcode, mnm, fst, snd)
    local adm = fst..snd
    
    -- add this addr mode to the suffix list
    if     fst == "dp" or fst == "dx" or fst == "dy" or snd == "dp" or snd == "dx" or snd == "dy" then
        suffixes[".b"][adm] = adm
    elseif fst == "ab" or fst == "ax" or fst == "ay" or snd == "ab" or snd == "ax" or snd == "ay" then
        suffixes[".w"][adm] = adm
    end
    
    -- Add it to opcode lookup table
    opcodeLookup[mnm..adm] = opcode
    
    -- Add it to the priority list
    addModePriority(fst,snd)
    
    -- add it to the mode sizes
    if modeSizes[adm] == nil then
        modeSizes[adm] = 1 + baseModeSizes[fst] + baseModeSizes[snd]
    end
    
    -- Add it to the mnemonic table for the assembler
    if asmMnemonics[mnm] == nil then
        asmMnemonics[mnm] = {adm}
    else
        table.insert(asmMnemonics[mnm], adm)
    end
    
    -- and add the pattern to the addrmode patterns for the assembler
    if addrModePatterns[adm] == nil then
        pattern = tblConcat( {}, baseModePatterns[fst] )
        if snd ~= "--" then
            pattern[#pattern + 1] = ","
            pattern = tblConcat( pattern, baseModePatterns[snd] )
        end
        addrModePatterns[adm] = pattern
    end
end

buildOpcodeTables = function()
    local impliedExceptions = { -- some instructions technically have a mode, but could
        ["div"]= true,          --   easily be considered implied mode.  So allow for implied
        ["mul"]= true,          --   mode with these
        ["das"]= true,
        ["daa"]= true,
        ["xcn"]= true
    }
    for opcode=0,255 do
        local mnm = mnemonicTable[opcode]
        if mnm ~= nil then
            local fst = addrModeTable_1[opcode]
            local snd = addrModeTable_2[opcode]
            addOpcodeConfig(opcode,mnm,fst,snd)
            if impliedExceptions[mnm] then
                addOpcodeConfig(opcode,mnm,"ip","--")
            end
        end
    end
end


--[[
Determining the "best" mode requires that modes be prioritized.

    - Modes that have keywords like "A", "(X)", "PSW", etc need to be preferred
        over any mode that contains an expression, so that they don't get mistaken
        for expressions
    - Likewise, modes that add X or Y (dp+X, ab+Y) have to be preferred above
        their non-indexed counterparts to prevent the +X/+Y from being part of the
        expression
    - Indirect modes should be preferred over non-indirect modes, so that the parens
        don't get included in the expression (though SPC uses brackets ... so that's
        kind of a non-issue here.  Whatever)
        
    SO... priority is:
        - Keyword modes         (best)
        - Indirect modes
        - Misc modes
        - Indexed modes
        - Absolute / DirectPage (worst)
        
    Misc modes, like tcall, m.b, rl, etc don't really matter because those will never
    conflict, so there is never a need to sort by priority.  But put them above Absolute/DP
    and indexed anyway just because.
        
    The mode with the LOWEST priority is the "best" matching mode when assembling
--]]


baseModePriorities = {
-- keyword
    ["ac"]= 0,
    ["cc"]= 0,
    ["xx"]= 0,
    ["yy"]= 0,
    ["ya"]= 0,
    ["sp"]= 0,
    ["st"]= 0,
    ["xi"]= 0,
    ["xp"]= 0,
    ["yi"]= 0,
-- indirect
    ["ix"]= 10,
    ["iy"]= 10,
    ["ij"]= 10,
-- misc
    ["mb"]= 100,
    ["nb"]= 100,
    ["im"]= 100,
    ["ip"]= 100,
    ["tc"]= 100,
    ["rl"]= 100,
    ["d*"]= 100,
    ["--"]= 100,
-- indexed
    ["ax"]= 1000,
    ["ay"]= 1000,
    ["dx"]= 1000,
    ["dy"]= 1000,
-- absolute
    ["ab"]= 10000,
    ["dp"]= 10000
}
modePriorities = {}
addModePriority = function(fst, snd)
    local md = fst..snd
    if modePriorities[md] == nil then
        modePriorities[md] = baseModePriorities[fst] + baseModePriorities[snd]
    end
end

getBestMode = function(patterns)
    -- The "best" mode is the one that has the lowest priority value.
    -- There could be a tie (Direct Page / Absolute) .. in which case, we want to prefer
    --    the "short" (direct page) option if possible... but fall back to the long
    --    option if not.
    local best = nil            -- the best option that isn't one of the below
    local bestShort = nil       -- the best option that is short  (dp/dx/dy)
    local bestLong  = nil       -- the best option that is longer (ab/ax/ay)
    
    local bestPrio = 99999
    local bestShortPrio = 99999
    local bestLongPrio = 99999
    
    for mode, _ in pairs(patterns) do
        local prio = modePriorities[mode]
        -- is it short?
        if suffixes[".b"][mode] ~= nil then
            if prio < bestShortPrio then
                bestShortPrio = prio
                bestShort = mode
            end
        elseif suffixes[".w"][mode] ~= nil then
            if prio < bestLongPrio then
                bestLongPrio = prio
                bestLong = mode
            end
        else
            if prio < bestPrio
                bestPrio = prio
                best = mode
            end
        end
    end
    
    -- if we had a 'best' mode, just use it
    if best ~= nil then
        return best
    end
    -- if we have no short version, use the long
    if bestShort == nil then
        return bestLong
    end
    -- if we have no long version, use the short
    if bestLong == nil then
        return bestShort
    end
    
    -- Only possible to reach here if:
    --    best == nil  and  bestShort ~= nil  and   bestLong ~= nil
    --  IE:  we have no "high priority" option, and we have both short/long options
    --    so we need to determine if we should use the short or the long version.
    
    --  Short versions usually have the "short" part as the first argument.. like:
    --      "dp"                (ex:    ASL dp      )
    --      "dp, <register>"    (ex:    MOV dp, A   )
    --  For these, the dp byte is the first argument in the arg list.
    --
    --  But some modes are in the below forms:
    --      "<register>, dp"    (ex:    MOV A, dp   )
    --      "<register>, dx"    (ex:    MOV A, dp+X )
    --      "dp, dp"            (ex:    MOV dst, src)
    --
    --  In the first two cases, the short argument is STILL the first argument, because registers
    --    do not have an argument.  And the "dp,dp" case is actually an impossibility to have
    --    at this point, because there is no "long" pattern that will also match the "dp,dp" 
    --    pattern.  Therefore... we know that the argument for this short mode is the first
    --    expression in the arg list
    local ex = pattern[bestShort][1]        -- get the expression
    if ex ~= nil then                       -- has it been evaluated?
        if ex >= 0x00 and ex <= 0xFF then   -- is it within a byte range?
            return bestShort                --  if yes, we can use the short version!
        end
    end
    
    return bestLong         -- otherwise, we can't use the short version ... use the long version
end

-------------------------

getModeBinary_immediate = function(output, args, size)
    -- immediate mode:  single byte output, negatives allowed
    local v = args[#args]
    table.remove(args)
    if v < 0 then v = v + 256 end
    if v < 0x00 or v > 0xFF then
        error("Value out of range for immediate mode")
    end
    output[#output+1] = v
    return output, args
end

getModeBinary_singleByte = function(output, args, size)
    -- Single byte modes, no negative allowed
    local v = args[#args]
    table.remove(args)
    if v < 0x00 or v > 0xFF then
        error("Value out of range")
    end
    output[#output+1] = v
    return output, args
end

getModeBinary_doubleByte = function(output, args, size)
    -- Double byte (absolute) modes, no negative allowed
    local v = args[#args]
    table.remove(args)
    if v < 0x00 or v > 0xFFFF then
        error("Value out of range")
    end
    output[#output+1] = v & 0xFF
    output[#output+1] = (v >> 8) & 0xFF
    return output, args
end

getModeBinary_mb = function(output, args, size)
    -- m.b and /m.b modes -- "m" must be between 0-1FFF and b must be 0-7
    --   output is in the form (b<<13)|m  (high 3 bits are b, low 13 bits are m)
    local b = args[#args]
    table.remove(args)
    local m = args[#args]
    table.remove(args)
    
    if b < 0 or b > 7 then
        error("Bit value for 'm.b' mode must be between 0-7")
    end
    if m < 0 or m > 0x1FFF then
        error("Address for 'm.b' mode must be between $0000-$1FFF")
    end
    output[#output+1] = m & 0xFF
    output[#output+1] = (m >> 8) | (v << 5)
    return output, args
end

getModeBinary_relative = function(output, args, size)
    local pc = bodoasm.getPC()
    local target = args[#args]
    table.remove(args)
    
    local dif = target - (pc + size)
    if dif < -128 or dif > 127 then
        error("Branch out of range")
    end
    
    output[#output+1] = (dif + 128) ^ 0x80
    return output, args
end

getModeBinary_tcall = function(output, args, size)
    -- tcall has an arg, but the arg just mangles the opcode rather than
    --   being a separate byte of output.  Only values 0-15 are valid
    local v = args[#args]
    table.remove(args)
    
    if v < 0 or v > 15 then
        error("TCALL operand out of range.  Only values 0-15 are valid")
    end
    
    output[1] = output[1] + (v * 0x10)
    return output, args
end

getModeBinary_deeStar = function(output, args, size)
    -- d* modes are effectively just direct page, but they have an additional
    --   argument that mangles the opcode to specify the bit
    local b = args[#args]
    table.remove(args)
    
    if b < 0 or b > 7 then
        error("Bit selection value is out of range.  Must be between 0-7")
    end
    
    -- b mangles the opcode
    output[1] = output[1] + (b * 0x20)
    
    -- the rest is just direct page
    return getModeBinary_singleByte(output, args, size)
end

getModeBinary_lut = {
    ["im"]= getModeBinary_immediate,
    ["dp"]= getModeBinary_singleByte,
    ["dx"]= getModeBinary_singleByte,
    ["dy"]= getModeBinary_singleByte,
    ["ix"]= getModeBinary_singleByte,
    ["iy"]= getModeBinary_singleByte,
    ["ab"]= getModeBinary_doubleByte,
    ["ax"]= getModeBinary_doubleByte,
    ["ay"]= getModeBinary_doubleByte,
    ["ij"]= getModeBinary_doubleByte,
    ["mb"]= getModeBinary_mb,
    ["nb"]= getModeBinary_mb,
    ["rl"]= getModeBinary_relative,
    ["tc"]= getModeBinary_tcall,
    ["d*"]= getModeBinary_deeStar
    --  other modes have no binary output
}

getModeBinary = function(md, output, args, size)
    local func = getModeBinary_lut[md]
    if func ~= nil then
        return func(output, args, size)
    end
    return output, args
end


-------------------------

bodoasm_init = function()
    buildOpcodeTables()
    return {
        ["Mnemonics"]=  asmMnemonics,
        ["AddrModes"]=  modePatterns,
        ["Suffixes"]=   suffixes
    }
end

bodoasm_guessSize = function(mnemonic, patterns)
    local mode = getBestMode(patterns)
    return modeSizes[mode], {mode}
end

bodoasm_getBinary = function(mnemonic, patterns)
    local mode = getBestMode(patterns)
    local fst = string.sub(mode,1,2)
    local snd = string.sub(mode,3,4)
    local size = modeSizes[mode]
    
    args = patterns[mode]
    output = { opcodeLookup[mnemonic..mode] }
    output, args = getModeBinary(snd, output, args, size)
    output, args = getModeBinary(fst, output, args, size)
    
    -- special case... if 'snd' is relative mode, reverse the argument bytes
    if snd == "rl" and #output == 3 then
        local tmp = output[2]
        output[2] = output[3]
        output[3] = tmp
    end
    
    return table.unpack(output)
end
