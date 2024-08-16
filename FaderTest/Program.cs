using FaderTest;
using FaderTest.IO;

AudioManager audioManager = new AudioManager();

SerialConnection con = new SerialConnection();
PluginPage[] pages = [new AudioPlugin(audioManager, 3, 0),
                        new AudioPlugin(audioManager, 3, 3),
                        new AudioPlugin(audioManager, 3, 6)];
int page = 0;

con.sync();

pages[page].Setup(con);
while (true) {
    DataPacket[] packets = con.Poll(1);
    foreach (var packet in packets) {
        //Console.WriteLine("Received: " + packet);
        if (packet.type == DataPacket.Type.Error) {
            con.sync(true);
            pages[page].Setup(con);
        } else if (packet.type == DataPacket.Type.PageChange) {
            page = packet.identifier;
            Console.WriteLine("Changed to page {0}", page);
            pages[page].Setup(con);
        } else if (packet.type == DataPacket.Type.Debug) {
            Console.Write((char) packet.c1);
            Console.Write((char) packet.c2);
            Console.Write((char) packet.c3);
        } else {
            pages[page].Handle(packet);
        }
    }

    pages[page].Update(con);
     
}