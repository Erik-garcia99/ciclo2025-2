package com.pseudothinkrun.intermediate;

import com.pseudothinkrun.symbols.*;
import java.util.*;

public class RepiteTupla extends Tupla {
    private int iteraciones;
    private List<Tupla> instrucciones;

    public RepiteTupla(int iteraciones, int sv, int sf) {
        super(sv, sf);
        this.iteraciones = iteraciones;
        this.instrucciones = new ArrayList<>();
    }

    public void agregarInstruccion(Tupla instruccion) {
        instrucciones.add(instruccion);
    }

    @Override
    public int ejecutar(SymbolTable symbolTable) {
        for (int i = 0; i < iteraciones; i++) {
            for (Tupla instruccion : instrucciones) {
                int salto = instruccion.ejecutar(symbolTable);
                if (salto == -1) return -1;
            }
        }
        return saltoVerdadero;
    }

    @Override
    public String getDescripcion() {
        return "REPITE " + iteraciones + " VECES";
    }
}