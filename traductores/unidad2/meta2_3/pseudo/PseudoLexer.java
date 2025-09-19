// Archivo: PseudoLexer.java
import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class PseudoLexer {
    private ArrayList<TipoToken> tipos = new ArrayList<>();
    private ArrayList<Token> tokens = new ArrayList<>();

    public PseudoLexer() {
        tipos.add(new TipoToken(TipoToken.NUMERO, "-?[0-9]+(\\.[0-9]+)?"));
        tipos.add(new TipoToken(TipoToken.CADENA, "\".*?\"")); 
        tipos.add(new TipoToken(TipoToken.OPARITMETICO, "[*|/|\\+|-]"));
        tipos.add(new TipoToken(TipoToken.OPRELACIONAL, "<=|>=|==|<|>|!="));
        tipos.add(new TipoToken(TipoToken.IGUAL, "="));
        tipos.add(new TipoToken(TipoToken.COMA, ","));
        tipos.add(new TipoToken(TipoToken.PARENTESISIZQ, "\\("));
        tipos.add(new TipoToken(TipoToken.PARENTESISDER, "\\)"));
        tipos.add(new TipoToken(TipoToken.INICIOPROGRAMA, "inicio-programa"));
        tipos.add(new TipoToken(TipoToken.FINPROGRAMA, "fin-programa"));
        tipos.add(new TipoToken(TipoToken.LEER, "leer"));
        tipos.add(new TipoToken(TipoToken.ESCRIBIR, "escribir"));
        tipos.add(new TipoToken(TipoToken.SI, "si"));
        tipos.add(new TipoToken(TipoToken.ENTONCES, "entonces"));
        tipos.add(new TipoToken(TipoToken.FINSI, "fin-si"));
        tipos.add(new TipoToken(TipoToken.MIENTRAS, "mientras"));
        tipos.add(new TipoToken(TipoToken.FINMIENTRAS, "fin-mientras"));
        
        tipos.add(new TipoToken(TipoToken.VARIABLE, "[a-zA-Z_][a-zA-Z0-9_]*"));
        tipos.add(new TipoToken(TipoToken.ESPACIO, "[ \\t\\f\\r\\n]+"));
        
        tipos.add(new TipoToken(TipoToken.ERROR, ".+"));

        tipos.add(new TipoToken(TipoToken.VARIABLES, "variables"));
        tipos.add(new TipoToken(TipoToken.DOS_PUNTOS, ":"));
        tipos.add(new TipoToken(TipoToken.REPITE, "repite"));
        tipos.add(new TipoToken(TipoToken.FINREPITE, "fin-repite"));
    }

    public ArrayList<Token> getTokens() {
        return tokens;
    }

    public void analizar(String entrada) throws Exception {
        tokens.clear(); 
        StringBuilder erBuilder = new StringBuilder();
        for (TipoToken tt : tipos) {
            erBuilder.append(String.format("|(?<%s>%s)", tt.getNombre(), tt.getPatron()));
        }
        Pattern p = Pattern.compile(erBuilder.substring(1));
        Matcher m = p.matcher(entrada);

        while (m.find()) {
            if (m.group(TipoToken.ESPACIO) != null) {
                continue;
            }

            for (TipoToken tt : tipos) {
                if (m.group(tt.getNombre()) != null) {
                    if (tt.getNombre().equals(TipoToken.ERROR)) {
                        throw new Exception("El token '" + m.group(tt.getNombre()) + "' es inválido.");
                    }
                    String nombre = m.group(tt.getNombre());
                    if (tt.getNombre().equals(TipoToken.CADENA)) {
                        nombre = nombre.substring(1, nombre.length() - 1);
                    }
                    tokens.add(new Token(tt, nombre));
                    break;
                }
            }
        }
    }
}