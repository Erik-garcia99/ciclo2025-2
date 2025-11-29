package com.pseudothinkrun.parser;

import com.pseudothinkrun.lexer.*;
import com.pseudothinkrun.symbols.*;
import com.pseudothinkrun.intermediate.*;
import java.util.*;

/**
 * Parser descendente recursivo con lookahead inteligente
 * 
 * Estrategia principal:
 * - Token actual en currentToken
 * - match() consume si coincide tipo
 * - expect() fuerza coincidencia o lanza error
 * - lookahead para disambiguar gramática ambigua
 * 
 * 
 */


public class PseudoParser {
    private List<Token> tokens;
    private int currentTokenIndex = 0;
    private Token currentToken;
    private SymbolTable symbolTable;
    private List<Tupla> tuplas = new ArrayList<>();
    private Map<String, FunctionSymbol> funciones = new HashMap<>();

    
    /**
     * parsePrograma() es el dispatcher principal
     * Cada keyword determina qué método de parseo invocar
     * 
     * Facil extensión para nuevas instrucciones
     */
    public PseudoParser(List<Token> tokens, SymbolTable symbolTable) {
        this.tokens = tokens;
        this.symbolTable = symbolTable;
        if (tokens != null && !tokens.isEmpty()) {
            this.currentToken = tokens.get(0);
        } else {
            this.currentToken = new Token(TokenType.EOF, "", 0, 0);
        }
    }

    public List<Tupla> parse() throws Exception {
        parsePrograma();
        return tuplas;
    }

    private void advance() {
        currentTokenIndex++;
        if (currentTokenIndex < tokens.size()) {
            currentToken = tokens.get(currentTokenIndex);
        } else {
            currentToken = new Token(TokenType.EOF, "", 0, 0);
        }
    }

    private boolean match(TokenType type) {
        if (currentToken.getType() == type) {
            advance();
            return true;
        }
        return false;
    }

    private void expect(TokenType type) throws Exception {
        if (!match(type)) {
            throw new Exception(String.format("Error sintáctico: Se esperaba %s en línea %d, columna %d",
                              type, currentToken.getLine(), currentToken.getColumn()));
        }
    }

    private void expect(TokenType... types) throws Exception {
        for (TokenType type : types) {
            if (currentToken.getType() == type) {
                advance();
                return;
            }
        }
        String expected = Arrays.toString(types);
        throw new Exception(String.format("Error sintáctico: Se esperaba uno de %s, encontró %s en línea %d",
                          expected, currentToken.getType(), currentToken.getLine()));
    }

    private void parsePrograma() throws Exception {
        while (currentToken.getType() != TokenType.EOF) {
            TokenType type = currentToken.getType();
            
            if (type == TokenType.FUNCION) {
                parseFuncion();
            } else if (type == TokenType.TIPO) {
                parseStructDefinition();
            } else if (type == TokenType.DEFINIR) {
                parseDeclaracion();
            } else if (type == TokenType.SI) {
                parseSi();
            } else if (type == TokenType.MIENTRAS) {
                parseMientras();
            } else if (type == TokenType.REPITE) {
                parseRepite();
            } else if (type == TokenType.ESCRIBIR) {
                parseEscribir();
            } else if (type == TokenType.LEER) {
                parseLeer();
            } else if (type == TokenType.IDENTIFICADOR) {
                // Distinguir entre Asignación y Llamada
                if (esLlamadaFuncionSuelta()) {
                    parseLlamadaSuelta();
                } else {
                    parseAsignacion();
                }
            } else {
                throw new Exception("Error sintáctico: Token inesperado " + currentToken);
            }
        }
        tuplas.add(new FinProgramaTupla());
    }

    // Método auxiliar para detectar si es 

