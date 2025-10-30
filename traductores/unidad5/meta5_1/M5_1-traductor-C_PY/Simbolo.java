public class Simbolo{

    private String nombre;
    private Type tipo;
    Scope scope;


    public Simbolo(String nombre){
        this.nombre = nombre;
        
    }
    
    public Simbolo(String nombre, Type tipo){
        this(nombre);
        this.tipo = tipo;
    }

    

    public String getNombre() {
        return nombre;
    }   

    public Type getTipo(){
        return tipo;
    }

    public String toString(){
        if(tipo != null){
            return "<" + getNombre() + ":"+ tipo + " >";
        }

        return getNombre();
    }


}