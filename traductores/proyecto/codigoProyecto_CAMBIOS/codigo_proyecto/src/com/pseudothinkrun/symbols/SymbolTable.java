package com.pseudothinkrun.symbols;

import java.util.*;

/**
 * Tabla de simbolos que implementa scopes anidados usando Stack
 * 
 * Diseño: Patron Chain of Responsibility para resolucion de simbolos
 * - Busca en scope actual → scope padre → scope global
 * - Cada scope (función, estructura) tiene su propio namespace
 * 
 * Importancia: Permite variables locales, ocultación de nombres y
 * generacion de errores precisos "variable no declarada"
 */

public class SymbolTable implements Scope {
    private Map<String, Symbol> symbols = new HashMap<>();
    private Scope currentScope;
    private Stack<Scope> scopeStack = new Stack<>();
    private Map<String, StructSymbol> structTypes = new HashMap<>();

    public SymbolTable() {
        currentScope = new GlobalScope(null);
        scopeStack.push(currentScope);
        
        define(new BuiltInTypeSymbol("ENTERO"));
        define(new BuiltInTypeSymbol("REAL"));
        define(new BuiltInTypeSymbol("CADENA"));
        define(new BuiltInTypeSymbol("LOGICO"));
    }

    public void pushScope(Scope scope) {
        scopeStack.push(scope);
        currentScope = scope;
    }

    public void popScope() {
        if (scopeStack.size() > 1) {
            scopeStack.pop();
            currentScope = scopeStack.peek();
        }
    }

    public Scope getCurrentScope() { return currentScope; }

    @Override
    public String getScopeName() { return currentScope.getScopeName(); }

    @Override
    public Scope getEnclosingScope() { return currentScope.getEnclosingScope(); }

    @Override
    public void define(Symbol sym) {
        symbols.put(sym.getName(), sym);
        sym.setScope(currentScope);
    }

     /**
     * Resolucion jerarquica de simbolos
     * Por que: Variables locales deben ocultar globales con mismo nombre
     * Retorna null si no existe (error semántico en parser)
     */
    @Override
    public Symbol resolve(String name) {
        Symbol s = symbols.get(name);
        if (s != null) return s;
        return currentScope.resolve(name);
    }

    public void defineStruct(StructSymbol struct) {
        structTypes.put(struct.getName(), struct);
        define(struct);
    }

    public StructSymbol resolveStruct(String name) {
        return structTypes.get(name);
    }

    public VariableSymbol resolveVariable(String name) {
        Symbol s = resolve(name);
        if (s instanceof VariableSymbol) return (VariableSymbol) s;
        return null;
    }

    public FunctionSymbol resolveFunction(String name) {
        Symbol s = resolve(name);
        if (s instanceof FunctionSymbol) return (FunctionSymbol) s;
        return null;
    }

    private class GlobalScope implements Scope {
        private Scope enclosingScope;
        GlobalScope(Scope enclosingScope) { this.enclosingScope = enclosingScope; }
        @Override public String getScopeName() { return "global"; }
        @Override public Scope getEnclosingScope() { return enclosingScope; }
        @Override public void define(Symbol sym) { symbols.put(sym.getName(), sym); sym.setScope(this); }
        @Override public Symbol resolve(String name) {
            Symbol s = symbols.get(name);
            if (s != null) return s;
            if (enclosingScope != null) return enclosingScope.resolve(name);
            return null;
        }
    }
}