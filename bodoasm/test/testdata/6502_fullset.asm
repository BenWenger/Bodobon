
#org $8000, 0

one = $11
two = $2222

ADC #one
ADC one
ADC one, X
ADC two
ADC two, X
ADC two, Y
ADC (one, X)
ADC (one), Y

AND #one
AND one
AND one, X
AND two
AND two, X
AND two, Y
AND (one, X)
AND (one), Y

ASL A
ASL
ASL one
ASL one, X
ASL two
ASL two, X

:
BPL :-
BMI :-
BVC :-
BVS :-
BCC :-
BCS :-
BNE :-
BEQ :-

BIT one
BIT two

BRK #one

CLC
CLD
CLI
CLV
SEC
SED
SEI

CMP #one
CMP one
CMP one, X
CMP two
CMP two, X
CMP two, Y
CMP (one, X)
CMP (one), Y

CPX #one
CPX one
CPX two

CPY #one
CPY one
CPY two

DEX
DEY
DEC one
DEC one, X
DEC two
DEC two, X

EOR #one
EOR one
EOR one, X
EOR two
EOR two, X
EOR two, Y
EOR (one, X)
EOR (one), Y

INX
INY
INC one
INC one, X
INC two
INC two, X

JMP two
JMP (two)
JSR two
NOP
RTI
RTS

LDA #one
LDA one
LDA one, X
LDA two
LDA two, X
LDA two, Y
LDA (one, X)
LDA (one), Y

LDX #one
LDX one
LDX one, Y
LDX two
LDX two, Y

LDY #one
LDY one
LDY one, X
LDY two
LDY two, X

LSR A
LSR
LSR one
LSR one, X
LSR two
LSR two, X

ORA #one
ORA one
ORA one, X
ORA two
ORA two, X
ORA two, Y
ORA (one, X)
ORA (one), Y

PHA
PHP
PLA
PLP

ROL A
ROL
ROL one
ROL one, X
ROL two
ROL two, X

ROR A
ROR
ROR one
ROR one, X
ROR two
ROR two, X

SBC #one
SBC one
SBC one, X
SBC two
SBC two, X
SBC two, Y
SBC (one, X)
SBC (one), Y

STA one
STA one, X
STA two
STA two, X
STA two, Y
STA (one, X)
STA (one), Y

STX one
STX one, Y
STX two

STY one
STY one, X
STY two

TAX
TAY
TSX
TXA
TXS
TYA