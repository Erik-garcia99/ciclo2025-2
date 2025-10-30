import java.util.LinkedHashMap;
import java.util.Map;

public class StructSymbol extends ScopedSymbol implements Type, Scope{
    
    Map<String, Simbolo> fields = new LinkedHashMap<String, Simbolo>();

    public StructSymbol(String name, Scope parent){
        super(name, parent);
    }

    public Simbolo resolveMember(String name){
        return fields.get(name);
    }

    public Map<String, Simbolo> getMembers(){
        return fields;
    }

    public String toStrign(){
        return "Struct "+getName()+":{"+fields.keySet().toString() + "}";
    }

    @Override
    public String getName() {
        // TODO Auto-generated method stub
        throw new UnsupportedOperationException("Unimplemented method 'getName'");
    }


}
