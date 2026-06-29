# SimplifyCFG (our-simplifycfg)

Pojednostavljena verzija LLVM `SimplifyCFG` optimizacije. Radi sledece
transformacije nad grafom toka kontrole (CFG):

- pretvara uslovnu granu u bezuslovnu ako je uslov grananja konstantan (true ili false).
- uslovni skok pretvara u bezuslovni kada obe grane
  vode na isti cilj.
- uklanja nedostizne blokove (bez prethodnika).
- eliminise trivijalne PHI cvorove (jedan ulaz).
- spaja blok sa jedinstvenim prethodnikom i zaobilazi
  prazne prolazne blokove.

## Pokretanje

    ./bin/opt -load lib/LLVMOurSimplifyCFG.so -enable-new-pm=0 \
      -our-simplifycfg -S ulaz.ll -o izlaz.ll
