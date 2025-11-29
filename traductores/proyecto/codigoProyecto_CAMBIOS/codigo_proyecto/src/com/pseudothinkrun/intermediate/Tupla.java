//clase base abstracta 

package com.pseudothinkrun.intermediate;

import com.pseudothinkrun.symbols.*;

/**
 * Clase base abstracta para el patrón Command
 * 
 * Propósito: Cada instrucción del pseudocódigo se convierte en un objeto
 * ejecutable con método ejecutar(). Esto permite:
 * - Flujo de control mediante punteros de salto (saltoVerdadero/saltoFalso)
 * - Ejecución polimórfica en el Interpreter
 * - Debugging: getDescripcion() muestra instrucción humana
 * 
 * Inspirado en código de 3 direcciones pero simplificado para pedagogía
 */

public abstract class Tupla {
    protected int saltoVerdadero;
    protected int saltoFalso;

    public Tupla(int saltoVerdadero, int saltoFalso) {
        this.saltoVerdadero = saltoVerdadero;
        this.saltoFalso = saltoFalso;
    }

    public int getSaltoVerdadero() { return saltoVerdadero; }
    public int getSaltoFalso() { return saltoFalso; }
    
    public void setSaltoVerdadero(int sv) { this.saltoVerdadero = sv; }
    public void setSaltoFalso(int sf) { this.saltoFalso = sf; }

    /**
     * Método ejecutar() implementa lógica específica de cada instrucción
     * 
     * Retorno:
     * -1: Fin de programa o error fatal
     * -2: Retorno de función (código especial)
     * n >= 0: Índice de la siguiente tupla a ejecutar
     * 
     * Por qué: Permite salto condicional sin goto explícito en Interpreter
     */
    public abstract int ejecutar(SymbolTable symbolTable);
    
    // Método abstracto para obtener descripción
    public abstract String getDescripcion();
}