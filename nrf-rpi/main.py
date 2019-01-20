#!/usr/bin/python
# NRF_VCC  - > 3v3
# NRF_GND  - > GND
# NRF_CE   - > GPIO25
NRF_CE = 25
# NRF_CEN  - > GPIO7
# NRF_SCK	 - > GPIO11
# NRF_MOSI - > GPIO19
# NRF_MISO - > GPIO9
# NFR_IRQ  - > GPIO4
NFR_IRQ = 4

from nrf24 import NRF24
import time
import pdb
import MySQLdb

def line_prepender(filename, line):
  with open(filename, 'r+') as f:
    content = f.read()
    f.seek(0, 0)
    f.write(line.rstrip('\r\n') + '\n' + content)

db = MySQLdb.connect(host="localhost",
                     user="root",
                     passwd="pageup",
                     db="pageup")

pipes = [[0xF0, 0xF0, 0xF0, 0xF0, 0xF0], [0xF0, 0xF0, 0xF0, 0xF0, 0xF0]]

radio = NRF24()
radio.begin(0, 0, NRF_CE, NFR_IRQ)

radio.setRetries(15,15)

radio.write_register(NRF24.EN_AA, 0x01)
radio.write_register(NRF24.EN_RXADDR, 0x01)
radio.write_register(NRF24.SETUP_AW, 0x03)
radio.write_register(NRF24.RF_CH, 0x01)

radio.openWritingPipe(pipes[0])
radio.openReadingPipe(0, pipes[1])

radio.write_register(NRF24.RX_PW_P0, 0x01)
radio.write_register(NRF24.RF_SETUP, 0b00001110)
radio.write_register(NRF24.SETUP_RETR, 0b00111111)
radio.write_register(NRF24.CONFIG, 0b10111111)

radio.startListening()
radio.stopListening()

radio.printDetails()

radio.startListening()

mycursor = db.cursor()

while True:
  pipe = [0]
  fifo = radio.read_register(NRF24.FIFO_STATUS,1)
  
  if not (fifo >> NRF24.RX_EMPTY & 1):
    recv_buffer = []
    radio.read(recv_buffer)

    print recv_buffer
    
    # Put into db
    mycursor.execute("INSERT INTO Pages VALUES ('NULL',%d)" % recv_buffer[0])
    db.commit()
    # db.close()
 
    # Put into the webserver 
    t=open("/proc/uptime","r")
    tnow = t.read().split()
    t.close()
    
    # Write to file
    line_prepender("/var/www/index.html", str(recv_buffer[0]) + ";" + str(int(float(tnow[0])*1000)) + ";1<br> \n")
    
    radio.write_register(NRF24.STATUS, 0x70)
    time.sleep(0.01)
