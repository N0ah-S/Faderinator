using System.Text;
using FaderTest.IO;

namespace FaderTest;

public class AudioPlugin : PluginPage {

    private AudioManager audio;
    private byte amount;
    private byte first;

    public AudioPlugin(AudioManager audio, byte amount, byte first) {
        this.audio = audio;
        this.amount = amount;
        this.first = first;
    }
    
    public void Setup(HWConnection con) {
        for (byte id = first; id < first + amount; id++) {
            ConfigurationPacket packet = default;
            packet.type = ConfigurationPacket.CommandType.Display_Text;
            packet.identifier = id;
            packet.image = Encoding.ASCII.GetBytes(audio.channels.Count > id ? audio.channels[id].GetName() : "None");
            packet.length = (byte)packet.image.Length;
            con.Send(packet);
            
            Thread.Sleep(100);
        }
        
        for (byte id = first; id < first + amount; id++) {
            ConfigurationPacket packet = default;
            packet.type = ConfigurationPacket.CommandType.Position;
            packet.identifier = id;
            packet.value = (ushort)(audio.channels.Count > id ? audio.channels[id].GetVolumeState().volume * 4000 : 0);
            con.Send(packet);
            con.Send(packet);
            con.Send(packet);
        }
        
        /*foreach (byte id in ids) {
            ConfigurationPacket packet = default;
            packet.type = ConfigurationPacket.CommandType.Image;
            packet.identifier = id;
            packet.image = audio.channels[id].GetIcon();
            con.Send(packet);
            
            Thread.Sleep(100);
        }*/
    }

    public void Handle(DataPacket packet) {
        if(audio.channels.Count <= packet.identifier) {
            Console.WriteLine("Ignored: " + packet);
            return;
        }
        switch (packet.type) {
            case DataPacket.Type.Position:
                audio.channels[packet.identifier].SetVolume(packet.value/4000f);
                Console.WriteLine(packet);
                break;
            case DataPacket.Type.State:
                break;
            default:
                Console.WriteLine(packet);
                break;
        }
    }

    public void Update(HWConnection con) {
        for (byte id = first; id < first + amount; id++) {
            if(audio.channels.Count <= id) return;
            VolumeState volumeState = audio.channels[id].GetVolumeState();
            if (!volumeState.changed) continue;
            ConfigurationPacket cPacket = default;
            cPacket.type = ConfigurationPacket.CommandType.Position;
            cPacket.identifier = id;
            cPacket.value = (ushort)(volumeState.volume * 4000);
            Console.WriteLine(cPacket);
            con.Send(cPacket);
        }
        
    }
}