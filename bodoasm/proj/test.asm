
#org $008000, 0

#setm 8
#automx 0
LDA #0          ;8
REP #$20
LDA #0          ; still 8
LDA #future      ; also 8

NOP


future = 0