import java.util.Map;

public abstract class ScopedSymbol extends Simbolo implements Scope {

    Scope enclosingScope;

    public ScopedSymbol(String name, Type type, Scope enclosingScope){
        super(name, type);
        this.enclosingScope = enclosingScope;
    }

    public ScopedSymbol(String name, Scope enclosingScope){
        super(name);
        this.enclosingScope = enclosingScope;
    }

    public Simbolo resolve(String name){
        Simbolo s = getMembers().get(name);

        if(s != null){
            return s;
        }

        if(getEnclosingScope() != null){
            return getEnclosingScope().resolve(name);
        }

        return null;
    }

    public Simbolo resolveType(String name){
        return resolve(name);
    }

    public void define(Simbolo sym){
        getMembers().put(sym.getNombre(), sym);
        sym.scope = this;
    }

    public Scope getEnclosingScope(){
        return enclosingScope;
    }

    public String getScopeName(){
        return getNombre();
    }

    public abstract Map<String, Simbolo> getMembers();
}