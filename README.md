Program runs on Visual Studio Community 2019 in both Debug and Release mode in 64 bit.
DO NOT run in 32 bit.

TO RUN THE PROGRAM:
1. Using Visual Studio Community 2019 in either Debug or Release in 64 bit mode.

TO CHANGE THE AUDIO FILES:
1. From the solution level, navigate through common->assets->audio
2. In the audio folder, add and remove any audio files
3. Reflect any changes made by copying the names of any new audio files to the audioList.txt (including the file extension)
4. Reflect any changes made by removing the names of any removed audio files from the audioList.txt
5. Currently only works with .mp3 and .wav files

TO OPERATE THE PROGRAM:
1. This is a simple sound board programmed to play sounds using keys 1-0 and space (must be on the correct channel group or the master channel to play sounds)
2. Pressing TAB will pause/unpause the currently selected channel group
3. Pressing keypad UP and keypad DOWN will raise and lower the volume of the current channel group
4. Pressing keypad LEFT and keypad RIGHT will move the pan of the current channel group
5. Pressing Z will change the channel group to _cgMusic
6. Pressing X will change the channel group to _cgSE1
7. Pressing C will change the channel group to _cgSE2
8. Pressing V will change the channel group to _cgSE3
9. Pressing M will change the channel group to _cgMaster
10. Pressing QWER will switch the DSP's active state for the four DSPs on the current channel group
11. Pressing T will switch all of the current channel group's DSPs active state
12. Pressing ASDF will switch the DSP's bypass state for the four DSPs on the current channel group
13. Pressing G will switch all of the current channel group's DSPs bypass state
14. Pressing U and I will decrease and increase the echo delay (which only affects _cgMusic)
15. Pressing O and P will decrease and increase the highpass cutoff (which only affects _cgSE1)
16. Pressing H and J will decrease and increase the tremolo frequency (which only affects _cgSE2)
17. Pressing K and L will decrease and increase the oscillator frequency (which only affects _cgSE3)
18. Pressing , and . will decrease and increase the chorus rate (which affects the _cgMaster, which will in turn affect all other channel groups)

POTENTIAL SOUND WARNING! (In Recording)
At 24:10 I start the program and it can potentially be very loud (it surprised me how loud it was) and 33:00-34:00 I mess with the oscillator which some with sensitive hearing will want to skip (it gets very high pitched)

VIDEO
https://youtu.be/8vHP-Q8LqvI