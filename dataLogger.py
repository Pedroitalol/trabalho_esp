import serial
porta = "/dev/ttyUSB0"
baud = 9600
arquivo = "logger.csv"

ser = serial.Serial(porta,baud)
ser.flushInput()
print("Abrindo Serial")

amostra = 150
linha = 0
while linha <= amostra:
    
    data = str(ser.readline().decode("utf-8"))
    print(data)
    file = open(arquivo,"a")
    file.write(data)
    linha = linha+1

print("Final de leituras")
file.close()
ser.close()