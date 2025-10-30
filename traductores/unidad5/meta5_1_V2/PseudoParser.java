
import java.util.ArrayList;
import java.io.FileWriter;
import java.io.IOException;

public class PseudoParser{
    private ArrayList<Token> tokens;
    private int indiceToken = 0;
    private SyntaxException ex;
    private TablaSimbolos ts;
    private TipoIncorporado real;
    
    private StringBuilder codigoC;
    private StringBuilder codigoPython;
    private int indentacion = 0;
    
    // Nombres de archivos de salida
    private String archivoC = "programa_generado.c";
    private String archivoPython = "programa_generado.py";

    public PseudoParser(TablaSimbolos ts) {
        this.ts = ts;
        this.codigoC = new StringBuilder();
        this.codigoPython = new StringBuilder();
    }
    
 
    public PseudoParser(TablaSimbolos ts, String archivoC, String archivoPython) {
        this(ts);
        this.archivoC = archivoC;
        this.archivoPython = archivoPython;
    }

    public void analizar(PseudoLexer lexer) throws SyntaxException, SemanticException, IOException {
        tokens = lexer.getTokens();
        real = new TipoIncorporado("real");
        
        try {
            ts.resolver("real");
        } catch (SemanticException e) {
            ts.definir(real);
        }

        codigoC.append("#include <stdio.h>\n\nint main() {\n");
        codigoPython.append("# Programa generado desde pseudocódigo\n\n");
        indentacion = 1;

        if (Programa()) {
            if (indiceToken == tokens.size()) {
                agregarLineaC("return 0;");
                agregarLineaC("}");
                
                // Generar archivos físicos
                generarArchivos();
                
                System.out.println("\n*** TRADUCCION COMPLETADA ***");
                System.out.println("\n=== CODIGO C (por pantalla) ===");
                System.out.println(codigoC.toString());
                System.out.println("\n=== CODIGO PYTHON (por pantalla) ===");
                System.out.println(codigoPython.toString());
                
                System.out.println("\nARCHIVOS GENERADOS:");
                System.out.println("codigo C - "+archivoC);
                System.out.println("codigo python -"+ archivoPython);
                
                return;
            }
        }

        if (ex == null) {
            ex = new SyntaxException("error de sintaxis");
        }
        throw ex;
    }
    
    private void generarArchivos() throws IOException {
        // Generar archivo C
        try (FileWriter writerC = new FileWriter(archivoC)) {
            writerC.write(codigoC.toString());
            writerC.flush();
        }
        
        // Generar archivo Python
        try (FileWriter writerPython = new FileWriter(archivoPython)) {
            writerPython.write(codigoPython.toString());
            writerPython.flush();
        }
    }

    private void agregarLineaC(String linea) {
        for (int i = 0; i < indentacion; i++) codigoC.append("    ");
        codigoC.append(linea).append("\n");
    }

    private void agregarLineaPython(String linea) {
        for (int i = 0; i < indentacion; i++) codigoPython.append("    ");
        codigoPython.append(linea).append("\n");
    }

