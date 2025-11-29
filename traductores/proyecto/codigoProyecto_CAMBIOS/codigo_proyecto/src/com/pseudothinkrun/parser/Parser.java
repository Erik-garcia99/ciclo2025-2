package com.pseudothinkrun.parser;

import com.pseudothinkrun.lexer.*;
import java.util.*;

/**
 * Clase base abstracta para parsers recursivos descendentes.
 * Proporciona la infraestructura común de manejo de tokens.
 */
public abstract class Parser {
    protected List<Token> tokens;
    protected int currentTokenIndex;
    protected Token currentToken;
    
    public Parser(List<Token> tokens) {
        this.tokens = tokens;
        this.currentTokenIndex = 0;
        this.currentToken = tokens.isEmpty() ? null : tokens.get(0);
    }
    
    /**
     * Avanza al siguiente token en la secuencia.
     */
    protected void advance() {
        currentTokenIndex++;
        if (currentTokenIndex < tokens.size()) {
            currentToken = tokens.get(currentTokenIndex);
        } else {
            currentToken = new Token(TokenType.EOF, "", 0, 0);
        }
    }
    
    /**
     * Verifica si el token actual coincide con el tipo esperado.
     * Si coincide, consume el token y avanza.
     * @param type Tipo de token esperado
     * @return true si hubo coincidencia, false en otro caso
     */
    protected boolean match(TokenType type) {
        if (currentToken.getType() == type) {
            advance();
            return true;
        }
        return false;
    }
    
    /**
     * Exige que el token actual sea del tipo especificado.
     * Si no coincide, lanza una excepción con mensaje detallado.
     * @param type Tipo de token esperado
     * @throws Exception si el token actual no coincide
     */
    protected void expect(TokenType type) throws Exception {
        if (!match(type)) {
            throw new Exception(
                String.format("Error sintáctico: Se esperaba '%s' en línea %d, columna %d, pero se encontró '%s'",
                    type.name(),
                    currentToken.getLine(),
                    currentToken.getColumn(),
                    currentToken.getLexeme())
            );
        }
    }
    
    /**
     * Verifica si el token actual es uno de los tipos especificados.
     * @param types Varargs de tipos de token
     * @return true si hay coincidencia con alguno
     */
    protected boolean checkToken(TokenType... types) {
        for (TokenType type : types) {
            if (currentToken.getType() == type) {
                return true;
            }
        }
        return false;
    }
    
    /**
     * Método abstracto que debe implementar cada parser específico.
     * Inicia el análisis sintáctico del programa completo.
     * @throws Exception en caso de errores de parsing
     */
    public abstract void parsePrograma() throws Exception;
}