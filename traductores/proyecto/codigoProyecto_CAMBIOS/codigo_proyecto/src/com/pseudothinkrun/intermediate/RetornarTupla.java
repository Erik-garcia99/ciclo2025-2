package com.pseudothinkrun.intermediate;

import com.pseudothinkrun.symbols.*;

public class RetornarTupla extends Tupla {
    private String expression;

    public RetornarTupla(String expression, int sv, int sf) {
        super(sv, sf);
        this.expression = expression;
    }

    public String getExpression() {
        return expression;
    }

    @Override
    public int ejecutar(SymbolTable symbolTable) {
        // El retorno se maneja en el Interpreter
        return -2; // Código especial para retorno
    }

    @Override
    public String getDescripcion() {
        return "RETORNAR " + expression;
    }
}