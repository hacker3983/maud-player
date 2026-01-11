import os
import platform

LINK_FILES = ""
if platform.system() == "Windows":
    LINK_FILES = "-lmingw32 -lshlwapi -lcomdlg32 -lsetupapi -lcfgmgr32 -lole32 -lgdi32 -limm32 "
LINK_FILES += "-lm -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer"

C_FILES = [
    "maud_player.c",
    "maud_string.c",
    "maud_tooltips.c",
    "stringlist.c",
    "maud_queue.c",
    "maud_queue_renderer.c",
    "maud_colorpicker.c",
    "maud_notification.c",
    "maud_checkbox.c",
    "maud_inputboxes.c",
    "maud_menumanager.c",
    "maud_filemanager.c",
    "maud_textmanager.c",
    "maud_texturemanager.c",
    "maud_itemcontainer.c",
    "maud_scrollcontainer.c",
    "maud_selectionmenu.c",
    "maud_selectionmenu_renderer.c",
    "maud_settingsmenu.c",
    "maud_settingsmenu_customize.c",
    "maud_settingsmenu_musiclibrary.c",
    "maud_settingsmenu_about.c",
    "maud_playerscroll_types.c",
    "maud_playerbutton_manager.c",
    "maud_playlists.c",
    "maud_playlistmanager.c",
    "maud_playlistmanager_layout.c",
    "maud_playlistmanager_buttonbar.c",
    "maud_playlistmanager_newplaylist_input.c",
    "maud_playlistmanager_layoutdropdown_menu.c",
    "maud_playlistmanager_gridrenderer.c",
    "maud_playlistmanager_listrenderer.c",
    "maud_playlistmanager_datareader.c",
    "maud_playlistmanager_datawriter.c",
    "maud_songsmanager.c"
]

print("Building music player...")
os.system(f"gcc main.c {' '.join(C_FILES)} {LINK_FILES} -o maud")
print("Running music player...")

if platform.system() == "Linux":
    os.system("./maud")
else:
    os.system(f"maud.exe")
