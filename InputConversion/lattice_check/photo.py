import fitz
import sys
import os
import cv2

def list_image_formats(fname):
    # PDFファイルのディレクトリとベース名を取得
    directory = os.path.dirname(os.path.abspath(fname))
    base_name = os.path.splitext(os.path.basename(fname))[0]
    
    with fitz.open(fname) as doc:
        for i, page in enumerate(doc):
            for j, img in enumerate(page.get_images()):
                x = doc.extract_image(img[0])
                img_data = x['image']
                img_ext = x['ext']
                
                # 画像ファイルの名前を作成 (ページ番号と画像番号)
                img_filename = f"{base_name}_page{i+1}_img{j+1}.{img_ext}"
                img_path = os.path.join(directory, img_filename)
                
                # 画像を保存
                with open(img_path, "wb") as f:
                    f.write(img_data)
                
                print(f"Page {i + 1}, Image {j + 1}: Saved as {img_filename}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        fname = "timetable.pdf"  # デフォルトのPDFファイル
    else:
        fname = sys.argv[1]
    
    list_image_formats(fname)

    # 画像の読み込み
    image_path = 'timetable_page1_img1.png'  # 白黒画像のパス
    image = cv2.imread(image_path, cv2.IMREAD_GRAYSCALE)  # グレースケールで読み込み
