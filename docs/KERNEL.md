# CAN WE DO COOL PROGRAMMATIC STUFF WITH THE KERNEL?

The Kernel seems to be the portal between hardware and software.

GUI is a naked digital projection of byte combinations in memory.
e.g., when you type on the keyboard, its ASCII byte combination will
manifest itself as transistor on/off states, logged in memory then
rendered through kernel, which acts as proxy, relays to appropriate
app/window server (organises bitmap i.e., pixel grid), which then
requests the GPU to light up the char to display itself as each unit of data on a file.

EXAMPLE:
1. Byte 0x61 → 97 decimal → “a” in ASCII (American Standard Code for Information Interchange).
2. Your program calls a text API; the font engine fetches the “a” outline from a TrueType/OpenType font file.
3. Rasterizer turns that vector outline into a pixel mask (bitmap).
4. GPU writes that mask into its frame-buffer in Video RAM (Random-Access Memory dedicated to graphics).
5. Display controller streams those pixel values to the LCD/LED panel, driving transistors that modulate light and you see the glyph.


# Accessing Kernel on MACOS
1. I know there is a non-persistent (temporary) dir called `/dev` (computer functional devices control dir), which is initialised on boot by the kernel. There is `./mem` (which I assume to be direct access to RAM stack and heap) that can only be accessed if we disable System Integrity Protection.

2. `/dev` has `device nodes` that communicate with hardware that allows the computer to function (e.g., disks, keyboards)

3. macOS mounts `/dev` as `devfs` live into RAM.

**Observation:** Can't `open .` `/dev` or find it on finder (Flagged as UF_HIDDEN so can never appear on GUI).

That's enough for now.