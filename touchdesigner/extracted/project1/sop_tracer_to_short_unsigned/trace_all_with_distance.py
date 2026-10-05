def generate_laser_code():
    header = op('header').text
    namespattern = op('names').text + '*'
    full_laser_code = [header]

    total_points_count = 0
    replicants = parent().findChildren(name=namespattern)  # Все операторы, имена которых начинаются с заданного шаблона
    
    DIST_THRESHOLD = 25  # пороговое расстояние между точками (в масштабированных единицах)
    
    for replicant in replicants:
        points_dat = replicant.op('points')
        vertices_dat = replicant.op('vertices')
        primitives_dat = replicant.op('primitives')
        name = replicant.op('name').text  # имя символа
        
        symbol_points_count = points_dat.numRows - 1
        print(f"Символ: {name}, количество точек: {symbol_points_count}")
        total_points_count += symbol_points_count

        scale_factor = 4095
        laser_on_bit = 0x8000

        # Массив для хранения команд лазерного кода для текущего символа
        laser_code = []

        # Обработка каждого примитива (например, контура)
        for prim_index in range(1, primitives_dat.numRows):
            prim_vertices = primitives_dat[prim_index, 'vertices'].val.split(' ')
            first_vertex_index = int(prim_vertices[0]) + 1
            first_x = float(points_dat[first_vertex_index, 'P(0)'])
            first_y = float(points_dat[first_vertex_index, 'P(1)'])
            
            if first_x < 0 or first_y < 0:
                error_message = f"Ошибка: отрицательные координаты для символа {name}. Скорректируйте положение символов."
                op('string_to_send').text = error_message
                print(error_message)
                return

            first_x_scaled = int(first_x * scale_factor)
            first_y_scaled = int(first_y * scale_factor)

            # Сначала перемещаем лазер к первой точке без включения
            laser_code.append(f"0x{first_x_scaled:04X}, 0x{first_y_scaled:04X}")
            # Затем включаем лазер (устанавливаем старший бит)
            laser_code.append(f"0x{laser_on_bit | first_x_scaled:04X}, 0x{first_y_scaled:04X}")

            # Запоминаем первую точку как предыдущую
            prev_x = first_x_scaled
            prev_y = first_y_scaled

            # Проходим по остальным вершинам примитива
            for vert in prim_vertices[1:]:
                vert_index = int(vert) + 1
                x = float(points_dat[vert_index, 'P(0)'])
                y = float(points_dat[vert_index, 'P(1)'])
                
                if x < 0 or y < 0:
                    error_message = f"Ошибка: отрицательные координаты для символа {name}. Скорректируйте положение символов."
                    op('string_to_send').text = error_message
                    print(error_message)
                    return

                x_scaled = int(x * scale_factor)
                y_scaled = int(y * scale_factor)

                # Вычисляем расстояние от предыдущей точки
                dx = x_scaled - prev_x
                dy = y_scaled - prev_y
                distance = (dx*dx + dy*dy) ** 0.5

                if distance > DIST_THRESHOLD:
                    # Если расстояние большое, сначала "отключаем" лазер в предыдущей точке
                    laser_code.append(f"0x{prev_x:04X}, 0x{prev_y:04X}")
                    # Затем перемещаемся к новой точке с включенным лазером
                    laser_code.append(f"0x{laser_on_bit | x_scaled:04X}, 0x{y_scaled:04X}")
                else:
                    # Если расстояние маленькое, просто продолжаем рисовать с включенным лазером
                    laser_code.append(f"0x{laser_on_bit | x_scaled:04X}, 0x{y_scaled:04X}")
                    
                # Обновляем предыдущую точку
                prev_x = x_scaled
                prev_y = y_scaled

            # Если у примитива более двух вершин, проверяем, замыкается ли контур:
            if len(prim_vertices) > 2:
                last_vertex_index = int(prim_vertices[-1]) + 1
                last_x = float(points_dat[last_vertex_index, 'P(0)'])
                last_y = float(points_dat[last_vertex_index, 'P(1)'])
                last_x_scaled = int(last_x * scale_factor)
                last_y_scaled = int(last_y * scale_factor)
                
                if last_x_scaled != first_x_scaled or last_y_scaled != first_y_scaled:
                    # Если не замыкается, добавляем первую точку в качестве завершающей
                    laser_code.append(f"0x{laser_on_bit | first_x_scaled:04X}, 0x{first_y_scaled:04X}")
            # В конце примитива отключаем лазер (например, возвращаем в исходную точку без включенного лазера)
            laser_code.append(f"0x{first_x_scaled:04X}, 0x{first_y_scaled:04X}")

        result = ',\n'.join(laser_code)
        symbol_code = f"const unsigned short draw_{name}[] PROGMEM = {{\n{result}\n}};"
        full_laser_code.append(symbol_code)

    print(f"Общее количество точек для всех символов: {total_points_count}")
    final_code = '\n\n'.join(full_laser_code) + "\n#endif"
    op('string_to_send').text = final_code
    print("Код успешно сгенерирован для всех символов, добавлен #endif и записан в string_to_send.text")

# Запускаем функцию генерации кода
generate_laser_code()
