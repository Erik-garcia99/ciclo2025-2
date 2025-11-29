package com.pseudothinkrun.symbols;

public class VariableSymbol extends Symbol {
    private Object value;

    public VariableSymbol(String name, Type type) {
        super(name, type);
    }

    public Object getValue() { return value; }
    public void setValue(Object value) { this.value = value; }
}