    /**
     * esLlamadaFuncionSuelta() resuelve ambigüedad gramatical
     * detecta si es : 'Funcion()' o 'Variable ='
     * Solución: Lookahead hasta encontrar '(' (llamada) o '=' (asignación)
     * 
     * Retorna true si: IDENTIFICADOR ... PARENTESIS_IZQ
     */
    private boolean esLlamadaFuncionSuelta() {
        int lookahead = 1;
        // Avanzar sobre posibles puntos (struct.func())
        int tempIndex = currentTokenIndex;
        
        while (tempIndex + lookahead < tokens.size()) {
            Token t = tokens.get(tempIndex + lookahead);
            if (t.getType() == TokenType.PUNTO) {
                lookahead++; // Saltar el punto
                if (tempIndex + lookahead < tokens.size()) lookahead++; // Saltar el ID
            } else if (t.getType() == TokenType.PARENTESIS_IZQ) {
                return true; // Es funcion()
            } else {
                return false; // Es variable = ...
            }
        }
        return false;
    }

    // --- ESTRUCTURAS ---
    private void parseStructDefinition() throws Exception {
        expect(TokenType.TIPO);
        String structName = currentToken.getLexeme();
        expect(TokenType.IDENTIFICADOR);

        StructSymbol struct = new StructSymbol(structName, symbolTable.getCurrentScope());
        symbolTable.defineStruct(struct);
        symbolTable.pushScope(struct);

        while (currentToken.getType() != TokenType.FINTIPO && currentToken.getType() != TokenType.EOF) {
            if (currentToken.getType() == TokenType.DEFINIR) {
                parseDeclaracion();
            } else {
                throw new Exception("Error: Solo se permiten declaraciones dentro de TIPO.");
            }
        }
        expect(TokenType.FINTIPO);
        symbolTable.popScope();
    }

    private void parseDeclaracion() throws Exception {
        expect(TokenType.DEFINIR);
        String varName = currentToken.getLexeme();
        expect(TokenType.IDENTIFICADOR);
        expect(TokenType.COMO);
        
        Type type = null;
        if (currentToken.getType() == TokenType.ENTERO || 
            currentToken.getType() == TokenType.REAL || 
            currentToken.getType() == TokenType.CADENA || 
            currentToken.getType() == TokenType.LOGICO) {
            type = new BuiltInTypeSymbol(currentToken.getType().name());
            advance();
        } else if (currentToken.getType() == TokenType.IDENTIFICADOR) {
            String typeName = currentToken.getLexeme();
            StructSymbol structType = symbolTable.resolveStruct(typeName);
            if (structType == null) throw new Exception("Error: Tipo desconocido '" + typeName + "'");
            type = structType;
            advance();
        } else {
            throw new Exception("Error: Se esperaba un tipo de dato válido");
        }
        
        VariableSymbol var = new VariableSymbol(varName, type);
        if (type instanceof StructSymbol) {
            var.setValue(new HashMap<String, Object>());
        }
        symbolTable.define(var);
    }

    private void parseAsignacion() throws Exception {
        tuplas.add(parseAsignacionInterna());
    }

    private AsignacionTupla parseAsignacionInterna() throws Exception {
        StringBuilder varNameBuilder = new StringBuilder();
        varNameBuilder.append(currentToken.getLexeme());
        expect(TokenType.IDENTIFICADOR);

        while (match(TokenType.PUNTO)) {
            varNameBuilder.append(".");
            varNameBuilder.append(currentToken.getLexeme());
            expect(TokenType.IDENTIFICADOR);
        }

        expect(TokenType.ASIGNACION);
        
        StringBuilder expr = new StringBuilder();
        int parenDepth = 0;
        
        while (currentToken.getType() != TokenType.EOF) {
            // Solo rompemos si estamos fuera de parentesis y es una instrucción clara
            if (parenDepth == 0 && esInicioDeInstruccion()) break;
            
            if (currentToken.getType() == TokenType.CADENA_LITERAL) {
                expr.append("\"").append(currentToken.getLexeme()).append("\" ");
            } else {
                expr.append(currentToken.getLexeme()).append(" ");
            }
            
            if (currentToken.getType() == TokenType.PARENTESIS_IZQ) parenDepth++;
            if (currentToken.getType() == TokenType.PARENTESIS_DER) {
                parenDepth--;
                if (parenDepth < 0) { advance(); break; }
            }
            advance();
        }
        
        return new AsignacionTupla(varNameBuilder.toString(), expr.toString().trim(), 
                                tuplas.size() + 1, tuplas.size() + 1);
    }

   
    // Reemplaza parseLlamadaSuelta() en PseudoParser.java

