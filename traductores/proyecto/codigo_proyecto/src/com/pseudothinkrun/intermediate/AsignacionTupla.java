package com.pseudothinkrun.intermediate;

import com.pseudothinkrun.symbols.*;
import java.util.Map;

/**
 * Tupla para asignación de valores y evaluación de expresiones
 * 
 * Complejidad: Maneja múltiples casos en una sola clase:
 * 1. Asignación simple: x = 5
 * 2. Asignación con expresión: x = a + b * 2
 * 3. Acceso a estructura: miRect.base = 10.5
 * 4. Llamadas en expresión: x = Cuadrado(a) + 1
 * 
 * reutilización de lógica de evaluación en múltiples contextos
 */

public class AsignacionTupla extends Tupla {
    private String variableName;
    private String expression;

    public AsignacionTupla(String variableName, String expression, int sv, int sf) {
        super(sv, sf);
        this.variableName = variableName;
        this.expression = expression;
    }

    @Override
    public int ejecutar(SymbolTable symbolTable) {
        try {
            Object value = evaluarExpresion(expression, symbolTable);
            
            // Caso: Asignación a Estructura (variable.campo)
            if (variableName.contains(".")) {
                // Limpiar espacios: "rect . base" -> "rect.base"
                String cleanVarName = variableName.replaceAll("\\s+", "");
                String[] parts = cleanVarName.split("\\.");
                String structVarName = parts[0];
                String fieldName = parts[1];

                VariableSymbol structVar = symbolTable.resolveVariable(structVarName);
                if (structVar == null) {
                    System.err.println("Error: Variable no encontrada: " + structVarName);
                    return saltoFalso;
                }

                @SuppressWarnings("unchecked")
                Map<String, Object> instance = (Map<String, Object>) structVar.getValue();
                // Si la estructura no se inicializó, lo hacemos aquí (seguridad)
                if (instance == null) {
                    instance = new java.util.HashMap<>();
                    structVar.setValue(instance);
                }
                
                instance.put(fieldName, value);
            } 
            // Caso: Asignación Normal
            else {
                VariableSymbol var = symbolTable.resolveVariable(variableName);
                if (var == null) {
                    System.err.println("Error: Variable no declarada: " + variableName);
                    return saltoFalso;
                }
                var.setValue(value);
            }
        } catch (Exception e) {
            e.printStackTrace();
            return saltoFalso;
        }
        return saltoVerdadero;
    }
    /**
     * evaluarExpresion() implementa mini-interprete de expresiones
     * 
     * Soporta:
     * - Literales: "hola", 123, 45.67
     * - Variables: x, miVar
     * - Campos: miRect.base
     * - Funciones: Cuadrado(x)
     * - Operaciones: a + b, x * y
     * 
     * Limitación: No soporta precedencia de operadores más allá de paréntesis
     * Por qué: Suficiente para pseudocódigo educativo, simplifica implementación
     */
    private Object evaluarExpresion(String exprInput, SymbolTable symbolTable) {
        // 1. Arreglar puntos con espacios para matemáticas
        String expr = exprInput.replaceAll("\\s*\\.\\s*", ".");

        // 2. Manejo de cadenas (quitar comillas)
        if (expr.trim().startsWith("\"")) {
            String trimmed = expr.trim();
            if (trimmed.endsWith("\"")) return trimmed.substring(1, trimmed.length() - 1);
            return trimmed.substring(1);
        }

        // 3. Manejo de Funciones
        if (expr.matches("\\w+\\s*\\([^)]+\\)")) {
            return evaluarLlamadaFuncionEnExpresion(expr, symbolTable);
        }
        
        // 4. Operaciones Matemáticas Simples
        String[] tokens = expr.split("\\s+");
        if (tokens.length == 1) return getValor(tokens[0], symbolTable);
        else if (tokens.length == 3) {
            Object op1 = getValor(tokens[0], symbolTable);
            String operador = tokens[1];
            Object op2 = getValor(tokens[2], symbolTable);
            return realizarOperacion(op1, operador, op2);
        }
        return null;
    }

    private Object evaluarLlamadaFuncionEnExpresion(String expr, SymbolTable symbolTable) {
        int parenStart = expr.indexOf('(');
        String funcName = expr.substring(0, parenStart).trim();
        String argsStr = expr.substring(parenStart + 1, expr.length() - 1).trim();
        String[] args = argsStr.isEmpty() ? new String[0] : argsStr.split("\\s*,\\s*");
        
        LlamadaFuncionTupla llamada = new LlamadaFuncionTupla(funcName, args, 0, 0);
        llamada.ejecutar(symbolTable);
        return llamada.getValorRetorno();
    }

    private Object getValor(String token, SymbolTable symbolTable) {
        try {
            // Soporte para leer estructuras (rect.base)
            if (token.contains(".")) {
                if (Character.isDigit(token.charAt(0))) return Double.parseDouble(token);
                String[] parts = token.split("\\.");
                VariableSymbol var = symbolTable.resolveVariable(parts[0]);
                if (var != null && var.getValue() instanceof Map) {
                    return ((Map<String, Object>) var.getValue()).get(parts[1]);
                }
            }
            if (token.matches("-?\\d+\\.\\d+")) return Double.parseDouble(token);
            if (token.matches("-?\\d+")) return Integer.parseInt(token);
        } catch (Exception e) {}
        VariableSymbol var = symbolTable.resolveVariable(token);
        if (var != null) return var.getValue();
        return token;
    }

    private Object realizarOperacion(Object op1, String operador, Object op2) {
        if (op1 instanceof Number && op2 instanceof Number) {
            double v1 = ((Number) op1).doubleValue();
            double v2 = ((Number) op2).doubleValue();
            boolean esEntero = (op1 instanceof Integer) && (op2 instanceof Integer);
            switch (operador) {
                case "+": return esEntero ? (int)(v1 + v2) : v1 + v2;
                case "-": return esEntero ? (int)(v1 - v2) : v1 - v2;
                case "*": return esEntero ? (int)(v1 * v2) : v1 * v2;
                case "/": return v1 / v2;
            }
        }
        return null;
    }

    @Override public String getDescripcion() { return variableName + " = " + expression; }
}