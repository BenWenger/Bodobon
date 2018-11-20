
#include 'support.asm'
#org $8000, 0

one = $11
fst = $34
snd = $12
two = $2222
mm = $1412
bb = 1

`hdr('ADC / ADDW')
ADC (X), (Y)
ADC A, #one
ADC A, (X)
ADC A, [one]+Y
ADC A, [one+X]
ADC A, one
ADC A, one+X
ADC A, two
ADC A, two+X
ADC A, two+Y
ADC fst, snd
ADC fst, #snd
ADDW YA, one

`hdr('AND / AND1')
AND (X), (Y)
AND A, #one
AND A, (X)
AND A, [one]+Y
AND A, [one+X]
AND A, one
AND A, one+X
AND A, two
AND A, two+X
AND A, two+Y
AND fst, snd
AND fst, #snd
AND1 C, /mm.bb
AND1 C, mm.bb

`hdr('ASL')
ASL A
ASL one
ASL one+X
ASL two

`hdr('BBC')
:
BBC one.0, :-
BBC one.1, :-
BBC one.2, :-
BBC one.3, :-
BBC one.4, :-
BBC one.5, :-
BBC one.6, :-
BBC one.7, :-

`hdr('BBS')
:
BBS one.0, :-
BBS one.1, :-
BBS one.2, :-
BBS one.3, :-
BBS one.4, :-
BBS one.5, :-
BBS one.6, :-
BBS one.7, :-

`hdr('Branch')
:
BCC :-
BCS :-
BEQ :-
BMI :-
BNE :-
BPL :-
BVC :-
BVS :-
BRA :-

`hdr('BRK/CALL/CBNE')
:
BRK
CALL two
CBNE one+X, :-
CBNE one, :-


`hdr('CLR1 / SET1')
CLR1 one.0
CLR1 one.1
CLR1 one.2
CLR1 one.3
CLR1 one.4
CLR1 one.5
CLR1 one.6
CLR1 one.7
SET1 one.0
SET1 one.1
SET1 one.2
SET1 one.3
SET1 one.4
SET1 one.5
SET1 one.6
SET1 one.7

`hdr('Flags')
CLRC
CLRP
CLRV
DI
EI
NOTC
SETC
SETP

`hdr('CMP')
CMP (X), (Y)
CMP A, #one
CMP A, (X)
CMP A, [one]+Y
CMP A, [one+X]
CMP A, one
CMP A, one+X
CMP A, two
CMP A, two+X
CMP A, two+Y
CMP X, #one
CMP X, one
CMP X, two
CMP Y, #one
CMP Y, one
CMP Y, two
CMP fst, snd
CMP fst, #snd
CMPW YA, one

`hdr('Misc')
:
DBNZ Y, :-
DBNZ one, :-
DAA A
DAS A
DIV YA, X
JMP [two+X]
JMP two
MUL
NOP
NOT1 mm.bb
PCALL $FF00 | one
RET
RET1
SLEEP
STOP
TCLR1 two
TSET1 two
XCN A

`hdr('DEC')
DEC A
DEC X
DEC Y
DEC one
DEC one+X
DEC two
DECW one

`hdr('EOR')
EOR (X), (Y)
EOR A, #one
EOR A, (X)
EOR A, [one]+Y
EOR A, [one+X]
EOR A, one
EOR A, one+X
EOR A, two
EOR A, two+X
EOR A, two+Y
EOR fst, snd
EOR fst, #snd
EOR1 C, mm.bb

`hdr('INC')
INC A
INC X
INC Y
INC one
INC one+X
INC two
INCW one

`hdr('LSR')
LSR A
LSR one
LSR one+X
LSR two

`hdr('MOV')
MOV (X)+, A
MOV (X), A
MOV [one]+Y, A
MOV [one+X], A
MOV A, #one
MOV A, (X)
MOV A, (X)+
MOV A, [one]+Y
MOV A, [one+X]
MOV A, X
MOV A, Y
MOV A, one
MOV A, one+X
MOV A, two
MOV A, two+X
MOV A, two+Y
MOV SP, X
MOV X, #one
MOV X, A
MOV X, SP
MOV X, one
MOV X, one+Y
MOV X, two
MOV Y, #one
MOV Y, A
MOV Y, one
MOV Y, one+X
MOV Y, two
MOV fst, snd
MOV one+X, A
MOV one+X, Y
MOV one+Y, X
MOV fst, #snd
MOV one, A
MOV one, X
MOV one, Y
MOV two+X, A
MOV two+Y, A
MOV two, A
MOV two, X
MOV two, Y
MOV1 C, mm.bb
MOV1 mm.bb, C
MOVW YA, one
MOVW one, YA

`hdr('OR / OR1')
OR (X), (Y)
OR A, #one
OR A, (X)
OR A, [one]+Y
OR A, [one+X]
OR A, one
OR A, one+X
OR A, two
OR A, two+X
OR A, two+Y
OR fst, snd
OR fst, #snd
OR1 C, /mm.bb
OR1 C, mm.bb

`hdr('Stack')
POP A
POP PSW
POP X
POP Y
PUSH A
PUSH PSW
PUSH X
PUSH Y

`hdr('ROL / ROR')
ROL A
ROL one
ROL one+X
ROL two
ROR A
ROR one
ROR one+X
ROR two

`hdr('SBC')
SBC (X), (Y)
SBC A, #one
SBC A, (X)
SBC A, [one]+Y
SBC A, [one+X]
SBC A, one
SBC A, one+X
SBC A, two
SBC A, two+X
SBC A, two+Y
SBC fst, snd
SBC fst, #snd
SUBW YA, one

`hdr('TCALL')
TCALL 0
TCALL 1
TCALL 2
TCALL 3
TCALL 4
TCALL 5
TCALL 6
TCALL 7
TCALL 8
TCALL 9
TCALL 10
TCALL 11
TCALL 12
TCALL 13
TCALL 14
TCALL 15