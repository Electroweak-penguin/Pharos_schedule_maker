import cv2
import fitz
import sys
import os
import numpy as np
import matplotlib.pyplot as plt

def get_image_from_pdf(fname):
    with fitz.open(fname) as doc:
        for page in doc:  # 1ページずつ処理
            for img_index, img in enumerate(page.get_images(), start=1):  # 画像を抽出
                x = doc.extract_image(img[0])
                img_data = x["image"]  # バイト形式の画像データ
                img_ext = x["ext"]  # 拡張子
                
                # OpenCVで処理可能な形式に変換
                np_img = np.frombuffer(img_data, dtype=np.uint8)
                if img_ext.lower() in ['jpg', 'jpeg']:
                    print("Oh, this is a jpeg file")
                elif img_ext.lower() == 'png':
                    image = cv2.imdecode(np_img, cv2.IMREAD_UNCHANGED)  # PNG形式をそのまま読み込み
                else:
                    raise ValueError(f"Unsupported image format: {img_ext}")
                
                print(f"Page {page.number + 1}, Image {img_index}: Loaded")
                return image  # 最初の画像のみ処理する前提なので、ここで終了
    raise ValueError("No images found in the PDF file.")

def consecutive(array, isVertical=True, sequential = 10):
    # 各1成分目が0か1に更新されるリストを生成
    if not isVertical:
        array = array.T
    updated_first_component = [
        0 if 0 in row[:] else 1  # 0があれば1成分目を0に
        for row in array
    ]
    # 更新後のリストから連続する1の数をチェック
    current_consecutive_ones = 0
    for value in updated_first_component:
        if value == 1:
            current_consecutive_ones += 1
            if current_consecutive_ones >= sequential:  # 10回連続0が見つかれば True を返す
                return True
        else:
            current_consecutive_ones = 0
    return False  # 10回連続が見つからなければ False を返す

image = get_image_from_pdf("timetable.pdf")

# 画像のサイズ（ピクセル数）を取得
height, width = image.shape

# 白なら 1, 黒なら 0 の配列を作成
binary_array = np.where(image == 255, 1, 0)

# スキャンの設定
rect_height = 420  # 四角形の高さ
rect_width = 1280  # 四角形の幅
line_thickness = 20  # 枠の太さ
fill_ratio = 1  # 黒の充填率
xvalue = 0
yvalue = 0
# 四角形のスキャン
for y in range(1050, height - rect_height + 1 - 700, 5):  # 5ピクセルごとにスキャン
    for x in range(0, width - rect_width + 1, 5):  # 5ピクセルごとにスキャン
        # 四角形内の領域を抽出
        roi = binary_array[y:y + rect_height, x:x + rect_width]

        # 四角形の枠部分を抽出
        mergin = 20
        frame_top = roi[:line_thickness, mergin:-mergin]  # 上辺
        frame_bottom = roi[-line_thickness:, mergin:-mergin]  # 下辺
        frame_left = roi[mergin:-mergin, :line_thickness]  # 左辺
        frame_right = roi[mergin:-mergin, -line_thickness:]  # 右辺
        if (consecutive(frame_top, False, 30) or consecutive(frame_bottom, False, 30) or consecutive(frame_left, True, 30) or consecutive(frame_right, True, 30)):
            continue
        point_top = np.sum(frame_top)/frame_top.size
        point_bottom = np.sum(frame_bottom)/frame_bottom.size
        point_left = np.sum(frame_left)/frame_left.size
        point_right = np.sum(frame_right)/frame_right.size

        tmp_fill_ratio = (point_bottom + point_left + point_right + point_top) / 4
        if tmp_fill_ratio < fill_ratio:
            fill_ratio = tmp_fill_ratio
            xvalue = x
            yvalue = y
print(xvalue, yvalue, fill_ratio)
roitable = binary_array[yvalue:yvalue + rect_height, xvalue:xvalue + rect_width]
print("gere")
# 縦線と横線を抽出する機能
horizontal_line = []
vertical_line = []
horizontal_lines = []
vertical_lines = []

vertical_threshold = 0.9
horizontal_threshold = 0.9
# 横線の抽出
y_tmp = 0
for y in range(0, rect_height + 1, 5):  # 3ピクセル間隔でスキャン
    mergin = 20
    width_tmp = 15
    roi = roitable[y:y + width_tmp, mergin:-mergin]  # 幅全体を含む領域
    if (consecutive(roi, False, 20)):
        continue
    fill_ratio = np.sum(roi) / (rect_width*width_tmp)  # 各行ごとの充填率を計算
    if fill_ratio < horizontal_threshold:
        if (y - y_tmp) < 20:
            continue
        y_tmp = y
        horizontal_line.append(y)
        horizontal_lines.append((0, y, rect_width, y))

# 縦線の抽出
x_tmp = 0
for x in range(0, rect_width + 1, 5):  # 3ピクセル間隔でスキャン
    mergin = 20
    width_tmp = 10
    roi = roitable[mergin+40:-mergin, x:x + width_tmp]  # 高さ全体を含む領域
    if (consecutive(roi, True, 20)):
        continue
    fill_ratio = np.sum(roi) / (rect_height*width_tmp)   # 各列ごとの充填率を計算
    if fill_ratio < vertical_threshold:
        if (x - x_tmp) < 20:
            continue
        x_tmp = x
        vertical_line.append(x)
        vertical_lines.append((x, 0, x, rect_height))

mergin = 20
output_data = []

# Available Slots: の情報を作成
for x in range(0, len(vertical_line) - 1):
    class_count = []
    for y in range(1, len(horizontal_line) - 1):
        roi = roitable[
            horizontal_line[y] + mergin : horizontal_line[y + 1] - mergin,
            vertical_line[x] + mergin : vertical_line[x + 1] - mergin,
        ]
        if (np.sum(roi) / (roi.shape[0] * roi.shape[1]) < 0.98):
            class_count.append(y)

    # 日付 x+1 をベースに出力フォーマットを整形
    if len(class_count) == 0:
        output_data.append(f"5/{x+6} 0")
    else:
        output_data.append(f"5/{x+6} " + " ".join(map(str, class_count)))

# テキストファイルにダンプ
output_file = "available_slots.txt"
with open(output_file, "w") as file:
    file.write("Available Slots:\n")
    file.write("\n".join(output_data))

# 結果を描画
line_image = cv2.cvtColor((roitable * 255).astype(np.uint8), cv2.COLOR_GRAY2BGR)

# 横線を赤で描画
for line in horizontal_lines:
    x1, y1, x2, y2 = line
    cv2.line(line_image, (x1, y1), (x2, y2), (0, 0, 255), 1)

# 縦線を青で描画
for line in vertical_lines:
    x1, y1, x2, y2 = line
    cv2.line(line_image, (x1, y1), (x2, y2), (255, 0, 0), 1)

# 結果を表示
plt.figure(figsize=(12, 6))
plt.imshow(cv2.cvtColor(line_image, cv2.COLOR_BGR2RGB))
plt.axis('off')
plt.show()