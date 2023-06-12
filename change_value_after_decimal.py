import os
import re
# Carpeta con los archivos a procesar
carpeta = "C:\\Users\\bosco\\Desktop\\TFG\\TFG-Kobol-BoscoOlives\\rsf_kobol_expander_2-vco\\audio\\vco1\\triangular-sawtooth +"


# Expresión regular para buscar números con uno o dos dígitos delante del decimal
regex = re.compile(r"^(.*?)(\d?\.\d+)(.*?)$")

# Leer cada archivo en la carpeta
for archivo in os.listdir(carpeta):
    # Comprobar si el nombre del archivo coincide con la expresión regular
    if regex.match(archivo):
        # Si el nombre del archivo coincide, obtener el número y el resto del nombre
        partes = regex.search(archivo).groups()
        nombre = partes[0]
        numero = partes[1]
        resto = partes[2]
        # Si el número tiene uno o dos dígitos delante del decimal y no tiene dos dígitos después del decimal, renombrar el archivo
        if "." in numero and float(numero) < 10 and len(numero.split(".")[1]) != 2:
            numero = "0" + numero
            nuevo_nombre = f"{nombre}{numero}{resto}"
            ruta_vieja = os.path.join(carpeta, archivo)
            ruta_nueva = os.path.join(carpeta, nuevo_nombre)
            os.rename(ruta_vieja, ruta_nueva)
            print(f"Renombrado {archivo} a {nuevo_nombre}")