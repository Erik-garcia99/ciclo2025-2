package com.pseudothinkrun.parser;

import com.pseudothinkrun.lexer.*;
import com.pseudothinkrun.symbols.*;
import com.pseudothinkrun.intermediate.*;
import java.util.*;

public class PseudoParser {
    private List<Token> tokens;
    private int currentTokenIndex = 0;
    private Token currentToken;
    private SymbolTable symbolTable;
    private List<Tupla> tuplas = new ArrayList<>();
    private Map<String, FunctionSymbol> funciones = new HashMap<>();

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

    private boolean esLlamadaFuncionSuelta() {
        int lookahead = 1;
        int tempIndex = currentTokenIndex;
        
        while (tempIndex + lookahead < tokens.size()) {
            Token t = tokens.get(tempIndex + lookahead);
            if (t.getType() == TokenType.PUNTO) {
                lookahead++;
                if (tempIndex + lookahead < tokens.size()) lookahead++;
            } else if (t.getType() == TokenType.PARENTESIS_IZQ) {
                return true;
            } else {
                return false;
            }
        }
        return false;
    }

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
        
        Type type = parseTipo();
        
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

    private EscribirTupla parseEscribirInterna() throws Exception {
        expect(TokenType.ESCRIBIR);
        List<String> exprs = new ArrayList<>();
        
        while (currentToken.getType() != TokenType.EOF) {
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
            
            if (currentToken.getType() == TokenType.IDENTIFICADOR) {
                int lookahead = currentTokenIndex + 1;
                boolean esAsignacion = false;
                
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
                    break;
                }
            }
            
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
                 parseAsignacionInterna();
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
        
        String expr1 = currentToken.getLexeme(); 
        advance();
        String operador = currentToken.getLexeme(); 
        expect(TokenType.MENOR_QUE, TokenType.MAYOR_QUE, TokenType.MENOR_IGUAL, 
            TokenType.MAYOR_IGUAL, TokenType.IGUAL, TokenType.DISTINTO);
        String expr2 = currentToken.getLexeme(); 
        advance();
        
        ComparacionTupla condicion = new ComparacionTupla(expr1, operador, expr2, 1, 0);
        expect(TokenType.HACER);
        
        MientrasTupla mientrasTupla = new MientrasTupla(condicion, tuplas.size() + 1, tuplas.size() + 1);
        
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
                    Tupla llamada = parseLlamadaSuelta();
                    mientrasTupla.agregarInstruccion(llamada);
                } else {
                    mientrasTupla.agregarInstruccion(parseAsignacionInterna());
                }
            }
            else if (tipo == TokenType.SI) {
                SiTupla siAnidado = parseSiInterna();
                mientrasTupla.agregarInstruccion(siAnidado);
            }
            else if (tipo == TokenType.MIENTRAS) {
                MientrasTupla mientrasAnidado = parseMientrasInterna();
                mientrasTupla.agregarInstruccion(mientrasAnidado);
            }
            else if (tipo == TokenType.REPITE) {
                RepiteTupla repiteAnidado = parseRepiteInterna();
                mientrasTupla.agregarInstruccion(repiteAnidado);
            }
            else {
                throw new Exception("Token inesperado dentro de MIENTRAS: " + currentToken + 
                                " en línea " + currentToken.getLine());
            }
        }
        
        expect(TokenType.FINMIENTRAS);
        
        tuplas.add(mientrasTupla);
    }

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

    private void parseFuncion() throws Exception {
        expect(TokenType.FUNCION);
        String funcName = currentToken.getLexeme();
        expect(TokenType.IDENTIFICADOR);
        
        expect(TokenType.PARENTESIS_IZQ);
        
        FunctionSymbol funcion = new FunctionSymbol(funcName, null, symbolTable.getCurrentScope());
        funciones.put(funcName, funcion);
        symbolTable.define(funcion);
        symbolTable.pushScope(funcion);
        
        if (currentToken.getType() != TokenType.PARENTESIS_DER) {
            do {
                String paramName = currentToken.getLexeme();
                expect(TokenType.IDENTIFICADOR);
                expect(TokenType.COMO);
                
                Type paramType = parseTipo();
                VariableSymbol param = new VariableSymbol(paramName, paramType);
                funcion.addParameter(param);
                symbolTable.define(param);
            } while (match(TokenType.COMA));
        }
        
        expect(TokenType.PARENTESIS_DER);
        
        expect(TokenType.COMO);
        
        Type returnType = parseTipo();
        funcion.setType(returnType);
        
        List<Tupla> funcTuplas = new ArrayList<>();
        while (currentToken.getType() != TokenType.FINFUNCION) {
            if (currentToken.getType() == TokenType.DEFINIR) {
                parseDeclaracion();
            }
            else if (currentToken.getType() == TokenType.IDENTIFICADOR) {
                if (esLlamadaFuncionSuelta()) {
                    funcTuplas.add(parseLlamadaSuelta());
                } else {
                    funcTuplas.add(parseAsignacionInterna());
                }
            }
            else if (currentToken.getType() == TokenType.ESCRIBIR) {
                funcTuplas.add(parseEscribirInterna());
            }
            else if (currentToken.getType() == TokenType.LEER) {
                funcTuplas.add(parseLeerInterna());
            }
            else if (currentToken.getType() == TokenType.RETORNAR) {
                funcTuplas.add(parseRetornarInterna());
                break;
            }
            else if (currentToken.getType() == TokenType.MIENTRAS) {
                funcTuplas.add(parseMientrasInterna());
            }
            else if (currentToken.getType() == TokenType.REPITE) {
                funcTuplas.add(parseRepiteInterna());
            }
            else {
                throw new Exception("Token inesperado en función: " + currentToken);
            }
        }
        
        funcion.setFunctionTuplas(funcTuplas);
        expect(TokenType.FINFUNCION);
        symbolTable.popScope();
    }

    private Type parseTipo() throws Exception {
        if (currentToken.getType() == TokenType.ENTERO || 
            currentToken.getType() == TokenType.REAL || 
            currentToken.getType() == TokenType.CADENA || 
            currentToken.getType() == TokenType.LOGICO) {
            
            Type type = new BuiltInTypeSymbol(currentToken.getType().name());
            advance();
            return type;
        } 
        else if (currentToken.getType() == TokenType.IDENTIFICADOR) {
            String typeName = currentToken.getLexeme();
            StructSymbol structType = symbolTable.resolveStruct(typeName);
            if (structType == null) {
                throw new Exception("Error: Tipo desconocido '" + typeName + "'");
            }
            advance();
            return structType;
        } 
        else {
            throw new Exception("Error: Se esperaba un tipo de dato válido, encontró " + currentToken.getType());
        }
    }

    private RetornarTupla parseRetornarInterna() throws Exception {
        expect(TokenType.RETORNAR);
        StringBuilder expr = new StringBuilder();
        
        while (currentToken.getType() != TokenType.FINFUNCION && 
               currentToken.getType() != TokenType.EOF &&
               currentToken.getType() != TokenType.DEFINIR &&
               currentToken.getType() != TokenType.ESCRIBIR &&
               currentToken.getType() != TokenType.LEER &&
               currentToken.getType() != TokenType.SI &&
               currentToken.getType() != TokenType.MIENTRAS &&
               currentToken.getType() != TokenType.REPITE) {
            
            expr.append(currentToken.getLexeme()).append(" ");
            advance();
        }
        
        return new RetornarTupla(expr.toString().trim(), tuplas.size() + 1, tuplas.size() + 1);
    }
}