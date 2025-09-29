/*
 * garcia chavez erik 01275863
 * ingenieria en computacion 
 * parser recursivo descendente LL(1)
 * traductores
 * viernes 28 septiembre 2025
 * 
 */

public class Main {

    public static void main(String[] args) {
        ListLexer lexer = new ListLexer(args[0]);
        ListParser parser = new ListParser(lexer);

        try {
            parser.list(); 
            System.out.println("parsing exitoso: la entrada es valida");
        } catch (Error e) {
            System.out.println("error de parsing: " + e.getMessage());
        }
    }
    
}
