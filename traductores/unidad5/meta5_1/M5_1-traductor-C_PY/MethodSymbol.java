import java.util.LinkedHashMap;
import java.util.Map;

public class MethodSymbol extends Simbolo implements Scope {
    
    Scope enclosingScope;

    Map<String, Simbolo> members = new LinkedHashMap<String, Simbolo>();

    public MethodSymbol(String name, VariableSymbol[] orderdArgs, Scope enclosingScope){

        super(name); 
        this.enclosingScope = enclosingScope;
        if( orderdArgs != null){
            for(VariableSymbol v : orderdArgs){
                define(v);
            }
        }

    }


    public String getScopeName(){
        return enclosingScope.getScopeName();
    }

    public Scope getEnclosingScope(){
        return enclosingScope;
    }

    public void define(Simbolo sym){
        members.put(sym.getNombre(), sym);
    }

    public Simbolo resolve(String name){
        Simbolo s = members.get(name);

        if( s != null){
            return s;
        }

        if(enclosingScope != null){
            return enclosingScope.resolve(name);
        }

        return null;
    }


}
