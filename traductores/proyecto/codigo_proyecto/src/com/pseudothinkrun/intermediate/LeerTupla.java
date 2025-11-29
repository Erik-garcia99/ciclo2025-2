package com.pseudothinkrun.intermediate;

import com.pseudothinkrun.symbols.*;
import java.util.Scanner;

public class LeerTupla extends Tupla {
    private String variableName;
    
    // === CORRECCIÓN: Scanner estático (único para todo el programa) ===
    private static final Scanner scanner = new Scanner(System.in);

    public LeerTupla(String variableName, int sv, int sf) {
        super(sv, sf);
        this.variableName = variableName;
    }

    @Override
    public int ejecutar(SymbolTable symbolTable) {
        VariableSymbol var = symbolTable.resolveVariable(variableName);
        
        if (var != null) {
            // Nota: He simplificado el texto para que se vea más limpio en consola
            System.out.print("> "); 
            
            try {
                String input = scanner.nextLine(); // Leemos la línea completa siempre
                
                String typeName = var.getType().getName();
                
                if (typeName.equals("ENTERO")) {
                    var.setValue(Integer.parseInt(input));
                } else if (typeName.equals("REAL")) {
                    var.setValue(Double.parseDouble(input));
                } else if (typeName.equals("CADENA")) {
                    var.setValue(input);
                } else if (typeName.equals("LOGICO")) {
                    var.setValue(Boolean.parseBoolean(input));
                }
            } catch (Exception e) {
                System.err.println("Error: Entrada inválida para " + variableName + " (" + var.getType().getName() + ")");
                // Si falla, podrías querer detener el programa retornando -1
                return -1; 
            }
        } else {
            System.err.println("Error: Variable no definida '" + variableName + "'");
            return saltoFalso;
        }
        
        return saltoVerdadero;
    }

    @Override
    public String getDescripcion() {
        return "LEER " + variableName;
    }
}