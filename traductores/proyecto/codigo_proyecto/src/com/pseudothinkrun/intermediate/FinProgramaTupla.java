package com.pseudothinkrun.intermediate;

import com.pseudothinkrun.symbols.*;

public class FinProgramaTupla extends Tupla {
    public FinProgramaTupla() {
        super(-1, -1);
    }

    @Override
    public int ejecutar(SymbolTable symbolTable) {
        return -1; // Indica fin de programa
    }

    @Override
    public String getDescripcion() {
        return "FINPROGRAMA";
    }
}