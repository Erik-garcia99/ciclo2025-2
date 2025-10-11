import java.io.FileReader;
import java.io.IOException;

public class PruebaTablaSimbolos {
    
   
    public static void main(String[] args) throws LexicalException, SyntaxException, SemanticException {
        String entrada = leerPrograma("algo.txt");
        PseudoLexer lexer = new PseudoLexer();
        lexer.analizar(entrada);

        System.out.println("*** Analisis lexico ***\n");

        for(Token t: lexer.getTokens()){
            System.out.println(t);
        }

        System.out.println("*** Analisis sintactico ***\n");

        TablaSimbolos ts = new TablaSimbolos();
        PseudoParser parser = new PseudoParser(ts);
        parser.analizar(lexer);

        System.out.println("Tabal de simbolos");

        for(Simbolo s: ts.getSimbolos()){
            System.out.println(s);
        }
    }


    private static String leerPrograma(String nombre){
        String entrada = "";

        try{
            FileReader reader = new FileReader(nombre);
            int caracter;

            while((caracter = reader.read()) != -1){
                entrada += (char)caracter;
            }

            reader.close();
            return entrada;
        }catch(IOException e){
            return "";
        }
        
    }
    
}
