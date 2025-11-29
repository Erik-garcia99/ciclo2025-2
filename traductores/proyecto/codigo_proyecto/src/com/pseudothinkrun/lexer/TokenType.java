package com.pseudothinkrun.lexer;

public enum TokenType {

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    // Palabras reservadas
    DEFINIR, COMO, ENTERO, REAL, CADENA, LOGICO, VERDADERO, FALSO,
    TIPO, FINTIPO,
    SI, ENTONCES, SINO, FINSI,
    MIENTRAS, HACER, FINMIENTRAS,
    REPITE, VECES, FINREPITE,
    FUNCION, RETORNAR, FINFUNCION,
    ESCRIBIR, LEER,
    
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // Operadores aritmeticos
    SUMA("+"), RESTA("-"), MULTIPLICACION("*"), DIVISION("/"),
    //operadores de comparacion 
    ASIGNACION("="), MENOR_QUE("<"), MAYOR_QUE(">"), MENOR_IGUAL("<="), MAYOR_IGUAL(">="), IGUAL("=="), DISTINTO("!="),
    //operadores logicos 
    Y("Y"), 
    O("O"), 
    NO("NO"),

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    // Puntuación
    COMA(","), PUNTO("."), PARENTESIS_IZQ("("), PARENTESIS_DER(")"),
    
    // Literales e identificadores
    IDENTIFICADOR, NUMERO_ENTERO, NUMERO_REAL, CADENA_LITERAL,
    
    // Fin de archivo
    EOF, FINPROGRAMA;
    
    private final String symbol;
    
    TokenType() { this.symbol = null; }
    TokenType(String symbol) { this.symbol = symbol; }
    
    public String getSymbol() { return symbol; }
}