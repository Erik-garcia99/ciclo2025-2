/*
 * garcia chavez erik 01275863
 * ingenieria en computacion 
 * analizador lexico
 * traductores
 * viernes 5 de septiembre del 2025
 * 
 */

public class Main {

    public static void main(String[] args) {
        ListLexer lexer = new ListLexer(args[0]);

        Token t= lexer.nexToken();
        while(t.type != Lexer.EOF_TYPE){
            System.out.println(t);
            t=lexer.nexToken();
        }

        System.out.println(t); //EOF
    }
    
}
