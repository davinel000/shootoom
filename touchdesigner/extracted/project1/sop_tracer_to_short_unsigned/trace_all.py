def generate_laser_code():
    # Получаем заголовок
    header = op('header').text
    namespattern = op('names').text+'*'
    
    # Результирующий текстовый блок с лазерным кодом
    full_laser_code = [header]
    
    

    # Переменная для хранения общего числа точек
    total_points_count = 0

    # Ищем все операторы с именем 'character'
    replicants = parent().findChildren(name=namespattern)  # Найдём всех операторов, чьи имена начинаются с 'character'
    
    # Проходим по каждому найденному репликанту
    for replicant in replicants:
        # Получаем операторы points, vertices и primitives внутри репликанта
        points_dat = replicant.op('points')
        vertices_dat = replicant.op('vertices')
        primitives_dat = replicant.op('primitives')

        # Название символа для текущего репликанта
        name = replicant.op('name').text  # Предположим, что у тебя есть параметр с именем для каждого репликанта
        
        if op('law_name'):
            law_name = op('law_name').text+"_"
        else:
            law_name = ""
        

        # Подсчет точек для текущего символа
        symbol_points_count = points_dat.numRows - 1  # Минус одна строка заголовка

        # Вывод количества точек для текущего символа
        print(f"Символ: {name}, количество точек: {symbol_points_count}")

        # Добавляем количество точек текущего символа к общему числу
        total_points_count += symbol_points_count

        # Коэффициент для масштабирования координат (0-1 -> 0-4095)
        scale_factor = 4095
        laser_on_bit = 0x8000  # Бит для включения лазера

        # Лазерный код для текущего символа
        laser_code = []

        # Проходим по примитивам
        for prim_index in range(1, primitives_dat.numRows):
            prim_vertices = primitives_dat[prim_index, 'vertices'].val.split(' ')  # Получаем список вершин примитива
            first_vertex_index = int(prim_vertices[0]) + 1  # Индекс первой вершины

            # Координаты первой вершины
            first_x = float(points_dat[first_vertex_index, 'P(0)'])
            first_y = float(points_dat[first_vertex_index, 'P(1)'])

            # Проверка на отрицательные координаты
            if first_x < 0 or first_y < 0:
                error_message = f"Ошибка: отрицательные координаты для символа {name}. Скорректируйте положение символов."
                op('string_to_send').text = error_message
                print(error_message)
                return

            # Масштабируем координаты первой вершины
            first_x_scaled = int(first_x * scale_factor)
            first_y_scaled = int(first_y * scale_factor)

            # Ставим лазер в начальную точку примитива (без включения лазера)
            laser_code.append(f"0x{first_x_scaled:04X}, 0x{first_y_scaled:04X}")

            # Теперь включаем лазер в этой же точке для начала рисования
            laser_code.append(f"0x{laser_on_bit | first_x_scaled:04X}, 0x{first_y_scaled:04X}")

            # Проходим по остальным вершинам примитива
            for vert in prim_vertices[1:]:
                vert_index = int(vert) + 1  # Индекс вершины
                x = float(points_dat[vert_index, 'P(0)'])  # Координата X
                y = float(points_dat[vert_index, 'P(1)'])  # Координата Y

                # Проверка на отрицательные координаты
                if x < 0 or y < 0:
                    error_message = f"Ошибка: отрицательные координаты для символа {name}. Скорректируйте положение символов."
                    op('string_to_send').text = error_message
                    print(error_message)
                    return

                # Масштабируем координаты в диапазон 0-4095
                x_scaled = int(x * scale_factor)
                y_scaled = int(y * scale_factor)

                # Включаем лазер для всех остальных вершин
                laser_code.append(f"0x{laser_on_bit | x_scaled:04X}, 0x{y_scaled:04X}")

            # Проверка на корректное замыкание примитива
            if len(prim_vertices) > 2:
                last_vertex_index = int(prim_vertices[-1]) + 1
                last_x = float(points_dat[last_vertex_index, 'P(0)'])
                last_y = float(points_dat[last_vertex_index, 'P(1)'])
                last_x_scaled = int(last_x * scale_factor)
                last_y_scaled = int(last_y * scale_factor)
                
                if last_x_scaled != first_x_scaled or last_y_scaled != first_y_scaled:
                    # Замыкаем примитив только если последняя точка не совпадает с первой
                    laser_code.append(f"0x{laser_on_bit | first_x_scaled:04X}, 0x{first_y_scaled:04X}")

            # Отключаем лазер в последней точке
            laser_code.append(f"0x{first_x_scaled:04X}, 0x{first_y_scaled:04X}")

        # Преобразуем результат для текущего символа в текст
        result = ',\n'.join(laser_code)
        symbol_code = f"const unsigned short draw_{law_name}{name}[] PROGMEM = {{\n{result}\n}};"

        # Добавляем код для символа в общий блок
        full_laser_code.append(symbol_code)

    # Вывод общего числа точек для всех символов
    print(f"Общее количество точек для всех символов: {total_points_count}")

    # Объединяем весь код в одну строку и добавляем #endif в конце
    final_code = '\n\n'.join(full_laser_code) + "\n#endif"
    op('string_to_send').text = final_code

    print("Код успешно сгенерирован для всех символов, добавлен #endif и записан в string_to_send.text")

# Запускаем функцию
generate_laser_code()
