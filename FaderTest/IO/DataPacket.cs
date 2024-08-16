using System.Runtime.InteropServices;
using StructPacker;

namespace FaderTest.IO;

[StructLayout(LayoutKind.Explicit)] 
public struct DataPacket {
    [FieldOffset(0)] public Type type;
    [FieldOffset(1)] public byte identifier; // virtual fader - fader+page
    [FieldOffset(2)] public ushort value; // position or state
    
    
    [FieldOffset(1)] public byte c1; // position or state
    [FieldOffset(2)] public byte c2; // position or state
    [FieldOffset(3)] public byte c3; // position or state
    
    public enum Type : byte {
        Error, Position, State, PageChange, Debug = 100
    }

    public Byte[] asBytes() {
        Byte[] data = new Byte[4];
        Util.StructureToArray(this, ref data);
        return data;
    }

    public static DataPacket from(byte[] data) {
        return Util.ArrayToStructure<DataPacket>(data);
    }

    public override string ToString() {
        return $"Data: {type} - {identifier} - {value}";
    }
}