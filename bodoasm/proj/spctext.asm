
#org $1000, 0, $1000, $00

dp = $11
ab = $2222
mb = $0FFF
pcl = $FF62

butt:

ADC (X), (Y)
ADC A, #dp
ADC A, (X)
ADC A, [dp]+Y
ADC A, [dp+X]
ADC A, dp
ADC A, dp+X
ADC A, ab
ADC A, ab+X
ADC A, ab+Y
ADC dp+1, dp
ADC dp+1, #dp

ADDW YA, dp

AND1 C, /mb.2
AND1 C, mb.3

BPL :+
:


#byte 0,0,0,0,0,0,0,0

loop:

    BBC   dp.6, loop
    BBS   dp.0, loop
    CBNE  dp+X, loop
    CLR1  dp.1
    TCALL 0
    TCALL 5
    TCALL 15
    XCN   A
    XCN
    MOV   SP, X
    MOV   X, SP
    PUSH  PSW
    POP   X
    
#byte 0,0,0,0,0,0,0,0
    MOV   (X)+, A
    MOV   (X), A
    JMP   [:+ +X]
    JMP   :+
    EI
:   DI
    DIV   YA, X
    DIV
    PCALL pcl
    