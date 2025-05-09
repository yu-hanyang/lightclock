import serial
import time

# 配置串口参数
ser = serial.Serial(
    port='COM8',
    baudrate=9600,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=1
)
hex_data = "FF 20 25 00 00 FE"
# 处理 HEX 数据
if isinstance(hex_data, str):
    # 移除字符串中的空格和换行符
    hex_data = hex_data.replace(" ", "").replace("\n", "")
    # 将 HEX 字符串转换为字节串
    byte_data = bytes.fromhex(hex_data)
else:
    byte_data = hex_data

# 发送数据
bytes_sent = ser.write(byte_data)
print(f"成功发送 {bytes_sent} 字节数据")
print(f"发送的 HEX 数据: {byte_data.hex(' ').upper()}")
print(byte_data)
while True:
    if ser.in_waiting > 0:


        data = ser.readline()  # 读取数据
        print("Received from STM32:", data.decode('utf-8').strip())

    time.sleep(0.5)