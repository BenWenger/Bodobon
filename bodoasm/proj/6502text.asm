#org $8000, 0

#macro foo(x)
    LDA #1
    x
#endmacro

#macro bar
    LDA ($FF\,X\)
#endmacro

; The below outputs:
;   LDA #1
;   LDA ($FF,X)
; fuck yeah boiiiiiii
    `foo(`bar)
    
    