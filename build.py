import os
import platform

LINK_FILES = "-lm -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer"

print("Building music player...")
os.system("gcc -c music_player.c music_filemanager.c music_selectionmenu.c")
print("Running music player...")

if platform.system() != "Windows":
    os.system(f"gcc main.c music_player.o music_filemanager.o music_selectionmenu.o {LINK_FILES} -o mplayer && ./mplayer")
else:
    os.system(f"gcc -ggdb main.c music_player.o music_filemanager.o music_selectionmenu.o -lmingw32 -lshlwapi -lcomdlg32 {LINK_FILES} -o mplayer.exe && .\\mplayer.exe")