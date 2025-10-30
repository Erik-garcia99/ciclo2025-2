public class Translator {
    private StringBuilder codigoC;
    private StringBuilder codigoPython;
    private int indentLevel;
    
    public Translator() {
        codigoC = new StringBuilder();
        codigoPython = new StringBuilder();
        indentLevel = 0;
    }
    
    private String getIndent() {
        return "    ".repeat(indentLevel);
    }
    
    public void translateProgram() {
        // Cabeceras para C
        codigoC.append("#include <stdio.h>\n\n");
        codigoC.append("int main() {\n");
        indentLevel++;
        
        // Python no necesita cabecera especial
        indentLevel++;
    }
    
    public void endProgram() {
        indentLevel--;
        codigoC.append(getIndent()).append("return 0;\n");
        codigoC.append("}\n");
        
        indentLevel--;
    }


    public void generateRead(String variable, String message) {
    // C
    codigoC.append(getIndent()).append("printf(\"").append(message).append(" \");\n");
    codigoC.append(getIndent()).append("scanf(\"%d\", &").append(variable).append(");\n");
        
        // Python
    codigoPython.append(getIndent()).append(variable).append(" = int(input(\"").append(message).append("\"))\n");
    }


    public void generateWrite(String message, String expression) {
        // C
        codigoC.append(getIndent()).append("printf(\"").append(message).append(" %d\\n\", ").append(expression).append(");\n");
        
        // Python
        codigoPython.append(getIndent()).append("print(\"").append(message).append("\", ").append(expression).append(")\n");
    }
}