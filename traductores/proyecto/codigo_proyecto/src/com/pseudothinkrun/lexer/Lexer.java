package com.pseudothinkrun.lexer;

import java.util.*;
import java.io.*;


/**
 * Analizador léxico que convierte flujo de caracteres en tokens estructurados
 * 
 * Estrategia de implementacin:
 * - Maquina de estados implícita usando métodos especializados
 * - Lookahead de 1 caracter para operadores compuestos (<=, ==)
 * - Reporting de errores con posición exacta (línea/columna)
 * 
 * Decisiones clave:
 * - Comentarios se eliminan en lugar de generar tokens (eficiencia)
 * - Cadenas soportan escaping con \" para comillas internas
 * - Números reales pueden tener notación científica (e/E)
 */

public class Lexer {
    private String input;
    private int pos = 0;
    private int line = 1;
    private int column = 1;
    private char currentChar;
    
    // Mapeo de palabras reservadas
    private static final Map<String, TokenType> keywords = new HashMap<>();
    private static final Map<String, TokenType> operators = new HashMap<>();
    
    static {
        // Palabras reservadas
        keywords.put("DEFINIR", TokenType.DEFINIR);
        keywords.put("COMO", TokenType.COMO);
        keywords.put("ENTERO", TokenType.ENTERO);
        keywords.put("REAL", TokenType.REAL);
        keywords.put("CADENA", TokenType.CADENA);
        keywords.put("LOGICO", TokenType.LOGICO);
        keywords.put("VERDADERO", TokenType.VERDADERO);
        keywords.put("FALSO", TokenType.FALSO);
        keywords.put("TIPO", TokenType.TIPO);
        keywords.put("FINTIPO", TokenType.FINTIPO);
        keywords.put("SI", TokenType.SI);
        keywords.put("ENTONCES", TokenType.ENTONCES);
        keywords.put("SINO", TokenType.SINO);
        keywords.put("FINSI", TokenType.FINSI);
        keywords.put("MIENTRAS", TokenType.MIENTRAS);
        keywords.put("HACER", TokenType.HACER);
        keywords.put("FINMIENTRAS", TokenType.FINMIENTRAS);
        keywords.put("REPITE", TokenType.REPITE);
        keywords.put("VECES", TokenType.VECES);
        keywords.put("FINREPITE", TokenType.FINREPITE);
        keywords.put("FUNCION", TokenType.FUNCION);
        keywords.put("RETORNAR", TokenType.RETORNAR);
        keywords.put("FINFUNCION", TokenType.FINFUNCION);
        keywords.put("FINFUNCION", TokenType.FINPROGRAMA);
        keywords.put("ESCRIBIR", TokenType.ESCRIBIR);
        keywords.put("LEER", TokenType.LEER);
        keywords.put("Y", TokenType.Y);
        keywords.put("O", TokenType.O);
        keywords.put("NO", TokenType.NO);
        
        // Operadores compuestos
        operators.put("<=", TokenType.MENOR_IGUAL);
        operators.put(">=", TokenType.MAYOR_IGUAL);
        operators.put("==", TokenType.IGUAL);
        operators.put("!=", TokenType.DISTINTO);

        keywords.put("FUNCION", TokenType.FUNCION);
        keywords.put("RETORNAR", TokenType.RETORNAR);
        keywords.put("FINFUNCION", TokenType.FINFUNCION);
    }
    
    public Lexer(String input) {
        this.input = input;
        this.currentChar = input.length() > 0 ? input.charAt(0) : '\0';
    }
    
    /**
     * Metodo principal de tokenización
     * Recorre todo el input generando tokens hasta EOF
     * Excepción detallada con Unicode para caracteres no reconocidos
     */
    public List<Token> tokenize() throws Exception {
        List<Token> tokens = new ArrayList<>();
        Token token;
        
        do {
            token = nextToken();
            tokens.add(token);
        } while (token.getType() != TokenType.EOF);
        
        return tokens;
    }
    /**
     * nextToken() implementa el corazon del lexer con lógica de lookahead
     * Orden de verificación:
     * 1. Operadores 2 caracteres (<=, ==)
     * 2. Operadores 1 caracter (+, -)
     * 3. Identificadores/palabras reservadas
     * 4. Numeros (enteros y reales con notación cientifica)
     * 5. Cadenas literales
     * 
     * Por qué este orden: Evita que "==" se tokenice como "=" "="
     */
    private Token nextToken() throws Exception {
        skipWhitespace();
        skipComments();
        
        if (isEOF()) {
            return new Token(TokenType.EOF, "", line, column);
        }
        
        // Operadores de dos caracteres
        if (currentChar == '<' && nextCharIs('=')) {
            consume();
            consume();
            return new Token(TokenType.MENOR_IGUAL, "<=", line, column - 1);
        }
        if (currentChar == '>' && nextCharIs('=')) {
            consume();
            consume();
            return new Token(TokenType.MAYOR_IGUAL, ">=", line, column - 1);
        }
        if (currentChar == '=' && nextCharIs('=')) {
            consume();
            consume();
            return new Token(TokenType.IGUAL, "==", line, column - 1);
        }
        if (currentChar == '!' && nextCharIs('=')) {
            consume();
            consume();
            return new Token(TokenType.DISTINTO, "!=", line, column - 1);
        }
        
        // Operadores de un caracter
        switch (currentChar) {
            case '+': consume(); return new Token(TokenType.SUMA, "+", line, column - 1);
            case '-': consume(); return new Token(TokenType.RESTA, "-", line, column - 1);
            case '*': consume(); return new Token(TokenType.MULTIPLICACION, "*", line, column - 1);
            case '/': consume(); return new Token(TokenType.DIVISION, "/", line, column - 1);
            case '=': consume(); return new Token(TokenType.ASIGNACION, "=", line, column - 1);
            case '<': consume(); return new Token(TokenType.MENOR_QUE, "<", line, column - 1);
            case '>': consume(); return new Token(TokenType.MAYOR_QUE, ">", line, column - 1);
            case ',': consume(); return new Token(TokenType.COMA, ",", line, column - 1);
            case '.': consume(); return new Token(TokenType.PUNTO, ".", line, column - 1);
            case '(': consume(); return new Token(TokenType.PARENTESIS_IZQ, "(", line, column - 1);
            case ')': consume(); return new Token(TokenType.PARENTESIS_DER, ")", line, column - 1);
        }
        
        // Identificadores y palabras reservadas
        if (isLetter()) {
            return readIdentifierOrKeyword();
        }
        
        // Números
        if (isDigit()) {
            return readNumber();
        }
        
        // Cadenas literales
        if (currentChar == '"') {
            return readString();
        }
        
        // throw new Exception("Caracter inesperado '" + currentChar + "' en línea " + line + ", columna " + column);
        int unicode = (int) currentChar;
        throw new Exception(String.format("Caracter inesperado '%s' (Unicode: U+%04X) en línea %d, columna %d",currentChar, unicode, line, column));
    }
    
