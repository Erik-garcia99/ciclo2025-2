package com.pseudothinkrun.intermediate;

import com.pseudothinkrun.symbols.*;
import java.util.Map;

public class EscribirTupla extends Tupla {
    private String[] expressions;

    public EscribirTupla(String[] expressions, int sv, int sf) {
        super(sv, sf);
        this.expressions = expressions;
    }

    @Override
    public int ejecutar(SymbolTable symbolTable) {
        StringBuilder output = new StringBuilder();
        
        for (String expr : expressions) {
            if (expr.startsWith("\"") && expr.endsWith("\"")) {
                output.append(expr.substring(1, expr.length() - 1));
            } else if (expr.contains(".")) {
                // Manejo de estructuras en ESCRIBIR (ej: miRect.base)
                String[] parts = expr.split("\\.");
                VariableSymbol var = symbolTable.resolveVariable(parts[0]);
                if (var != null && var.getValue() instanceof Map) {
                    @SuppressWarnings("unchecked")
                    Map<String, Object> instance = (Map<String, Object>) var.getValue();
                    Object val = instance.get(parts[1]);
                    output.append(val != null ? val : "null");
                } else {
                    output.append("null");
                }
            } else {
                VariableSymbol var = symbolTable.resolveVariable(expr);
                if (var != null) {
                    output.append(var.getValue());
                } else {
                    output.append(expr);
                }
            }
        }
        
        System.out.println(output.toString());
        return saltoVerdadero;
    }

    @Override
    public String getDescripcion() {
        return "ESCRIBIR " + String.join(", ", expressions);
    }
}