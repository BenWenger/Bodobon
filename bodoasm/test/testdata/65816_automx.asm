
#org $008000, 0

#automx 1
SEP #$30        ; M=8
LDA #0          ; 8
REP #$20        ; M=16
LDA #0          ; 16
SEP #$20        ; M=8
LDA #future     ; 8
REP #$20
LDA #future

NOP

#automx 0
#setm 8
LDA #0          ;8
REP #$20
LDA #0          ; still 8
LDA #future      ; also 8

NOP


future = 0