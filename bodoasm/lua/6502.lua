--
--  Define the actual opcodes and business
--

mnemonicTable = { [0]=
--         x0    x1    x2    x3     x4    x5    x6    x7       x8    x9    xA    xB     xC    xD    xE    xF
--[[0x]] "brk","ora","---","---", "---","ora","asl","---",   "php","ora","asl","---", "---","ora","asl","---",  --[[0x]]
--[[1x]] "bpl","ora","---","---", "---","ora","asl","---",   "clc","ora","---","---", "---","ora","asl","---",  --[[1x]]
--[[2x]] "jsr","and","---","---", "bit","and","rol","---",   "plp","and","rol","---", "bit","and","rol","---",  --[[2x]]
--[[3x]] "bmi","and","---","---", "---","and","rol","---",   "sec","and","---","---", "---","and","rol","---",  --[[3x]]
--[[4x]] "rti","eor","---","---", "---","eor","lsr","---",   "pha","eor","lsr","---", "jmp","eor","lsr","---",  --[[4x]]
--[[5x]] "bvc","eor","---","---", "---","eor","lsr","---",   "cli","eor","---","---", "---","eor","lsr","---",  --[[5x]]
--[[6x]] "rts","adc","---","---", "---","adc","ror","---",   "pla","adc","ror","---", "jmp","adc","ror","---",  --[[6x]]
--[[7x]] "bvs","adc","---","---", "---","adc","ror","---",   "sei","adc","---","---", "---","adc","ror","---",  --[[7x]]
--         x0    x1    x2    x3     x4    x5    x6    x7       x8    x9    xA    xB     xC    xD    xE    xF
--[[8x]] "---","sta","---","---", "sty","sta","stx","---",   "dey","---","txa","---", "sty","sta","stx","---",  --[[8x]]
--[[9x]] "bcc","sta","---","---", "sty","sta","stx","---",   "tya","sta","txs","---", "---","sta","---","---",  --[[9x]]
--[[Ax]] "ldy","lda","ldx","---", "ldy","lda","ldx","---",   "tay","lda","tax","---", "ldy","lda","ldx","---",  --[[Ax]]
--[[Bx]] "bcs","lda","---","---", "ldy","lda","ldx","---",   "clv","lda","tsx","---", "ldy","lda","ldx","---",  --[[Bx]]
--[[Cx]] "cpy","cmp","---","---", "cpy","cmp","dec","---",   "iny","cmp","dex","---", "cpy","cmp","dec","---",  --[[Cx]]
--[[Dx]] "bne","cmp","---","---", "---","cmp","dec","---",   "cld","cmp","---","---", "---","cmp","dec","---",  --[[Dx]]
--[[Ex]] "cpx","sbc","---","---", "cpx","sbc","inc","---",   "inx","sbc","nop","---", "cpx","sbc","inc","---",  --[[Ex]]
--[[Fx]] "beq","sbc","---","---", "---","sbc","inc","---",   "sed","sbc","---","---", "---","sbc","inc","---"   --[[Fx]]
--         x0    x1    x2    x3     x4    x5    x6    x7       x8    x9    xA    xB     xC    xD    xE    xF
}

addrModeTable = { [0]=
--        x0   x1   x2   x3    x4   x5   x6   x7      x8   x9   xA   xB    xC   xD   xE   xF
--[[0x]] "im","ix","--","--", "--","zp","zp","--",   "ip","im","ac","--", "--","ab","ab","--",  --[[0x]]
--[[1x]] "rl","iy","--","--", "--","zx","zx","--",   "ip","ay","--","--", "--","ax","ax","--",  --[[1x]]
--[[2x]] "ab","ix","--","--", "zp","zp","zp","--",   "ip","im","ac","--", "ab","ab","ab","--",  --[[2x]]
--[[3x]] "rl","iy","--","--", "--","zx","zx","--",   "ip","ay","--","--", "--","ax","ax","--",  --[[3x]]
--[[4x]] "ip","ix","--","--", "--","zp","zp","--",   "ip","im","ac","--", "ab","ab","ab","--",  --[[4x]]
--[[5x]] "rl","iy","--","--", "--","zx","zx","--",   "ip","ay","--","--", "--","ax","ax","--",  --[[5x]]
--[[6x]] "ip","ix","--","--", "--","zp","zp","--",   "ip","im","ac","--", "in","ab","ab","--",  --[[6x]]
--[[7x]] "rl","iy","--","--", "--","zx","zx","--",   "ip","ay","--","--", "--","ax","ax","--",  --[[7x]]
--        x0   x1   x2   x3    x4   x5   x6   x7      x8   x9   xA   xB    xC   xD   xE   xF
--[[8x]] "--","ix","--","--", "zp","zp","zp","--",   "ip","--","ip","--", "ab","ab","ab","--",  --[[8x]]
--[[9x]] "rl","iy","--","--", "zx","zx","zy","--",   "ip","ay","ip","--", "--","ax","--","--",  --[[9x]]
--[[Ax]] "im","ix","im","--", "zp","zp","zp","--",   "ip","im","ip","--", "ab","ab","ab","--",  --[[Ax]]
--[[Bx]] "rl","iy","--","--", "zx","zx","zy","--",   "ip","ay","ip","--", "ax","ax","ay","--",  --[[Bx]]
--[[Cx]] "im","ix","--","--", "zp","zp","zp","--",   "ip","im","ip","--", "ab","ab","ab","--",  --[[Cx]]
--[[Dx]] "rl","iy","--","--", "--","zx","zx","--",   "ip","ay","--","--", "--","ax","ax","--",  --[[Dx]]
--[[Ex]] "im","ix","--","--", "zp","zp","zp","--",   "ip","im","ip","--", "ab","ab","ab","--",  --[[Ex]]
--[[Fx]] "rl","iy","--","--", "--","zx","zx","--",   "ip","ay","--","--", "--","ax","ax","--"   --[[Fx]]
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
