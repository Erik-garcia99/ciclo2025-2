import java.io.FileReader;
import java.io.IOException;

public class PseudocodeToCLPython {
    public static void main(String[] args) {
        try {
            // Leer archivo de pseudocódigo
            String input = leerArchivo("programa.txt");
            
            // Inicializar componentes
            Lexer lexer = new Lexer(input);
            Translator translator = new Translator();
            ParserTraductor parser = new ParserTraductor(lexer, translator);
            
            // Traducir
            translator.translateProgram();
            parser.parse();
            translator.endProgram();
            
            // Guardar archivos generados
            guardarArchivo("programa.c", translator.getCodigoC());
            guardarArchivo("programa.py", translator.getCodigoPython());
            
            System.out.println("Traducción completada exitosamente!");
            
        } catch (Exception e) {
            System.err.println("Error en traducción: " + e.getMessage());
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