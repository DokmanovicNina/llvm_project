; Pokretanje:
;   ./bin/opt -load lib/LLVMOurSimplifyCFG.so -enable-new-pm=0 -our-simplifycfg -S updateGrade.ll -o updateGrade_out.ll

define i32 @updateGrade(i32 %currPoints) {
entry:
  %withBonus = add i32 %currPoints, 11
  br label %passedExams

passedExams:
  %newPoints = phi i32 [ %withBonus, %entry ]
  %division = sdiv i32 %newPoints, 10
  %newGrade = add i32 %division, 1
  ret i32 %newGrade
}
