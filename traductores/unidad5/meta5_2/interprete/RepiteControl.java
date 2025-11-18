public class RepiteControl extends Tupla {
    Token variable;
    Token limite;

    public RepiteControl(Token variable, Token limite, int sv, int sf) {
        super(sv, sf);
        this.variable = variable;
        this.limite = limite;
    }

    public int ejecutar(TablaSimbolos ts) throws SemanticException {
        try {
            Varibale var = (Varibale) ts.resolver(variable.getNombre());
            float valorVar = var.getValor();
            
            float valorLimite = (limite.getTipo().getNombre().equals(TipoToken.NUMERO)) 
                ? Float.parseFloat(limite.getNombre()) 
                : ((Varibale) ts.resolver(limite.getNombre())).getValor();
            
            // Si variable <= limite, continuar el bucle
            return (valorVar <= valorLimite) ? saltoVerdadero : saltoFalso;
        } catch (Exception e) {
            e.printStackTrace();
            return saltoFalso; // Salir del bucle si hay error
        }
    }

    public String toString() {
        return "(RepiteControl, " + saltoVerdadero + ", " + saltoFalso + ", [ " + variable + " <= " + limite + " ]) ";
    }
}