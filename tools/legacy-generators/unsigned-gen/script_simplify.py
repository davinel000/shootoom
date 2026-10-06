import sys
import subprocess
from svgpathtools import svg2paths2
import math

def install_package(package_name):
    subprocess.check_call([sys.executable, "-m", "pip", "install", package_name])

def distance(x1, y1, x2, y2):
    return math.sqrt((x2 - x1)**2 + (y2 - y1)**2)

def svg_to_unsigned_short(svg_file, threshold=10, step=5):
    # Чтение путей из SVG файла
    paths, attributes, svg_attributes = svg2paths2(svg_file)

    # Инициализация массива
    unsigned_short_array = []

    # Обработка каждого пути
    for path in paths:
        last_x, last_y = None, None
        for i, segment in enumerate(path):
            # Пропускаем точки в зависимости от шага (снижение частоты дискретизации)
            if i % step != 0:
                continue

            # Получаем координаты начала и конца сегмента
            start = segment.start
            end = segment.end

            x1, y1 = int(start.real * 4096), int(start.imag * 4096)
            x2, y2 = int(end.real * 4096), int(end.imag * 4096)

            # Фильтрация точек, чтобы не добавлять близкие друг к другу
            if last_x is None or distance(last_x, last_y, x1, y1) > threshold:
                unsigned_short_array.append((0x8000 | x1, y1))  # Включаем лазер
                unsigned_short_array.append((x2, y2))  # Выключаем лазер
                last_x, last_y = x2, y2

    return unsigned_short_array

def save_to_file(unsigned_short_array, filename):
    with open(filename, 'w') as f:
        for point in unsigned_short_array:
            f.write(f"0x{point[0]:04X},0x{point[1]:04X},\n")
    print(f"Данные успешно сохранены в {filename}")


# Пример использования
unsigned_short_array = svg_to_unsigned_short('File_NIN-beltu_Cuneiform.svg', threshold=10, step=5)

# Сохраняем в файл
save_to_file(unsigned_short_array, 'output.txt')