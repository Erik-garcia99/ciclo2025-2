import java.util.ArrayList;

public class TablaSimbolos {
    


    private ArrayList<Simbolo> simbolos = new ArrayList();


    public void definir(Simbolo simbolo) throws SemanticException{
        for(Simbolo s: simbolos){
            if(simbolo.getNombre().equals(simbolo.getNombre())){
                throw new SemanticException("El simbolo " +
                                            s.getNombre()+" ya fue declarado");
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
        throw new SemanticException("El simbolo " +
                                            s.getNombre()+" ya fue declarado");
    }

    public ArrayList<Simbolo> getSimbolos(){
        return simbolos;
    }

}
