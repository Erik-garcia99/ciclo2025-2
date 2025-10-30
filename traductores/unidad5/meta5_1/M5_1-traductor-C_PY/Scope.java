public interface Scope{

    public String getScopeName();
    public Scope getEnclosingScope();
    public void define(Simbolo sym);
    public Simbolo resolve(String name);

}