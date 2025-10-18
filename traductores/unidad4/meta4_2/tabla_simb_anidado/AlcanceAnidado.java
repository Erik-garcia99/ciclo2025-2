public class AlcanceAnidado {
    
    private static void imprimirEntrada(Scope scope, String descripcion) {
        System.out.println(">>> entrando a " + scope.getScopeName() + " scope" + 
                         (descripcion.isEmpty() ? "" : ": " + descripcion));
    }
    
    private static void imprimirSalida(Scope scope, String descripcion) {
        System.out.println("<<< saliendo de " + scope.getScopeName() + " scope" + 
                         (descripcion.isEmpty() ? "" : ": " + descripcion));
    }

    public static void main(String[] args) {
        Scope currentScope;

        // Crear el scope global
        currentScope = new GlobalScope();
        System.out.println("INICIO\n");

        // Definir tipos built-in
        currentScope.define(new BuiltInTypeSymbol("int"));
        currentScope.define(new BuiltInTypeSymbol("float"));
        currentScope.define(new BuiltInTypeSymbol("void"));
        System.out.println("tipos primitivos definidos: int, float, void\n");

        // int i = 9
        System.out.println("linea 1: int i = 9;");
        BuiltInTypeSymbol t = (BuiltInTypeSymbol) currentScope.resolve("int");
        if(t == null) {
            System.err.println("ERROR: tipo 'int' no encontrado");
        }
        currentScope.define(new VariableSymbol("i", t));
        System.out.println("variable global 'i' definida\n");

        // float f(int x, float y)
        System.out.println("linea 2: float f(int x, float y)");
        BuiltInTypeSymbol rt = (BuiltInTypeSymbol) currentScope.resolve("float");
        if(rt == null) {
            System.err.println("ERROR: tipo 'float' no encontrado");
        }

        t = (BuiltInTypeSymbol) currentScope.resolve("int");
        if(t == null) {
            System.err.println("ERROR: tipo 'int' no encontrado");
        }

        BuiltInTypeSymbol t2 = (BuiltInTypeSymbol) currentScope.resolve("float");
        if(t2 == null) {
            System.err.println("ERROR: tipo 'float' no encontrado");
        }

        VariableSymbol[] arguments = {new VariableSymbol("x", t), new VariableSymbol("y", t2)};
        MethodSymbol m = new MethodSymbol("f", arguments, currentScope);
        currentScope.define(m);
        
        // Entrar al método f
        currentScope = m;
        imprimirEntrada(currentScope, "metodo f(int x, float y)");

        // Línea 3: float i;
        System.out.println("linea 3: float i;");
        t = (BuiltInTypeSymbol) currentScope.resolve("float");
        if(t == null) {
            System.err.println("ERROR: tipo 'float' no encontrado");
        }
        currentScope.define(new VariableSymbol("i", t));
        System.out.println("variable local 'i' definida en metodo f\n");

        // Línea 4: primer bloque { float z = x+y; i = z; }
        currentScope = new LocalScope(currentScope);
        imprimirEntrada(currentScope, "bloque linea 4");
        
        System.out.println("Línea 4: { float z = x+y; i = z; }");
        t = (BuiltInTypeSymbol) currentScope.resolve("float");
        if(t == null) {
            System.err.println("ERROR: tipo 'float' no encontrado");
        }
        currentScope.define(new VariableSymbol("z", t));
        System.out.println("Variable 'z' definida en bloque");

        Simbolo s = currentScope.resolve("x");
        if(s == null) {
            System.err.println("ERROR: variable 'x' no encontrada");
        } else {
            System.out.println("Resuelto 'x': " + s);
        }
        
        s = currentScope.resolve("y");
        if(s == null) {
            System.err.println("ERROR: variable 'y' no encontrada");
        } else {
            System.out.println("Resuelto 'y': " + s);
        }

        s = currentScope.resolve("i");
        if(s == null) {
            System.err.println("ERROR: variable 'i' no encontrada");
        } else {
            System.out.println("Resuelto 'i': " + s);
        }

        s = currentScope.resolve("z");
        if(s == null) {
            System.err.println("ERROR: variable 'z' no encontrada");
        } else {
            System.out.println("Resuelto 'z': " + s);
        }

        // Salir del primer bloque
        imprimirSalida(currentScope, "bloque línea 4");
        currentScope = currentScope.getEnclosingScope();
        System.out.println();

        // Línea 5: segundo bloque { float z = i+1; i = z; }
        currentScope = new LocalScope(currentScope);
        imprimirEntrada(currentScope, "bloque línea 5");
        
        System.out.println("Línea 5: { float z = i+1; i = z; }");
        t = (BuiltInTypeSymbol) currentScope.resolve("float");
        if(t == null) {
            System.err.println("ERROR: tipo 'float' no encontrado");
        }
        currentScope.define(new VariableSymbol("z", t));
        System.out.println("Variable 'z' definida en bloque (nueva instancia)");

        s = currentScope.resolve("i");
        if(s == null) {
            System.err.println("ERROR: variable 'i' no encontrada");
        } else {
            System.out.println("Resuelto 'i': " + s);
        }

        s = currentScope.resolve("i");
        if(s == null) {
            System.err.println("ERROR: variable 'i' no encontrada");
        }

        s = currentScope.resolve("z");
        if(s == null) {
            System.err.println("ERROR: variable 'z' no encontrada");
        } else {
            System.out.println("Resuelto 'z': " + s);
        }

        // Salir del segundo bloque
        imprimirSalida(currentScope, "bloque línea 5");
        currentScope = currentScope.getEnclosingScope();
        System.out.println();

        // return i;
        System.out.println("Línea 6: return i;");
        s = currentScope.resolve("i");
        if(s == null) {
            System.err.println("ERROR: variable 'i' no encontrada");
        } else {
            System.out.println("Resuelto 'i' para retorno: " + s);
        }

        // Salir del método f
        imprimirSalida(currentScope, "método f");
        currentScope = currentScope.getEnclosingScope();
        System.out.println();

        // Línea 7: void g()
        System.out.println("Línea 7: void g()");
        rt = (BuiltInTypeSymbol) currentScope.resolve("void");
        if(rt == null) {
            System.err.println("ERROR: tipo 'void' no encontrado");
        }

        m = new MethodSymbol("g", null, currentScope);
        currentScope.define(m);
        
        // Entrar al método g
        currentScope = m;
        imprimirEntrada(currentScope, "método g()");

        // Línea 8: bloque { f(i, 2); }
        currentScope = new LocalScope(currentScope);
        imprimirEntrada(currentScope, "bloque línea 8");
        
        System.out.println("Línea 8: { f(i, 2); }");
        s = currentScope.resolve("f");
        if(s == null) {
            System.err.println("ERROR: función 'f' no encontrada");
        } else {
            System.out.println("Resuelto 'f': " + s);
        }

        s = currentScope.resolve("i");
        if(s == null) {
            System.err.println("ERROR: variable 'i' no encontrada");
        } else {
            System.out.println("Resuelto 'i': " + s);
        }

        // Salir del bloque
        imprimirSalida(currentScope, "bloque línea 8");
        currentScope = currentScope.getEnclosingScope();
        
        // Salir del método g
        imprimirSalida(currentScope, "método g");
        currentScope = currentScope.getEnclosingScope();
        
        System.out.println("\nFIN: Regreso a Scope Global ");
    }
}