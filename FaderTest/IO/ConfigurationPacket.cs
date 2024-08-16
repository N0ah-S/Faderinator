using System.Runtime.InteropServices;
using System.Runtime.Serialization;

namespace FaderTest.IO;

[StructLayout(LayoutKind.Explicit)] 
public struct ConfigurationPacket {
    [FieldOffset(0)] public CommandType type;
    [FieldOffset(1)] public byte identifier; // virtual fader - fader+page
    [FieldOffset(2)] public ushort value; // position or state
    [FieldOffset(2)] public byte length; // position or state
    [FieldOffset(8), OptionalField] public byte[] image; // 128x64 image in grayscale
    
    public byte[] asBytes() {
        int size = 4;
        if (type == CommandType.Image) size += 64 * 8 - 2; // A 64x8 Image - the two irrelevant value bytes
        if (type == CommandType.Display_Text) size += length - 1; // A char string - the irrelevant value byte
        Byte[] data = new Byte[size];
        Util.StructureToArray(this, ref data);
        //(data[2], data[3]) = (data[3], data[2]);
        if (type == CommandType.Image) image.CopyTo(data, 2);
        if (type == CommandType.Display_Text) image.CopyTo(data, 3);
        return data;
    }
    
    
    public override string ToString() {
        return $"CP: {type} - {identifier} - {value}";
    }
    
    public enum CommandType : byte {
        RESET_CON, Position, Image, Display_Text, Mode, IGNORE = 101
    }
}