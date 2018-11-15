
#org $1000, 0, $1000, $00

dp = $11
ab = $2222
mb = $0FFF

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