" lmnlang syntax highlighting for Vim/Neovim
if exists("b:current_syntax")
  finish
endif

syntax keyword lmnKeyword if else while break continue func return

syntax keyword lmnBuiltin lmuck lmport lmout lmtod lmtype lmlen lmpush

syntax keyword lmnBoolean true false

syntax match lmnComment "//.*$"
syntax region lmnComment start="/--" end="--/"

syntax region lmnString start='"' end='"' contains=@Spell

syntax match lmnNumber "\v\b[0-9]+(\.[0-9]+)?\b"

syntax match lmnOperator "\v(\+|\-|\*|\/|\%|\=|\=\=|!\=|\<|\>|\<\=|\>\=|&&|\|\||\!)"

highlight default link lmnKeyword Keyword
highlight default link lmnBuiltin Function
highlight default link lmnBoolean Boolean
highlight default link lmnComment Comment
highlight default link lmnString String
highlight default link lmnNumber Number
highlight default link lmnOperator Operator

let b:current_syntax = "lmn"
