; 'br i1 true' postaje bezuslovni skok  (transformacija foldConstantBranches):
; elseBranch ostaje bez prethodnika (nedostizan), pa se blokovi spoje u jedan.

; Pokretanje:
;   ./bin/opt -load lib/LLVMOurSimplifyCFG.so -enable-new-pm=0 -our-simplifycfg -S constBranch.ll -o out.ll

define i32 @compute(i32 %a) {
entry:
  br i1 true, label %thenBranch, label %elseBranch

thenBranch:
  %t = add i32 %a, 10
  ret i32 %t

elseBranch:
  %e = sub i32 %a, 20
  ret i32 %e
}

