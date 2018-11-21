

mnemonicTable = { [0]=
--         x0    x1    x2    x3     x4    x5    x6    x7       x8    x9    xA    xB     xC    xD    xE    xF
--[[0x]] "brk","ora","cop","ora", "tsb","ora","asl","ora",   "php","ora","asl","phd", "tsb","ora","asl","ora",  --[[0x]]
--[[1x]] "bpl","ora","ora","ora", "trb","ora","asl","ora",   "clc","ora","inc","tcs", "trb","ora","asl","ora",  --[[1x]]
--[[2x]] "jsr","and","jsl","and", "bit","and","rol","and",   "plp","and","rol","pld", "bit","and","rol","and",  --[[2x]]
--[[3x]] "bmi","and","and","and", "bit","and","rol","and",   "sec","and","dec","tsc", "bit","and","rol","and",  --[[3x]]
--[[4x]] "rti","eor",nil  ,"eor", "mvp","eor","lsr","eor",   "pha","eor","lsr","phk", "jmp","eor","lsr","eor",  --[[4x]]
--[[5x]] "bvc","eor","eor","eor", "mvn","eor","lsr","eor",   "cli","eor","phy","tcd", "jml","eor","lsr","eor",  --[[5x]]
--[[6x]] "rts","adc","per","adc", "stz","adc","ror","adc",   "pla","adc","ror","rtl", "jmp","adc","ror","adc",  --[[6x]]
--[[7x]] "bvs","adc","adc","adc", "stz","adc","ror","adc",   "sei","adc","ply","tdc", "jmp","adc","ror","adc",  --[[7x]]
--         x0    x1    x2    x3     x4    x5    x6    x7       x8    x9    xA    xB     xC    xD    xE    xF
--[[8x]] "bra","sta","brl","sta", "sty","sta","stx","sta",   "dey","bit","txa","phb", "sty","sta","stx","sta",  --[[8x]]
--[[9x]] "bcc","sta","sta","sta", "sty","sta","stx","sta",   "tya","sta","txs","txy", "stz","sta","stz","sta",  --[[9x]]
--[[Ax]] "ldy","lda","ldx","lda", "ldy","lda","ldx","lda",   "tay","lda","tax","plb", "ldy","lda","ldx","lda",  --[[Ax]]
--[[Bx]] "bcs","lda","lda","lda", "ldy","lda","ldx","lda",   "clv","lda","tsx","tyx", "ldy","lda","ldx","lda",  --[[Bx]]
--[[Cx]] "cpy","cmp","rep","cmp", "cpy","cmp","dec","cmp",   "iny","cmp","dex","wai", "cpy","cmp","dec","cmp",  --[[Cx]]
--[[Dx]] "bne","cmp","cmp","cmp", "pei","cmp","dec","cmp",   "cld","cmp","phx","stp", "jml","cmp","dec","cmp",  --[[Dx]]
--[[Ex]] "cpx","sbc","sep","sbc", "cpx","sbc","inc","sbc",   "inx","sbc","nop","xba", "cpx","sbc","inc","sbc",  --[[Ex]]
--[[Fx]] "beq","sbc","sbc","sbc", "pea","sbc","inc","sbc",   "sed","sbc","plx","xce", "jsr","sbc","inc","sbc"   --[[Fx]]
--         x0    x1    x2    x3     x4    x5    x6    x7       x8    x9    xA    xB     xC    xD    xE    xF
}

