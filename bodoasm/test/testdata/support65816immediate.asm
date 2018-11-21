
`hdr('> Always 8')
REP #0
SEP #0
BRK #0
COP #0
NOP

`hdr('> Always 16')
PEA #0
NOP

`hdr('> M flag')
ADC #0      ; 69
AND #0      ; 29
BIT #0      ; 89
CMP #0      ; C9
EOR #0      ; 49
LDA #0      ; A9
ORA #0      ; 09
SBC #0      ; E9
NOP

`hdr('> X flag')
LDX #0      ; A2
LDY #0      ; A0
CPX #0      ; E0
CPY #0      ; C0
NOP