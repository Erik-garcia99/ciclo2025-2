# Programa generado desde pseudocódigo

# Variables declaradas
    
n = float(input('cuantas calificaciones'))
prom = 0
i = 0
while i<n:
    cal = float(input('da una calificacion'))
    prom = prom+cal
    i = i+1
prom = prom/n
if prom>5:
    print('aprobado con : ', prom)
if prom==10:
    print('excelente')
