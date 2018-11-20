
#include 'support.asm'
#org $8000, 0

one = $11
two = $2222

`hdr('ADC')
ADC #one
ADC one
ADC one, X
ADC two
ADC two, X
ADC two, Y
ADC (one, X)
ADC (one), Y

`hdr('AND')
AND #one
AND one
AND one, X
AND two
AND two, X
AND two, Y
AND (one, X)
AND (one), Y

`hdr('ASL')
ASL A
ASL
ASL one
ASL one, X
ASL two
ASL two, X

`hdr('Branches')
:
BPL :-
BMI :-
BVC :-
BVS :-
BCC :-
BCS :-
BNE :-
BEQ :-

`hdr('Flag')
CLC
CLD
CLI
CLV
SEC
SED
SEI

`hdr('CMP')
CMP #one
CMP one
CMP one, X
CMP two
CMP two, X
CMP two, Y
CMP (one, X)
CMP (one), Y

`hdr('CPX')
CPX #one
CPX one
CPX two

`hdr('CPY')
CPY #one
CPY one
CPY two

`hdr('DEC')
DEX
DEY
DEC one
DEC one, X
DEC two
DEC two, X

`hdr('EOR')
EOR #one
EOR one
EOR one, X
EOR two
EOR two, X
EOR two, Y
EOR (one, X)
EOR (one), Y

`hdr('INC')
INX
INY
INC one
INC one, X
INC two
INC two, X

`hdr('Misc')
BIT one
BIT two
BRK #one
JMP two
JMP (two)
JSR two
NOP
RTI
RTS
PHA
PHP
PLA
PLP

`hdr('LDA')
LDA #one
LDA one
LDA one, X
LDA two
LDA two, X
LDA two, Y
LDA (one, X)
LDA (one), Y

`hdr('LDX')
LDX #one
LDX one
LDX one, Y
LDX two
LDX two, Y

`hdr('LDY')
LDY #one
LDY one
LDY one, X
LDY two
LDY two, X

`hdr('LSR')
LSR A
LSR
LSR one
LSR one, X
LSR two
LSR two, X

`hdr('ORA')
ORA #one
ORA one
ORA one, X
ORA two
ORA two, X
ORA two, Y
ORA (one, X)
ORA (one), Y

`hdr('ROL')
ROL A
ROL
ROL one
ROL one, X
ROL two
ROL two, X

`hdr('ROR')
ROR A
ROR
ROR one
ROR one, X
ROR two
ROR two, X

`hdr('SBC')
SBC #one
SBC one
SBC one, X
SBC two
SBC two, X
SBC two, Y
SBC (one, X)
SBC (one), Y

`hdr('STA')
STA one
STA one, X
STA two
STA two, X
STA two, Y
STA (one, X)
STA (one), Y

`hdr('STX')
STX one
STX one, Y
STX two

`hdr('STY')
STY one
STY one, X
STY two

`hdr('Xfer')
TAX
TAY
TSX
TXA
TXS
TYA