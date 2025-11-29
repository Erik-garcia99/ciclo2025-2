package com.pseudothinkrun.symbols;

public class BuiltInTypeSymbol extends Symbol implements Type {
    public BuiltInTypeSymbol(String name) {
        super(name);
    }

    @Override
    public String getName() {
        return name;
    }
}