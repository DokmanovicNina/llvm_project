
; Pokretanje:

;   ./bin/opt -load lib/LLVMOurSimplifyCFG.so -enable-new-pm=0 -our-simplifycfg -S isDogWalked.ll -o isDogWalked_out.ll

define i1 @isDogWalked() {

entry:

  br label %askBrother

askBrother:

  br label %askSister

askSister:

  br label %askOtherBrother

askOtherBrother:

  br label %askMother

askMother:

  br label %askDad

askDad:

  ret i1 true

}