addrModes = { [0]=
--         x0    x1    x2    x3     x4    x5    x6    x7       x8    x9    xA    xB     xC    xD    xE    xF
--[[0x]] "im8","ix" ,"im8","sr" , "dp" ,"dp" ,"dp" ,"inl",   "ip" ,"imm","ac" ,"ip" , "ab" ,"ab" ,"ab" ,"abl",  --[[0x]]
--[[1x]] "rl" ,"iy" ,"in" ,"siy", "dp" ,"dx" ,"dx" ,"iyl",   "ip" ,"ay" ,"ac" ,"ip" , "ab" ,"ax" ,"ax" ,"axl",  --[[1x]]
--[[2x]] "ab" ,"ix" ,"abl","sr" , "dp" ,"dp" ,"dp" ,"inl",   "ip" ,"imm","ac" ,"ip" , "ab" ,"ab" ,"ab" ,"abl",  --[[2x]]
--[[3x]] "rl" ,"iy" ,"in" ,"siy", "dx" ,"dx" ,"dx" ,"iyl",   "ip" ,"ay" ,"ac" ,"ip" , "ax" ,"ax" ,"ax" ,"axl",  --[[3x]]
--[[4x]] "ip" ,"ix" ,nil  ,"sr" , "mv" ,"dp" ,"dp" ,"inl",   "ip" ,"imm","ac" ,"ip" , "ab" ,"ab" ,"ab" ,"abl",  --[[4x]]
--[[5x]] "rl" ,"iy" ,"in" ,"siy", "mv" ,"dx" ,"dx" ,"iyl",   "ip" ,"ay" ,"ip" ,"ip" , "abl","ax" ,"ax" ,"axl",  --[[5x]]
--[[6x]] "ip" ,"ix" ,"per","sr" , "dp" ,"dp" ,"dp" ,"inl",   "ip" ,"imm","ac" ,"ip" , "jin","ab" ,"ab" ,"abl",  --[[6x]]
--[[7x]] "rl" ,"iy" ,"in" ,"siy", "dx" ,"dx" ,"dx" ,"iyl",   "ip" ,"ay" ,"ip" ,"ip" , "jix","ax" ,"ax" ,"axl",  --[[7x]]
--         x0    x1    x2    x3     x4    x5    x6    x7       x8    x9    xA    xB     xC    xD    xE    xF
--[[8x]] "rl" ,"ix" ,"rll","sr" , "dp" ,"dp" ,"dp" ,"inl",   "ip" ,"imm","ip" ,"ip" , "ab" ,"ab" ,"ab" ,"abl",  --[[8x]]
--[[9x]] "rl" ,"iy" ,"in" ,"siy", "dx" ,"dx" ,"dy" ,"iyl",   "ip" ,"ay" ,"ip" ,"ip" , "ab" ,"ax" ,"ax" ,"axl",  --[[9x]]
--[[Ax]] "imx","ix" ,"imx","sr" , "dp" ,"dp" ,"dp" ,"inl",   "ip" ,"imm","ip" ,"ip" , "ab" ,"ab" ,"ab" ,"abl",  --[[Ax]]
--[[Bx]] "rl" ,"iy" ,"in" ,"siy", "dx" ,"dx" ,"dy" ,"iyl",   "ip" ,"ay" ,"ip" ,"ip" , "ax" ,"ax" ,"ay" ,"axl",  --[[Bx]]
--[[Cx]] "imx","ix" ,"im8","sr" , "dp" ,"dp" ,"dp" ,"inl",   "ip" ,"imm","ip" ,"ip" , "ab" ,"ab" ,"ab" ,"abl",  --[[Cx]]
--[[Dx]] "rl" ,"iy" ,"in" ,"siy", "in" ,"dx" ,"dx" ,"iyl",   "ip" ,"ay" ,"ip" ,"ip" , "jil","ax" ,"ax" ,"axl",  --[[Dx]]
--[[Ex]] "imx","ix" ,"im8","sr" , "dp" ,"dp" ,"dp" ,"inl",   "ip" ,"imm","ip" ,"ip" , "ab" ,"ab" ,"ab" ,"abl",  --[[Ex]]
--[[Fx]] "rl" ,"iy" ,"in" ,"siy", "i16","dx" ,"dx" ,"iyl",   "ip" ,"ay" ,"ip" ,"ip" , "jix","ax" ,"ax" ,"axl"   --[[Fx]]
--         x0    x1    x2    x3     x4    x5    x6    x7       x8    x9    xA    xB     xC    xD    xE    xF
}

