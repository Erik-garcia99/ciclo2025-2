public class AlcanceStruct {
    
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
        System.out.println("=== INICIO: Programa con Structs ===\n");

        // Definir tipos built-in
        currentScope.define(new BuiltInTypeSymbol("int"));
        currentScope.define(new BuiltInTypeSymbol("void"));
        System.out.println("Tipos primitivos definidos: int, void\n");

        // Linea 2: struct A {
        System.out.println("Linea 2: struct A {");
        StructSymbol structA = new StructSymbol("A", currentScope);
        currentScope.define(structA);
        
        // Entrar al scope de struct A
        currentScope = structA;
        imprimirEntrada(currentScope, "definicion de struct A");
        
        // Linea 3: int x;
        System.out.println("  Linea 3: int x;");
        BuiltInTypeSymbol intType = (BuiltInTypeSymbol) currentScope.resolve("int");
        if(intType == null) {
            System.err.println("  ERROR: tipo 'int' no encontrado");
        } else {
            currentScope.define(new VariableSymbol("x", intType));
            System.out.println("  Campo 'x' definido en struct A");
        }
        
        // Linea 4: struct B { int y; };
        System.out.println("  Linea 4: struct B { int y; };");
        StructSymbol structB = new StructSymbol("B", currentScope);
        currentScope.define(structB);
        
        // Entrar al scope de struct B (anidado en A)
        Scope scopeA = currentScope;
        currentScope = structB;
        imprimirEntrada(currentScope, "definicion de struct B (anidada en A)");
        
        intType = (BuiltInTypeSymbol) currentScope.resolve("int");
        if(intType == null) {
            System.err.println("    ERROR: tipo 'int' no encontrado");
        } else {
            currentScope.define(new VariableSymbol("y", intType));
            System.out.println("    Campo 'y' definido en struct B");
        }
        
        // Salir de struct B
        imprimirSalida(currentScope, "struct B");
        currentScope = currentScope.getEnclosingScope();
        
        // Linea 5: B b;
        System.out.println("  Linea 5: B b;");
        StructSymbol typeB = (StructSymbol) currentScope.resolve("B");
        if(typeB == null) {
            System.err.println("  ERROR: tipo 'B' no encontrado");
        } else {
            currentScope.define(new VariableSymbol("b", typeB));
            System.out.println("  Campo 'b' de tipo B definido en struct A");
        }
        
        // Linea 6: struct C { int z; };
        System.out.println("  Linea 6: struct C { int z; };");
        StructSymbol structC = new StructSymbol("C", currentScope);
        currentScope.define(structC);
        
        // Entrar al scope de struct C (anidado en A)
        currentScope = structC;
        imprimirEntrada(currentScope, "definicion de struct C (anidada en A)");
        
        intType = (BuiltInTypeSymbol) currentScope.resolve("int");
        if(intType == null) {
            System.err.println("    ERROR: tipo 'int' no encontrado");
        } else {
            currentScope.define(new VariableSymbol("z", intType));
            System.out.println("    Campo 'z' definido en struct C");
        }
        
        // Salir de struct C
        imprimirSalida(currentScope, "struct C");
        currentScope = currentScope.getEnclosingScope();
        
        // Linea 7: C c;
        System.out.println("  Linea 7: C c;");
        StructSymbol typeC = (StructSymbol) currentScope.resolve("C");
        if(typeC == null) {
            System.err.println("  ERROR: tipo 'C' no encontrado");
        } else {
            currentScope.define(new VariableSymbol("c", typeC));
            System.out.println("  Campo 'c' de tipo C definido en struct A");
        }
        
        // Linea 8: };  (fin de struct A)
        System.out.println("Linea 8: };");
        imprimirSalida(currentScope, "struct A");
        currentScope = currentScope.getEnclosingScope();
        
        // Linea 9: A a; (en scope global)
        System.out.println("\nLínea 9: A a;");
        StructSymbol typeA = (StructSymbol) currentScope.resolve("A");
        if(typeA == null) {
            System.err.println("ERROR: tipo 'A' no encontrado");
        } else {
            currentScope.define(new VariableSymbol("a", typeA));
            System.out.println("Variable global 'a' de tipo A definida\n");
        }

        // //linea donde habra un error: , la linea perosna no se encuentra registrada y se quiere asignar una variable. 
        // System.out.println("Linea 10: Persona p;");
        // System.out.println("Buscando tipo 'Persona'...");
        // StructSymbol personaType = (StructSymbol) currentScope.resolve("Persona");
        // if(personaType == null) {
        //     System.err.println("\nERROR SEMANTICO EN LIaNEA 10:");
        //     System.err.println("   El tipo 'Persona' no ha sido declarado");
        //     return;
        // }
        
        // Linea 11: void f()
        System.out.println("Linea 11: void f()");
        BuiltInTypeSymbol voidType = (BuiltInTypeSymbol) currentScope.resolve("void");
        if(voidType == null) {
            System.err.println("ERROR: tipo 'void' no encontrado");
        }
        
        MethodSymbol methodF = new MethodSymbol("f", null, currentScope);
        currentScope.define(methodF);
        
        // Entrar al método f
        currentScope = methodF;
        imprimirEntrada(currentScope, "metodo f()");
        
        // Linea 13: struct D {
        System.out.println("  Linea 13: struct D {");
        StructSymbol structD = new StructSymbol("D", currentScope);
        currentScope.define(structD);
        
        // Entrar al scope de struct D
        currentScope = structD;
        imprimirEntrada(currentScope, "definicion de struct D (dentro de metodo f)");
        
        // Linea 14: int i;
        System.out.println("    Línea 14: int i;");
        intType = (BuiltInTypeSymbol) currentScope.resolve("int");
        if(intType == null) {
            System.err.println("    ERROR: tipo 'int' no encontrado");
        } else {
            currentScope.define(new VariableSymbol("i", intType));
            System.out.println("    Campo 'i' definido en struct D");
        }
        
        // Linea 15: };
        System.out.println("  Linea 15: };");
        imprimirSalida(currentScope, "struct D");
        currentScope = currentScope.getEnclosingScope();
        
        // Linea 16: D d;
        System.out.println("  Linea 16: D d;");
        StructSymbol typeD = (StructSymbol) currentScope.resolve("D");
        if(typeD == null) {
            System.err.println("  ERROR: tipo 'D' no encontrado");
        } else {
            currentScope.define(new VariableSymbol("d", typeD));
            System.out.println("  Variable local 'd' de tipo D definida en metodo f");
        }
        

        // //error de querer buscar una variable que quiere asignasr unn valor sin antes delcararla 
        // System.out.println("  Linea 17: x = 100;");
        // System.out.println("  Buscando variable 'x'...");
        // Simbolo varX = currentScope.resolve("x");
        // if(varX == null) {
        //     System.err.println("\n ERROR SEMANTICO EN LINEA 17:");
        //     System.err.println("     La variable 'x' no ha sido declarada en este scope");
        //     System.err.println("     Variables disponibles: d");
        //     System.err.println("    COMPILACION DETENIDA\n");
        //     return;
        // }


        // Linea 17: d.i = a.b.y;
        System.out.println("  Linea 17: d.i = a.b.y;");
        
        // Resolver d
        Simbolo d = currentScope.resolve("d");
        if(d == null) {
            System.err.println("  ERROR: variable 'd' no encontrada");
        } else {
            System.out.println("  Resuelto 'd': " + d);
            
            // Resolver el campo 'i' de D
            if(d.getTipo() instanceof StructSymbol) {
                StructSymbol dType = (StructSymbol) d.getTipo();
                Simbolo i = dType.resolveMember("i");
                if(i == null) {
                    System.err.println("  ERROR: campo 'i' no encontrado en struct D");
                } else {
                    System.out.println("  Resuelto 'd.i': " + i);
                }
            }
        }
        
        // Resolver a
        Simbolo a = currentScope.resolve("a");
        if(a == null) {
            System.err.println("  ERROR: variable 'a' no encontrada");
        } else {
            System.out.println("  Resuelto 'a': " + a);
            
            // Resolver el campo 'b' de A
            if(a.getTipo() instanceof StructSymbol) {
                StructSymbol aType = (StructSymbol) a.getTipo();
                Simbolo b = aType.resolveMember("b");
                if(b == null) {
                    System.err.println("  ERROR: campo 'b' no encontrado en struct A");
                } else {
                    System.out.println("  Resuelto 'a.b': " + b);
                    
                    // Resolver el campo 'y' de B
                    if(b.getTipo() instanceof StructSymbol) {
                        StructSymbol bType = (StructSymbol) b.getTipo();
                        Simbolo y = bType.resolveMember("y");
                        if(y == null) {
                            System.err.println("  ERROR: campo 'y' no encontrado en struct B");
                        } else {
                            System.out.println("  Resuelto 'a.b.y': " + y);
                        }
                    }
                }
            }
        }
        
        // Linea 18: } (fin del método f)
        System.out.println("Línea 18: }");
        imprimirSalida(currentScope, "metodo f");
        currentScope = currentScope.getEnclosingScope();
        
        System.out.println("\n=== FIN: Regreso a Scope Global ===");
        System.out.println("\nTabla de símbolos global:");
        System.out.println("- Tipos: int, void, A");
        System.out.println("- Variables: a");
        System.out.println("- Metodos: f");
    }
}

