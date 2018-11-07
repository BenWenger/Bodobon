--
--  Define the actual opcodes and business
--

mnemonicTable = { [0]=
--         x0      x1      x2      x3       x4      x5      x6      x7        x8      x9      xA      xB       xC      xD      xE      xF
--[[0x]] "nop"  ,"tcall","set1" ,"bbs"  , "or"   ,"or"   ,"or"   ,"or"   ,  "or"   ,"or"   ,"or1"  ,"asl"  , "asl"  ,"push" ,"tset1","brk"  ,  --[[0x]]
--[[1x]] "bpl"  ,"tcall","clr1" ,"bbc"  , "or"   ,"or"   ,"or"   ,"or"   ,  "or"   ,"or"   ,"decw" ,"asl"  , "asl"  ,"dec"  ,"cmp"  ,"jmp"  ,  --[[1x]]
--[[2x]] "clrp" ,"tcall","set1" ,"bbs"  , "and"  ,"and"  ,"and"  ,"and"  ,  "and"  ,"and"  ,"or1"  ,"rol"  , "rol"  ,"push" ,"cbne" ,"bra"  ,  --[[2x]]
--[[3x]] "bmi"  ,"tcall","clr1" ,"bbc"  , "and"  ,"and"  ,"and"  ,"and"  ,  "and"  ,"and"  ,"incw" ,"rol"  , "rol"  ,"inc"  ,"cmp"  ,"call" ,  --[[3x]]
--[[4x]] "setp" ,"tcall","set1" ,"bbs"  , "eor"  ,"eor"  ,"eor"  ,"eor"  ,  "eor"  ,"eor"  ,"and1" ,"inc"  , "inc"  ,"push" ,"tclr1","pcall",  --[[4x]]
--[[5x]] "bvc"  ,"tcall","clr1" ,"bbc"  , "eor"  ,"eor"  ,"eor"  ,"eor"  ,  "eor"  ,"eor"  ,"cmpw" ,"inc"  , "inc"  ,"mov"  ,"cmp"  ,"jmp"  ,  --[[5x]]
--[[6x]] "clrc" ,"tcall","set1" ,"bbs"  , "cmp"  ,"cmp"  ,"cmp"  ,"cmp"  ,  "cmp"  ,"cmp"  ,"and1" ,"ror"  , "ror"  ,"push" ,"dbnz" ,"ret"  ,  --[[6x]]
--[[7x]] "bvs"  ,"tcall","clr1" ,"bbc"  , "cmp"  ,"cmp"  ,"cmp"  ,"cmp"  ,  "cmp"  ,"cmp"  ,"addw" ,"ror"  , "ror"  ,"mov"  ,"cmp"  ,"ret1" ,  --[[7x]]
--         x0      x1      x2      x3       x4      x5      x6      x7        x8      x9      xA      xB       xC      xD      xE      xF
--[[8x]] "setc" ,"tcall","set1" ,"bbs"  , "adc"  ,"adc"  ,"adc"  ,"adc"  ,  "adc"  ,"adc"  ,"eor1" ,"dec"  , "dec"  ,"mov"  ,"pop"  ,"mov"  ,  --[[8x]]
--[[9x]] "bcc"  ,"tcall","clr1" ,"bbc"  , "adc"  ,"adc"  ,"adc"  ,"adc"  ,  "adc"  ,"adc"  ,"subw" ,"dec"  , "dec"  ,"mov"  ,"div"  ,"xcn"  ,  --[[9x]]
--[[Ax]] "ei"   ,"tcall","set1" ,"bbs"  , "sbc"  ,"sbc"  ,"sbc"  ,"sbc"  ,  "sbc"  ,"sbc"  ,"mov1" ,"inc"  , "inc"  ,"cmp"  ,"pop"  ,"mov"  ,  --[[Ax]]
--[[Bx]] "bcs"  ,"tcall","clr1" ,"bbc"  , "sbc"  ,"sbc"  ,"sbc"  ,"sbc"  ,  "sbc"  ,"sbc"  ,"movw" ,"inc"  , "inc"  ,"mov"  ,"das"  ,"mov"  ,  --[[Bx]]
--[[Cx]] "di"   ,"tcall","set1" ,"bbs"  , "mov"  ,"mov"  ,"mov"  ,"mov"  ,  "cmp"  ,"mov"  ,"mov1" ,"mov"  , "mov"  ,"mov"  ,"pop"  ,"mul"  ,  --[[Cx]]
--[[Dx]] "bne"  ,"tcall","clr1" ,"bbc"  , "mov"  ,"mov"  ,"mov"  ,"mov"  ,  "mov"  ,"mov"  ,"movw" ,"mov"  , "dec"  ,"mov"  ,"cbne" ,"daa"  ,  --[[Dx]]
--[[Ex]] "clrv" ,"tcall","set1" ,"bbs"  , "mov"  ,"mov"  ,"mov"  ,"mov"  ,  "mov"  ,"mov"  ,"not1" ,"mov"  , "mov"  ,"notc" ,"pop"  ,"sleep",  --[[Ex]]
--[[Fx]] "beq"  ,"tcall","clr1" ,"bbc"  , "mov"  ,"mov"  ,"mov"  ,"mov"  ,  "mov"  ,"mov"  ,"mov"  ,"mov"  , "inc"  ,"mov"  ,"dbnz" ,"stop"    --[[Fx]]
--         x0      x1      x2      x3       x4      x5      x6      x7        x8      x9      xA      xB       xC      xD      xE      xF
}


