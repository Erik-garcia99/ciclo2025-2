import java.util.ArrayList;

public class PseudoParser{

    private ArrayList<Token> tokens;
    private int indiceToken = 0;
    private SyntaxException ex;

    private TablaSimbolos ts;
    private TipoIncorporado real;
    private PseudoGenerador generador;


    public PseudoParser(TablaSimbolos ts, PseudoGenerador generador){
        this.ts = ts;
        this.generador = generador;
    }

    public void analizar(PseudoLexer lexer) throws SyntaxException, SemanticException{
        tokens = lexer.getTokens();

        real = new TipoIncorporado("real");
        
        try {
            ts.resolver("real"); // Si ya existe, no lo definimos nuevamente
        } catch (SemanticException e) {
            // Si no existe, entonces lo definimos
            ts.definir(real);
        }

        if(Programa()){
            if(indiceToken == tokens.size()){
                System.out.println("\nLa sintaxis del programa es correcta");
                return;
            }
        }

        if(ex == null){
            ex = new SyntaxException("error de sintaxis");
        }
        throw ex;
    }

    private boolean Programa() throws SemanticException {

        if (match(TipoToken.INICIOPROGRAMA)){
            if (Enunciados()){
                if (match(TipoToken.FINPROGRAMA)) {

                    generador.crearTuplaFinPrograma();
                    return true;
                }
            }
        }
        return false;
    }



    private boolean Enunciados() throws SemanticException{
        int indiceAux = indiceToken;


        if(Enunciado()){
            while(Enunciado()){
                
            }
            return true;
        }

        indiceToken = indiceAux;
        return false;
    }

    private boolean Enunciado() throws SemanticException{
        int indiceAux = indiceToken;


        if(Declaracion()){
            return true;
        }
        indiceToken = indiceAux;
        

        if(tokens.get(indiceToken).getTipo().getNombre().equals("VARIABLE")){
            if(Asignacion()){
                return true;
            }
        }
        indiceToken = indiceAux;

        if(tokens.get(indiceToken).getTipo().getNombre().equals("LEER")) {
            if(Leer()){
                return true;
            }
        }
        indiceToken = indiceAux;

        if(tokens.get(indiceToken).getTipo().getNombre().equals("ESCRIBIR")){
            if(Escribir()){
                return true;
            }
        }
        indiceToken = indiceAux;

        if(tokens.get(indiceToken).getTipo().getNombre().equals("SI")){
            if(Si()){
                return true;
            }
        }
        indiceToken = indiceAux;
        
        if(tokens.get(indiceToken).getTipo().getNombre().equals("MIENTRAS")){
            if(Mientras()){
                return true;
            }
        }
        indiceToken = indiceAux;

        // AÑADIR ESTA LÍNEA: Manejar declaración de variables
        if(tokens.get(indiceToken).getTipo().getNombre().equals("VARIABLES")){
            if(DeclaracionVariables()){
                return true;
            }
        }
        indiceToken = indiceAux;

        if(tokens.get(indiceToken).getTipo().getNombre().equals("REPITE")){
            if(Repite()){
                return true;
            }
        }

        indiceToken = indiceAux;
        return false;
    }


    private boolean Declaracion() throws SemanticException{

        if(match(TipoToken.VARIABLES)){
            if(match(TipoToken.DOSPUNTOS)){
                if(match(TipoToken.VARIABLE)){
                    ts.definir(new Varibale(tokens.get(indiceToken-1).getNombre(),real));
                    System.out.println("Definida");

                    while(match(TipoToken.COMA)){
                        if(!match(TipoToken.VARIABLE)){
                            return false;
                        }

                        ts.definir(new Varibale(tokens.get(indiceToken-1).getNombre(), real));
                        System.out.println("Definida");
                    }
                    return true;
                }
            }
        }

        return false;

    }
    // <Asignacion> -> VARIABLE = <Expresion>
    private boolean Asignacion() {
        int indiceAux = indiceToken;

        if (match(TipoToken. VARIABLE)){
            if (match(TipoToken. IGUAL)){
                if (Expresion()) {
                    generador.crearTuplaAsignacion(indiceAux, indiceToken);
                    return true;

                }
            }
        }
        indiceToken = indiceAux;
        return false;
    }

