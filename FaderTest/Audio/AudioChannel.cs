using System.Diagnostics;
using System.Diagnostics.CodeAnalysis;
using System.Drawing;
using System.Text;
using NAudio.CoreAudioApi;
using NAudio.CoreAudioApi.Interfaces;

namespace FaderTest;

public class AudioChannel : IAudioSessionEventsHandler, IDisposable {
    private const long TIMEOUT = 100;

    private AudioManager manager;
    private MMDevice device;
    private AudioSessionControl session;

    private VolumeState state;
    private long silencedUntilMs;

    public AudioChannel(AudioManager manager, MMDevice device, AudioSessionControl session) {
        this.manager = manager;
        this.device = device;
        this.session = session;

        state = new VolumeState(session.SimpleAudioVolume.Volume);

        session.RegisterEventClient(this);
    }

    public void SetVolume(float volume) {
        session.SimpleAudioVolume.Volume = volume;
        session.SimpleAudioVolume.Mute = false;
        state.changed = false;
        silencedUntilMs = Environment.TickCount64 + TIMEOUT;
    }

    public VolumeState GetVolumeState() {
        VolumeState lastState = new VolumeState(state.volume, state.changed);
        state.changed = false;
        return lastState;
    }

    [SuppressMessage("Interoperability", "CA1416:Plattformkompatibilität überprüfen")]
    public byte[] GetIcon() {
        string? path = Process.GetProcessById((int)session.GetProcessID).MainModule?.FileName;
        if (string.IsNullOrEmpty(path)) return [];
        Icon? ico = Icon.ExtractAssociatedIcon(path);
        if (ico == null) return [];

        byte[] flatGreyscale = new byte[64 * 8];
        Bitmap bitmap = ico.ToBitmap();
        for (int y = 0; y < 64; y++) {
            for (int x = 0; x < 56;) {
                byte output = 0;
                for (int bitIndex = 0; bitIndex < 8; bitIndex++) {
                    output = (byte)(output << 1);
                    
                    Color pixelColor = bitmap.GetPixel(x++/2, y/2);
                    int grey = (int)(pixelColor.R * 0.3 + pixelColor.G * 0.54 + pixelColor.B * 0.11);
                    byte secureGrey = grey > 100 ? (byte)1 : (byte)0;
                    output += pixelColor.A < 150 ? (byte)0 : secureGrey;
                }
                //Console.WriteLine("{0} - {1} - {2}", x, y, y * 8 + x/8);
                flatGreyscale[y * 8 + x/8] = output; // black is the new transparent
            }
        }

        return flatGreyscale;
    }

    // Things that need to be overriden
    public void OnVolumeChanged(float volume, bool isMuted) {
        if (silencedUntilMs > Environment.TickCount64) return;
        state = new VolumeState(volume);
        //Console.WriteLine(volume);
    }

    public void OnDisplayNameChanged(string displayName) {
        throw new NotImplementedException();
    }

    public void OnIconPathChanged(string iconPath) {
        throw new NotImplementedException();
    }

    public void OnChannelVolumeChanged(uint channelCount, IntPtr newVolumes, uint channelIndex) {
        throw new NotImplementedException();
    }

    public void OnGroupingParamChanged(ref Guid groupingId) {
        throw new NotImplementedException();
    }

    public void OnStateChanged(AudioSessionState state) {
        throw new NotImplementedException();
    }

    public void OnSessionDisconnected(AudioSessionDisconnectReason disconnectReason) {
        throw new NotImplementedException();
    }

    public void Dispose() {
        throw new NotImplementedException();
    }

    public string GetName() {
        return AudioManager.GetProcessName(session);
    }
}

public class VolumeState {
    public bool changed;
    public float volume;

    public VolumeState(float volume) {
        this.volume = volume;
        this.changed = true;
    }

    public VolumeState(float volume, bool changed) {
        this.volume = volume;
        this.changed = changed;
    }
}