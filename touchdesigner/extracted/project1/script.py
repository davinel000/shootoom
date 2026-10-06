import math

def generate_choreo_element():
    """
    Generates a choreographic element from the 'points' and 'primitives' DATs.
    For each primitive, it interpolates points between key vertices using a fixed step length.
    Each generated command is formatted as:
    
      <frame>,\"<hex_command>\"\n
      
    where <hex_command> is a complete command packet constructed as follows:
      - Start byte: "FF"
      - Command ID: "02"
      - Data length: "04"
      - 4 bytes of payload (2 bytes for X and 2 bytes for Y), with the laser ON flag (0x8000) set in X when the laser should be on.
      - End byte: "FE"
      
    The final output is a text block formatted for PROGMEM in Arduino:
    
      const char choreographyData[] PROGMEM =
      "81056,\"FF0204857708F5FE\"\n"
      "81057,\"FF0204857708F5FE\"\n"
      ...
      ;
      #endif
      
    This output is written into the DAT named 'result'.
    """
    # Get the DATs
    points = op('points')         # Table DAT with point coordinates (columns 'P(0)' and 'P(1)')
    primitives = op('primitives') # Table DAT with a column 'vertices' (space-separated vertex indices)
    
    # Parameters
    scale_factor = 4095         # Scale factor to convert normalized coordinates to 0-4095 range
    v = 10                      # Desired travel distance (in coordinate units) per frame
    laser_on_bit = 0x8000       # Bit flag to indicate that the laser is ON
    
    # Initialize frame counter (starting frame)
    current_frame = 0
    
    lines = []  # List to collect each line (command)

    # Helper function to format a 4-byte payload into a complete command packet.
    def format_command(payload):
        # payload should be an 8-character hex string (4 bytes)
        # Prepend "FF0204" and append "FE"
        return "FF0204" + payload.upper() + "FE"
    
    # Process each primitive (assuming first row is header, so start from row 1)
    for r in range(1, primitives.numRows):
        prim_line = primitives[r, "vertices"].val
        if not prim_line:
            continue
        vert_indices = prim_line.split()
        if len(vert_indices) < 2:
            continue
        
        # Get the first vertex of the primitive
        first_index = int(vert_indices[0]) + 1  # +1 because row 0 is header in points DAT
        x0 = float(points[first_index, "P(0)"])
        y0 = float(points[first_index, "P(1)"])
        # Scale coordinates
        x0_scaled = int(x0 * scale_factor)
        y0_scaled = int(y0 * scale_factor)
        
        # Generate two commands for the first vertex:
        # 1. Move to starting point with laser OFF (no flag)
        cmd_off = f"{x0_scaled:04X}{y0_scaled:04X}"
        full_cmd_off = format_command(cmd_off)
        line_str = f'{current_frame},\\"{full_cmd_off}\\"\\n'
        lines.append(line_str)
        # 2. Then start drawing: same coordinates but with laser ON (flag set in X)
        cmd_on = f"{(laser_on_bit | x0_scaled):04X}{y0_scaled:04X}"
        full_cmd_on = format_command(cmd_on)
        line_str = f'{current_frame},\\"{full_cmd_on}\\"\\n'
        lines.append(line_str)
        
        prev_x = x0_scaled
        prev_y = y0_scaled
        
        # Process subsequent vertices of the primitive
        for idx in range(1, len(vert_indices)):
            v_index = int(vert_indices[idx]) + 1
            x = float(points[v_index, "P(0)"])
            y = float(points[v_index, "P(1)"])
            x_scaled = int(x * scale_factor)
            y_scaled = int(y * scale_factor)
            
            dx = x_scaled - prev_x
            dy = y_scaled - prev_y
            d = math.sqrt(dx*dx + dy*dy)
            steps = max(1, int(math.ceil(d / v)))
            
            for i in range(1, steps+1):
                t = i / steps
                interp_x = int(prev_x + t * dx)
                interp_y = int(prev_y + t * dy)
                # For intermediate points, laser stays ON
                interp_x_flag = laser_on_bit | (interp_x & 0x7FFF)
                hex_cmd = f"{interp_x_flag:04X}{interp_y:04X}"
                full_cmd_interp = format_command(hex_cmd)
                current_frame += 1
                line_str = f'{current_frame},\\"{full_cmd_interp}\\"\\n'
                lines.append(line_str)
            
            prev_x = x_scaled
            prev_y = y_scaled
        
        # At end of primitive, add a command to turn the laser OFF at the last vertex.
        hex_cmd_off = f"{prev_x:04X}{prev_y:04X}"  # Without laser on flag
        full_cmd_end = format_command(hex_cmd_off)
        current_frame += 1
        line_str = f'{current_frame},\\"{full_cmd_end}\\"\\n'
        lines.append(line_str)
        
        # Optionally, add a pause of several frames between primitives
        current_frame += 5  # Pause: 5 frames
        
    # Build final string with header and footer
    header = "const char choreographyData[] PROGMEM =\n"
    # Each line is enclosed in double quotes so the compiler concatenates them
    body = "".join([f'"{line}"\n' for line in lines])
    footer = ";\n#endif"
    final_str = header + body + footer
    
    # Write result to the DAT named 'result'
    op('result').text = final_str
    print("Choreographic element generated and written to 'result' DAT.")

# Run the function
generate_choreo_element()
