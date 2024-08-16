using System.Collections;
using System.Diagnostics;
using System.Drawing;

namespace FaderTest;

using NAudio.CoreAudioApi;
using NAudio.CoreAudioApi.Interfaces;

public class AudioManager : IMMNotificationClient {
    private const string ACTUAL_AMAZON_MUSIC_PROCESS_ID =
        @"{0.0.0.00000000}.{e1a65454-b40d-4264-bea1-7c772aad61c9}|\Device\HarddiskVolume8\Users\noaha\AppData\Local\Amazon Music\Amazon Music.exe%b{00000000-0000-0000-0000-000000000000}";
    
    private MMDeviceEnumerator deviceEnum;
    private MMDevice device;

    public List<AudioChannel> channels;

    public AudioManager() {
        deviceEnum = new MMDeviceEnumerator();
        channels = new List<AudioChannel>();
        
        SetAudioSessions();

        foreach (var channel in channels) {
            Console.WriteLine("We did it? {0}", channel.GetIcon().Length);
        }
        
        deviceEnum.RegisterEndpointNotificationCallback(this);
    }

    public void SetAudioSessions() {
        try {
            if (device != null) {
                device.AudioSessionManager.OnSessionCreated -= AddAudioSession;
            }
            else {
                Console.WriteLine("impossible");
            }

            /*audioSessions.Clear();*/

            device = deviceEnum.GetDefaultAudioEndpoint(DataFlow.Render, Role.Multimedia);
            var sessions = device.AudioSessionManager.Sessions;
            if (sessions != null) {
                for (int i = 0; i < sessions.Count; i++) {
                    var session = sessions[i];
                    Console.WriteLine("{0}: {1} ---- [{2}]", GetProcessName(session), session.SimpleAudioVolume.Volume,
                        session.GetSessionIdentifier);
                    if (session.IsSystemSoundsSession || !ProcessExists(session.GetProcessID)) continue;
                    if (GetProcessName(session) == "Amazon Music") {
                        if(session.GetSessionIdentifier != ACTUAL_AMAZON_MUSIC_PROCESS_ID) continue;
                    }
                    AddAudioSession(session);
                }
            }

            device.AudioSessionManager.OnSessionCreated += AddAudioSession;
        }
        catch (Exception ex) {
            /*SentrySdk.CaptureException(ex, scope => { scope.TransactionName = "AudioManager"; });*/
        }
    }

    private void Dispose() {
        deviceEnum.UnregisterEndpointNotificationCallback(this);
        device.AudioSessionManager.OnSessionCreated -= AddAudioSession;
        channels.Clear();
    }

    bool ProcessExists(uint processId) {
        try {
            var process = Process.GetProcessById((int)processId);
            return true;
        } catch (ArgumentException) {
            return false;
        }
    }

    public static string GetProcessName(AudioSessionControl audioSessionControl) {
        return Process.GetProcessById((int)audioSessionControl.GetProcessID).ProcessName;
    }

    void AddAudioSession(AudioSessionControl session) {
        var channel = new AudioChannel(this, device, session);
        channels.Add(channel);
    }

    void AddAudioSession(object sender, IAudioSessionControl audioSessionControl) {
        var session = new AudioSessionControl(audioSessionControl);
        AddAudioSession(session);
    }

    public void OnDeviceStateChanged(string deviceId, DeviceState newState) {
    }

    public void OnDeviceAdded(string pwstrDeviceId) {
    }

    public void OnDeviceRemoved(string deviceId) {
    }

    public void OnDefaultDeviceChanged(DataFlow flow, Role role, string defaultDeviceId) {
        SetAudioSessions();
    }

    public void OnPropertyValueChanged(string pwstrDeviceId, PropertyKey key) {
    }
}