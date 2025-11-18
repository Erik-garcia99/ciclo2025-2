import java.util.ArrayList;

public class PseudoGenerador {

    private ArrayList<Tupla> tuplas = new ArrayList<>();

    ArrayList<Token> tokens;

    public PseudoGenerador(ArrayList<Token> tokens){
        this. tokens = tokens;
    }

    public void crearTuplaAsignacion(int indiceInicial, int indiceFinal){
        if(indiceFinal - indiceInicial == 3){
            tuplas.add(new Asignacion(tokens.get(indiceInicial), tokens.get(indiceInicial+2), tuplas.size()+1, tuplas.size()+1));
        }
        else if(indiceFinal - indiceInicial ==5){
            tuplas.add(new Asignacion(tokens.get(indiceInicial), tokens.get(indiceInicial+2), tokens.get(indiceInicial+3), tokens.get(indiceInicial+4), tuplas.size()+1, tuplas.size()+1));
        }
    }

    public void crearTuplaLeer(int indiceInical){
        tuplas.add(new Leer(tokens.get(indiceInical),tuplas.size()+1, tuplas.size()+1));
    }
    
    // public void crearTuplaEscribir(int indiceInical, int indiceFinal){
    //     if(indiceFinal - indiceInical ==1){
    //         tuplas.add(new Escribir(tokens.get(indiceInical), tuplas.size()+1, tuplas.size()+1));
    //     }
    //     else if(indiceFinal - indiceInical ==3){
    //         tuplas.add(new Escribir(tokens.get(indiceInical),tokens.get(indiceInical+3), tuplas.size()+1, tuplas.size()+1));
    //     }
    // }

    public void crearTuplaEscribir(int indiceInical, int indiceFinal){
        if(indiceFinal - indiceInical == 1){
            tuplas.add(new Escribir(tokens.get(indiceInical), tuplas.size()+1, tuplas.size()+1));
        }
        else if(indiceFinal - indiceInical == 3){
            
            tuplas.add(new Escribir(tokens.get(indiceInical), tokens.get(indiceInical + 2), tuplas.size()+1, tuplas.size()+1));
        }
    }


    public void crearTuplaComparacion(int indiceInicial) {
        tuplas.add(new Comparacion(tokens.get(indiceInicial),tokens.get(indiceInicial+1),tokens.get(indiceInicial+2), tuplas.size()+1, tuplas.size()+1));

    } 
    

    public void crearTuplaFinPrograma() {
        tuplas.add(new FinPrograma());

    }


    public void conectarSi(int tuplaInicial) {
        int tuplaFinal = tuplas.size()-1;

        if (tuplaInicial >= tuplas.size() || tuplaInicial >= tuplaFinal){
            return;
        }
            

        tuplas.get(tuplaInicial).setSaltoFalso(tuplaFinal+1);
    }


    public void conectarMientras(int tuplaInicial) {
        int tuplaFinal = tuplas.size()-1;

        if (tuplaInicial >= tuplas.size() || tuplaInicial >= tuplaFinal){
            return;
        }


        tuplas.get(tuplaInicial).setSaltoFalso(tuplaFinal + 1);
        tuplas.get(tuplaFinal).setSaltoVerdadero(tuplaInicial);
        tuplas.get(tuplaFinal).setSaltoFalso(tuplaInicial);

        for (int i = tuplaFinal; i > tuplaInicial; i -- ) {
            Tupla t = tuplas.get(i);

            if (t instanceof Comparacion && t.getSaltoFalso() == tuplaFinal + 1){
                t.setSaltoFalso(tuplaInicial);
            }
                

        }

    }

    //repite


    
    public void crearTuplaRepite(Token variable, Token inicio, Token fin) {
        // 1. Inicialización: variable = inicio
        tuplas.add(new Asignacion(variable, inicio, tuplas.size() + 1, tuplas.size() + 1));
        
        // 2. Comparación: variable <= fin
        tuplas.add(new RepiteControl(variable, fin, tuplas.size() + 1, tuplas.size() + 1));
    }

    public void crearTuplaIncremento(Token variable) {
        Token operadorMas = new Token(new TipoToken(TipoToken.OPARITMETICO, "\\+"), "+");
        Token uno = new Token(new TipoToken(TipoToken.NUMERO, "-?[0-9]+"), "1");
        tuplas.add(new Asignacion(variable, variable, operadorMas, uno, tuplas.size() + 1, tuplas.size() + 1));
    }

    public void crearTuplaGoTo(int destino) {
        tuplas.add(new GoTo(destino));
    }

    public void conectarRepite(int indiceComparacion) {
        int tuplaFinal = tuplas.size() - 1;
        
        // Si el falso de la comparación no se ha conectado, conectarlo al final
        if (tuplas.get(indiceComparacion).getSaltoFalso() == indiceComparacion + 1) {
            tuplas.get(indiceComparacion).setSaltoFalso(tuplaFinal + 1);
        }
        
        // El GoTo al final debe saltar a la comparación
        tuplas.get(tuplaFinal).setSaltoVerdadero(indiceComparacion);
    }


    public ArrayList<Tupla> getTuplas() {
        return tuplas;
    }

}
