# Annie's Panner

Welcome to **Annie's Panner**! ִ

࣪ ˖ ࣪ ᨰꫀᥣᥴ᥆ꩇꫀ ! ᰔ ִ°🥂⋆.ೃ🍾࿔*:･ ׄ

<img src="Annie's%20Panner.png" alt="Annie's Panner Screenshot" width="60%">

Annie's Panner is a stereo panner made with **JUCE**.  
It includes two modes: **Balance** and **Stereo Pan**.


## Modes

### Balance Mode

This mode moves the stereo image by controlling the volume balance between the left and right channels:

- At the center position, the original stereo content is unchanged  
- Panning left or right only attenuates the opposite channel  
- The original stereo width is preserved  



### Stereo Pan Mode

This mode redistributes energy between the left and right channels using a stereo matrix and equal-power calculation:

- The stereo image moves as a whole, with cross-mixing between channels  
- The spatial perception is more natural  

#### Auto Width

Inspired by the stereo pan in **Logic Pro**:

- The closer the sound is to the center, the wider the stereo image  
- The closer the sound is to the edges, the stereo width automatically narrows  

---

It has zero revolutionary features.  
It moves sound left...  
It moves sound right...

If you are on macOS, you can download the AU and VST3 versions from the **Builds > macOS > build > Release** folder.
