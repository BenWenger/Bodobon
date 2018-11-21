
#include 'support.asm'
#org $008000, 0


`hdr('>> M=8, X=8')
#include 'support65816immediate.asm'

`hdr('>> M=8, X=16')
#setX 16
#include 'support65816immediate.asm'

`hdr('>> M=16, X=16')
#setM 16
#include 'support65816immediate.asm'

`hdr('>> M=16, X=8')
#setX 8
#include 'support65816immediate.asm'
