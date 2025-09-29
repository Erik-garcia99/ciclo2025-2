import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;


public class PseudoLexer{


    private ArrayList<TipoToken> tipos = new ArrayList<>();
    private ArrayList<Token> tokens = new ArrayList<>();

    public PseudoLexer(){
        tipos.add(new TipoToken(TipoToken.NUMERO, "-?[0-9]+(\\.[0-9]+)?"));
        tipos.add(new TipoToken(TipoToken.CADENA, "\".*?\""));

        tipos.add(new TipoToken(TipoToken.OPARITMETICO, "[*/+-]"));
        tipos.add(new TipoToken(TipoToken.OPRELACIONAL, "<=|>=|==|!=|<|>"));
        tipos.add(new TipoToken(TipoToken.IGUAL, "="));

        tipos.add(new TipoToken(TipoToken.COMA, ","));
        tipos.add(new TipoToken(TipoToken.PARENTESISIZQ, "\\("));
        tipos.add(new TipoToken(TipoToken.PARENTESISDER, "\\)"));
        tipos.add(new TipoToken(TipoToken.DOSPUNTOS, ":"));
        tipos.add(new TipoToken(TipoToken.INICIOPROGRAMA, "inicio-programa"));
        tipos.add(new TipoToken(TipoToken.FINPROGRAMA, "fin-programa"));
        tipos.add(new TipoToken(TipoToken.VARIABLES, "variables"));
        tipos.add(new TipoToken(TipoToken.LEER, "leer"));
        tipos.add(new TipoToken(TipoToken.ESCRIBIR, "escribir"));
        tipos.add(new TipoToken(TipoToken.SI, "si"));
        tipos.add(new TipoToken(TipoToken.ENTONCES, "entonces"));
        tipos.add(new TipoToken(TipoToken.FINSI, "fin-si"));
        tipos.add(new TipoToken(TipoToken.MIENTRAS, "mientras"));
        tipos.add(new TipoToken(TipoToken.FINMIENTRAS, "fin-mientras"));
        tipos.add(new TipoToken(TipoToken.REPITE, "repite"));
        tipos.add(new TipoToken(TipoToken.FINREPITE, "fin-repite"));
        tipos.add(new TipoToken(TipoToken.VARIABLE, "[a-zA-Z_][a-zA-Z0-9_]*"));
        tipos.add(new TipoToken(TipoToken.ESPACIO, "[ \t\f\r\n]+"));
        tipos.add(new TipoToken(TipoToken.ERROR, "."));
    }

    public ArrayList<Token> getTokens() {
        return tokens;
    }

    public void analizar(String entrada) throws LexicalException{

        StringBuffer er = new StringBuffer();


        for(TipoToken tt : tipos){
            er.append(String.format("|(?<%s>%s)",tt.getNombre(),tt.getPatron()));
        }

        Pattern p = Pattern.compile(new String(er.substring(1)));
        Matcher m = p.matcher(entrada);

        while(m.find()){

            for(TipoToken tt: tipos){
                if(m.group(TipoToken.ESPACIO) != null){
                    continue;
                }
                else if(m.group(tt.getNombre()) != null){
                    if(tt.getNombre().equals(TipoToken.ERROR)){
                        LexicalException ex = new LexicalException(m.group(tt.getNombre()));
                        throw ex;
                    }

                    String nombre = m.group(tt.getNombre());

                    if(tt.getNombre().equals(TipoToken.CADENA)){
                        nombre = nombre.substring(1,nombre.length()-1);
                    }

                    tokens.add(new Token(tt, nombre));
                    break; 
                }
            }

        }

    }
}