package com.pseudothinkrun.intermediate;
import java.util.*;
import com.pseudothinkrun.symbols.*;

public class SiTupla extends Tupla {
    private ComparacionTupla condicion;
    private List<Tupla> instruccionesEntonces = new ArrayList<>();
    private List<Tupla> instruccionesSino = new ArrayList<>();

    public SiTupla(ComparacionTupla condicion, int sv, int sf) {
        super(sv, sf);
        this.condicion = condicion;
    }

    public void agregarInstruccionEntonces(Tupla t) { instruccionesEntonces.add(t); }
    public void agregarInstruccionSino(Tupla t) { instruccionesSino.add(t); }

    @Override
    public int ejecutar(SymbolTable st) {
        // Ejecutamos la condición: devuelve 'saltoVerdadero' (true) o 'saltoFalso' (false)
        int resultado = condicion.ejecutar(st);
        
        if (resultado == 1) {  // true
            for(Tupla t : instruccionesEntonces) t.ejecutar(st);
        } else {  // false
            for(Tupla t : instruccionesSino) t.ejecutar(st);
        }
        return saltoVerdadero;
    }
    @Override public String getDescripcion() { return "SI ..."; }
}