patterns = {
["ab"]=     {1},
["abl"]=    {1},
["ac"]=     {"a"},
["ax"]=     {1, ",", "x"},
["axl"]=    {1, ",", "x"},
["ay"]=     {1, ",", "y"},
["dp"]=     {1},
["dx"]=     {1, ",", "x"},
["dy"]=     {1, ",", "y"},
["i16"]=    {"#", 1},
["im8"]=    {"#", 1},
["imm"]=    {"#", 1},
["imx"]=    {"#", 1},
["in"]=     {"(", 1, ")"},
["inl"]=    {"[", 1, "]"},
["ip"]=     {},
["ix"]=     {"(", 1, ",", "x", ")"},
["iy"]=     {"(", 1, ")", ",", "y"},
["iyl"]=    {"[", 1, "]", ",", "y"},
["jil"]=    {"[", 1, "]"},
["jin"]=    {"(", 1, ")"},
["jix"]=    {"(", 1, ",", "x", ")"},
["mv"]=     {1, ",", 1},
["rl"]=     {1},
["rll"]=    {1},
["siy"]=    {"(", 1, ",", "s", ")", ",", "y"},
["sr"]=     {1, ",", "s"},
["per"]=    {1}
}

suffixes = {
[".b"]=     {"dp" ,"dx" ,"dy" ,"im8"},
[".w"]=     {"ab" ,"ax" ,"ay" ,"i16"},
[".l"]=     {"abl","axl"}
}

modeSizes = {
["ab"]=     3,
["abl"]=    4,
["ac"]=     1,
["ax"]=     3,
["axl"]=    4,
["ay"]=     3,
["dp"]=     2,
["dx"]=     2,
["dy"]=     2,
["i16"]=    3,
["im8"]=    2,
["imm"]=    0,
["imx"]=    0,
["in"]=     2,
["inl"]=    2,
["ip"]=     1,
["ix"]=     2,
["iy"]=     2,
["iyl"]=    2,
["jil"]=    3,
["jin"]=    3,
["jix"]=    3,
["mv"]=     3,
["rl"]=     2,
["rll"]=    3,
["siy"]=    2,
["sr"]=     2,
["per"]=    3
}

--
-- Stuff that is built computationally
--
opcodeLookup = {}
mnemonics = {}

addOp = function(opcode, mn, md)
    -- add to opcodeLookup
    opcodeLookup[mn..md] = opcode
    
    -- add to mnemonics list
    if mnemonics[mn] == nil then
        mnemonics[mn] = {md}
    else
        table.insert(mnemonics[mn], md)
    end
end

buildInstructions = function()
    for opcode=0,255 do
        local mn = mnemonicTable[opcode]
        if mn ~= nil then
            local md = addrModes[opcode]
            addOp(opcode, mn, md)
            
            -- immediate M, and X modes need to also allow 8/16 bit immediate modes
            --   since it could be either one of those
            if md == "imm" or md == "imx" then
                addOp(opcode, mn, "im8")
                addOp(opcode, mn, "i16")
            elseif md == "ac" then          -- ac mode could also be implied, if the user is lazy
                addOp(opcode, mn, "ip")
            end
        end
    end
end

-------------------------
-------------------------
-------------------------
stackDP = {}
stackDB = {}
stackM = {}
stackX = {}

