public abstract class Parser {
    Lexer input;
    Token lookahead;


     public Parser(Lexer input) {
        this.input = input;
        consume();
    }

    public void consume() {
        lookahead = input.nexToken();
    }

    public void match(int x){
        if(lookahead.type == x) consume();
        else{
            throw new Error("expecting " + input.getTokenName(x) + 
                          "; found " + lookahead);
        }
    }
}
