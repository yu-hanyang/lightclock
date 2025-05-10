from opcua import Server
import serial

import time


ser = serial.Serial(
    port='COM9',
    baudrate=9600,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=1
)


server = Server()


server.set_endpoint("opc.tcp://0.0.0.0:4840/freeopcua/server/")


uri = server.register_namespace("urn:example.org:OPCUA:server")

myobj = server.nodes.objects.add_object(uri, "MyObject")

myvar = myobj.add_variable(uri, "MyVariable", 6.7)
myvar.set_writable()
user_year = myobj.add_variable(uri, "user_year", '2025')
user_year.set_writable()
user_month= myobj.add_variable(uri, "user_month", '04')
user_month.set_writable()
user_day = myobj.add_variable(uri, "user_day", '20')
user_day.set_writable()
user_hour = myobj.add_variable(uri, "user_hour", '01')
user_hour.set_writable()
user_minute = myobj.add_variable(uri, "user_minute", '01')
user_minute.set_writable()
user_second = myobj.add_variable(uri, "user_second", '01')
user_second.set_writable()
clock_year = myobj.add_variable(uri, "clock_year", '2025')
clock_year.set_writable()
clock_month= myobj.add_variable(uri, "clock_moenth", '04')
clock_month.set_writable()
clock_day = myobj.add_variable(uri, "clock_day", '20')
clock_day.set_writable()
clock_hour = myobj.add_variable(uri, "clock_hour", '01')
clock_hour.set_writable()
clock_minute = myobj.add_variable(uri, "clock_minute", '01')
clock_minute.set_writable()
clock_second = myobj.add_variable(uri, "clock_second", '10')
clock_second.set_writable()
user_mode = myobj.add_variable(uri, "user_mode", '00')
user_mode.set_writable()
user_value = myobj.add_variable(uri, "user_value", '00')
user_value.set_writable()

server.start()
print("server start...")
try:
    while True:

        #myvar.set_value(myvar.get_value() + 0.1)
        print(myvar.get_value())
        time.sleep(1)
        hex_data = "FF"+" "+user_year.get_value()[0]+user_year.get_value()[1]+" "+user_year.get_value()[2]+user_year.get_value()[3]+" "+user_month.get_value()+" "+user_day.get_value()+" "+user_hour.get_value()+" "+user_minute.get_value()+" "+user_second.get_value()+" "+clock_year.get_value()[0]+clock_year.get_value()[1]+" "+clock_year.get_value()[2]+clock_year.get_value()[3]+" "+clock_month.get_value()+" "+clock_day.get_value()+" "+clock_hour.get_value()+" "+clock_minute.get_value()+" "+clock_second.get_value()+" "+user_mode.get_value()+" "+user_value.get_value()+" FE"

        print(hex_data)

        if isinstance(hex_data, str):

            hex_data = hex_data.replace(" ", "").replace("\n", "")

            byte_data = bytes.fromhex(hex_data)
        else:
            byte_data = hex_data
        print(byte_data)

        bytes_sent = ser.write(byte_data)

        print(f" HEX: {byte_data.hex(' ').upper()}")
        time.sleep(5)
except KeyboardInterrupt:
    print("server closing...")
finally:
    server.stop()
    print("server closed。")