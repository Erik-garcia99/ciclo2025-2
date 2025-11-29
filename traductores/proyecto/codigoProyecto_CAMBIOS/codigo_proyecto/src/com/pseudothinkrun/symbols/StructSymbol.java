package com.pseudothinkrun.symbols;

import java.util.*;

public class StructSymbol extends Symbol implements Scope, Type {
    private Map<String, Symbol> members = new LinkedHashMap<>();
    private Scope enclosingScope;

    public StructSymbol(String name, Scope enclosingScope) {
        super(name);
        this.enclosingScope = enclosingScope;
    }

    @Override
    public String getName() {
        return name;
    }

    public void define(Symbol sym) {
        members.put(sym.getName(), sym);
        sym.setScope(this);
    }

    public Symbol resolve(String name) {
        Symbol s = members.get(name);
        if (s != null) return s;
        if (enclosingScope != null) return enclosingScope.resolve(name);
        return null;
    }

    public Symbol resolveMember(String name) {
        return members.get(name);
    }

    @Override
    public String getScopeName() {
        return "struct:" + name;
    }

    @Override
    public Scope getEnclosingScope() {
        return enclosingScope;
    }
}