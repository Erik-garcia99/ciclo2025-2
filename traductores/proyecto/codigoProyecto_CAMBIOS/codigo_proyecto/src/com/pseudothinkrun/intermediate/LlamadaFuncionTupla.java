package com.pseudothinkrun.intermediate;

import com.pseudothinkrun.symbols.*;
import java.util.List;
import java.util.Map;


/**
 * Tupla para llamadas a función con paso de argumentos y scope exclusivo
 * 
 * Estrategia de implementación:
 * 1. Crear SymbolTable temporal para la función (scope local)
 * 2. Mapear argumentos a parámetros por posición (no por nombre)
 * 3. Ejecutar tuplas del cuerpo en scope aislado
 * 4. Capturar valor de RETORNAR y propagar al scope llamador
 * 
 * Por qué scope exclusivo: Evita contaminación de variables locales
 * y soporta llamadas anidadas/recursivas
 */



public class LlamadaFuncionTupla extends Tupla {
    private String funcionNombre;
    private String[] argumentos; 
    private Object valorRetorno;

    public LlamadaFuncionTupla(String funcionNombre, String[] argumentos, int sv, int sf) {
        super(sv, sf);
        this.funcionNombre = funcionNombre;
        this.argumentos = argumentos;
    }

    public Object getValorRetorno() { return valorRetorno; }

    @Override
    public int ejecutar(SymbolTable symbolTable) {
        FunctionSymbol funcion = symbolTable.resolveFunction(funcionNombre);
        if (funcion == null) {
            System.err.println("Error: Función no definida: " + funcionNombre);
            return saltoFalso;
        }

        // 1. Crear Scope Exclusivo para la función
        SymbolTable functionScope = new SymbolTable();
        functionScope.pushScope(funcion); 

        // 2. Mapear Argumentos -> Parámetros
        List<VariableSymbol> parametros = funcion.getParameters();
        if (argumentos.length != parametros.size()) {
             System.err.println("Error: Argumentos incorrectos para " + funcionNombre);
             return saltoFalso;
        }

        for (int i = 0; i < argumentos.length; i++) {
            // Evaluamos los argumentos con el scope del que llama (symbolTable original)
            Object valorArg = evaluarExpresion(argumentos[i], symbolTable);
            
            VariableSymbol paramDef = parametros.get(i);
            VariableSymbol paramVar = new VariableSymbol(paramDef.getName(), paramDef.getType());
            paramVar.setValue(valorArg);
            
            // Guardamos la variable en el scope interno de la función
            functionScope.define(paramVar);
        }

        // 3. Ejecutar cuerpo de la función
        for (Tupla instruccion : funcion.getFunctionTuplas()) {
            if (instruccion instanceof RetornarTupla) {
                RetornarTupla ret = (RetornarTupla) instruccion;
                // AQUI ESTABA EL ERROR: Ahora usamos evaluarExpresion con el scope interno
                this.valorRetorno = evaluarExpresion(ret.getExpression(), functionScope); 
                return saltoVerdadero;
            }
            int res = instruccion.ejecutar(functionScope);
            if (res == -1) return -1;
        }
        return saltoVerdadero;
    }

    // === MOTOR DE EVALUACIÓN (Copiado y adaptado de AsignacionTupla) ===

    private Object evaluarExpresion(String exprInput, SymbolTable scope) {
        // Limpieza básica
        String expr = exprInput.replaceAll("\\s*\\.\\s*", ".");
        
        if (expr.trim().startsWith("\"")) {
            String trimmed = expr.trim();
            if (trimmed.endsWith("\"")) return trimmed.substring(1, trimmed.length() - 1);
            return trimmed.substring(1);
        }

        String[] tokens = expr.split("\\s+");
        
        if (tokens.length == 1) {
            return getValor(tokens[0], scope);
        } else if (tokens.length == 3) {
            Object op1 = getValor(tokens[0], scope);
            String operador = tokens[1];
            Object op2 = getValor(tokens[2], scope);
            return realizarOperacion(op1, operador, op2);
        }
        
        return null;
    }

    private Object getValor(String token, SymbolTable scope) {
        try {
            if (token.contains(".")) {
                if (Character.isDigit(token.charAt(0))) return Double.parseDouble(token);
                String[] parts = token.split("\\.");
                VariableSymbol var = scope.resolveVariable(parts[0]);
                if (var != null && var.getValue() instanceof Map) {
                    return ((Map<String, Object>) var.getValue()).get(parts[1]);
                }
            }
            if (token.matches("-?\\d+\\.\\d+")) return Double.parseDouble(token);
            if (token.matches("-?\\d+")) return Integer.parseInt(token);
        } catch (Exception e) {}
        
        VariableSymbol var = scope.resolveVariable(token);
        return (var != null) ? var.getValue() : token;
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

    @Override public String getDescripcion() { return "LLAMAR " + funcionNombre; }
}