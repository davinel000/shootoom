import sys
import subprocess

# Функция для установки пакета
def install_package(package_name):
    subprocess.check_call([sys.executable, "-m", "pip", "install", package_name])

# Проверка и установка необходимых библиотек
try:
    from svgpathtools import svg2paths2
except ImportError:
    print("Необходимые библиотеки не установлены. Устанавливаем...")
    install_package("svgpathtools")
    from svgpathtools import svg2paths2

# Основной код
def svg_to_unsigned_short(svg_file):
    # Чтение путей из SVG файла
    paths, attributes, svg_attributes = svg2paths2(svg_file)

    # Инициализация массива
    unsigned_short_array = []

    # Обработка каждого пути
    for path in paths:
        for segment in path:
            # Получаем координаты начала и конца сегмента
            start = segment.start
            end = segment.end

            # Масштабируем координаты до диапазона (например, от 0 до 4096)
            x1, y1 = int(start.real * 4096), int(start.imag * 4096)
            x2, y2 = int(end.real * 4096), int(end.imag * 4096)

            # Записываем координаты в формате unsigned short
            unsigned_short_array.append((0x8000 | x1, y1))  # Включаем лазер
            unsigned_short_array.append((x2, y2))  # Выключаем лазер

    return unsigned_short_array

def save_to_file(unsigned_short_array, filename):
    # Сохраняем массив в файл
    with open(filename, 'w') as f:
        for point in unsigned_short_array:
            f.write(f"0x{point[0]:04X}, 0x{point[1]:04X},\n")
    print(f"Данные успешно сохранены в {filename}")

# Пример использования
unsigned_short_array = svg_to_unsigned_short('Akkadian_syllabary.svg')

# Сохраняем в файл
save_to_file(unsigned_short_array, 'output.txt')