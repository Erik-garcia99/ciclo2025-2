public class GoTo extends Tupla {
    public GoTo(int destino) {
        super(destino, destino);
    }

    public int ejecutar(TablaSimbolos ts) {
        return saltoVerdadero; // Salto incondicional
    }

    public String toString() {
        return "(GoTo, " + saltoVerdadero + ", " + saltoFalso + ", [] )";
    }
}