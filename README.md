# Annie's Panner

Welcome to **Annie's Panner**! ִ

࣪ ˖ ࣪ ᨰꫀᥣᥴ᥆ꩇꫀ ! ᰔ ִ°🥂⋆.ೃ🍾࿔*:･ ׄ

<img src="Annie's%20Panner.png" alt="Annie's Panner Screenshot" width="60%">

Annie's Panner is a stereo panner made with **JUCE**.  
It includes two modes: **Balance** and **Stereo Pan**.



## Demo Video

The following video demonstrates the difference between the two pan modes.

In this demo, I used a test WAV file with different content in the left and right channels:

- The left channel contains a minimal-style track  
- The right channel contains a rhythmic track

Watch the video: https://youtu.be/2zmgMn7s8Ec?si=W2HwXww34XUHyh1d 



## Modes

### Balance Mode

This mode moves the stereo image by controlling the volume balance between the left and right channels:

- At the center position, the original stereo content is unchanged  
- Panning left or right only attenuates the opposite channel  
- The original stereo width is preserved  



### Stereo Pan Mode

By using a $2 \times 2$ stereo matrix and the Equal-Power Law, this mode remaps input signals to the output soundstage through inter-channel cross-mixing. Unlike the Balance mode, this mode preserves the original signal's energy distribution and sonic details throughout the panning process, ensuring that no information from either side is lost.

#### Auto Width & Field Folding

Inspired by the stereo pan in **Logic Pro**:

- Center Position: The soundstage is fully expanded at its maximum geometric width ($100\%$ Width).
- The Panning Process: As the sound moves away from the center, the algorithm asymmetrically compresses the soundstage boundaries. This creates a "Field Folding" effect where the closer the sound is to the edges, the more the stereo width automatically narrows.
- Edge Positions: When the pan reaches an extreme (hard left or hard right), the soundstage completely closes and converges into a monophonic signal ($0\%$ Width). 

---

### Installation

#### Option 1: Download Binaries

[Download here](https://github.com/dbb1019/Annie-s-Panner/releases/tag/v0.9.0)

Move the files to:

# VST3
```~/Library/Audio/Plug-Ins/VST3/```

# AU (.component)
```~/Library/Audio/Plug-Ins/Components/```


macOS Security Fix: If blocked, go to System Settings > Privacy & Security and click "Open Anyway".
(Or run ```sudo xattr -cr [plugin_path]``` in Terminal).

#### Option 2: Build from Source

Clone the repo: ```git clone https://github.com/dbb1019/Annie-s-Panner.git```

Open ```Annie's Panner.jucer``` in Projucer.

Export to Xcode or Visual Studio and build.

---

It is a stereo panner...  
It moves sound left...  
It moves sound right...
