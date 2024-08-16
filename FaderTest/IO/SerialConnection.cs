using System.IO.Ports;

namespace FaderTest.IO;

public class SerialConnection : HWConnection {
    public SerialPort _serialPort;

    public SerialConnection() {
        _serialPort = new SerialPort("COM3", 115200/2, Parity.Odd, 8, StopBits.Two);
        _serialPort.Handshake = Handshake.None;
        _serialPort.RtsEnable = true;
        _serialPort.DtrEnable = true;


        //_serialPort.DataReceived += new SerialDataReceivedEventHandler(sp_DataReceived);
        _serialPort.Open();
    }

    public int Send(ConfigurationPacket dataPacket) {
        byte[] data = dataPacket.asBytes();
        byte check = 0;
        for(int i = 0; i < data.Length && i < 8; i++) {
            check ^= data[i];
            Console.Write("{0}, ", data[i]);
        }
        Console.WriteLine(" [Size: {0}]", data.Length);
        _serialPort.Write(data, 0, data.Length);
        data[0] = check; // always re-use your arrays for the environment 
        _serialPort.Write(data, 0, 1);
        return 0;
    }

    public DataPacket[] Poll(int max) {
        int read = 0;
        while (_serialPort.BytesToRead >= MIN_PACKET_SIZE && read++ < max) {
            byte[] buffer = new byte[16];
            _serialPort.Read(buffer, 0, 1); // Get Type
            _serialPort.Read(buffer, 1, 3);
            byte check = 0;
            for (int i = 0; i < 4; i++) {
                check ^= buffer[i];
            }

            if (_serialPort.ReadByte() == check) {
                return [DataPacket.from(buffer)];
            } else {
                Console.WriteLine($"Error with {buffer[0]}, {buffer[1]}, {buffer[2]}, {buffer[3]}");
                sync();
            }
        }

        return [];
    }

    private const int MIN_PACKET_SIZE = 4 + 1;

    public void Close() {
        throw new NotImplementedException();
    }

    public void sync(bool inProgress = false) {
        byte[] offset = [0];
        if(!inProgress) Send(new ConfigurationPacket(){type = ConfigurationPacket.CommandType.RESET_CON});
        int counter = 0;
        while (_serialPort.ReadByte() != 0) {}
        byte b = (byte)_serialPort.ReadByte();
        while (counter < 50 || b != 1) {
            if(b == 0) counter++;
            else if(b != 1) {
                Console.WriteLine("Sync failed (malformed) after {0}", counter);
                _serialPort.Write(offset, 0, 1);
                sync();
                return;
            }
            if (counter >= 110) {
                Console.WriteLine("Sync failed (timeout)");
                _serialPort.Write(offset, 0, 1);
                sync();
                return;
            }
            b = (byte)_serialPort.ReadByte();
            //Console.WriteLine("At {0}", counter);
        }
        Console.WriteLine("Synced");
    }
}