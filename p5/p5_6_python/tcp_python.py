import socket, re, os


HOST = '10.199.128.149'   
PORT = 9000
ARCHIVO = 'log/datos_sensor.txt'
TIPO_SENSOR = 'A'
RE_FLOAT3 = re.compile(r'\s*(-?\d+\.\d+)\s*,\s*(-?\d+\.\d+)\s*,\s*(-?\d+\.\d+)\s*')


os.makedirs(os.path.dirname(ARCHIVO), exist_ok=True)
with open(ARCHIVO, 'w') as f:
    f.write("Tipo;X;Y;Z\n")
print(f"[INFO] Archivo '{ARCHIVO}' inicializado.")


with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    print(f"[INFO] Conectando a {HOST}:{PORT} …")
    s.connect((HOST, PORT))
    print(f"[INFO] Conectado al ESP32, esperando datos…")

    fileobj = s.makefile('r')
    while True:
        linea = fileobj.readline()
        if not linea:
            print("[INFO] Servidor cerró conexión.")
            break
        linea = linea.strip()
        print(f"[DEBUG] Recibido: '{linea}'")
        m = RE_FLOAT3.match(linea)
        if m:
            x, y, z = m.groups()
            fila = f"{TIPO_SENSOR};{x};{y};{z}\n"
            with open(ARCHIVO, 'a') as f:
                f.write(fila)
            print(f"[INFO] Guardado: {fila.strip()}")
        else:
            print("[WARN] Formato no válido")