    private Tupla parseLlamadaSuelta() throws Exception {
        StringBuilder funcCall = new StringBuilder();
        funcCall.append(currentToken.getLexeme());
        expect(TokenType.IDENTIFICADOR);
        
        expect(TokenType.PARENTESIS_IZQ);
        funcCall.append(" ( ");
        
        while(currentToken.getType() != TokenType.PARENTESIS_DER && 
            currentToken.getType() != TokenType.EOF) {
            if (currentToken.getType() == TokenType.CADENA_LITERAL) {
                funcCall.append("\"").append(currentToken.getLexeme()).append("\" ");
            } else {
                funcCall.append(currentToken.getLexeme()).append(" ");
            }
            advance();
        }
        
        expect(TokenType.PARENTESIS_DER);
        funcCall.append(" )");
        
        String fullCall = funcCall.toString();
        int pStart = fullCall.indexOf('(');
        String fName = fullCall.substring(0, pStart).trim();
        String argsStr = fullCall.substring(pStart + 1, fullCall.lastIndexOf(')')).trim();
        String[] args = argsStr.isEmpty() ? new String[0] : argsStr.split("\\s*,\\s*");
        
        // IMPORTANTE: Crear y retornar la tupla sin agregarla a tuplas principales
        return new LlamadaFuncionTupla(fName, args, 0, 0);
    }

    private boolean esInicioDeInstruccion() {
        TokenType type = currentToken.getType();
        if (type == TokenType.ESCRIBIR || type == TokenType.LEER || type == TokenType.SI ||
            type == TokenType.MIENTRAS || type == TokenType.REPITE || type == TokenType.DEFINIR ||
            type == TokenType.FUNCION || type == TokenType.RETORNAR || type == TokenType.FINFUNCION ||
            type == TokenType.TIPO || type == TokenType.FINTIPO || type == TokenType.EOF || 
            type == TokenType.FINSI || type == TokenType.FINMIENTRAS || type == TokenType.FINREPITE ||
            type == TokenType.SINO || type == TokenType.ENTONCES) {
            return true;
        }
        
        
        if (type == TokenType.IDENTIFICADOR) {
            int lookahead = 1;
            while (currentTokenIndex + lookahead < tokens.size()) {
                Token t = tokens.get(currentTokenIndex + lookahead);
                if (t.getType() == TokenType.PUNTO) {
                    lookahead++;
                    if (currentTokenIndex + lookahead < tokens.size() && 
                        tokens.get(currentTokenIndex + lookahead).getType() == TokenType.IDENTIFICADOR) {
                        lookahead++;
                        continue; 
                    } else {
                        return false; 
                    }
                }
                if (t.getType() == TokenType.ASIGNACION) return true;
                

                return false; 
            }
        }
        return false;
    }

    private void parseEscribir() throws Exception {
        tuplas.add(parseEscribirInterna());
    }
    // Reemplaza parseEscribirInterna() en PseudoParser.java