-- TODO I stopped at DAA/DAS
-- First arg in addr mode
addrModeTable_1 = { [0]=
--        x0   x1   x2   x3    x4   x5   x6   x7      x8   x9   xA   xB    xC   xD   xE   xF
--[[0x]] "--","--","d*","d*", "--","--","--","--",   "--","--","--","dp", "ab","--","--","ip",  --[[0x]]
--[[1x]] "rl","--","d*","d*", "--","--","--","--",   "--","--","--","dx", "ac","--","xx","--",  --[[1x]]
--[[2x]] "ip","--","--","--", "ac","ac","ac","ac",   "ac","dp","--","--", "--","--","--","rl",  --[[2x]]
--[[3x]] "rl","--","--","--", "ac","ac","ac","ac",   "dp","xi","--","--", "--","--","xx","ab",  --[[3x]]
--[[4x]] "--","--","--","--", "--","--","--","--",   "--","--","cc","--", "--","--","--","--",  --[[4x]]
--[[5x]] "rl","--","--","--", "--","--","--","--",   "--","--","ya","--", "--","--","yy","--",  --[[5x]]
--[[6x]] "ip","--","--","--", "ac","ac","ac","ac",   "ac","dp","cc","--", "--","--","--","--",  --[[6x]]
--[[7x]] "rl","--","--","--", "ac","ac","ac","ac",   "dp","xi","ya","--", "--","--","yy","--",  --[[7x]]
--        x0   x1   x2   x3    x4   x5   x6   x7      x8   x9   xA   xB    xC   xD   xE   xF
--[[8x]] "--","--","--","--", "ac","ac","ac","ac",   "ac","dp","--","--", "--","--","--","--",  --[[8x]]
--[[9x]] "rl","--","--","--", "ac","ac","ac","ac",   "dp","xi","--","--", "--","--","--","--",  --[[9x]]
--[[Ax]] "--","--","--","--", "--","--","--","--",   "--","--","--","--", "--","yy","--","--",  --[[Ax]]
--[[Bx]] "rl","--","--","--", "--","--","--","--",   "--","--","--","--", "--","--","ac","--",  --[[Bx]]
--[[Cx]] "--","--","--","--", "--","--","--","--",   "xx","--","--","--", "--","--","--","--",  --[[Cx]]
--[[Dx]] "rl","--","--","--", "--","--","--","--",   "--","--","--","--", "--","--","--","ac",  --[[Dx]]
--[[Ex]] "ip","--","--","--", "--","--","--","--",   "--","--","--","--", "--","--","--","--",  --[[Ex]]
--[[Fx]] "rl","--","--","--", "--","--","--","--",   "--","--","--","--", "--","--","--","--"   --[[Fx]]
--        x0   x1   x2   x3    x4   x5   x6   x7      x8   x9   xA   xB    xC   xD   xE   xF
}

-- First arg in addr mode
addrModeTable_2 = { [0]=
--        x0   x1   x2   x3    x4   x5   x6   x7      x8   x9   xA   xB    xC   xD   xE   xF
--[[0x]] "--","--","--","--", "--","--","--","--",   "--","--","--","--", "--","--","--","--",  --[[0x]]
--[[1x]] "--","--","--","--", "--","--","--","--",   "--","--","--","--", "--","--","--","--",  --[[1x]]
--[[2x]] "--","--","--","--", "--","--","--","--",   "--","--","--","--", "--","--","--","--",  --[[2x]]
--[[3x]] "--","--","--","--", "--","--","--","--",   "--","--","--","--", "--","--","--","--",  --[[3x]]
--[[4x]] "--","--","--","--", "--","--","--","--",   "--","--","--","--", "--","--","--","--",  --[[4x]]
--[[5x]] "--","--","--","--", "--","--","--","--",   "--","--","--","--", "--","--","--","--",  --[[5x]]
--[[6x]] "--","--","--","--", "--","--","--","--",   "--","--","--","--", "--","--","--","--",  --[[6x]]
--[[7x]] "--","--","--","--", "--","--","--","--",   "--","--","--","--", "--","--","--","--",  --[[7x]]
--        x0   x1   x2   x3    x4   x5   x6   x7      x8   x9   xA   xB    xC   xD   xE   xF
--[[8x]] "--","--","--","--", "--","--","--","--",   "--","--","--","--", "--","--","--","--",  --[[8x]]
--[[9x]] "--","--","--","--", "--","--","--","--",   "--","--","--","--", "--","--","--","--",  --[[9x]]
--[[Ax]] "--","--","--","--", "--","--","--","--",   "--","--","--","--", "--","--","--","--",  --[[Ax]]
--[[Bx]] "--","--","--","--", "--","--","--","--",   "--","--","--","--", "--","--","--","--",  --[[Bx]]
--[[Cx]] "--","--","--","--", "--","--","--","--",   "--","--","--","--", "--","--","--","--",  --[[Cx]]
--[[Dx]] "--","--","--","--", "--","--","--","--",   "--","--","--","--", "--","--","--","--",  --[[Dx]]
--[[Ex]] "--","--","--","--", "--","--","--","--",   "--","--","--","--", "--","--","--","--",  --[[Ex]]
--[[Fx]] "--","--","--","--", "--","--","--","--",   "--","--","--","--", "--","--","--","--"   --[[Fx]]
--        x0   x1   x2   x3    x4   x5   x6   x7      x8   x9   xA   xB    xC   xD   xE   xF
}