    private boolean DeclaracionVariables(){
        int indiceAux = indiceToken;
        
        if(match("VARIABLES")){
            if(match("DOSPUNTOS")){
                if(ListaVariables()){
                    return true;
                }
            }
        }
        
        indiceToken = indiceAux;
        return false;
    }

    private boolean ListaVariables(){
        if(match("VARIABLE")){
            while(match("COMA")){
                if(!match("VARIABLE")){
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    // private boolean Asignacion(){
    //     int indiceAux = indiceToken;

    //     if(match("VARIABLE")){
    //         if(match("IGUAL")){
    //             if(Expresion()){
    //                 return true;
    //             }
    //         }
    //     }

    //     indiceToken = indiceAux;
    //     return false;
    // }

    private boolean Expresion(){
        int indiceAux = indiceToken;

        if(Valor()){
            if(match("OPARITMETICO")){
                if(Valor()){
                    return true;
                }
            }
            return true; 
        }

        indiceToken = indiceAux;
        return false;
    }

    private boolean Valor(){
        if(match("VARIABLE") || match("NUMERO")){
            return true;
        }
        return false;
    }

    // <Leer> -> leer VARIABLE
    private boolean Leer() {
        int indiceAux = indiceToken;

        if (match(TipoToken.LEER)){
            if (match(TipoToken. VARIABLE)) {
                generador.crearTuplaLeer(indiceAux+1);
                return true;

            }
        }


        indiceToken = indiceAux;
        return false;

    }


    private boolean Escribir() {
        int indiceAux = indiceToken;

        if (match(TipoToken.ESCRIBIR)){
            if (match(TipoToken.CADENA)){
                if (match(TipoToken.COMA)){
                    if (match(TipoToken. VARIABLE)) {
                        generador.crearTuplaEscribir(indiceAux+1, indiceToken);
                    return true;
                    }
                }
            }
        }

        indiceToken = indiceAux;

        if (match(TipoToken.ESCRIBIR)){
            if (match(TipoToken.CADENA)) {
                generador.crearTuplaEscribir(indiceAux+1, indiceToken);
                return true;
            }
        }

        indiceToken = indiceAux;
        if (match(TipoToken.ESCRIBIR)){
            if (match(TipoToken.VARIABLE)) {
                generador.crearTuplaEscribir(indiceAux+1, indiceToken);
                return true;
            }
        }
        
        indiceToken = indiceAux;
        return false;
    }
    private boolean Si() throws SemanticException {
        int indiceAux = indiceToken;
        int indiceTupla = generador.getTuplas().size();

        if (match(TipoToken.SI)){
            if (Comparacion()){
                if (match(TipoToken.ENTONCES)){
                    if (Enunciados()){
                        if (match(TipoToken.FINSI)) {
                            generador.conectarSi(indiceTupla);
                            return true;
                        }
                    }
                }
            }
        }
    
        indiceToken = indiceAux;
        return false;

    }

    private boolean Mientras() throws SemanticException {
        int indiceAux = indiceToken;
        int indiceTupla = generador.getTuplas().size();

        if (match(TipoToken.MIENTRAS)){
            if (Comparacion()){
                if (Enunciados()){
                    if (match(TipoToken.FINMIENTRAS)) {
                        generador.conectarMientras(indiceTupla);
                        return true;
                    }
                }
            }          
        }

        indiceToken = indiceAux;
        return false;

    }

    // private boolean Repite() throws SemanticException{
    //     int indiceAux = indiceToken;
        
    //     if(match("REPITE")){
    //         if(match("PARENTESISIZQ")){
    //             if(match("VARIABLE")){
    //                 if(match("COMA")){
    //                     if(match("NUMERO")){
    //                         if(match("COMA")){
    //                             if(match("VARIABLE")){
    //                                 if(match("PARENTESISDER")){
    //                                     if(Enunciados()){
    //                                         if(match("FINREPITE")){
    //                                             return true;
    //                                         }
    //                                     }
    //                                 }
    //                             }
    //                         }
    //                     }
    //                 }
    //             }
    //         }
    //     }

    //     indiceToken = indiceAux;
    //     return false;
    // }

    private boolean Repite() throws SemanticException {
        int indiceAux = indiceToken;
        
        if (match(TipoToken.REPITE)) {
            if (match(TipoToken.PARENTESISIZQ)) {
                if (match(TipoToken.VARIABLE)) {
                    Token varControl = tokens.get(indiceToken - 1);
                    
                    // VALIDAR QUE LA VARIABLE ESTÉ DECLARADA
                    try {
                        ts.resolver(varControl.getNombre());
                    } catch (SemanticException e) {
                        ex = new SyntaxException("Variable no declarada: " + varControl.getNombre());
                        return false;
                    }
                    
                    if (match(TipoToken.COMA)) {
                        if (Valor()) {  // Valor inicial
                            Token valInicio = tokens.get(indiceToken - 1);
                            if (match(TipoToken.COMA)) {
                                if (Valor()) {  // Valor final
                                    Token valFin = tokens.get(indiceToken - 1);
                                    if (match(TipoToken.PARENTESISDER)) {
                                        
                                        // Generar estructura del bucle
                                        int indiceInicioBucle = generador.getTuplas().size();
                                        generador.crearTuplaRepite(varControl, valInicio, valFin);
                                        
                                        if (Enunciados()) {
                                            if (match(TipoToken.FINREPITE)) {
                                                
                                                // Generar incremento y GoTo
                                                generador.crearTuplaIncremento(varControl);
                                                generador.crearTuplaGoTo(indiceInicioBucle + 1);
                                                
                                                // Conectar los saltos
                                                generador.conectarRepite(indiceInicioBucle + 1);
                                                
                                                return true;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        indiceToken = indiceAux;
        return false;
    }

    private boolean Comparacion() {
        int indiceAux = indiceToken;

        if (match(TipoToken.PARENTESISIZQ))
            if (Valor())
                if (match(TipoToken. OPRELACIONAL))
                    if (Valor())
                        if (match(TipoToken.PARENTESISDER)) {
                            generador.crearTuplaComparacion(indiceAux+1);
                            return true;

                        }

        indiceToken = indiceAux;
        return false;

    }

    // private boolean match(String nombre){
    //     if(indiceToken < tokens.size() && 
    //        tokens.get(indiceToken).getTipo().getNombre().equals(nombre)){
    //         System.out.println(nombre + ":" + tokens.get(indiceToken).getNombre());
    //         indiceToken++;
    //         return true;
    //     }

        
    //     if(ex == null){
    //         String encontrado = (indiceToken < tokens.size()) ? 
    //             tokens.get(indiceToken).getTipo().getNombre() : "EOF";
    //         ex = new SyntaxException(nombre, encontrado);
    //     }

    //     return false;
    // }


    
    private boolean match(String nombre){
        if(indiceToken < tokens.size() && 
           tokens.get(indiceToken).getTipo().getNombre().equals(nombre)){
            System.out.println(nombre + ":" + tokens.get(indiceToken).getNombre());
            indiceToken++;
            return true;
        }
        
        if(ex == null){
            String encontrado = (indiceToken < tokens.size()) ? 
                tokens.get(indiceToken).getTipo().getNombre() : "EOF";
            ex = new SyntaxException(nombre, encontrado);
        }
        return false;
    }
}