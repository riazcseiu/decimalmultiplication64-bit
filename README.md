# decimalmultiplication64-bit
DecimalMultiplication64-bit compatable with - gem-5 RISCV (testable)
  
  ## Fully executable and compatible with GCC 4.X and above
  gcc DecMulTimeMeasure.c decContext.c decDouble.c decQuad.c -o examples-pk-DecMulTimeMeasure
  ## To execute
  ./examples-pk-DecMulTimeMeasure
  ## output
    All in one testAvg. TIME 64-bitSW Lib Result =0.084017
    Avg. TIME 64-bitSW Lib Rounding =0.128663
    Avg. TIME 64-bitSW Lib Overflow =0.126296
    Avg. SW 64-bitTIME Lib Underflow =0.137448
  ## ===================================================
    Avg. TIME Method 64-bit Result =0.054354
    Avg. TIME Method 64-bit Rounding =0.054684
    Avg. TIME Method 64-bit Overflow =0.057068
    Avg. TIME Method 64-bit Underflow =0.057878
    
    
 ## To test gem 5
 To execute project in in RISCV with cpu se.py in gem5 simulator
 make RISC-V binary and then execute 
    ./build/RISCV/gem5.opt configs/example/se.py -c tests/test-progs/hello/bin/riscv/linux/decNumberBinary
