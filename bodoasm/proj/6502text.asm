#org $8000, 0

example = 15

:
    BEQ :-
    BEQ :+
:
    NOP
    NOP
    NOP
    NOP
:
    BEQ :--
    BEQ :++
:
    NOP
    NOP
    NOP
    NOP
;ScrewMeUp:
:
    BEQ :-
    BEQ :--
:
    NOP
    NOP
    NOP
    NOP
:
    NOP
    NOP
    NOP
    NOP

    LDA example