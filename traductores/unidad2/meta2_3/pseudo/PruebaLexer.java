// Archivo: PruebaLexer.java
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;

public class PruebaLexer {

    public static void main(String[] args) {
        try {
            
            String rutaArchivo = "algo.txt"; 
            String entrada = leerPrograma(rutaArchivo);
            
            if (entrada.isEmpty()) {
                System.out.println("El archivo está vacío o no se pudo leer.");
                return;
            }

            PseudoLexer lexer = new PseudoLexer();
            lexer.analizar(entrada);
            
            System.out.println("*** Análisis léxico ***");
            for (Token t : lexer.getTokens()) {
                System.out.println(t);
            }

        } catch (Exception e) {
            System.err.println("Error durante el análisis: " + e.getMessage());
        }
    }

    private static String leerPrograma(String nombre) {
        StringBuilder entrada = new StringBuilder();
        try (FileReader reader = new FileReader(nombre)) {
            int caracter;
            while ((caracter = reader.read()) != -1) {
                entrada.append((char) caracter);
            }
        } catch (IOException e) {
            System.err.println("Error al leer el archivo: " + e.getMessage());
            return "";
        }
        return entrada.toString();
    }
}