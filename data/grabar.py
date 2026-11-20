import os
import time
import serial

PUERTO = "COM3"
BAUD = 460800

sesion = int(input("Sesión (1, 2, 3...): "))
grab = int(input("Grabación dentro de la sesión (1, 2, 3...): "))
modo = input("¿'c' para solo revisar señal o Enter para revisar + grabar? ").strip()

os.makedirs("data/raw", exist_ok=True)
ruta = f"data/raw/S{sesion:02d}_R{grab:02d}.csv"
if modo != "c" and os.path.exists(ruta):
    raise SystemExit(f"{ruta} ya existe. No se sobrescriben datos crudos.")

def revisar(ser):
    ser.write(b"c")
    print("Revisando señal 10 s (brazo relajado)...")
    while True:
        linea = ser.readline().decode(errors="ignore").strip()
        if linea.startswith("#CHECK"):
            print(linea)
            p = linea.split(",")
            return float(p[6]), linea   # mean

with serial.Serial(PUERTO, BAUD, timeout=2) as ser:
    time.sleep(2)
    ser.reset_input_buffer()

    mean, check = revisar(ser)
    if modo == "c":
        raise SystemExit()

    if mean < 5000 or mean > 11000:
        raise SystemExit("⚠ SEÑAL ATORADA. No se grabó nada. Reinicia el circuito y vuelve a intentar.")

    ser.reset_input_buffer()
    ser.write(b"s")
    print(f"Señal OK. Grabando en {ruta} ... (no uses la compu)")
    with open(ruta, "w", newline="") as f:
        f.write(f"#session,{sesion}\n#recording,{grab}\n")
        f.write(f"#date,{time.strftime('%Y-%m-%d %H:%M:%S')}\n")
        f.write(check.replace("#CHECK", "#PRECHECK") + "\n")
        while True:
            linea = ser.readline().decode(errors="ignore").strip()
            if not linea:
                continue
            f.write(linea + "\n")
            if linea.startswith("#TRIAL"):
                print(linea)
            if linea == "#END":
                break
    print("Listo.")