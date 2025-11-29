package com.pseudothinkrun.intermediate;

import com.pseudothinkrun.symbols.*;

public class ComparacionTupla extends Tupla {
    private String expr1;
    private String operador;
    private String expr2;

    public ComparacionTupla(String expr1, String operador, String expr2, int sv, int sf) {
        super(sv, sf);
        this.expr1 = expr1;
        this.operador = operador;
        this.expr2 = expr2;
    }

    @Override
    public int ejecutar(SymbolTable symbolTable) {
        Object val1 = getValor(expr1, symbolTable);
        Object val2 = getValor(expr2, symbolTable);
        
        boolean result = comparar(val1, operador, val2);
        // Si es verdadero, devuelve saltoVerdadero (siguiente instrucción o salto)
        // Si es falso, devuelve saltoFalso (salto al SINO o fin de bucle)
        return result ? saltoVerdadero : saltoFalso;
    }

    private Object getValor(String token, SymbolTable symbolTable) {
        try {
            // Número entero
            if (token.matches("-?\\d+")) return Integer.parseInt(token);
            // Número real
            if (token.matches("-?\\d+\\.\\d+")) return Double.parseDouble(token);
        } catch (Exception e) {}
        
        // Buscar variable
        VariableSymbol var = symbolTable.resolveVariable(token);
        if (var == null) {
            System.err.println("ADVERTENCIA: Variable '" + token + "' no encontrada, usando 0");
            return 0; // Valor por defecto para evitar crash
        }
        
        Object value = var.getValue();
        if (value == null) {
            System.err.println("ADVERTENCIA: Variable '" + token + "' tiene valor null, usando 0");
            return 0;
        }
        
        return value;
    }

    private boolean comparar(Object op1, String op, Object op2) {
        if (op1 instanceof Number && op2 instanceof Number) {
            double v1 = ((Number) op1).doubleValue();
            double v2 = ((Number) op2).doubleValue();
            switch (op) {
                case "<": return v1 < v2;
                case ">": return v1 > v2;
                case "<=": return v1 <= v2;
                case ">=": return v1 >= v2;  
                case "==": return v1 == v2;
                case "!=": return v1 != v2;  
            }
        }
        return false;
    }

    @Override public String getDescripcion() { return expr1 + " " + operador + " " + expr2; }
}