namespace FaderTest.IO;

public interface HWConnection {
    int Send(ConfigurationPacket dataPacket);
    DataPacket[] Poll(int max);
    void Close();
}