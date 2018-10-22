--
--  Define the actual opcodes and business
--

mnemonicTable = { [0]=
--         x0    x1    x2    x3     x4    x5    x6    x7       x8    x9    xA    xB     xC    xD    xE    xF
--[[0x]] "BRK","ORA","---","---", "---","ORA","ASL","---",   "PHP","ORA","ASL","---", "---","ORA","ASL","---",  --[[0x]]
--[[1x]] "BPL","ORA","---","---", "---","ORA","ASL","---",   "CLC","ORA","---","---", "---","ORA","ASL","---",  --[[1x]]
--[[2x]] "JSR","AND","---","---", "BIT","AND","ROL","---",   "PLP","AND","ROL","---", "BIT","AND","ROL","---",  --[[2x]]
--[[3x]] "BMI","AND","---","---", "---","AND","ROL","---",   "SEC","AND","---","---", "---","AND","ROL","---",  --[[3x]]
--[[4x]] "RTI","EOR","---","---", "---","EOR","LSR","---",   "PHA","EOR","LSR","---", "JMP","EOR","LSR","---",  --[[4x]]
--[[5x]] "BVC","EOR","---","---", "---","EOR","LSR","---",   "CLI","EOR","---","---", "---","EOR","LSR","---",  --[[5x]]
--[[6x]] "RTS","ADC","---","---", "---","ADC","ROR","---",   "PLA","ADC","ROR","---", "JMP","ADC","ROR","---",  --[[6x]]
--[[7x]] "BVS","ADC","---","---", "---","ADC","ROR","---",   "SEI","ADC","---","---", "---","ADC","ROR","---",  --[[7x]]
--         x0    x1    x2    x3     x4    x5    x6    x7       x8    x9    xA    xB     xC    xD    xE    xF
--[[8x]] "---","STA","---","---", "STY","STA","STX","---",   "DEY","---","TXA","---", "STY","STA","STX","---",  --[[8x]]
--[[9x]] "BCC","STA","---","---", "STY","STA","STX","---",   "TYA","STA","TXS","---", "---","STA","---","---",  --[[9x]]
--[[Ax]] "LDY","LDA","LDX","---", "LDY","LDA","LDX","---",   "TAY","LDA","TAX","---", "LDY","LDA","LDX","---",  --[[Ax]]
--[[Bx]] "BCS","LDA","---","---", "LDY","LDA","LDX","---",   "CLV","LDA","TSX","---", "LDY","LDA","LDX","---",  --[[Bx]]
--[[Cx]] "CPY","CMP","---","---", "CPY","CMP","DEC","---",   "INY","CMP","DEX","---", "CPY","CMP","DEC","---",  --[[Cx]]
--[[Dx]] "BNE","CMP","---","---", "---","CMP","DEC","---",   "CLD","CMP","---","---", "---","CMP","DEC","---",  --[[Dx]]
--[[Ex]] "CPX","SBC","---","---", "CPX","SBC","INC","---",   "INX","SBC","NOP","---", "CPX","SBC","INC","---",  --[[Ex]]
--[[Fx]] "BEQ","SBC","---","---", "---","SBC","INC","---",   "SED","SBC","---","---", "---","SBC","INC","---"   --[[Fx]]
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
    ["ac"]= {"A"},
    ["im"]= {"#", 1},
    ["zp"]= {1},
    ["zx"]= {1, ",", "X"},
    ["zy"]= {1, ",", "Y"},
    ["ab"]= {1},
    ["ax"]= {1, ",", "X"},
    ["ay"]= {1, ",", "Y"},
    ["in"]= {"(", 1, ")"},
    ["ix"]= {"(", 1, ",", "X", ")"},
    ["iy"]= {"(", 1, ")", ",", "Y"},
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
    
    if asmMnemonics[mnm] == nil
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
            if adm == "ac"
                addOpcodeConfig(opcode,mnm,"ip")
            end
        end
    end
end

-------------------------

bodo_init = function()
    buildOpcodeTables()
    bodo.setMnemonics(asmMnemonics)
    bobo.setOperandPatterns(operandPatterns)
    bodo.addMnemonicSuffixes(suffixes)
end

bodo_approxSize = function(mnemonic, patterns)
    -- default to larger (word size) versions if they work
    if patterns["ab"] or patterns["ax"] or patterns["ay"] or patterns["in"]
        return 2
    end
    return 1
end

