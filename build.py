import os
import platform

LINK_FILES = ""
if platform.system() == "Windows":
    LINK_FILES = "-lmingw32 -lshlwapi -lcomdlg32 "
LINK_FILES += "-lm -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer"

C_FILES = [
    "music_player.c",
    "music_filemanager.c",
    "music_scrollcontainers.c",
    "music_selectionmenu.c",
    "music_settingsmenu.c",
    "music_playerscroll_types.c"
]

O_FILES = " ".join(C_FILES).replace(".c", ".o")

print("Building music player...")
os.system(f"gcc -c {' '.join(C_FILES)}")
print("Running music player...")

if platform.system() != "Windows":
    os.system(f"gcc main.c {O_FILES} {LINK_FILES} -o mplayer && ./mplayer")
else:
    os.system(f"gcc -ggdb main.c {O_FILES} {LINK_FILES} -o mplayer.exe && .\\mplayer.exe")