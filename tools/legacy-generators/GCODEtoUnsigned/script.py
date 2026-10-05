import re

# Constants for scaling and laser commands
X_MAX = 4096
Y_MAX = 4096

def parse_gcode(file_path, scale_factor=5.0):
    coordinates = []
    laser_on = False
    scale_to_dac = X_MAX / scale_factor  # Use provided scale factor to map 0.000-<scale_factor> to 0-4096

    with open(file_path, 'r') as file:
        for line in file:
            # Remove comments and split into parts
            line = line.strip()
            parts = re.split(r'\s+', line)

            # Parse commands
            if 'S1000' in parts:
                laser_on = True
            elif 'S0' in parts:
                laser_on = False
            elif any(part.startswith('G') for part in parts):
                x_val, y_val = None, None

                for part in parts:
                    if part.startswith('X'):
                        x_val = float(part[1:])  # Convert X to float
                    if part.startswith('Y'):
                        y_val = float(part[1:])  # Convert Y to float

                # Process X and Y values if they are available
                if x_val is not None and y_val is not None:
                    # Scale to 0-4096 (16-bit range) based on scale factor
                    x_scaled = int(x_val * scale_to_dac)
                    y_scaled = int(y_val * scale_to_dac)

                    # Ensure we stay in the range 0-4096
                    x_scaled = max(0, min(X_MAX - 1, x_scaled))
                    y_scaled = max(0, min(Y_MAX - 1, y_scaled))

                    # Set the 0x8000 bit if laser is on
                    if laser_on:
                        x_scaled |= 0x8000

                    coordinates.append((x_scaled, y_scaled))

    return coordinates

def convert_to_unsigned_short(coordinates):
    unsigned_shorts = []
    for x, y in coordinates:
        unsigned_shorts.append(f"0x{x:04X}, 0x{y:04X}")
    return unsigned_shorts

def save_to_file(unsigned_shorts, output_path):
    with open(output_path, 'w') as file:
        file.write('const unsigned short object[] PROGMEM = {\n')
        file.write(',\n'.join(unsigned_shorts))
        file.write('\n};\n')

# Example usage:
gcode_file = 'photoshop.txt'
output_file = 'output_object.h'
scale_factor = 10.0  # Adjust as needed

coordinates = parse_gcode(gcode_file, scale_factor=scale_factor)

unsigned_shorts = convert_to_unsigned_short(coordinates)

save_to_file(unsigned_shorts, output_file)

print(f"Conversion completed and saved to {output_file}")
