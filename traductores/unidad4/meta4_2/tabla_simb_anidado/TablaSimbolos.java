import java.util.ArrayList;

public class TablaSimbolos {
    private ArrayList<Simbolo> simbolos = new ArrayList();

    public void definir(Simbolo simbolo) throws SemanticException{
        for(Simbolo s: simbolos){
            if(simbolo.getNombre().equals(s.getNombre())){
                throw new SemanticException("El simbolo " + s.getNombre() + " ya fue declarado");
            }
        }
        simbolos.add(simbolo);
    }

    public Simbolo resolver(String nombre) throws SemanticException{
        for(Simbolo s: simbolos){
            if(s.getNombre().equals(nombre)){
                return s;
            }
        }
        // CORRECCIÓN: Mensaje correcto para símbolo no encontrado
        throw new SemanticException("El simbolo " + nombre + " no ha sido declarado");
    }

    public ArrayList<Simbolo> getSimbolos(){
        return simbolos;
    }
}