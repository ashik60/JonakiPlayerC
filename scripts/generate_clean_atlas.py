"""
Procedural Clean-Room Spritesheet Generator for JonakiPlayer Default Skin.
Generates an independent, pixel-aligned RGBA atlas at exactly (1070, 1456)
matching reference coordinate layouts without copying proprietary artwork.
"""

import os
from PIL import Image, ImageDraw, ImageFont

def generate_clean_atlas():
    w, h = 1070, 1456
    img = Image.new('RGBA', (w, h), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    # 1. Title bar background slice: (3, 490, 347, 31)
    # Dark charcoal gradient #242426 -> #18181a with 1px top border #3a3a3c
    for y in range(490, 490 + 31):
        ratio = (y - 490) / 30.0
        val = int(36 * (1.0 - ratio) + 24 * ratio)
        draw.line([(3, y), (3 + 346, y)], fill=(val, val, val, 255))
    draw.line([(3, 490), (3 + 346, 490)], fill=(58, 58, 60, 255))
    draw.line([(3, 490 + 30), (3 + 346, 490 + 30)], fill=(20, 20, 20, 255))

    # 2. Main Menu Button: (0, 43, 89, 30)
    # Renders "POT" or "JONAKI" brand style button with dropdown arrow
    # States: Normal (0..88), Hover (0..88 y+30), Pressed (0..88 y+60)
    for state, bg_alpha in enumerate([255, 230, 200]):
        sy = 43 + state * 30
        draw.rectangle([0, sy, 88, sy + 29], fill=(30, 30, 33, bg_alpha))
        draw.rectangle([0, sy, 88, sy + 29], outline=(50, 50, 55, 255), width=1)
        # Down arrow at x=74, y=sy+13
        draw.polygon([(72, sy + 12), (78, sy + 12), (75, sy + 16)], fill=(180, 180, 180, 255))

    # 3. Window Control Buttons (14x13):
    # Minimize (0, 20), Maximize (65, 20), Restore (130, 20), Close (260, 20), Pin (485, 68)
    def draw_window_btn(x, y, icon_type, color=(180, 180, 180, 255)):
        # 4 states horizontally: normal (x), hover (x+14), pressed (x+28), disabled (x+42)
        for s in range(4):
            bx = x + s * 14
            if icon_type == 'min':
                draw.line([(bx + 2, y + 9), (bx + 11, y + 9)], fill=color, width=1)
            elif icon_type == 'max':
                draw.rectangle([bx + 2, y + 2, bx + 11, y + 10], outline=color, width=1)
                draw.line([(bx + 2, y + 3), (bx + 11, y + 3)], fill=color, width=1)
            elif icon_type == 'restore':
                draw.rectangle([bx + 4, y + 1, bx + 11, y + 8], outline=color, width=1)
                draw.rectangle([bx + 2, y + 4, bx + 9, y + 11], outline=color, width=1)
            elif icon_type == 'close':
                draw.line([(bx + 3, y + 2), (bx + 10, y + 9)], fill=(220, 80, 80, 255) if s == 1 else color, width=1)
                draw.line([(bx + 3, y + 9), (bx + 10, y + 2)], fill=(220, 80, 80, 255) if s == 1 else color, width=1)
            elif icon_type == 'pin':
                draw.line([(bx + 6, y + 2), (bx + 6, y + 7)], fill=color, width=2)
                draw.line([(bx + 4, y + 7), (bx + 8, y + 7)], fill=color, width=1)
                draw.line([(bx + 6, y + 7), (bx + 6, y + 11)], fill=color, width=1)

    draw_window_btn(0, 20, 'min')
    draw_window_btn(65, 20, 'max')
    draw_window_btn(130, 20, 'restore')
    draw_window_btn(260, 20, 'close')
    draw_window_btn(485, 68, 'pin')

    # 4. Bottom Playback Buttons (41x40):
    # Play (0, 259), Pause (164, 259), Stop (0, 300), Prev (164, 300), Next (328, 300), Open (0, 341)
    def draw_media_btn(x, y, icon_type):
        for s in range(4):
            bx = x + s * 41
            c = (220, 220, 220, 255) if s == 0 else ((255, 255, 255, 255) if s == 1 else (160, 160, 160, 255))
            if icon_type == 'play':
                draw.polygon([(bx + 16, y + 13), (bx + 16, y + 27), (bx + 28, y + 20)], fill=c)
            elif icon_type == 'pause':
                draw.rectangle([bx + 15, y + 13, bx + 18, y + 27], fill=c)
                draw.rectangle([bx + 22, y + 13, bx + 25, y + 27], fill=c)
            elif icon_type == 'stop':
                draw.rectangle([bx + 15, y + 14, bx + 26, y + 25], fill=c)
            elif icon_type == 'prev':
                draw.line([(bx + 13, y + 14), (bx + 13, y + 26)], fill=c, width=2)
                draw.polygon([(bx + 26, y + 14), (bx + 26, y + 26), (bx + 15, y + 20)], fill=c)
            elif icon_type == 'next':
                draw.polygon([(bx + 14, y + 14), (bx + 14, y + 26), (bx + 25, y + 20)], fill=c)
                draw.line([(bx + 27, y + 14), (bx + 27, y + 26)], fill=c, width=2)
            elif icon_type == 'open':
                # Folder icon
                draw.rectangle([bx + 13, y + 16, bx + 27, y + 25], outline=c, width=1)
                draw.polygon([(bx + 13, y + 16), (bx + 18, y + 16), (bx + 20, y + 14), (bx + 25, y + 14), (bx + 27, y + 16)], fill=c)

    draw_media_btn(0, 259, 'play')
    draw_media_btn(164, 259, 'pause')
    draw_media_btn(0, 300, 'stop')
    draw_media_btn(164, 300, 'prev')
    draw_media_btn(328, 300, 'next')
    draw_media_btn(0, 341, 'open')

    # 5. Bottom control area background slices:
    # 22px Seekbar rail area: (3, 668, 306, 22)
    # 40px Playback button area: (3, 690, 306, 40)
    for y in range(668, 668 + 22):
        ratio = (y - 668) / 21.0
        val = int(32 * (1.0 - ratio) + 24 * ratio)
        draw.line([(3, y), (3 + 305, y)], fill=(val, val, val, 255))
    draw.line([(3, 668), (3 + 305, 668)], fill=(45, 45, 48, 255))

    for y in range(690, 690 + 40):
        ratio = (y - 690) / 39.0
        val = int(24 * (1.0 - ratio) + 18 * ratio)
        draw.line([(3, y), (3 + 305, y)], fill=(val, val, val, 255))
    draw.line([(3, 690), (3 + 305, 690)], fill=(38, 38, 40, 255))

    # 6. Timeline Seekbar Rails & Thumb:
    # Rail background: (0, 179, 95, 10)
    # Buffered range bar: (0, 193, 95, 10)
    # Active progress fill: (0, 207, 95, 10)
    # Thumb knob (11x10): (1, 239)
    draw.rectangle([0, 179 + 3, 94, 179 + 6], fill=(45, 45, 48, 255))
    draw.rectangle([0, 193 + 3, 94, 193 + 6], fill=(70, 70, 75, 255))
    draw.rectangle([0, 207 + 3, 94, 207 + 6], fill=(240, 180, 20, 255)) # PotPlayer yellow fill

    # Thumb knob (11x10 pill shape)
    draw.rounded_rectangle([1, 239, 1 + 10, 239 + 9], radius=3, fill=(220, 220, 225, 255), outline=(100, 100, 105, 255))

    # 7. Volume Slider & Mute Button:
    # Mute button (13x10): (175, 167)
    # Volume rail (70x10): (175, 194)
    # Volume thumb (11x10): (85, 239)
    draw.rectangle([175, 194 + 3, 175 + 69, 194 + 6], fill=(45, 45, 48, 255))
    draw.rectangle([175, 207 + 3, 175 + 69, 207 + 6], fill=(240, 180, 20, 255))
    draw.rounded_rectangle([85, 239, 85 + 10, 239 + 9], radius=3, fill=(200, 200, 205, 255))

    # Speaker mute icon (13x10)
    draw.polygon([(176, 170), (178, 170), (181, 168), (181, 176), (178, 174), (176, 174)], fill=(180, 180, 180, 255))
    draw.arc([179, 169, 185, 175], start=-60, end=60, fill=(180, 180, 180, 255), width=1)

    # 8. Playlist Toggle & Control Box Toggle Buttons (23x22):
    # Playlist (202, 391), Control Box (303, 391)
    draw.rectangle([202 + 4, 391 + 4, 202 + 18, 391 + 17], outline=(180, 180, 180, 255), width=1)
    draw.line([(202 + 7, 391 + 7), (202 + 15, 391 + 7)], fill=(180, 180, 180, 255))
    draw.line([(202 + 7, 391 + 10), (202 + 15, 391 + 10)], fill=(180, 180, 180, 255))
    draw.line([(202 + 7, 391 + 13), (202 + 15, 391 + 13)], fill=(180, 180, 180, 255))

    draw.rectangle([303 + 4, 391 + 4, 303 + 18, 391 + 17], outline=(180, 180, 180, 255), width=1)
    draw.rectangle([303 + 7, 391 + 7, 303 + 15, 391 + 14], fill=(180, 180, 180, 255))

    os.makedirs('resources/skins/default', exist_ok=True)
    out_file = 'resources/skins/default/Main.png'
    img.save(out_file, 'PNG')
    print(f'Successfully generated clean-room atlas at {out_file} (size: {img.size})')

if __name__ == '__main__':
    generate_clean_atlas()
