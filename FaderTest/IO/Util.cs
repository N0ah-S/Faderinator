using System.Runtime.InteropServices;

namespace FaderTest.IO;

public class Util {
    public static S ArrayToStructure<S>(System.Array aData) where S : struct {
        GCHandle iHandle = default;
        S sRecord;
        try {
            iHandle = GCHandle.Alloc(aData, GCHandleType.Pinned);
            sRecord = Marshal.PtrToStructure<S>(iHandle.AddrOfPinnedObject());
        }
        finally {
            iHandle.Free();
        }

        return sRecord;
    }

    public static void StructureToArray<S, E>(S rStruct, ref E[] aArray)
        where S : struct
        where E : struct {
        GCHandle iHandle = default;
        try {
            iHandle = GCHandle.Alloc(aArray, GCHandleType.Pinned);
            Marshal.StructureToPtr<S>(rStruct, iHandle.AddrOfPinnedObject(), false);
        } finally {
            iHandle.Free();
        }
    }
}