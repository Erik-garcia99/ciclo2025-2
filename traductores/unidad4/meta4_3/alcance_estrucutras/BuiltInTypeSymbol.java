public class BuiltInTypeSymbol extends Simbolo implements Type{
    public BuiltInTypeSymbol(String name){
        super(name);
    }
    
    // Implementación del método de la interfaz Type
    @Override
    public String getName() {
        return getNombre();
    }
    
    // El método getNombre() ya viene de Simbolo, que satisface la interfaz Tipo
}