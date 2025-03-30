import os
import platform

LINK_FILES = ""
if platform.system() == "Windows":
    LINK_FILES = "-lmingw32 -lshlwapi -lcomdlg32 -lsetupapi -lcfgmgr32 -lole32 -lsetupapi -lgdi32 -limm32 "
LINK_FILES += "-lm -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer"

C_FILES = [
    "music_player.c",
    "music_string.c",
    "music_tooltips.c",
    "stringlist.c",
    "music_queue.c",
    "music_notification.c",
    "music_checkboxes.c",
    "music_inputboxes.c",
    "music_menumanager.c",
    "music_filemanager.c",
    "music_textmanager.c",
    "music_texturemanager.c",
    "music_itemcontainer.c",
    "music_scrollcontainer.c",
    "music_selectionmenu.c",
    "music_settingsmenu.c",
    "music_playerscroll_types.c",
    "music_playerbutton_manager.c",
    "music_playlists.c",
    "music_playlistmanager.c",
    "music_playlistmanager_datareader.c",
    "music_playlistmanager_datawriter.c",
    "music_songsmanager.c"
]

print("Building music player...")
os.system(f"gcc main.c {' '.join(C_FILES)} {LINK_FILES} -o mplayer")
print("Running music player...")
if platform.system() == "Linux":
    os.system("./mplayer")
else:
    os.system(f"mplayer.exe")