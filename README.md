# BeatTogether for v1.17.1

This is a version I forked from the original project development. Differences from the latest official release other than being compatible with Beat Saber Quest v1.17.1:

* Added username configuration, just in case you want to customize your username in Beat Together.
* I have updated the configuration system to be compatible with the configuration system provided by the BeatSaber-Hook library. This allows:
    * To configure the server address:port and the status address by editing the json config file in /sdcar/ModData/Configs folder.
    * Username override can also be enabled and configured by editing the same json file.

**I am not part of the Beat Together Team, responsible for the [original project development](https://github.com/pythonology/BeatTogether.Quest). Below, you can find the rest of the original readme information.**


# BeatTogether
A multiplayer private server for the modding community. Supports crossplay between PC and Quest. **This is the Quest Plugin.**

Feel free to join our Discord! https://discord.com/invite/gezGrFG4tz (Support, Coordinating games with friends, etc) 

Want to support development and server costs? [Click Here](https://www.patreon.com/BeatTogether)

## Features
* Private server free from Beat Saber official; allowing Quest to play freely on modded installs
* Crossplay between all compatible platforms
* Custom songs between platforms (With MultiQuestensions installed)
* 10 Player lobbies (If a PC players hosts the lobby and has MultiplayerExtensions plugin installed which is PC only) **This Feature is EXPERIMENTAL. For optimal experience its recommended to stick with 5 players.**

## Installation

**Recommended Install:**

Here is how to install the mod in headset:
- Go to the Browser section of the BMBF app.
- Click the small globe icon in the top right-hand corner and click QuestBoard
- Hit "Download Mods"
- Scroll down till you find "BeatTogether" and hit the blue download button on the right hand side.
- It will say its downloaded. Make sure to hit "Sync to Beat Saber" to apply changes

You are now ready to play!

**Manual Install:**

To install, Download the latest mod from our releases. [Click Here](https://github.com/pythonology/BeatTogether.Quest/releases)

After downloading, upload the .zip to BMBF on the upload tab of the web interface.

## Custom Song Support
Customs in multiplayer requires [MultiQuestensions](https://github.com/Goobwabber/MultiQuestensions)

## Special Thanks
Special thanks to [Sc2ad](https://github.com/Sc2ad) for helping me debug this.

## Credits
* [zoller27osu](https://github.com/zoller27osu), [Sc2ad](https://github.com/Sc2ad) and [jakibaki](https://github.com/jakibaki) - [beatsaber-hook](https://github.com/sc2ad/beatsaber-hook)
* [raftario](https://github.com/raftario) - [vscode-bsqm](https://github.com/raftario/vscode-bsqm) and [this template](https://github.com/raftario/bmbf-mod-template)