    private EscribirTupla parseEscribirInterna() throws Exception {
        expect(TokenType.ESCRIBIR);
        List<String> exprs = new ArrayList<>();
        
        // Parsear expresiones separadas por comas
        boolean primerArgumento = true;
        while (currentToken.getType() != TokenType.EOF) {
            // Romper cuando encontramos palabras clave que terminan ESCRIBIR
            if (currentToken.getType() == TokenType.FINMIENTRAS ||
                currentToken.getType() == TokenType.FINREPITE ||
                currentToken.getType() == TokenType.FINSI ||
                currentToken.getType() == TokenType.SINO ||
                currentToken.getType() == TokenType.FINFUNCION ||
                currentToken.getType() == TokenType.ESCRIBIR ||
                currentToken.getType() == TokenType.LEER ||
                currentToken.getType() == TokenType.SI ||
                currentToken.getType() == TokenType.MIENTRAS ||
                currentToken.getType() == TokenType.REPITE) {
                break;
            }
            
            // Si encontramos un IDENTIFICADOR seguido de ASIGNACION, es otra instrucción
            if (currentToken.getType() == TokenType.IDENTIFICADOR) {
                int lookahead = currentTokenIndex + 1;
                boolean esAsignacion = false;
                
                // Saltar posibles puntos (para struct.field)
                while (lookahead < tokens.size()) {
                    Token t = tokens.get(lookahead);
                    if (t.getType() == TokenType.PUNTO) {
                        lookahead++;
                        if (lookahead < tokens.size() && 
                            tokens.get(lookahead).getType() == TokenType.IDENTIFICADOR) {
                            lookahead++;
                        }
                    } else if (t.getType() == TokenType.ASIGNACION) {
                        esAsignacion = true;
                        break;
                    } else {
                        break;
                    }
                }
                
                if (esAsignacion) {
                    break; // Es una asignación, terminar ESCRIBIR
                }
            }
            
            // Parsear el argumento actual
            if (currentToken.getType() == TokenType.IDENTIFICADOR) {
                StringBuilder sb = new StringBuilder();
                sb.append(currentToken.getLexeme());
                advance();
                while (match(TokenType.PUNTO)) {
                    sb.append(".").append(currentToken.getLexeme());
                    expect(TokenType.IDENTIFICADOR);
                }
                exprs.add(sb.toString());
            } else if (currentToken.getType() == TokenType.CADENA_LITERAL) {
                exprs.add("\"" + currentToken.getLexeme() + "\""); 
                advance();
            } else {
                exprs.add(currentToken.getLexeme());
                advance();
            }
            
            // Si hay coma, continuar con el siguiente argumento
            if (!match(TokenType.COMA)) {
                break;
            }
        }
        
        return new EscribirTupla(exprs.toArray(new String[0]), 0, 0);
    }

    private void parseLeer() throws Exception {
        tuplas.add(parseLeerInterna());
    }

    private LeerTupla parseLeerInterna() throws Exception {
        expect(TokenType.LEER);
        String varName = currentToken.getLexeme();
        expect(TokenType.IDENTIFICADOR);
        return new LeerTupla(varName, tuplas.size() + 1, tuplas.size() + 1);
    }

    private void parseSi() throws Exception {
        expect(TokenType.SI);
        String expr1 = currentToken.getLexeme(); advance();
        String operador = currentToken.getLexeme();
        expect(TokenType.MENOR_QUE, TokenType.MAYOR_QUE, TokenType.MENOR_IGUAL, 
               TokenType.MAYOR_IGUAL, TokenType.IGUAL, TokenType.DISTINTO);
        String expr2 = currentToken.getLexeme(); advance();
        
        ComparacionTupla condicion = new ComparacionTupla(expr1, operador, expr2, 1, 0);
        expect(TokenType.ENTONCES);
        SiTupla siTupla = new SiTupla(condicion, tuplas.size() + 1, tuplas.size() + 1);
        
        while (currentToken.getType() != TokenType.SINO && currentToken.getType() != TokenType.FINSI) {
            parseBloqueInstruccion(siTupla, true);
        }
        if (match(TokenType.SINO)) {
            while (currentToken.getType() != TokenType.FINSI) {
                parseBloqueInstruccion(siTupla, false);
            }
        }
        expect(TokenType.FINSI);
        tuplas.add(siTupla);
    }

