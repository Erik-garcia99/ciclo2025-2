import java.io.FileReader;
import java.io.IOException;

public class PruebaConArchivos {
    
    public static void main(String[] args) {
        try {
            System.out.println("=== GENERANDO ARCHIVOS .C Y .PY ===\n");
            
            String entrada = leerPrograma("programa.txt");
            PseudoLexer lexer = new PseudoLexer();
            lexer.analizar(entrada);

            System.out.println("*** Analisis lexico ***\n");
            for(Token t: lexer.getTokens()){
                System.out.println(t);
            }

            System.out.println("\n*** Analisis sintactico y generacion de archivos ***\n");
            
            TablaSimbolos ts = new TablaSimbolos();
            
            // Usar el parser que genera archivos
            PseudoParser parser = new PseudoParser(ts);
            parser.analizar(lexer);

            System.out.println("\n*** Tabla de simbolos ***");
            for(Simbolo s: ts.getSimbolos()){
                System.out.println(s);
            }
            
            System.out.println("\nproceso terminado");
            System.out.println("Los archivos se han generado en el directorio actual.");
            
        } catch (Exception e) {
            System.out.println("\nERROR: " + e.getMessage());
            e.printStackTrace();
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
            System.out.println("Error leyendo archivo: " + e.getMessage());
            return "";
        }
    }
}