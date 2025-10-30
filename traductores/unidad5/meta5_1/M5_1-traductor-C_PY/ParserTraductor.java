public class ParserTraductor {
    private Translator translator;
    private Scope currentScope;
    
    public void assignment() {
        // Ejemplo: x = 5
        if (match("VARIABLE")) {
            String varName = previousToken().getText();
            match("IGUAL");
            String expr = expression();
            
            // Generar código C
            translator.genC(getIndent() + varName + " = " + expr + ";\n");
            
            // Generar código Python
            translator.genPython(getIndent() + varName + " = " + expr + "\n");
        }
    }
    
    public void ifStatement() {
        match("SI");
        String condition = expression();
        match("ENTONCES");
        
        // C
        translator.genC(getIndent() + "if (" + condition + ") {\n");
        // Python
        translator.genPython(getIndent() + "if " + condition + ":\n");
        
        translator.increaseIndent();
        statementList();
        translator.decreaseIndent();
        
        match("FIN_SI");
        
        // Solo para C
        translator.genC(getIndent() + "}\n");
    }
    
    public void whileLoop() {
        match("MIENTRAS");
        String condition = expression();
        match("HACER");
        
        // C
        translator.genC(getIndent() + "while (" + condition + ") {\n");
        // Python
        translator.genPython(getIndent() + "while " + condition + ":\n");
        
        translator.increaseIndent();
        statementList();
        translator.decreaseIndent();
        
        match("FIN_MIENTRAS");
        
        // Solo para C
        translator.genC(getIndent() + "}\n");
    }
}