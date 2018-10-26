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
--[[Ax]] "ldy","lda","ldx","---", "ldy","lda","ldx","---",   "tay","lda","tas","---", "ldy","lda","ldx","---",  --[[Ax]]
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
--[[Fx]] "rl","iy","--","--", "--","zx","zx","--",   "ip","ay","--","--", "--","ax","ax","--",  --[[Fx]]
--        x0   x1   x2   x3    x4   x5   x6   x7      x8   x9   xA   xB    xC   xD   xE   xF
}

suffixes = {
    [".b"]= {"zp","zx","zy"},
    [".w"]= {"ab","ax","ay"}
}

operandPatterns = {
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

-------------------------

bodoasm_init = function()
    buildOpcodeTables()
    return {
        ["Mnemonics"]=  asmMnemonics,
        ["AddrModes"]=  operandPatterns,
        ["Suffixes"]=   suffixes
    }
end

bodoasm_approxSize = function(mnemonic, patterns)
    -- default to larger (word size) versions if they work
    if patterns["ab"] or patterns["ax"] or patterns["ay"] or patterns["in"] then
        return 2
    end
    return 1
end