    private void parseBloqueInstruccion(SiTupla siTupla, boolean esEntonces) throws Exception {
        Tupla t = null;
        if (currentToken.getType() == TokenType.IDENTIFICADOR) {
             if (esLlamadaFuncionSuelta()) {
                 // Soporte básico para llamadas dentro de bloques
                 // Por brevedad, asumimos asignación o llamada simple. 
                 // Para un proyecto completo, parseLlamadaSuelta debería devolver Tupla.
                 parseAsignacionInterna(); // Fallback si no quieres duplicar lógica compleja ahora
             } else {
                 t = parseAsignacionInterna();
             }
        }
        else if (currentToken.getType() == TokenType.ESCRIBIR) t = parseEscribirInterna();
        else if (currentToken.getType() == TokenType.LEER) t = parseLeerInterna();
        else throw new Exception("Token inesperado dentro de bloque SI: " + currentToken);
        
        if (t != null) {
            if (esEntonces) siTupla.agregarInstruccionEntonces(t);
            else siTupla.agregarInstruccionSino(t);
        }
    }

    
    private void parseMientras() throws Exception {
        expect(TokenType.MIENTRAS);
        
        // Parsear condición
        String expr1 = currentToken.getLexeme(); 
        advance();
        String operador = currentToken.getLexeme(); 
        expect(TokenType.MENOR_QUE, TokenType.MAYOR_QUE, TokenType.MENOR_IGUAL, 
            TokenType.MAYOR_IGUAL, TokenType.IGUAL, TokenType.DISTINTO);
        String expr2 = currentToken.getLexeme(); 
        advance();
        
        // Crear condición (saltos dummy: 1=verdadero, 0=falso)
        ComparacionTupla condicion = new ComparacionTupla(expr1, operador, expr2, 1, 0);
        
        expect(TokenType.HACER);
        
        // Crear tupla MIENTRAS
        MientrasTupla mientrasTupla = new MientrasTupla(condicion, tuplas.size() + 1, tuplas.size() + 1);
        
        // Parsear instrucciones dentro del bucle hasta encontrar FINMIENTRAS
        while (currentToken.getType() != TokenType.FINMIENTRAS && 
            currentToken.getType() != TokenType.EOF) {
            
            TokenType tipo = currentToken.getType();
            
            if (tipo == TokenType.ESCRIBIR) {
                mientrasTupla.agregarInstruccion(parseEscribirInterna());
            }
            else if (tipo == TokenType.LEER) {
                mientrasTupla.agregarInstruccion(parseLeerInterna());
            }
            else if (tipo == TokenType.IDENTIFICADOR) {
                // Distinguir entre asignación y llamada a función
                if (esLlamadaFuncionSuelta()) {
                    Tupla llamada = parseLlamadaSuelta();
                    mientrasTupla.agregarInstruccion(llamada);
                } else {
                    mientrasTupla.agregarInstruccion(parseAsignacionInterna());
                }
            }
            else if (tipo == TokenType.SI) {
                // SI anidado: parseamos pero agregamos al MIENTRAS
                SiTupla siAnidado = parseSiInterna();
                mientrasTupla.agregarInstruccion(siAnidado);
            }
            else if (tipo == TokenType.MIENTRAS) {
                // MIENTRAS anidado
                MientrasTupla mientrasAnidado = parseMientrasInterna();
                mientrasTupla.agregarInstruccion(mientrasAnidado);
            }
            else if (tipo == TokenType.REPITE) {
                // REPITE anidado
                RepiteTupla repiteAnidado = parseRepiteInterna();
                mientrasTupla.agregarInstruccion(repiteAnidado);
            }
            else {
                throw new Exception("Token inesperado dentro de MIENTRAS: " + currentToken + 
                                " en línea " + currentToken.getLine());
            }
        }
        
        expect(TokenType.FINMIENTRAS);
        
        // Agregar la tupla MIENTRAS completa a la lista de tuplas
        tuplas.add(mientrasTupla);
    }

