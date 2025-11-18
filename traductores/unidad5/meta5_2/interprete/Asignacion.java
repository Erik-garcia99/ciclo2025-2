public class Asignacion extends Tupla{


    Token variable, valor1, valor2, operador;

    public Asignacion(Token variable, Token valor, int sv, int sf) {
        super(sv, sf);
        this.variable = variable;
        this.valor1 = valor;
    }

    public Asignacion(Token variable, Token valor1, Token operador, Token valor2, int sv, int sf){
        super(sv, sf);
         
        this.variable = variable;
        this.valor1 = valor1;
        this.valor2 = valor2;
        this.operador = operador;
    }


    public String toString(){
        if (operador == null){
            return "( " + super.toString() + ", [ \"" + variable + ", " + valor1 + "\" ] )";
        }
        else{
            return "( " + super.toString() + ", [ " + variable + ", "+ valor1 + ", " + operador + ", " + valor2 + " ] ) ";
        }
    }


    public int ejecutar(TablaSimbolos ts){

        try {
            Varibale v = (Varibale) ts.resolver(variable.getNombre());
            float operando1 = 0, operando2 =0;

            if(valor1.getTipo().getNombre().equals(TipoToken.NUMERO)){
                operando1= Float.parseFloat(valor1.getNombre());
            }
            else{
                operando1=((Varibale)ts.resolver(valor1.getNombre())).getValor();
            }

            if(valor2 != null){
                if(valor2.getTipo().getNombre().equals(TipoToken.NUMERO)){
                    operando2= Float.parseFloat(valor2.getNombre());
                }

                else{
                    operando2= ((Varibale) ts.resolver(valor2.getNombre())).getValor();
                }


                if(operador == null){
                    v.setValor(Float.parseFloat(valor1.getNombre()));
                }

                else{
                    switch ((operador.getNombre())) {
                        case "+":
                            v.setValor(operando1 + operando2);
                        break;

                        case "-":
                            v.setValor(operando1- operando2);
                        break;

                        case "*":
                            v.setValor(operando1 * operando2);
                        break;

                        case "/":
                            if(operando2 != 0){
                                v.setValor(operando1/operando2);
                            }
                            else{
                                System.out.println("Error: divicion entre cero");
                                System.exit(1);
                            }
                        break;
                    }
                }
            }


        } catch (SemanticException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        return saltoVerdadero;

    }

    
}
