package com.pseudothinkrun.intermediate;
import java.util.*;
import com.pseudothinkrun.symbols.*;

/**
 * Tupla MIENTRAS que maneja el bucle completo internamente
 * 
 * Estrategia:
 * 1. Evalúa condición en cada iteración
 * 2. Si es verdadera: ejecuta instrucciones
 * 3. Repite desde paso 1
 * 4. Si es falsa: sale y retorna control al siguiente PC
 * 
 * Por qué manejo interno:
 * - Simplifica el Interpreter (no necesita gestionar saltos hacia atrás)
 * - Evita problemas de estado compartido entre iteraciones
 * - Más cercano a la semántica natural del bucle MIENTRAS
 */

public class MientrasTupla extends Tupla {
    private ComparacionTupla condicion;
    private List<Tupla> instrucciones = new ArrayList<>();

    public MientrasTupla(ComparacionTupla condicion, int sv, int sf) {
        super(sv, sf);
        this.condicion = condicion;
    }
    
    public void agregarInstruccion(Tupla t) { 
        instrucciones.add(t); 
    }

    @Override
    public int ejecutar(SymbolTable st) {
       
        while (true) {
            // Evaluar condición en cada iteración
            int resultadoCondicion = condicion.ejecutar(st);
            
            // Si condición es falsa (0), salir del bucle
            if (resultadoCondicion == 0) {
                break;
            }
            
            // Si condición es verdadera, ejecutar cuerpo del bucle
            for (Tupla instruccion : instrucciones) {
                int res = instruccion.ejecutar(st);
                
                // Propagar errores fatales
                if (res == -1) {
                    return -1;
                }
                
                // Si hay un RETORNAR dentro del bucle, propagar
                if (res == -2) {
                    return -2;
                }
            }
            // Después de ejecutar todas las instrucciones,
            // el bucle vuelve a evaluar la condición automáticamente
        }
        
        // Cuando la condición es falsa, continuar con siguiente instrucción
        return saltoVerdadero;
    }

    @Override 
    public String getDescripcion() { 
        return "MIENTRAS (" + condicion.getDescripcion() + ")"; 
    }
}