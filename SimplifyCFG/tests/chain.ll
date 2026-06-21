; Pokretanje:
;   ./bin/opt -load lib/LLVMOurSimplifyCFG.so -enable-new-pm=0 -our-simplifycfg -S chain.ll -o chain_out.ll
define i32 @chain() {
entry:
  br label %a
a:
  br label %b
b:
  br label %c
c:
  ret i32 42
}
