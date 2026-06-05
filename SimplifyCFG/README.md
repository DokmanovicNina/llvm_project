# SimplifyCFG (our-simplifycfg)
Implementacija LLVM `SimplifyCFG` optimizacije. Ukljucene su cetiri transformacije:
- uklanja bazicne blokove bez prethodnika tj. nedostizne,
- eliminise trivijalne PHI cvorove (blok sa jednim prethodnikom),
- spaja blok sa jedinstvenim prethodnikom koji ima jedinog naslednika,
- zaobilazi prazne blokove tj. one sa bezuslovnim skokom.
## Pokretanje (legacy PM)
    ./bin/opt -load lib/LLVMOurSimplifyCFG.so -enable-new-pm=0 \
      -our-simplifycfg -S ulaz.ll -o izlaz.ll