    private Token readIdentifierOrKeyword() {
        StringBuilder sb = new StringBuilder();
        int startLine = line;
        int startColumn = column;
        
        while (isLetterOrDigit() || currentChar == '_') {
            sb.append(currentChar);
            consume();
        }
        
        String text = sb.toString();
        TokenType type = keywords.getOrDefault(text, TokenType.IDENTIFICADOR);
        
        return new Token(type, text, startLine, startColumn);
    }
    /**
     * readNumber() soporta:
     * - Enteros: 123
     * - Reales: 123.45
     * - Cientifica: 1.23e10, 5E-3
     * Importancia: Unifica manejo de numericos para operaciones matematicas
     */
    private Token readNumber() {
        StringBuilder sb = new StringBuilder();
        int startLine = line;
        int startColumn = column;
        boolean isReal = false;
        
        while (isDigit()) {
            sb.append(currentChar);
            consume();
        }
        
        if (currentChar == '.') {
            isReal = true;
            sb.append(currentChar);
            consume();
            while (isDigit()) {
                sb.append(currentChar);
                consume();
            }
        }
        
        if (currentChar == 'e' || currentChar == 'E') {
            isReal = true;
            sb.append(currentChar);
            consume();
            if (currentChar == '+' || currentChar == '-') {
                sb.append(currentChar);
                consume();
            }
            while (isDigit()) {
                sb.append(currentChar);
                consume();
            }
        }
        
        String text = sb.toString();
        TokenType type = isReal ? TokenType.NUMERO_REAL : TokenType.NUMERO_ENTERO;
        
        return new Token(type, text, startLine, startColumn);
    }
    
    private Token readString() {
        StringBuilder sb = new StringBuilder();
        consume(); // Consumir la comilla inicial
        int startLine = line;
        int startColumn = column;
        
        while (!isEOF() && currentChar != '"') {
            if (currentChar == '\\' && nextCharIs('"')) {
                consume(); // Consumir la barra
                sb.append('"');
                consume();
            } else {
                sb.append(currentChar);
                consume();
            }
        }
        
        if (currentChar == '"') {
            consume(); // Consumir la comilla final
        }
        
        return new Token(TokenType.CADENA_LITERAL, sb.toString(), startLine, startColumn);
    }
    
    // private void skipWhitespace() {
    //     while (currentChar == ' ' || currentChar == '\t' || currentChar == '\r' || currentChar == '\u00A0') {
    //         consume();
    //     }
    //     if (currentChar == '\n') {
    //         line++;
    //         column = 1;
    //         consume();
    //     }
    // }
    
    private void skipWhitespace() {
    while (currentChar == ' ' || currentChar == '\t' || currentChar == '\r' || currentChar == '\n') {
        if (currentChar == '\n') {
            line++;
            column = 1;
        }
        consume();
    }
}


    private void skipComments() {
        // Comentarios de línea con // o --
        if ((currentChar == '/' && nextCharIs('/')) || (currentChar == '-' && nextCharIs('-'))) {
            while (!isEOF() && currentChar != '\n') {
                consume();
            }
            skipWhitespace(); // Saltar el salto de línea
        }
    }
    
    private void consume() {
        pos++;
        column++;
        if (pos < input.length()) {
            currentChar = input.charAt(pos);
        } else {
            currentChar = '\0';
        }
    }
    
    private boolean isEOF() {
        return pos >= input.length();
    }
    
    private boolean isLetter() {
        return Character.isLetter(currentChar);
    }
    
    private boolean isDigit() {
        return Character.isDigit(currentChar);
    }
    
    private boolean isLetterOrDigit() {
        return Character.isLetterOrDigit(currentChar);
    }
    
    private boolean nextCharIs(char ch) {
        if (pos + 1 >= input.length()) return false;
        return input.charAt(pos + 1) == ch;
    }
}