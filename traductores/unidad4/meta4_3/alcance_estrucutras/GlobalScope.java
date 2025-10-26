public class GlobalScope extends BaseScope {
    //falta la esta madre de poner cunado entra a un scope, local o global 
    
    public GlobalScope(){
        super(null);
    }

    public String getScopeName(){
        return "global";
    }
}
