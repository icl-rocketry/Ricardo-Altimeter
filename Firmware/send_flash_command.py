# from SCons.Script import Import
# import serial
# import time
# from platformio.util import get_serial_ports

# Import("env")

# FLASH_CMD = b"FLASH\n"

# def before_upload(source, target, env):
#     print(">>> RUNNING PRE-UPLOAD SCRIPT <<<")

#     ports = get_serial_ports()
#     target_port = None

#     for p in ports:
#         print("FOUND PORT:", p)

#         hwid = p.get("hwid", "").upper()

#         # match TinyUSB CDC specifically
#         if "303A:4001" in hwid:
#             target_port = p["port"]
#             break

#     if not target_port:
#         print("ERROR: TinyUSB CDC device not found.")
#         return

#     print(f"Found TinyUSB CDC: {target_port}, sending FLASH command...")

#     try:
#         ser = serial.Serial(target_port, 115200, timeout=1)
#         ser.write(b"FLASH\n")
#         ser.flush()
#         ser.close()
#         print("FLASH command sent.")
#     except Exception as e:
#         print("Error:", e)

#     time.sleep(1.5)

# env.AddPreAction("upload", before_upload)
# # before_upload(None, None, None)

import serial
ser = serial.Serial("/dev/ttyACM0", 115200, timeout=1)
ser.write(b"HELLO\n")
ser.flush()
ser.close()