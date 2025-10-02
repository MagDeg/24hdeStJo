import serial
import pyvjoy

# Öffne COM-Port (anpassen!)
ser = serial.Serial('COM8', 115200, timeout=1)

# Erstelle vJoy Device #1
j = pyvjoy.VJoyDevice(1)

def float_to_vjoy_axis(value):
    # vJoy Achse erwartet Werte 0 - 0x8000 (32768)
    # Input: float 0.0 - 1.0
    return int(value * 0x8000)

while True:
    line = ser.readline().decode('utf-8').strip()
    print(line)
    #prüft ob überhaupt elemente in der variable enthalten sind
    if line:
        #versucht werte aufzuteilen
        try:
            steering_str, throttle_str, brake_str = line.split(',')
            #Umwandlung String in Fließkommazahl
            steering = float(steering_str)
            throttle = float(throttle_str)
            brake = float(brake_str)

            # Mappe 0.0-1.0 auf vJoy Achsen
            # Lenkeingabe evtl. umsetzen auf Mitte (0.5) = Neutral
            # Annahme hier: Lenkung 0=links, 1=rechts, 0.5 neutral
            steering_val = float_to_vjoy_axis(steering)
            throttle_val = float_to_vjoy_axis(throttle)
            brake_val = float_to_vjoy_axis(brake)

            # Achsen setzen: X=Lenkung, Y=Gas, Z=Bremse
            j.set_axis(pyvjoy.HID_USAGE_X, steering_val)
            j.set_axis(pyvjoy.HID_USAGE_Y, throttle_val)
            j.set_axis(pyvjoy.HID_USAGE_Z, brake_val)

        except Exception as e:
            print(f"Fehler beim Verarbeiten: {e}")
