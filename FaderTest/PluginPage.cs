using FaderTest.IO;

namespace FaderTest;

/**
 * Code is documentation enough
 */
public interface PluginPage {
    void Setup(HWConnection con);
    void Handle(DataPacket packet);
    void Update(HWConnection con);
}