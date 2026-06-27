; Pokretanje:
;   ./bin/opt -load lib/LLVMOurSimplifyCFG.so -enable-new-pm=0 -our-simplifycfg -S littleCombo.ll -o littleCombo_out.ll

define i32 @compute(i32 %a) {
entry:
  %init = add i32 %a, 10
  br label %pass1
pass1:
  br label %mid
mid:
  %phiVal = phi i32 [ %init, %pass1 ]
  %doubled = mul i32 %phiVal, 2
  br label %tail
tail:
  %result = sub i32 %doubled, 1
  ret i32 %result
orphan1:
  %junk1 = add i32 %a, 777
  br label %orphan2
orphan2:
  %junk2 = mul i32 %junk1, 888
  ret i32 %junk2
}
