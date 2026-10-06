# Пример отправки команды для отрисовки текста с новыми параметрами scale и speed
debug = False
record_mode = op('button10/out1')[0]

import time  # For timing

def record_command(message):
    """
    Records the command along with the current absolute frame number in a format suitable for a PROGMEM header.
    Each recorded line will have the following format:
    
    "12345,\"ff0204887406e5fe\"\n"
    
    where 12345 is the current frame number.
    
    :param message: A bytes object representing the command.
    """
    # Get the current frame number. 
    # Ensure that 'frame' is available; otherwise, replace with your method of obtaining the current frame.
    frameNumber = absTime.frame
    
    # Convert the message to a hex string.
    hex_str = message.hex()
    
    # Format the line as: "frameNumber,\"hex_command\"\n"
    formatted_line = '"' + str(frameNumber) + ',\\"' + hex_str + '\\"\\n"'
    
    # Append the formatted line to a DAT table called 'record_table'
    dt = op('record_table')
    if dt is not None:
        dt.appendRow([formatted_line])
    else:
        print("Recorded line:", formatted_line)


def send_draw_text_command(text='TEST', scale=1, rotation=0, position_x=2048, position_y=2048, count=3, speed=10):
    start_byte = b'\xFF'
    command_id = b'\x01'
    
    count = int(count)
    rotation = int(rotation)
    position_x = int(position_x)
    position_y = int(position_y)
    
    # Преобразуем scale в диапазон от 5 до 200
    #scale = max(0.05, min(scale, 2.0))  # Ограничиваем значения от 0.05 до 2.0
    scale_byte = int((scale - 0.05) / (2.0 - 0.05) * 195 + 5).to_bytes(1, 'big')
    
    speed = int(speed)
    print('speed', speed)
    
    # Преобразуем текст в байты
    text_bytes = text.encode('ascii')
    text_length = len(text_bytes).to_bytes(1, 'big')  # Длина текста
    
    # Преобразуем параметры
    count_byte = count.to_bytes(1, 'big')
    rotation_byte = rotation.to_bytes(1, 'big')
    position_x_byte = position_x.to_bytes(2, 'big')  # Два байта
    position_y_byte = position_y.to_bytes(2, 'big')  # Два байта
    speed_byte = speed.to_bytes(1, 'big')  # Новый байт для скорости
    
    # Рассчитываем длину всех данных (включая текст)
    total_data_length = (1 + len(text_bytes) + 1 + 1 + 2 + 2 + 1 + 1).to_bytes(1, 'big')

    # Собираем сообщение
    message = start_byte + command_id + total_data_length + text_length + text_bytes + count_byte + rotation_byte + position_x_byte + position_y_byte + scale_byte + speed_byte + b'\xFE'
    
    op('serial1').sendBytes(message)
    if debug: print(f"Отправлено сообщение: {message}")
    
    # Record the command if record_mode is enabled
    if record_mode: record_command(message)



def send_direct_laser_control_command(x_position, y_position, laser_on):
    """
    Отправляет команду для управления лазером.
    :param x_position: Позиция X (0-4095)
    :param y_position: Позиция Y (0-4095)
    :param laser_on: Включен ли лазер (True/False)
    """
    
    # Стартовый байт команды
    start_byte = b'\xFF'
    
    # Команда для управления лазером
    command_id = b'\x02'

    x_position = int(x_position)
    y_position = int(y_position)

    # Преобразуем параметры в байты
    laser_status = (0x8000 if laser_on else 0x0000)  # Устанавливаем старший бит для включения лазера
    x_position_byte = ((x_position & 0x7FFF) | laser_status).to_bytes(2, 'big')  # X + лазерный статус
    y_position_byte = y_position.to_bytes(2, 'big')  # Y как есть

    # Рассчитываем длину всех данных (4 байта для X и Y)
    total_data_length = (4).to_bytes(1, 'big')  # Всего 4 байта данных

    # Завершающий байт
    end_byte = b'\xFE'

    # Собираем весь пакет
    message = start_byte + command_id + total_data_length + x_position_byte + y_position_byte + end_byte

    # Отправка через Serial
    if debug: print(f"Отправлено сообщение: {message}")
    
    # Record the command if record_mode is enabled
    if record_mode: record_command(message)
    
    op('serial1').sendBytes(message)

# Пример отправки команды для отображения сетки
def send_draw_grid_command(count=5, scale=1.0):
    start_byte = b'\xFF'
    command_id = b'\x03'  # Команда номер 3 для сетки
    
    count = int(count)
    
    # Преобразуем scale в диапазон от 5 до 200
    scale = max(0.05, min(scale, 2.0))  # Ограничиваем значения от 0.05 до 2.0
    scale_byte = int((scale - 0.05) / (2.0 - 0.05) * 195 + 5).to_bytes(1, 'big')

    # Преобразуем параметры
    count_byte = count.to_bytes(1, 'big')  # Байт для количества циклов
    
    # Рассчитываем длину всех данных (в данном случае 2 байта)
    total_data_length = (1 + 1).to_bytes(1, 'big')

    # Собираем сообщение
    message = start_byte + command_id + total_data_length + count_byte + scale_byte + b'\xFE'
    
    op('serial1').sendBytes(message)
    if debug: print(f"Отправлено сообщение: {message}")
    
    # Record the command if record_mode is enabled
    if record_mode: record_command(message)
    
    
def send_laser_off_command():
    """
    Отправляет команду для выключения лазера.
    """
    start_byte = b'\xFF'
    command_id = b'\x04'
    data_length = b'\x00'  # Длина данных (0 байт)
    end_byte = b'\xFE'

    # Собираем весь пакет
    message = start_byte + command_id + data_length + end_byte

    # Отправка через Serial
    op('serial1').sendBytes(message)
    if debug: print(f"Отправлено сообщение для отключения лазера: {message}")
    
    # Record the command if record_mode is enabled
    if record_mode: record_command(message)

# Пример отправки команды на выполнение функции
def send_general_command(command_id):
    """
    Отправляет команду для выполнения простой функции.
    :param command_id: Идентификатор команды (байт)
    """
    
    # Стартовый байт команды
    start_byte = b'\xFF'
    
    # Завершающий байт
    end_byte = b'\xFE'

    # Собираем весь пакет команды
    message = start_byte + bytes([command_id]) + end_byte

    # Отправка через Serial
    op('serial1').sendBytes(message)
    
    # Record the command if record_mode is enabled
    if record_mode: record_command(message)