pushStat = function(stk, name, v)
    stk[#stk+1] = v
    bodoasm.set(name, v)
end
popStat = function(stk, name)
    if #stk == 1 then error("#pop"..name.." used when "..name.." stack is empty") end
    table.remove(stk)
    bodoasm.set(name,stk[#stk])
end
setStat = function(stk, name, v)
    stk[#stk] = v
    bodoasm.set(name, v)
end

v816 = function(v)
    if v == 8 or v == 16 then return v end
    error("M/X Register sizes must be either 8 or 16")
end

bound = function(v, mx, name)
    if v >= 0 and v <= mx then return v end
    error("Value for "..name.." must be between 0 and $" .. string.format("%X", mx))
end

pushDP = function(v)    pushStat(stackDP, "DP", bound(v,0xFFFF,"DP"))   end
popDP =  function()     popStat (stackDP, "DP")                         end
setDP =  function(v)    setStat (stackDP, "DP", bound(v,0xFFFF,"DP"))   end
pushDB = function(v)    pushStat(stackDB, "DB", bound(v,0xFF,"DB"))     end
popDB =  function()     popStat (stackDB, "DB")                         end
setDB =  function(v)    setStat (stackDB, "DB", bound(v,0xFF,"DB"))     end
pushM =  function(v)    pushStat(stackM,  "M",  v816(v))    end
popM =   function()     popStat (stackM,  "M" )             end
setM =   function(v)    setStat (stackM,  "M",  v816(v))    end
pushX =  function(v)    pushStat(stackX,  "X",  v816(v))    end
popX =   function()     popStat (stackX,  "X" )             end
setX =   function(v)    setStat (stackX,  "X",  v816(v))    end

-- TODO figure a way to add the directives to the assembler

-------------------------
-------------------------
-------------------------

-- Mode groupings
--   First group, "priority" group.  Always use these if they're an option
modeGroupPriority = {
    ["ac" ]=true,   ["imm"]=true,   ["imx"]=true,   ["in" ]=true,   ["inl"]=true,   ["jin"]=true,
    ["jix"]=true,   ["ix" ]=true,   ["iy" ]=true,   ["iyl"]=true,   ["ip" ]=true,   ["siy"]=true,
    ["sr" ]=true,   ["mv" ]=true,   ["rl" ]=true,   ["rll"]=true,   ["jil"]=true,   ["per"]=true
}
--   Other groups need size distinctions
modeGroupShort = {
    ["dp" ]=true,   ["dx" ]=true,   ["dy" ]=true,   ["im8"]=true
}
modeGroupMedium = {
    ["ab" ]=true,   ["ax" ]=true,   ["ay" ]=true,   ["i16"]=true
}
modeGroupLong = {
    ["abl"]=true,   ["axl"]=true
}

getBestMode = function(patterns)
    local short = nil
    local med = nil
    local long = nil
    for mode, _ in pairs(patterns) do
        if modeGroupPriority[mode] then
            -- if this is imx or imm... we need to use the size of M/X to determine the size of this op
            if mode == "imm" then
                if bodoasm.get("M") == 8 then return "im8" else return "i16"    end
            elseif mode == "imx" then
                if bodoasm.get("X") == 8 then return "im8" else return "i16"    end
            else
                return mode
            end
        elseif modeGroupShort[mode] then
            short = mode
        elseif modeGroupMedium[mode] then
            med = mode
        else
            long = mode
        end
    end
    -- if we only have one option, use it
    if      short == nil and long == nil then return med
    elseif  med   == nil and long == nil then return short
    elseif  short == nil and med  == nil then return long
    end
    
    -- otherwise, we don't know the size, and we have to figure it out.
    --   do we have an operand?
    mode = short or med or long
    local v = patterns[mode][1]
    if v == nil then
        -- no operand... we just have to pick one
        -- Currently we do Absolute > Long > Short
        ---   but should we do Long > Absolute > Short???
        -- TODO maybe make this configurable
        if med ~= nil then return med end       -- Absolute is most likely
        if long ~= nil then return long end     -- Then use long
        return short                            -- otherwise, use short
    end
    
    --  Use 'v' to figure out which mode to use
    local possible = nil
    if short ~= nil then
        possible = short
        -- can we use DP mode here?
        if short == "im8" then      -- immediate mode is pretty easy
            if v >= -255 and v <= 255 then return short end
        else
            if convertToDpAddr(v) ~= nil then return short end
        end
    end
    if med ~= nil then
        possible = med
        if med == "i16" then
            if v >= -65535 and v <= 65535 then return med end
        else
            if convertToAbsAddr(v) ~= nil then return med end
        end
    end
    
    if long ~= nil then return long end
    return possible
end

convertToDpAddr = function(v)
    if v < 0 or v > 0xFFFF then return nil end
    local dp = bodoasm.get("DP")
    local tmp = v - dp
    if tmp >= 0 and tmp <= 0xFF then return tmp end
    tmp = v + 0x10000 - dp
    if tmp >= 0 and tmp <= 0xFF then return tmp end
    return nil
end

convertToAbsAddr = function(v)
    if v < 0 or v > 0xFFFFFF then return nil end
    local db = bodoasm.get("DB")
    if (v >> 16) == db then return v & 0xFFFF end
    return nil
end

-------------------------
-------------------------
-------------------------
getBin_ip = function(opcode)
    return opcode
end

getBin_ab = function(opcode, v)
    local tmp = convertToAbsAddr(v)
    if tmp == nil then error("Absolute address out of range") end
    return opcode, tmp & 0xFF, (tmp>>8) & 0xFF
end

getBin_long = function(opcode, v)
    if v < 0 or v > 0xFFFFFF then error("Long address out of range") end
    return opcode, v & 0xFF, (v>>8) & 0xFF, v>>16
end

getBin_dp = function(opcode, v)
    local tmp = convertToDpAddr(v)
    if tmp == nil then error("Direct page address out of range") end
    return opcode, tmp
end

getBin_rl = function(opcode, v)
    local pc = bodoasm.getPC()
    pc = (pc & 0xFF0000) | ((pc + 2) & 0xFFFF)
    if (pc & 0xFF0000) ~= (v & 0xFF0000) then error("Branch cannot cross bank boundary") end
    local dif = v - pc
    if dif < -128 or dif > 127 then error("Branch out of range") end
    return opcode, (dif + 128) ~ 0x80
end

getBin_rll = function(opcode, v)
    local pc = bodoasm.getPC()
    pc = (pc & 0xFF0000) | ((pc + 3) & 0xFFFF)
    if (pc & 0xFF0000) ~= (v & 0xFF0000) then error("Branch cannot cross bank boundary") end
    local dif = v - pc
    if dif < 0 then dif = dif + 0x10000 end
    return opcode, (dif & 0xFF), (dif >> 8) & 0xFF
end

getBin_sr = function(opcode, v)
    if v < 0 or v > 0xFF then error("Stack relative value out of range") end
    return opcode, v
end

getBin_im8 = function(opcode, v)
    if v < 0 then v = v + 0x100 end
    if v < 0 or v > 0xFF then error("Immediate value out of range") end
    return opcode, v
end

getBin_im16 = function(opcode, v)
    if v < 0 then v = v + 0x10000 end
    if v < 0 or v > 0xFFFF then error("Immediate value out of range") end
    return opcode, (v & 0xFF), v >> 8
end

getBin_per = function(opcode, v)
    if v < 0 or v > 0xFFFF then error("PER value out of range") end
    return opcode, (v & 0xFF), v >> 8
end

getBin_jix = function(opcode, v)
    local pc = bodoasm.getPC() + 3
    if (pc & 0xFF0000) ~= (v & 0xFF0000) then error("Indirect,X Jump pointer must be in same bank as the PC") end
    return opcode, (v & 0xFF), (v >> 8) & 0xFF
end

getBin_mv = function(opcode, src, dst)
    if (src < 0) or (src > 0xFF) then error("Source bank byte is out of range") end
    if (dst < 0) or (dst > 0xFF) then error("Destination bank byte is out of range") end
    return opcode, dst, src
end


getBinClbk = {
["ac"]=     getBin_ip,
["ip"]=     getBin_ip,
["ab"]=     getBin_ab,
["ax"]=     getBin_ab,
["ay"]=     getBin_ab,
["jil"]=    getBin_ab,
["jin"]=    getBin_ab,
["abl"]=    getBin_long,
["axl"]=    getBin_long,
["dp"]=     getBin_dp,
["dx"]=     getBin_dp,
["dy"]=     getBin_dp,
["inl"]=    getBin_dp,
["in"]=     getBin_dp,
["ix"]=     getBin_dp,
["iy"]=     getBin_dp,
["iyl"]=    getBin_dp,
["rl"]=     getBin_rl,
["rll"]=    getBin_rll,
["siy"]=    getBin_sr,
["sr"]=     getBin_sr,
["im8"]=    getBin_im8,
["i16"]=    getBin_im16,
["jix"]=    getBin_jix,
["mv"]=     getBin_mv,
["per"]=    getBin_per
}

-------------------------
-------------------------
-------------------------

bodoasm_init = function()
    buildInstructions()
    pushDP(0)
    pushDB(0)
    pushM(8)
    pushX(8)
    
    return {
        ["Mnemonics"]=  mnemonics,
        ["AddrModes"]=  patterns,
        ["Suffixes"]=   suffixes
    }
end

bodoasm_guessSize = function(mnemonic, patterns)
    local mode = getBestMode(patterns)
    return modeSizes[mode], {mode}
end

bodoasm_getBinary = function(mnemonic, patterns)
    local mode = getBestMode(patterns)
    local opcode = opcodeLookup[mnemonic..mode]
    local pat = patterns[mode]
    
    return getBinClbk[mode](opcode, pat[1], pat[2])
end

