public class Varibale extends Simbolo {
    
    private float valor =0;


    public Varibale(String nombre, Tipo tipo){
        super(nombre, tipo);
    }

    public void setValor(float valor){
        this.valor = valor;
    }

    public float getValor(){
        return valor;
    }

}
