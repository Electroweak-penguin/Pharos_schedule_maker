import cv2
import fitz
import os
import numpy as np

def get_image_from_pdf(fname):
    with fitz.open(fname) as doc:
        for page in doc:
            for img_index, img in enumerate(page.get_images(), start=1):
                x = doc.extract_image(img[0])
                img_data = x["image"]
                img_ext = x["ext"]

                np_img = np.frombuffer(img_data, dtype=np.uint8)
                image = cv2.imdecode(np_img, cv2.IMREAD_UNCHANGED)
                print(f"{fname}: Page {page.number + 1}, Image {img_index}: Loaded")
                return image
    raise ValueError("No images found in the PDF file.")

def consecutive(array, isVertical=True, sequential=10):
    if not isVertical:
        array = array.T
    updated_first_component = [
        0 if 0 in row[:] else 1
        for row in array
    ]
    current_consecutive_ones = 0
    for value in updated_first_component:
        if value == 1:
            current_consecutive_ones += 1
            if current_consecutive_ones >= sequential:
                return True
        else:
            current_consecutive_ones = 0
    return False

def process_pdf(file_path, output_dir, image_output_dir):
    student_name = os.path.splitext(os.path.basename(file_path))[0]
    output_file = os.path.join(output_dir, f"{student_name}.txt")
    line_image_output = os.path.join(image_output_dir, f"{student_name}.png")

    image = get_image_from_pdf(file_path)
    height, width = image.shape
    binary_array = np.where(image == 255, 1, 0)

    rect_height = 420
    rect_width = 1280
    line_thickness = 20
    fill_ratio = 1
    xvalue, yvalue = 0, 0

    for y in range(1050, height - rect_height + 1 - 700, 5):
        for x in range(0, width - rect_width + 1, 5):
            roi = binary_array[y:y + rect_height, x:x + rect_width]
            mergin = 20
            frame_top = roi[:line_thickness, mergin:-mergin]
            frame_bottom = roi[-line_thickness:, mergin:-mergin]
            frame_left = roi[mergin:-mergin, :line_thickness]
            frame_right = roi[mergin:-mergin, -line_thickness:]
            if (consecutive(frame_top, False, 30) or consecutive(frame_bottom, False, 30) or 
                consecutive(frame_left, True, 30) or consecutive(frame_right, True, 30)):
                continue
            point_top = np.sum(frame_top)/frame_top.size
            point_bottom = np.sum(frame_bottom)/frame_bottom.size
            point_left = np.sum(frame_left)/frame_left.size
            point_right = np.sum(frame_right)/frame_right.size

            tmp_fill_ratio = (point_bottom + point_left + point_right + point_top) / 4
            if tmp_fill_ratio < fill_ratio:
                fill_ratio = tmp_fill_ratio
                xvalue, yvalue = x, y

    roitable = binary_array[yvalue:yvalue + rect_height, xvalue:xvalue + rect_width]
    horizontal_line, vertical_line = [], []
    horizontal_lines, vertical_lines = [], []
    vertical_threshold = 0.9
    horizontal_threshold = 0.9

    y_tmp = 0
    for y in range(0, rect_height + 1, 5):
        mergin = 20
        width_tmp = 15
        roi = roitable[y:y + width_tmp, mergin:-mergin]
        if consecutive(roi, False, 20):
            continue
        fill_ratio = np.sum(roi) / (rect_width*width_tmp)
        if fill_ratio < horizontal_threshold:
            if (y - y_tmp) < 20:
                continue
            y_tmp = y
            horizontal_line.append(y)
            horizontal_lines.append((0, y, rect_width, y))

    x_tmp = 0
    for x in range(0, rect_width + 1, 5):
        mergin = 20
        width_tmp = 10
        roi = roitable[mergin+40:-mergin, x:x + width_tmp]
        if consecutive(roi, True, 20):
            continue
        fill_ratio = np.sum(roi) / (rect_height*width_tmp)
        if fill_ratio < vertical_threshold:
            if (x - x_tmp) < 20:
                continue
            x_tmp = x
            vertical_line.append(x)
            vertical_lines.append((x, 0, x, rect_height))

    mergin = 20
    output_data = []
    for x in range(0, len(vertical_line) - 1):
        class_count = []
        for y in range(1, len(horizontal_line) - 1):
            roi = roitable[
                horizontal_line[y] + mergin:horizontal_line[y + 1] - mergin,
                vertical_line[x] + mergin:vertical_line[x + 1] - mergin,
            ]
            if (np.sum(roi) / (roi.shape[0] * roi.shape[1]) < 0.98):
                class_count.append(y)
        if len(class_count) == 0:
            output_data.append(f"5/{x+6} 0")
        else:
            output_data.append(f"5/{x+6} " + " ".join(map(str, class_count)))

    with open(output_file, "w") as file:
        file.write(f"Name: {student_name}\n\n")
        file.write("Available Slots:\n")
        file.write("\n".join(output_data))
        file.write("\n\n")
        file.write("class Count:\n")
        file.write("\n".join(output_data))

    line_image = cv2.cvtColor((roitable * 255).astype(np.uint8), cv2.COLOR_GRAY2BGR)
    for line in horizontal_lines:
        x1, y1, x2, y2 = line
        cv2.line(line_image, (x1, y1), (x2, y2), (0, 0, 255), 1)
    for line in vertical_lines:
        x1, y1, x2, y2 = line
        cv2.line(line_image, (x1, y1), (x2, y2), (255, 0, 0), 1)
    cv2.imwrite(line_image_output, line_image)

def main(load_all=False):
    input_dir = "RegistrationForm"
    output_dir = "student_tmp"
    image_output_dir = "GridCheck"

    os.makedirs(output_dir, exist_ok=True)
    os.makedirs(image_output_dir, exist_ok=True)

    if not load_all:
        with open("config.txt", "r") as config:
            students = [line.strip() for line in config if line.strip()]
    else:
        students = [os.path.splitext(f)[0] for f in os.listdir(input_dir) if f.endswith(".pdf")]

    for student in students:
        pdf_path = os.path.join(input_dir, f"{student}.pdf")
        if os.path.exists(pdf_path):
            process_pdf(pdf_path, output_dir, image_output_dir)

if __name__ == "__main__":
    load_all_flag = True
    main(load_all=load_all_flag)
