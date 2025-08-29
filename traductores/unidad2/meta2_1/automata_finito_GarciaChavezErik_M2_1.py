"""
erik garcia chavez 01275863
traductores 
ciclo 2025-2
ingenieria en computacion

M2.1 analaizador lexico
"""
def es_letra(c):
    return c.isalpha()

def es_digito(c):
    return c.isdigit()

def es_guion_bajo(c):
    return c == '_'

def es_punto(c):
    return c == '.'

def es_signo_mas(c):
    return c == '+'

def es_signo_menos(c):
    return c == '-'

def es_e_minuscula(c):
    return c == 'e'

def es_e_mayuscula(c):
    return c == 'E'

def es_espacio(c):
    return c == ' '

# Palabras reservadas que el autómata debe reconocer
PALABRAS_RESERVADAS = {'IF'}

def procesar_token(token):
    """Procesa un token individual usando el autómata finito"""
    if not token:
        return False, "Token vacío"
    
    estado = 'A'  
    posicion = 0
    
    while posicion < len(token):
        caracter = token[posicion]
        
        if estado == 'A':
            if es_letra(caracter):
                estado = 'B'
            elif es_digito(caracter):
                estado = 'G'
            else:
                return False, f"Caracter inválido '{caracter}' en posición {posicion}"
        
        elif estado == 'B':
            if es_letra(caracter) or es_digito(caracter) or es_guion_bajo(caracter):
                estado = 'D'
            else:
                return False, f"Caracter inválido '{caracter}' en posición {posicion}"
        
        elif estado == 'D':
            if es_letra(caracter) or es_digito(caracter) or es_guion_bajo(caracter):
                estado = 'D'  
            else:
                return False, f"Caracter inválido '{caracter}' en posición {posicion}"
        
        elif estado == 'E':
            if es_letra(caracter) or es_guion_bajo(caracter):
                estado = 'D'
            else:
                return False, f"Caracter inválido '{caracter}' en posición {posicion}"
        
        elif estado == 'F':
            if es_signo_mas(caracter) or es_signo_menos(caracter):
                estado = 'J'
            elif es_digito(caracter):
                estado = 'K'
            else:
                return False, f"Caracter inválido '{caracter}' en posición {posicion}"
        
        elif estado == 'G':
            if es_digito(caracter):
                estado = 'G'  
            elif es_punto(caracter):
                estado = 'H'
            else:
                return False, f"Caracter inválido '{caracter}' en posición {posicion}"
        
        elif estado == 'H':
            if es_digito(caracter):
                estado = 'H'  
            elif es_e_minuscula(caracter) or es_e_mayuscula(caracter):
                estado = 'I'
            else:
                return False, f"Caracter inválido '{caracter}' en posición {posicion}"
        
        elif estado == 'I':
            if es_signo_mas(caracter) or es_signo_menos(caracter):
                estado = 'J'
            elif es_digito(caracter):
                estado = 'K'
            else:
                return False, f"Caracter inválido '{caracter}' en posición {posicion}"
        
        elif estado == 'J':
            if es_digito(caracter):
                estado = 'K'
            else:
                return False, f"Caracter inválido '{caracter}' en posición {posicion}"
        
        elif estado == 'K':
            if es_digito(caracter):
                estado = 'K' 
            else:
                return False, f"Caracter inválido '{caracter}' en posición {posicion}"
        
        posicion += 1
    
    # Estados finales
    estados_finales = ['B', 'D', 'G', 'H', 'K']
    
    if estado in estados_finales:
        if estado in ['B', 'D']:
          
            if token.upper() in PALABRAS_RESERVADAS:
                return True, "CONDICIONAL"
            else:
                return True, "ID"
        elif estado == 'G':
            return True, "NUM"
        elif estado in ['H', 'K']:
            return True, "FLOAT"
    
    return False, f"Estado final inválido: {estado}"

def tokenizar(cadena):
    return cadena.split()

def procesar_automata(cadena):
    if not cadena:
        return False, "Cadena vacía"
    
    # Tokenizar la cadena
    tokens = tokenizar(cadena)
    resultados = []
    
    for i, token in enumerate(tokens):
        resultado, tipo = procesar_token(token)
        if not resultado:
            return False, f"Error en token '{token}' (posición {i+1}): {tipo}"
        resultados.append((token, tipo))
    
    return True, resultados

def main():
    print("Autómata Finito")
    print("Ingrese cadenas para probar (enter vacío para salir):")
    print(60 * "=")
    while True:
        entrada = input("\nCadena: ")
        if not entrada:
            break
        
        resultado, info = procesar_automata(entrada)
        if resultado:
            print("Resultado: ACEPTA")
            for token, tipo in info:
                print(f"  '{token}': {tipo}")
        else:
            print(f"Resultado: RECHAZA ({info})")

main()