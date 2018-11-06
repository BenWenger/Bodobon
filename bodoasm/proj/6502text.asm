#org $8000, 0

#macro foo
    LDA #1
#endmacro

Top:
    `foo
    
    NOP