public class Escribir extends Tupla {
    
    Token cadena, variable;

    public Escribir(Token variableCadena, int sv, int sf){
        super(sv, sf);

        if( variableCadena.getTipo().getNombre().equals(TipoToken.CADENA)){
            cadena = variableCadena;        
        }
        else{
            variable = variableCadena;
        }
    }

    public Escribir(Token cadena, Token varibale, int sv, int sf){
        super(sv, sf);
        this.cadena= cadena;
        this.variable= varibale;
    }

    public String toString(){
        if(variable==null){
            return "(" + super.toString() + ", [ "+cadena+"] )";
        }
        if(cadena == null){
            return "(" + super.toString() + ", [ "+variable+"] )";
        }

        return "(" + super.toString() + ", [ "+cadena+", "+variable+ "] )";
    }

    public int ejecutar(TablaSimbolos ts) {
        if (cadena == null){
            Varibale v;
            try {
                v = (Varibale) ts.resolver(variable.getNombre());
                float valor = v.getValor();
                System.out.println(valor);
            } catch (SemanticException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            
        } else if (variable == null) {
            System.out.println(cadena.getNombre());
        } else {
            Varibale v;
            try {
                v = (Varibale) ts.resolver(variable.getNombre());
                float valor = v.getValor();
                System.out.println(cadena.getNombre() + valor);
            } catch (SemanticException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
           
        }
        return saltoVerdadero;
    }
}
