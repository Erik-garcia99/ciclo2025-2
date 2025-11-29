package com.pseudothinkrun.symbols;

import java.util.*;

import com.pseudothinkrun.intermediate.Tupla;

public class FunctionSymbol extends Symbol implements Scope {
    private List<VariableSymbol> parameters = new ArrayList<>();
    private Scope enclosingScope;
    private List<Tupla> functionTuplas = new ArrayList<>();
    private VariableSymbol returnVariable;

    public FunctionSymbol(String name, Type returnType, Scope enclosingScope) {
        super(name, returnType);
        this.enclosingScope = enclosingScope;
    }

    public void addParameter(VariableSymbol param) {
        parameters.add(param);
    }

    public List<VariableSymbol> getParameters() {
        return parameters;
    }

    public void setFunctionTuplas(List<Tupla> tuplas) {
        this.functionTuplas = tuplas;
    }

    public List<Tupla> getFunctionTuplas() {
        return functionTuplas;
    }

    public void setReturnVariable(VariableSymbol var) {
        this.returnVariable = var;
    }

    public VariableSymbol getReturnVariable() {
        return returnVariable;
    }

    @Override
    public String getScopeName() {
        return "function:" + name;
    }

    @Override
    public Scope getEnclosingScope() {
        return enclosingScope;
    }

    @Override
    public void define(Symbol sym) {
        // Implementar si es necesario
    }

    @Override
    public Symbol resolve(String name) {
        // Buscar en parámetros
        for (VariableSymbol param : parameters) {
            if (param.getName().equals(name)) {
                return param;
            }
        }
        // Buscar en ámbito externo
        if (enclosingScope != null) {
            return enclosingScope.resolve(name);
        }
        return null;
    }
}