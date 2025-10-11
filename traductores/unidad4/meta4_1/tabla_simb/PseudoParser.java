import java.util.ArrayList;

public class PseudoParser{

    private ArrayList<Token> tokens;
    private int indiceToken = 0;
    private SyntaxException ex;

    private TablaSimbolos ts;
    private TipoIncorporado real;


    public PseudoParser(TablaSimbolos ts){
        this.ts = ts;
    }

    public void analizar(PseudoLexer lexer) throws SyntaxException, SemanticException{
        tokens = lexer.getTokens();

        real = new TipoIncorporado("real");
        
        // SOLUCIÓN: Verificar si "real" ya existe antes de definirlo
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
    private boolean Programa() throws SemanticException{
        if(match("INICIOPROGRAMA")){
            // if(Enunciados()){
            //     if(match("FINPROGRAMA")){
            //         return true;
            //     }
            // }
            if(Declaracion()){
                if(Enunciados()){
                    if(match(TipoToken.FINPROGRAMA)){
                        return true;
                    }
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

    private boolean Asignacion() throws SemanticException{
        int indiceAux = indiceToken;
        Varibale v;

        if(match(TipoToken.VARIABLE)){
            v = (Varibale) ts.resolver(tokens.get(indiceToken-1).getNombre());
            System.out.println("Resulta");

            if(match(TipoToken.IGUAL)){
                if(Expresion()){
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

    private boolean Leer(){
        int indiceAux = indiceToken;

        if(match("LEER")){
            if(match("VARIABLE")){
                return true;
            }
        }

        indiceToken = indiceAux;
        return false;
    }

    private boolean Escribir(){
        int indiceAux = indiceToken;

        if(match("ESCRIBIR")){
            if(match("CADENA")){
                if(match("COMA")){
                    if(match("VARIABLE")){
                        return true;
                    }
                }
                return true; 
            }
        }

        indiceToken = indiceAux;

        if(match("ESCRIBIR")){
            if(match("VARIABLE")){
                return true;
            }
        }

        indiceToken = indiceAux;
        return false;
    }

    private boolean Si() throws SemanticException{
        int indiceAux = indiceToken;

        if(match("SI")){
            if(Comparacion()){
                if(match("ENTONCES")){
                    if(Enunciados()){
                        if(match("FINSI")){
                            return true;
                        }
                    }
                }
            }
        }

        indiceToken = indiceAux;
        return false;
    }

    private boolean Mientras() throws SemanticException{
        int indiceAux = indiceToken;
    
        if(match("MIENTRAS")){
            if(Comparacion()){
                if(Enunciados()){
                    if(match("FINMIENTRAS")){
                        return true;
                    }
                }
            }
        }

        indiceToken = indiceAux;
        return false;
    }

    private boolean Repite() throws SemanticException{
        int indiceAux = indiceToken;
        
        if(match("REPITE")){
            if(match("PARENTESISIZQ")){
                if(match("VARIABLE")){
                    if(match("COMA")){
                        if(match("NUMERO")){
                            if(match("COMA")){
                                if(match("VARIABLE")){
                                    if(match("PARENTESISDER")){
                                        if(Enunciados()){
                                            if(match("FINREPITE")){
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

    private boolean Comparacion(){
        int indiceAux = indiceToken;

        if(match("PARENTESISIZQ")){
            if(Valor()){
                if(match("OPRELACIONAL")){
                    if(Valor()){
                        if(match("PARENTESISDER")){
                            return true;
                        }
                    }
                }
            }
        }

        indiceToken = indiceAux;
        return false;
    }

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