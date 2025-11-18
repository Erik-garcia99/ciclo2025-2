// import java.util.ArrayList;

// public class PseudoInterprete {
//     TablaSimbolos ts;

//     public PseudoInterprete(TablaSimbolos ts){
//         this.ts = ts;
//     }

//     public void interpretar(ArrayList<Tupla> tuplas) throws SemanticException{
//         int indiceTupla=0;
//         Tupla t = tuplas.get(0);

//         do{
//             indiceTupla= t.ejecutar(ts);
            
//             t = tuplas.get(indiceTupla);
//         }while(!(t instanceof FinPrograma));
//     }


//     // public void interpretar(ArrayList<Tupla> tuplas) throws SemanticException{
//     //     int indiceTupla=0;
//     //     Tupla t = tuplas.get(0);

//     //     while(true) {
//     //         indiceTupla = t.ejecutar(ts); 


//     //         if (indiceTupla < 0) {
//     //             break; 
//     //         }

    
//     //         t = tuplas.get(indiceTupla);
//     //     }

//     // }
// }


import java.util.ArrayList;

public class PseudoInterprete {
    TablaSimbolos ts;

    public PseudoInterprete(TablaSimbolos ts){
        this.ts = ts;
    }

    public void interpretar(ArrayList<Tupla> tuplas) throws SemanticException {
        int indiceTupla = 0;
        Tupla t = tuplas.get(0);
        
        // Protección contra bucles infinitos
        int maxIteraciones = 10000;
        int iteraciones = 0;
        
        while (true) {
            iteraciones++;
            if (iteraciones > maxIteraciones) {
                System.err.println("ERROR: Bucle infinito detectado");
                break;
            }
            
            indiceTupla = t.ejecutar(ts);
            
            // Condición de término
            if (indiceTupla < 0 || t instanceof FinPrograma) {
                break;
            }
            
            // Verificar índice válido
            if (indiceTupla >= tuplas.size()) {
                System.err.println("ERROR: Índice de tupla inválido: " + indiceTupla);
                break;
            }
            
            t = tuplas.get(indiceTupla);
        }
    }
}