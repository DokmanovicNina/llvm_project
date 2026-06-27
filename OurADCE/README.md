# OurADCE (our-adce)

Implementacija pojednostavljene LLVM `Aggressive Dead Code Elimination (ADCE)` optimizacije. Prolaz agresivno uklanja mrtav kod polazeći od pretpostavke da su sve instrukcije mrtve dok se ne dokaže suprotno. Implementirane su sledeće funkcionalnosti:

- određivanje trivijalno živih instrukcija (terminatori, pozivi funkcija, volatilne i atomske operacije),
- propagacija živosti pomoću worklist algoritma kroz SSA zavisnosti,
- određivanje živih `store` instrukcija korišćenjem `Alias Analysis` kako bi se očuvale memorijske zavisnosti između `load` i `store` instrukcija,
- uklanjanje svih instrukcija koje nisu označene kao žive.

## Pokretanje (legacy PM)

```bash
build/bin/opt -enable-new-pm=0 \
  -load build/lib/LLVMOurADCE.so \
  -our-adce \
  test.ll -S -o output.ll
```