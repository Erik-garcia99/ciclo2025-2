package com.pseudothinkrun.interpreter;

import com.pseudothinkrun.intermediate.*;
import com.pseudothinkrun.symbols.*;
import java.util.*;

public class Interpreter {
    private SymbolTable symbolTable;
    private List<Tupla> tuplas;

    public Interpreter(SymbolTable symbolTable) {
        this.symbolTable = symbolTable;
    }

    public void interpretar(List<Tupla> tuplas) {
        this.tuplas = tuplas;
        System.out.println("\n=== INICIO EJECUCIÓN ===\n");
        
        int pc = 0; // Program Counter
        while (pc < tuplas.size() && pc >= 0) {
            Tupla tuplaActual = tuplas.get(pc);
            
            // Ejecutar la tupla actual
            int resultado = tuplaActual.ejecutar(symbolTable);
            
            // Manejar diferentes códigos de retorno:
            
            // -1: Fin de programa o error fatal
            if (resultado == -1) {
                break;
            }
            
            // -2: Retorno de función (continuar normalmente)
            if (resultado == -2) {
                pc++;
                continue;
            }
            
            // Para MientrasTupla y RepiteTupla: 
            // Estas tuplas manejan su lógica internamente y siempre
            // devuelven saltoVerdadero (siguiente instrucción)
            // Por lo tanto, simplemente avanzamos al siguiente PC
            pc++;
        }
        
        System.out.println("\n=== FIN EJECUCIÓN ===");
    }
}