    // Método auxiliar para parsear MIENTRAS sin agregar a tuplas principales
    private MientrasTupla parseMientrasInterna() throws Exception {
        expect(TokenType.MIENTRAS);
        
        String expr1 = currentToken.getLexeme(); 
        advance();
        String operador = currentToken.getLexeme(); 
        expect(TokenType.MENOR_QUE, TokenType.MAYOR_QUE, TokenType.MENOR_IGUAL, 
            TokenType.MAYOR_IGUAL, TokenType.IGUAL, TokenType.DISTINTO);
        String expr2 = currentToken.getLexeme(); 
        advance();
        
        ComparacionTupla condicion = new ComparacionTupla(expr1, operador, expr2, 1, 0);
        expect(TokenType.HACER);
        
        MientrasTupla mientrasTupla = new MientrasTupla(condicion, 0, 0);
        
        while (currentToken.getType() != TokenType.FINMIENTRAS && 
            currentToken.getType() != TokenType.EOF) {
            
            TokenType tipo = currentToken.getType();
            
            if (tipo == TokenType.ESCRIBIR) {
                mientrasTupla.agregarInstruccion(parseEscribirInterna());
            }
            else if (tipo == TokenType.LEER) {
                mientrasTupla.agregarInstruccion(parseLeerInterna());
            }
            else if (tipo == TokenType.IDENTIFICADOR) {
                if (esLlamadaFuncionSuelta()) {
                    mientrasTupla.agregarInstruccion(parseLlamadaSuelta());
                } else {
                    mientrasTupla.agregarInstruccion(parseAsignacionInterna());
                }
            }
            else {
                throw new Exception("Token inesperado dentro de MIENTRAS anidado: " + currentToken);
            }
        }
        
        expect(TokenType.FINMIENTRAS);
        return mientrasTupla;
    }

    // Método auxiliar para parsear SI sin agregar a tuplas principales
    private SiTupla parseSiInterna() throws Exception {
        expect(TokenType.SI);
        String expr1 = currentToken.getLexeme(); 
        advance();
        String operador = currentToken.getLexeme();
        expect(TokenType.MENOR_QUE, TokenType.MAYOR_QUE, TokenType.MENOR_IGUAL, 
            TokenType.MAYOR_IGUAL, TokenType.IGUAL, TokenType.DISTINTO);
        String expr2 = currentToken.getLexeme(); 
        advance();
        
        ComparacionTupla condicion = new ComparacionTupla(expr1, operador, expr2, 1, 0);
        expect(TokenType.ENTONCES);
        SiTupla siTupla = new SiTupla(condicion, 0, 0);
        
        while (currentToken.getType() != TokenType.SINO && 
            currentToken.getType() != TokenType.FINSI) {
            parseBloqueInstruccion(siTupla, true);
        }
        
        if (match(TokenType.SINO)) {
            while (currentToken.getType() != TokenType.FINSI) {
                parseBloqueInstruccion(siTupla, false);
            }
        }
        
        expect(TokenType.FINSI);
        return siTupla;
    }

    // Método auxiliar para parsear REPITE sin agregar a tuplas principales
    private RepiteTupla parseRepiteInterna() throws Exception {
        expect(TokenType.REPITE);
        int veces = Integer.parseInt(currentToken.getLexeme());
        expect(TokenType.NUMERO_ENTERO);
        expect(TokenType.VECES);
        
        RepiteTupla repiteTupla = new RepiteTupla(veces, 0, 0);
        
        while (currentToken.getType() != TokenType.FINREPITE) {
            if (currentToken.getType() == TokenType.IDENTIFICADOR) {
                if (esLlamadaFuncionSuelta()) {
                    repiteTupla.agregarInstruccion(parseLlamadaSuelta());
                } else {
                    repiteTupla.agregarInstruccion(parseAsignacionInterna());
                }
            }
            else if (currentToken.getType() == TokenType.ESCRIBIR) {
                repiteTupla.agregarInstruccion(parseEscribirInterna());
            }
            else if (currentToken.getType() == TokenType.LEER) {
                repiteTupla.agregarInstruccion(parseLeerInterna());
            }
            else {
                throw new Exception("Token inesperado en REPITE");
            }
        }
        
        expect(TokenType.FINREPITE);
        return repiteTupla;
    }

    private void parseRepite() throws Exception {
        expect(TokenType.REPITE);
        int veces = Integer.parseInt(currentToken.getLexeme());
        expect(TokenType.NUMERO_ENTERO); expect(TokenType.VECES);
        RepiteTupla repiteTupla = new RepiteTupla(veces, tuplas.size() + 1, tuplas.size() + 1);
        while (currentToken.getType() != TokenType.FINREPITE) {
            if (currentToken.getType() == TokenType.IDENTIFICADOR) repiteTupla.agregarInstruccion(parseAsignacionInterna());
            else if (currentToken.getType() == TokenType.ESCRIBIR) repiteTupla.agregarInstruccion(parseEscribirInterna());
            else if (currentToken.getType() == TokenType.LEER) repiteTupla.agregarInstruccion(parseLeerInterna());
            else throw new Exception("Token inesperado en REPITE");
        }
        expect(TokenType.FINREPITE);
        tuplas.add(repiteTupla);
    }

    



