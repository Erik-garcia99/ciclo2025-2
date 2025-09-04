public class ListLexer extedns Lexer{
    public static int NAME = 2;
    public static int COMMA = 3;
    public static int LBRACK = 4;
    public static int RBRACK = 5;

    public static String[] tokenNames = {"n/a", "<EOF>", "NAME", "COMMA", "LBRACK", "RBRACK"};

    public String getTokenName(int x){
        return tokenNames[x];
    }

    public ListLexer(String input){super(input);}
    boolean isLETTER(){
        return c>='a'&&c<='z' ||  c>='A'&&c<='Z';
    }
}
