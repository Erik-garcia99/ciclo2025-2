package com.pseudothinkrun;
import java.nio.charset.StandardCharsets;
import com.pseudothinkrun.lexer.*;
import com.pseudothinkrun.parser.*;
import com.pseudothinkrun.symbols.*;
import com.pseudothinkrun.intermediate.Tupla;
import com.pseudothinkrun.interpreter.*;
import java.io.*;
import java.nio.file.*;
import java.util.*;


public class Main {
    public static void main(String[] args) {
        // Verificar que se haya pasado el archivo como argumento
        if (args.length != 1) {
            System.out.println("compilar: javac -d out -sourcepath src src\\com\\pseudothinkrun\\Main.java");
            
            System.out.println("debe de encontrarse en la ruta raiz del proyecto.");

            System.err.println("Uso: java -cp out com.pseudothinkrun.Main <archivo.psdo>");

            // System.err.println("Ejemplo: java com.pseudothinkrun.Main programa.psdo");
            System.exit(1);
        }

        String archivoPseudo = args[0];
        
        // Validar extensión del archivo
        if (!archivoPseudo.endsWith(".psdo")) {
            System.err.println("Error: El archivo debe tener extensión .psdo");
            System.exit(1);
        }

        // Verificar que el archivo existe
        File archivo = new File(archivoPseudo);
        if (!archivo.exists()) {
            System.err.println("Error: El archivo '" + archivoPseudo + "' no existe");
            System.exit(1);
        }

        try {
            // Leer todo el contenido del archivo
            String codigo = leerArchivoCompleto(archivoPseudo);
            
            // Mostrar archivo que se esta ejecutando
            System.out.println("=== EJECUTANDO: " + archivoPseudo + " ===\n");

            // Fase 1: Analisis Léxico
            Lexer lexer = new Lexer(codigo);
            List<Token> tokens = lexer.tokenize();
            
            // Opcional: Mostrar tokens (util para depuracion)
            System.out.println("=== TOKENS GENERADOS ===");
            for (Token token : tokens) {
                System.out.println(token);
            }
            
            // Fase 2: Anslisis Sintactico y Generación de Tuplas
            SymbolTable symbolTable = new SymbolTable();
            PseudoParser parser = new PseudoParser(tokens, symbolTable);
            List<Tupla> tuplas = parser.parse();
            
            System.out.println("\n=== TUPLAS GENERADAS ===");
            for (int i = 0; i < tuplas.size(); i++) {
                System.out.printf("%d: %s\n", i + 1, tuplas.get(i).getDescripcion());
            }
            
            // Fase 3: Ejecucion
            Interpreter interpreter = new Interpreter(symbolTable);
            interpreter.interpretar(tuplas);
            
        } catch (IOException e) {
            System.err.println("Error al leer el archivo: " + e.getMessage());
            System.exit(1);
        } catch (Exception e) {
            System.err.println("Error durante la ejecución: " + e.getMessage());
            e.printStackTrace();
            System.exit(1);
        }
    }

    /**
     * Lee todo el contenido de un archivo y lo devuelve como String
     * @param rutaArchivo Ruta del archivo a leer
     * @return Contenido del archivo
     * @throws IOException Si ocurre un error al leer
     */
    private static String leerArchivoCompleto(String rutaArchivo) throws IOException {
        String content = new String(Files.readAllBytes(Paths.get(rutaArchivo)), 
                                    StandardCharsets.UTF_8);
        // Eliminar BOM UTF-8 (U+FEFF) si existe
        if (!content.isEmpty() && content.charAt(0) == '\uFEFF') {
            content = content.substring(1);
        }
        return content;
    }
}



/*
compilacion 

desde windows 
New-Item -ItemType Directory -Force -Path out

# Compila todo en un SOLO comando
javac -d out -sourcepath src src\com\pseudothinkrun\Main.java

//ejecutar
java -cp out com.pseudothinkrun.Main <archivo.psdo>


probelma con el archivo BOM 

*/
