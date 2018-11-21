
#include 'support.asm'
#org $008000, 0

one = $11
two = $2222
long = $333333

`hdr('ADC')
ADC (one,X)
ADC one, S
ADC one
ADC [one]
ADC #one
ADC two
ADC long
ADC (one),Y
ADC (one)
ADC (one,S),Y
ADC one,X
ADC [one],Y
ADC two,Y
ADC two,X
ADC long,X

`hdr('AND')
AND (one,X)
AND one, S
AND one
AND [one]
AND #one
AND two
AND long
AND (one),Y
AND (one)
AND (one,S),Y
AND one,X
AND [one],Y
AND two,Y
AND two,X
AND long,X

`hdr('ASL / LSR')
ASL one
ASL A
ASL two
ASL one, X
ASL two, X
LSR one
LSR A
LSR two
LSR one, X
LSR two, X

`hdr('Branch')
:
BPL :- ; 10
BMI :- ; 30
BVC :- ; 50
BVS :- ; 70
BCC :- ; 90
BCS :- ; B0
BNE :- ; D0
BEQ :- ; F0
BRA :- ; 80
BRL :- ; 82

`hdr('BIT')
BIT one
BIT two
BIT one,X
BIT two,X
BIT #one

`hdr('Flag')
REP #0
SEP #0
CLC
CLD
CLI
CLV
SEC
SED
SEI

`hdr('Misc')
BRK #one
COP #one
MVN $34, $12
MVP $34, $12
NOP
PEA #two
PEI (one)
PER two
RTI
RTL
RTS
TRB one
TRB two
TSB one
TSB two
WAI
XBA
XCE

`hdr('CMP')
CMP (one,X)
CMP one, S
CMP one
CMP [one]
CMP #one
CMP two
CMP long
CMP (one),Y
CMP (one)
CMP (one,S),Y
CMP one,X
CMP [one],Y
CMP two,Y
CMP two,X
CMP long,X

`hdr('CPX / CPY')
CPX #one
CPX one
CPX two
CPY #one
CPY one
CPY two

`hdr('DEC')
DEC A
DEC one
DEC two
DEC one,X
DEC two,X
DEX
DEY

`hdr('EOR')
EOR (one,X)
EOR one, S
EOR one
EOR [one]
EOR #one
EOR two
EOR long
EOR (one),Y
EOR (one)
EOR (one,S),Y
EOR one,X
EOR [one],Y
EOR two,Y
EOR two,X
EOR long,X

`hdr('INC')
INC A
INC one
INC two
INC one,X
INC two,X
INX
INY

`hdr('JMP / JSR')
JMP two
JML long
JMP (two)
JMP (two,X)
JML [two]
JSR two
JSL long
JSR (two,X)

`hdr('LDA')
LDA (one,X)
LDA one, S
LDA one
LDA [one]
LDA #one
LDA two
LDA long
LDA (one),Y
LDA (one)
LDA (one,S),Y
LDA one,X
LDA [one],Y
LDA two,Y
LDA two,X
LDA long,X

`hdr('LDX')
LDX #one
LDX one
LDX two
LDX one,Y
LDX two,Y

`hdr('LDY')
LDY #one
LDY one
LDY two
LDY one,X
LDY two,X

`hdr('ORA')
ORA (one,X)
ORA one, S
ORA one
ORA [one]
ORA #one
ORA two
ORA long
ORA (one),Y
ORA (one)
ORA (one,S),Y
ORA one,X
ORA [one],Y
ORA two,Y
ORA two,X
ORA long,X

`hdr('Stack')
PHA
PHB
PHD
PHK
PHP
PHX
PHY
PLA
PLB
PLD
PLP
PLX
PLY

`hdr('ROL / ROR')
ROL one
ROL A
ROL two
ROL one, X
ROL two, X
ROR one
ROR A
ROR two
ROR one, X
ROR two, X

`hdr('SBC')
SBC (one,X)
SBC one, S
SBC one
SBC [one]
SBC #one
SBC two
SBC long
SBC (one),Y
SBC (one)
SBC (one,S),Y
SBC one,X
SBC [one],Y
SBC two,Y
SBC two,X
SBC long,X

`hdr('STA')
STA (one,X)
STA one, S
STA one
STA [one]
STA two
STA long
STA (one),Y
STA (one)
STA (one,S),Y
STA one,X
STA [one],Y
STA two,Y
STA two,X
STA long,X

`hdr('STX / STY')
STX one
STX two
STX one,Y
STY one
STY two
STY one,X

`hdr('STZ')
STZ one
STZ one,X
STZ two
STZ two,X

`hdr('Xfer')
TAX
TAY
TCD
TCS
TDC
TSC
TSX
TXA
TXS
TXY
TYA
TYX
