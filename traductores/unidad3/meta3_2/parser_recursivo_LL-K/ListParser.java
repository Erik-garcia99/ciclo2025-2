public class ListParser extends Parser {
    public ListParser(Lexer input, int k) {
        super(input, k);
    }

    // list : '[' elements ']'
    public void list() {
        match(ListLexer.LBRACK);
        elements();
        match(ListLexer.RBRACK);
    }

    // elements : element (',' element)*
    void elements() {
        element();
        while (LA(1) == ListLexer.COMMA) {
            match(ListLexer.COMMA);
            element();
        }
    }

    // element : NAME | list
    void element() {
        if (LA(1) == ListLexer.NAME) {
            match(ListLexer.NAME);
        } else if (LA(1) == ListLexer.LBRACK) {
            list();
        } else {
            throw new Error("expecting name or list; found " + LT(1));
        }
    }
}