    /**
     * parseFuncion() maneja declaración completa con parametros
     * 
     * Diseño: 
     * 1. Crea FunctionSymbol con tipo retorno
     * 2. Parsea parametros y crea scope interno
     * 3. Almacena tuplas del cuerpo para ejecucion futura
     * 
     * las Funciones son "templates" de código, no se ejecutan al parsear
     */
    private void parseFuncion() throws Exception {
        expect(TokenType.FUNCION);
        String funcName = currentToken.getLexeme();
        expect(TokenType.IDENTIFICADOR);
        TokenType tipoToken = currentToken.getType();
        expect(TokenType.ENTERO, TokenType.REAL, TokenType.CADENA, TokenType.LOGICO);
        Type returnType = new BuiltInTypeSymbol(tipoToken.name());
        FunctionSymbol funcion = new FunctionSymbol(funcName, returnType, symbolTable.getCurrentScope());
        funciones.put(funcName, funcion);
        symbolTable.define(funcion);
        symbolTable.pushScope(funcion);
        expect(TokenType.PARENTESIS_IZQ);
        if (currentToken.getType() != TokenType.PARENTESIS_DER) {
            do {
                String paramName = currentToken.getLexeme();
                expect(TokenType.IDENTIFICADOR);
                expect(TokenType.COMO);
                TokenType paramTypeToken = currentToken.getType();
                expect(TokenType.ENTERO, TokenType.REAL, TokenType.CADENA, TokenType.LOGICO);
                VariableSymbol param = new VariableSymbol(paramName, new BuiltInTypeSymbol(paramTypeToken.name()));
                funcion.addParameter(param); 
                symbolTable.define(param);
            } while (match(TokenType.COMA));
        }
        expect(TokenType.PARENTESIS_DER);
        List<Tupla> funcTuplas = new ArrayList<>();
        while (currentToken.getType() != TokenType.FINFUNCION) {
            if (currentToken.getType() == TokenType.DEFINIR) parseDeclaracion();
            else if (currentToken.getType() == TokenType.IDENTIFICADOR) funcTuplas.add(parseAsignacionInterna());
            else if (currentToken.getType() == TokenType.ESCRIBIR) funcTuplas.add(parseEscribirInterna());
            else if (currentToken.getType() == TokenType.LEER) funcTuplas.add(parseLeerInterna());
            else if (currentToken.getType() == TokenType.RETORNAR) { funcTuplas.add(parseRetornarInterna()); break; }
            else throw new Exception("Token inesperado en función: " + currentToken);
        }
        funcion.setFunctionTuplas(funcTuplas);
        expect(TokenType.FINFUNCION);
        symbolTable.popScope();
    }

    private RetornarTupla parseRetornarInterna() throws Exception {
        expect(TokenType.RETORNAR);
        StringBuilder expr = new StringBuilder();
        while (currentToken.getType() != TokenType.FINFUNCION && currentToken.getType() != TokenType.EOF) {
            expr.append(currentToken.getLexeme()).append(" ");
            advance();
        }
        return new RetornarTupla(expr.toString().trim(), tuplas.size()+1, tuplas.size()+1);
    }

    private Tupla parseInstruccion() throws Exception {
        TokenType type = currentToken.getType();
        
        if (type == TokenType.ESCRIBIR) {
            return parseEscribirInterna();
        }
        else if (type == TokenType.LEER) {
            return parseLeerInterna();
        }
        else if (type == TokenType.IDENTIFICADOR) {
            if (esLlamadaFuncionSuelta()) {
                return parseLlamadaSuelta();
            } else {
                return parseAsignacionInterna();
            }
        }
        else {
            throw new Exception("Instrucción no reconocida en bloque: " + currentToken);
        }
    }

    
}