    private boolean Programa() throws SemanticException {
        if (match(TipoToken.INICIOPROGRAMA)) {
            if (Declaracion()) {
                if (Enunciados()) {
                    if (match(TipoToken.FINPROGRAMA)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    private boolean Declaracion() throws SemanticException {
        // Verificar si hay declaracion de variables
        if (indiceToken >= tokens.size() || 
            !tokens.get(indiceToken).getTipo().getNombre().equals(TipoToken.VARIABLES)) {
            return true; // No hay declaracion, esto es valido
        }
        
        if (match(TipoToken.VARIABLES)) {
            if (match(TipoToken.DOSPUNTOS)) {
                if (match(TipoToken.VARIABLE)) {
                    String nombreVar = tokens.get(indiceToken - 1).getNombre();
                    ts.definir(new Variable(nombreVar, real));
                    agregarLineaC("double " + nombreVar + ";");
                    
                    // Usar lookahead para verificar si siguen mas variables
                    while (indiceToken < tokens.size() && 
                           tokens.get(indiceToken).getTipo().getNombre().equals(TipoToken.COMA)) {
                        match(TipoToken.COMA);
                        if (match(TipoToken.VARIABLE)) {
                            nombreVar = tokens.get(indiceToken - 1).getNombre();
                            ts.definir(new Variable(nombreVar, real));
                            agregarLineaC("double " + nombreVar + ";");
                        } else {
                            return false;
                        }
                    }
                    
                    agregarLineaPython("# Variables declaradas");
                    agregarLineaC("");
                    agregarLineaPython("");
                    return true;
                }
            }
        }
        return true;
    }

    private boolean Asignacion() throws SemanticException {
        int indiceAux = indiceToken;
        
        if (match(TipoToken.VARIABLE)) {
            String variable = tokens.get(indiceToken - 1).getNombre();
            Variable v = null;
            
            try {
                v = (Variable) ts.resolver(variable);
            } catch (SemanticException e) {
                // Variable no declarada, pero continuamos
                System.out.println("Advertencia: Variable '" + variable + "' no declarada");
            }

            if (match(TipoToken.IGUAL)) {
                String expresion = obtenerExpresion();
                if (expresion != null) {
                    agregarLineaC(variable + " = " + expresion + ";");
                    agregarLineaPython(variable + " = " + expresion);
                    return true;
                }
            }
        }

        indiceToken = indiceAux;
        return false;
    }

    private boolean Leer() {
        int indiceAux = indiceToken;

        if (match(TipoToken.LEER)) {
            if (match(TipoToken.CADENA)) {
                String mensaje = tokens.get(indiceToken - 1).getNombre();
                if (match(TipoToken.COMA)) {
                    if (match(TipoToken.VARIABLE)) {
                        String variable = tokens.get(indiceToken - 1).getNombre();
                        
                        agregarLineaC("printf(\"" + mensaje + " \");");
                        agregarLineaC("scanf(\"%lf\", &" + variable + ");");
                        agregarLineaPython(variable + " = float(input('" + mensaje + "'))");
                        return true;
                    }
                }
            }
        }

        indiceToken = indiceAux;
        return false;
    }

    private boolean Escribir() {
        int indiceAux = indiceToken;

        if (match(TipoToken.ESCRIBIR)) {
            if (match(TipoToken.CADENA)) {
                String mensaje = tokens.get(indiceToken - 1).getNombre();
                if (match(TipoToken.COMA)) {
                    if (match(TipoToken.VARIABLE)) {
                        String variable = tokens.get(indiceToken - 1).getNombre();
                        
                        agregarLineaC("printf(\"" + mensaje + " %f\\n\", " + variable + ");");
                        agregarLineaPython("print('" + mensaje + "', " + variable + ")");
                        return true;
                    }
                }
                // Solo cadena, sin variable
                agregarLineaC("printf(\"" + mensaje + "\\n\");");
                agregarLineaPython("print('" + mensaje + "')");
                return true;
            }
        }

        indiceToken = indiceAux;

        // Intentar escribir solo variable
        if (match(TipoToken.ESCRIBIR)) {
            if (match(TipoToken.VARIABLE)) {
                String variable = tokens.get(indiceToken - 1).getNombre();
                agregarLineaC("printf(\"%f\\n\", " + variable + ");");
                agregarLineaPython("print(" + variable + ")");
                return true;
            }
        }

        indiceToken = indiceAux;
        return false;
    }

    private boolean Si() throws SemanticException {
        int indiceAux = indiceToken;

        if (match(TipoToken.SI)) {
            String condicion = obtenerComparacion();
            if (condicion != null) {
                if (match(TipoToken.ENTONCES)) {
                    agregarLineaC("if (" + condicion + ") {");
                    agregarLineaPython("if " + condicion + ":");
                    indentacion++;
                    
                    if (Enunciados()) {
                        indentacion--;
                        if (match(TipoToken.FINSI)) {
                            agregarLineaC("}");
                            return true;
                        }
                    }
                }
            }
        }

        indiceToken = indiceAux;
        return false;
    }

    private boolean Mientras() throws SemanticException {
        int indiceAux = indiceToken;

        if (match(TipoToken.MIENTRAS)) {
            String condicion = obtenerComparacion();
            if (condicion != null) {
                agregarLineaC("while (" + condicion + ") {");
                agregarLineaPython("while " + condicion + ":");
                indentacion++;
                
                if (Enunciados()) {
                    indentacion--;
                    if (match(TipoToken.FINMIENTRAS)) {
                        agregarLineaC("}");
                        return true;
                    }
                }
            }
        }

        indiceToken = indiceAux;
        return false;
    }

    private String obtenerExpresion() {
        StringBuilder expr = new StringBuilder();
        int inicio = indiceToken;
        int tokensConsumidos = 0;
    
        // Consumir primer operando (variable o número)
        if (indiceToken < tokens.size()) {
            String tipo = tokens.get(indiceToken).getTipo().getNombre();
            if (tipo.equals(TipoToken.VARIABLE) || tipo.equals(TipoToken.NUMERO)) {
                expr.append(tokens.get(indiceToken).getNombre());
                indiceToken++;
                tokensConsumidos++;
                
                // Verificar si hay operador y segundo operando
                if (indiceToken < tokens.size() && 
                    tokens.get(indiceToken).getTipo().getNombre().equals(TipoToken.OPARITMETICO)) {
                    
                    // Consumir operador
                    expr.append(tokens.get(indiceToken).getNombre());
                    indiceToken++;
                    tokensConsumidos++;
                    
                    // Consumir segundo operando si existe
                    if (indiceToken < tokens.size()) {
                        String tipo2 = tokens.get(indiceToken).getTipo().getNombre();
                        if (tipo2.equals(TipoToken.VARIABLE) || tipo2.equals(TipoToken.NUMERO)) {
                            expr.append(tokens.get(indiceToken).getNombre());
                            indiceToken++;
                            tokensConsumidos++;
                        }
                    }
                }
            }
        }
    
        if (tokensConsumidos == 0) {
            indiceToken = inicio;
            return null;
        }
    
        return expr.toString();
    }

    private String obtenerComparacion() {
        int inicio = indiceToken;
        if (!match(TipoToken.PARENTESISIZQ)) {
            return null;
        }
    
        StringBuilder comp = new StringBuilder();
        boolean encontradoParentesisDer = false;
        
        while (indiceToken < tokens.size() && !encontradoParentesisDer) {
            String tipo = tokens.get(indiceToken).getTipo().getNombre();
            if (tipo.equals(TipoToken.PARENTESISDER)) {
                encontradoParentesisDer = true;
                indiceToken++;
                break;
            }
            if (tipo.equals(TipoToken.VARIABLE) || tipo.equals(TipoToken.NUMERO) || 
                tipo.equals(TipoToken.OPRELACIONAL)) {
                comp.append(tokens.get(indiceToken).getNombre());
                indiceToken++;
            } else {
                break;
            }
        }
        
        if (!encontradoParentesisDer) {
            indiceToken = inicio;
            return null;
        }
        
        return comp.toString();
    }

    private boolean Enunciados() throws SemanticException {
        // Enunciados puede ser vacio
        // Procesamos todos los enunciados que encontremos
        while (Enunciado()) {
            // Continuar procesando
        }
        // Siempre retornamos true porque una lista vacia de enunciados es valida
        return true;
    }

    private boolean Enunciado() throws SemanticException {
        // Verificar que no estemos al final
        if (indiceToken >= tokens.size()) {
            return false;
        }
        
        String tipoActual = tokens.get(indiceToken).getTipo().getNombre();
        int indiceAux = indiceToken;

        // Intentar asignacion
        if (tipoActual.equals(TipoToken.VARIABLE)) {
            if (Asignacion()) return true;
        }
        indiceToken = indiceAux;

        // Intentar leer
        if (tipoActual.equals(TipoToken.LEER)) {
            if (Leer()) return true;
        }
        indiceToken = indiceAux;

        // Intentar escribir
        if (tipoActual.equals(TipoToken.ESCRIBIR)) {
            if (Escribir()) return true;
        }
        indiceToken = indiceAux;

        // Intentar si
        if (tipoActual.equals(TipoToken.SI)) {
            if (Si()) return true;
        }
        indiceToken = indiceAux;
        
        // Intentar mientras
        if (tipoActual.equals(TipoToken.MIENTRAS)) {
            if (Mientras()) return true;
        }
        indiceToken = indiceAux;

        return false;
    }

    private boolean match(String nombre) {
        if (indiceToken < tokens.size() && 
           tokens.get(indiceToken).getTipo().getNombre().equals(nombre)) {
            indiceToken++;
            return true;
        }
        
        if (ex == null) {
            String encontrado = (indiceToken < tokens.size()) ? 
                tokens.get(indiceToken).getTipo().getNombre() : "EOF";
            ex = new SyntaxException(nombre, encontrado);
        }

        return false;
    }
}