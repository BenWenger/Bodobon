#org $8000, 0

ADC #$FF
ADC $FF
ADC $FF, X
ADC $AAAA
ADC $AAAA, X
ADC $AAAA, Y
ADC $AA, Y
ADC ($FF, X)
ADC ($FF), Y

AND #$FF
AND $FF
AND $FF, X
AND $AAAA
AND $AAAA, X
AND $AAAA, Y
AND ($FF, X)
AND ($FF), Y

CMP #$FF
CMP $FF
CMP $FF, X
CMP $AAAA
CMP $AAAA, X
CMP $AAAA, Y
CMP ($FF, X)
CMP ($FF), Y

EOR #$FF
EOR $FF
EOR $FF, X
EOR $AAAA
EOR $AAAA, X
EOR $AAAA, Y
EOR ($FF, X)
EOR ($FF), Y

LDA #$FF
LDA $FF
LDA $FF, X
LDA $AAAA
LDA $AAAA, X
LDA $AAAA, Y
LDA ($FF, X)
LDA ($FF), Y

ORA #$FF
ORA $FF
ORA $FF, X
ORA $AAAA
ORA $AAAA, X
ORA $AAAA, Y
ORA ($FF, X)
ORA ($FF), Y

SBC #$FF
SBC $FF
SBC $FF, X
SBC $AAAA
SBC $AAAA, X
SBC $AAAA, Y
SBC ($FF, X)
SBC ($FF), Y

STA $FF
STA $FF, X
STA $AAAA
STA $AAAA, X
STA $AAAA, Y
STA ($FF, X)
STA ($FF), Y

ASL A
ASL $FF
ASL $AAAA
ASL $FF, X
ASL $AAAA, X

LSR A
LSR $FF
LSR $AAAA
LSR $FF, X
LSR $AAAA, X

ROL A
ROL $FF
ROL $AAAA
ROL $FF, X
ROL $AAAA, X

ROR A
ROR $FF
ROR $AAAA
ROR $FF, X
ROR $AAAA, X

INX
INY
INC $FF
INC $AAAA
INC $FF, X
INC $AAAA, X

DEX
DEY
DEC $FF
DEC $AAAA
DEC $FF, X
DEC $AAAA, X

BPL foo
BMI foo
BNE foo
BEQ foo
BVC foo
BVS foo
BCC foo
BCS foo
        foo:

BIT $FF
BIT $AAAA

CLC
CLD
CLI
CLV
SEC
SED
SEI

NOP
PHA
PHP
PLA
PLP

CPX #$FF
CPX $FF
CPX $AAAA

CPY #$FF
CPY $FF
CPY $AAAA

JMP ($AAAA)
JMP $AAAA
JSR $AAAA

LDX #$FF
LDX $FF
LDX $FF,Y
LDX $AAAA
LDX $AAAA,Y

LDY #$FF
LDY $FF
LDY $FF, X
LDY $AAAA
LDY $AAAA , X

STX $FF
STX $FF,Y
STX $AAAA

STY $FF
STY $FF,X
STY $AAAA

RTI
RTS

TAX
TAY
TSX
TXA
TXS
TYA

BRK #$34



