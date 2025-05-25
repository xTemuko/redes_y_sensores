import serial
import time
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation


PORT = '/dev/ttyACM0' 
BAUDRATE = 115200

try:
    ser = serial.Serial(PORT, BAUDRATE, timeout=0.1)  
    print(f"Puerto serie {PORT} abierto exitosamente.")
    time.sleep(2) 
    ser.flushInput() 
except serial.SerialException as e:
    print(f"Error al abrir el puerto serie {PORT}: {e}")
    exit()


buffer_acel = []   
buffer_giro = []  


hist_medias_acel = {'x': [], 'y': [], 'z': []}
hist_desvs_acel  = {'x': [], 'y': [], 'z': []}
hist_medias_giro = {'x': [], 'y': [], 'z': []}
hist_desvs_giro  = {'x': [], 'y': [], 'z': []}


fig, axes = plt.subplots(nrows=2, ncols=3, figsize=(10, 6))

axes[0,0].set_title("Acelerómetro X"); axes[0,1].set_title("Acelerómetro Y"); axes[0,2].set_title("Acelerómetro Z")
axes[1,0].set_title("Giroscopio X");   axes[1,1].set_title("Giroscopio Y");   axes[1,2].set_title("Giroscopio Z")


for ax_idx, ax in enumerate(axes.flatten()):

    if ax_idx == 0: 
      handles, labels = ax.get_legend_handles_labels()
      if not handles: 
          pass 
    ax.set_xlabel("Bloque")  
    ax.set_ylabel("Valor")



def update(frame):

    
    while ser.in_waiting > 0:
        linea = ser.readline().decode('utf-8', errors='ignore').strip()
        if not linea:
            continue 

        parts = linea.split(',') 
        
        if len(parts) == 4: # Debe tener 4 partes: Tipo, X, Y, Z
            tipo_sensor = parts[0]
            vals_str = parts[1:4] # Los valores X, Y, Z como strings
            
            try:
                vals = [float(v) for v in vals_str] # Convertir strings a floats
            except ValueError:
                vals = None
            
            if vals: 
                if tipo_sensor == 'A':
                    buffer_acel.append(vals)
                    if len(buffer_acel) == 15:
                        arr = np.array(buffer_acel)
                        medias = np.mean(arr, axis=0)
                        desvs  = np.std(arr, axis=0)  

                        hist_medias_acel['x'].append(medias[0]); hist_desvs_acel['x'].append(desvs[0])
                        hist_medias_acel['y'].append(medias[1]); hist_desvs_acel['y'].append(desvs[1])
                        hist_medias_acel['z'].append(medias[2]); hist_desvs_acel['z'].append(desvs[2])
                        buffer_acel.clear()  
                elif tipo_sensor == 'G':
                    buffer_giro.append(vals)
                    if len(buffer_giro) == 15:
                        arr = np.array(buffer_giro)
                        medias = np.mean(arr, axis=0)
                        desvs  = np.std(arr, axis=0)
                        hist_medias_giro['x'].append(medias[0]); hist_desvs_giro['x'].append(desvs[0])
                        hist_medias_giro['y'].append(medias[1]); hist_desvs_giro['y'].append(desvs[1])
                        hist_medias_giro['z'].append(medias[2]); hist_desvs_giro['z'].append(desvs[2])
                        buffer_giro.clear()


    for i, eje in enumerate(['x','y','z']):
        ax = axes[0, i]
        ax.clear()
        ax.bar(range(1, len(hist_medias_acel[eje]) + 1), hist_medias_acel[eje],
               yerr=hist_desvs_acel[eje], capsize=5, color='skyblue', label="Media ± Desv. Est.")
        ax.set_title(f"Acelerómetro {eje.upper()}")
        ax.set_xlabel("Bloque")
        ax.set_ylabel("Valor")
        ax.legend(loc='upper right')

    for i, eje in enumerate(['x','y','z']):
        ax = axes[1, i]
        ax.clear()
        ax.bar(range(1, len(hist_medias_giro[eje]) + 1), hist_medias_giro[eje],
               yerr=hist_desvs_giro[eje], capsize=5, color='lightcoral', label="Media ± Desv. Est.")
        ax.set_title(f"Giroscopio {eje.upper()}")
        ax.set_xlabel("Bloque")
        ax.set_ylabel("Valor")
        ax.legend(loc='upper right')

    return axes

# ani = FuncAnimation(fig, update, interval=100, blit=True) 
ani = FuncAnimation(fig, update, interval=100, cache_frame_data=False) # Llama a update() cada 100 ms


try:
    plt.show()  
finally:
    if ser.is_open:
        ser.close()
        print("Puerto serie cerrado.")