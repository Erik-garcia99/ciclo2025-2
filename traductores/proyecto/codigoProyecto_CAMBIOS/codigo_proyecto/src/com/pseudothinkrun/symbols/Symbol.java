package com.pseudothinkrun.symbols;

public abstract class Symbol {
    protected String name;
    protected Type type;
    protected Scope scope;

    public Symbol(String name) {
        this.name = name;
    }

    public Symbol(String name, Type type) {
        this.name = name;
        this.type = type;
    }

    public String getName() { return name; }
    public Type getType() { return type; }
    public void setType(Type type) { this.type = type; }
    public Scope getScope() { return scope; }
    public void setScope(Scope scope) { this.scope = scope; }

    @Override
    public String toString() {
        if (type != null) {
            return "<" + name + ":" + type.getName() + ">";
        }
        return name;
    }
}