suffixes = {
    [".b"]= {"zp","zx","zy"},
    [".w"]= {"ab","ax","ay"}
}

modePatterns = {
    ["ip"]= {},
    ["ac"]= {"a"},
    ["im"]= {"#", 1},
    ["zp"]= {1},
    ["zx"]= {1, ",", "x"},
    ["zy"]= {1, ",", "y"},
    ["ab"]= {1},
    ["ax"]= {1, ",", "x"},
    ["ay"]= {1, ",", "y"},
    ["in"]= {"(", 1, ")"},
    ["ix"]= {"(", 1, ",", "x", ")"},
    ["iy"]= {"(", 1, ")", ",", "y"},
    ["rl"]= {1}
}

modeSizes = {
    ["ip"]= 1,
    ["ac"]= 1,
    ["im"]= 2,
    ["zp"]= 2,
    ["zx"]= 2,
    ["zy"]= 2,
    ["ab"]= 3,
    ["ax"]= 3,
    ["ay"]= 3,
    ["in"]= 3,
    ["ix"]= 2,
    ["iy"]= 2,
    ["rl"]= 2
}


--
--  Transform the above tables into something that can be used by the assembler, and where
--     we can look up the opcode by mnemonic+addrmode rather than the other way around
--

opcodeLookup = {}           -- used to look up an opcode from a mnemonic/addrmode pair
asmMnemonics = {}           -- the mnemonic table to give to the assembler

addOpcodeConfig = function(opcode, mnm, adm)
    opcodeLookup[mnm..adm] = opcode
    
    if asmMnemonics[mnm] == nil then
        asmMnemonics[mnm] = {adm}
    else
        table.insert(asmMnemonics[mnm], adm)
    end
end

buildOpcodeTables = function()
    for opcode=0,255 do
        mnm = mnemonicTable[opcode]
        adm = addrModeTable[opcode]
        if mnm ~= "---" then
            addOpcodeConfig(opcode,mnm,adm)
            -- special case for 'ac' mode, we also want to use 'ip' mode because some
            --   people are lazy and don't want to type out the explicit 'A'
            if adm == "ac" then
                addOpcodeConfig(opcode,mnm,"ip")
            end
        end
    end
end

modes_priority = {"ip","ac","im","in","rl","ix","iy"}   -- use these modes if they're an option -- always
modes_short = {"zp","zx","zy"}
modes_long = {"ab","ax","ay"}

getBestMode = function(patterns)
    -- if there is a priority mode, use it
    for i,v in ipairs(modes_priority) do
        if patterns[v] then return v end
    end
    
    local short = nil
    -- otherwise, see if this can be a short instruction
    for i,v in ipairs(modes_short) do
        if patterns[v] then
            short = v
            -- use it unconditionally if the expression is evaluated now
            --   and it can fit in a byte
            if patterns[v][1] ~= nil and patterns[v][1] >= 0 and patterns[v][1] <= 0xFF then
                return v
            end
        end
    end
    
    -- otherwise, it probably matches a long one
    for i,v in ipairs(modes_long) do
        if patterns[v] then return v end
    end
        
    -- if there wasn't a long version, but there was a short one, use the short one.
    --   this can happen for STX/STY instructions, which have zero page indexed, but not
    --   absolute indexed
    return short
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
    local size = modeSizes[mode]
    local opcode = opcodeLookup[mnemonic .. mode]
    local val = patterns[mode][1]
    
    if mode == "im" and val < 0 then
        val = val + 256
    end
    
    if mode == "rl" then
        local pc = bodoasm.getPC()
        local out = val - (pc + 2)
        if out < -128 or out > 127 then
            error("Relative branch out of range")
        end
        out = (out + 128) ~ 0x80
        return opcode, out
    elseif size == 3 then
        if val < 0 or val > 0xFFFF then
            error("value out of range")
        end
        return opcode, val & 0xFF, (val >> 8) & 0xFF
    elseif size == 2 then
        if val < 0 or val > 0xFF then
            error("value out of range")
        end
        return opcode, val & 0xFF
    end
    
    return opcode
end
