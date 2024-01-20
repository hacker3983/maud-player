import os
import platform

LINK_FILES = "-lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer"

print("Building music player...")
os.system("gcc -c music_player.c music_filemanager.c")
print("Running music player...")

if platform.system() != "Windows":
    os.system(f"gcc main.c music_player.o music_filemanager.o {LINK_FILES} -o mplayer && ./mplayer")
else:
    os.system(f"gcc main.c music_player.o music_filemanager.o -lmingw32 -lcomdlg32 {LINK_FILES} -o mplayer.exe && .\\mplayer.exe")