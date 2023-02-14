    # ----------------- bytecode header --------------
    # dsb_header_size = 8
    # pgm_start = dsb_header_size
    # dsb_header = bytearray()
    # dsb_header += pgm_start.to_bytes(2, endianness)
    # dsb_header += var_bin_start.to_bytes(2, endianness)
    # dsb_header += str_bin_start.to_bytes(2, endianness)
    # dsb_header += bytearray(2)
    # output_bin_array = dsb_header + output_bin_array
    # ------------------------------------------------

    # print("label_to_addr_dict:", label_to_addr_dict)  
    # print("var_addr_dict:", var_addr_dict)
    # print('var_lookup:', var_lookup)
    # print("var_bin_start:", var_bin_start)
    # print("str_bin_start:", str_bin_start)
    # print("str_list:", str_list)

    # print('assembly line count:', len(assembly_listing))
    # print("label_dict:", label_dict)
    # print("func_lookup:", func_lookup)
    # print("str_lookup:", str_lookup)
    # print("var_lookup:", var_lookup)

    # print("--------- Bytecode header ---------")
    # for index, number in enumerate(output_bin_array[:8]):
    #   print("0x{:02x}".format(number), end=' ')

def write_var(var_name, var_value):
    ins_list = []
    this_instruction = get_empty_instruction()
    this_instruction['opcode'] = OP_PUSHC
    this_instruction['oparg'] = var_value
    ins_list.append(this_instruction)
    this_instruction = get_empty_instruction()
    this_instruction['opcode'] = OP_POP
    this_instruction['oparg'] = var_name
    ins_list.append(this_instruction)
    return ins_list

def get_int_args(pgm_line):
    return [int(x) for x in pgm_line.split(" ")[1:] if len(x) > 0]

def backup_button_click():
    if config_dict['auto_backup_enabled']:
        messagebox.showinfo("Backups", "Auto backup is ON!\n\nAll your backups are here!")
        if 'darwin' in sys.platform:
            subprocess.Popen(["open", backup_path])
        elif 'linux' in sys.platform:
            subprocess.Popen(["xdg-open", backup_path])
        else:
            webbrowser.open(backup_path)
    else:
        messagebox.showinfo("Backups", "Auto backup is OFF!\n\nSelect a folder to save a backup.")
        dir_result = filedialog.askdirectory(initialdir=os.path.join(os.path.expanduser('~'), "Desktop"))
        if len(dir_result) <= 0:
            return
        save_everything(os.path.join(dir_result, make_default_backup_dir_name()))
DS3 was released by [Hak5](https://docs.hak5.org/hak5-usb-rubber-ducky/) in late 2022 with major improvements such as:

* 
    # search longest name first
    # define_list = sorted(list(define_dict.keys()), key=len, reverse=True)
    # for key in define_list:
    #   if key in rvalue_str:
    #       rvalue_str = rvalue_str.replace(key, str(define_dict[key])).strip()

### Bytecode Compiler

With DS3 now very much a general-purpose language, it takes a lot more work to process it properly.

Examples include:

* Expression evaluation via abstract syntax tree (AST)

* Flow control with jump tables

* Arithmetic and call stacks

This is much too complicated to do on-device, therefore, DS3 is now compiled into **bytecode**, and executed on a **virtual stack machine**.

The configurator takes care of everything, so it's business as usual.

However, if you want to manually edit the SD card, you'll need to compile the script and copy over the binary too.


DEFAULTCHARDELAY 25
DEFAULTDELAY 25

// WINDOWS R
// DELAY 750

// STRINGLN notepad
// DELAY 750

// STRINGLN Welcome to duckyScript 3 beta test!
// ENTER

STRINGLN Declare variables and perform operations!
VAR $spam = 10
$spam = 10 * 4 + 2
STRINGLN    Spam is: $spam
ENTER

//---------------
$spam = 0
$_RANDOM_MIN = 1
$_RANDOM_MAX = 10

STRINGLN Conditional statements and loops!

WHILE $spam < 4
    VAR $this_number = $_RANDOM_INT
    STRING  Random number: $this_number

    IF $this_number >= 5 THEN
        STRINGLN , it is more than 5!
    ELSE
        STRINGLN , it is less than 5!
    END_IF

    $spam = $spam + 1
END_WHILE
ENTER

//----------------
STRINGLN Print to OLED screen too!
STRINGLN Look at the screen!

OLED_CLEAR
OLED_CURSOR 20 0
OLED_PRINT Hello World!
OLED_CURSOR 20 12
OLED_PRINT Press a key!
OLED_UPDATE
DELAY 2000
OLED_RESTORE

BCLR
WHILE 1
    OLED_CLEAR
    OLED_CURSOR 20 0
    OLED_PRINT Hello World!
    OLED_CURSOR 20 12
    OLED_PRINT Press a key!
    OLED_CURSOR 10 52
    OLED_PRINT Press + to exit

    VAR $this_key = $_READKEY

    IF $this_key != 0 THEN
        OLED_CURSOR 10 30
        OLED_PRINT I pressed key $this_key!
    END_IF

    IF $this_key == 17 THEN
        BREAK
    END_IF

    OLED_UPDATE
    DELAY 50
END_WHILE

OLED_RESTORE

// WHILE 1
// OLED_CLEAR
// OLED_CURSOR 64 32
// OLED_PRINT $_READKEY
// OLED_UPDATE
// DELAY 100

// IF $_READKEY == 1 THEN
//  BREAK
// END_IF

// END_WHILE
// OLED_RESTORE

void restore_profile(uint8_t profile_id)
{
  load_profile(profile_id);
  print_legend();
  f_closedir(&dir);
  f_close(&sd_file);
  save_last_profile(profile_id);
  reset_hold_cache();
}
        

        # ret += "".join([chr(x) for x in result[3:]]).strip('\0')
                # no need to dump dsb file since we'll be generating a new one
                # if(fff[0].lower().endswith('.dsb')):
                #     continue
//---------------
STRINGLN Also conditional statements!
IF $spam > 40 THEN
    STRINGLN    It is more than 40!
ELSE IF $spam > 20 THEN
    STRINGLN    It is more than 20!
ELSE
    STRINGLN    Nevermind!
END_IF
ENTER

// WHILE 1
//  OLED_CLEAR
//  OLED_CURSOR 64 32
//  OLED_PRINT $_READKEY
//  OLED_UPDATE
//  DELAY 100

//  IF $_READKEY == 1 THEN
//      BREAK
//  END_IF

// END_WHILE
// REM sjldkf
// OLED_RESTORE



with open(dps_path, 'w+', encoding='utf8', newline='') as setting_file:
            for line in dp_settings.list_of_lines:
                setting_file.write(line.replace('\r', '').replace('\n', '')+'\n');
was released by [Hak5](https://docs.hak5.org/hak5-usb-rubber-ducky/)
    # if hash1 != hash2:
    #   print(f"{file1} and {file2} are different!")
    #   print(hash1, hash2)
    #   print(open(file1,'rb').read())
    #   print(open(file2,'rb').read())
LOOP0:
SWCC 1 200 0 0
STRINGLN first action

LOOP1:
SWCC 1 0 200 0
STRINGLN second action

LOOP2:
SWCC 1 0 0 200
STRINGLN third action




    if result_dict['state_save_needed']:
        this_instruction = get_empty_instruction()
        this_instruction['opcode'] = OP_PUSHC
        this_instruction['oparg'] = 1
        assembly_listing.append(this_instruction)
        this_instruction = get_empty_instruction()
        this_instruction['opcode'] = OP_POP
        this_instruction['oparg'] = "_NEEDS_SPS"
        assembly_listing.append(this_instruction)

$_LOOP_MAX = 3

IF $_KEYPRESS_COUNT % $_LOOP_MAX == 0 THEN
STRINGLN first action
END_IF

IF $_KEYPRESS_COUNT % $_LOOP_MAX == 1 THEN
STRINGLN second action
END_IF

IF $_KEYPRESS_COUNT % $_LOOP_MAX == 2 THEN
STRINGLN third action
END_IF



----------


BCLR
VAR $COND = 1
WHILE $COND
VAR $BUTT = $_READKEY
STRINGLN $BUTT
DELAY 500
IF $BUTT == 16 THEN
$COND = 0
END_IF
END_WHILE


---
VAR $C = 5
WHILE 1
    WHILE 1
    STRINGLN C IS $C
    $C = $C - 1
    BREAK
    IF $C == 2 THEN
    BREAK
    END_IF
    END_WHILE
BREAK
END_WHILE


---------------

VAR $INDEX = 2 * 3
VAR $RED = 33 * 2
VAR $GREEN = 44 / 2
VAR $BLUE = 55 + 4

SWCC $INDEX    8  $GREEN $BLUE

SWCC 1 100 0 0
DELAY 500
SWCC 1 0 100 0
DELAY 500
SWCC 1 0 0 100
DELAY 500

SWCA

VAR $C = 255

WHILE $C > 5
SWCC 1 $C $C $C
DELAY 10
$C = $C - 2
END_WHILE 


SWCC 1 200 0 0
SWCC 2 200 0 0
SWCC 3 200 0 0
SWCC 4 200 0 0
SWCC 5 200 0 0
SWCC 6 200 0 0
SWCC 7 200 0 0
SWCC 8 200 0 0
SWCC 9 200 0 0
SWCC 10 200 0 0
SWCC 11 200 0 0
SWCC 12 200 0 0
SWCC 13 200 0 0
SWCC 14 200 0 0
SWCC 15 200 0 0
DELAY 500

SWCC 1 0 200 0
SWCC 2 0 200 0
SWCC 3 0 200 0
SWCC 4 0 200 0
SWCC 5 0 200 0
SWCC 6 0 200 0
SWCC 7 0 200 0
SWCC 8 0 200 0
SWCC 9 0 200 0
SWCC 10 0 200 0
SWCC 11 0 200 0
SWCC 12 0 200 0
SWCC 13 0 200 0
SWCC 14 0 200 0
SWCC 15 0 200 0
DELAY 500

SWCC 1 0 0 200
SWCC 2 0 0 200
SWCC 3 0 0 200
SWCC 4 0 0 200
SWCC 5 0 0 200
SWCC 6 0 0 200
SWCC 7 0 0 200
SWCC 8 0 0 200
SWCC 9 0 0 200
SWCC 10 0 0 200
SWCC 11 0 0 200
SWCC 12 0 0 200
SWCC 13 0 0 200
SWCC 14 0 0 200
SWCC 15 0 0 200
DELAY 500


VAR $c = 63

SWCR   $c


---

VAR $YY = 20

OLC 0 $YY
OLP           
OLC 0 $YY
OLP $_RANDOM_INT
OLU
-------
WHILE TRUE
STRINGLN $_READKEY
DELAY 500
END_WHILE



def make_swcolor_instruction(pgm_line):
    split = [x for x in pgm_line.split(' ') if len(x) > 0]
    cmd = split[0].strip()
    sw = 0
    if cmd.startswith("SWCOLOR_"):
        sw = int(cmd.split('_')[1])
    rrrr = int(split[1])
    gggg = int(split[2])
    bbbb = int(split[3])

    ins_list = []
    this_instruction = get_empty_instruction()
    this_instruction['opcode'] = OP_SWCR
    this_instruction['oparg'] = get_combined_value(sw, rrrr)
    ins_list.append(this_instruction)

    this_instruction = get_empty_instruction()
    this_instruction['opcode'] = OP_SWCG
    this_instruction['oparg'] = get_combined_value(sw, gggg)
    ins_list.append(this_instruction)

    this_instruction = get_empty_instruction()
    this_instruction['opcode'] = OP_SWCB
    this_instruction['oparg'] = get_combined_value(sw, bbbb)
    ins_list.append(this_instruction)

    this_instruction = get_empty_instruction()
    this_instruction['opcode'] = OP_SWCE
    ins_list.append(this_instruction)

    return ins_list

def get_combined_value(b0, b1):
    return ((b0 % 0xff) << 8) | (b1 % 0xff)

  else if(this_opcode == OP_PUSHV)
  {
    
    uint16_t this_value;
    if(op_data == DEFAULTDELAY_ADDR)
      this_value = defaultdelay_value;
    else if (op_data == DEFAULTCHARDELAY_ADDR)
      this_value = defaultchardelay_value;
    else if (op_data == CHARJITTER_ADDR)
      this_value = charjitter_value;
    else
      this_value= make_uint16(pgm_start[op_data], pgm_start[op_data+1]);
    op_result = stack_push(&arithmetic_stack, this_value);
    if(op_result != STACK_OP_OK)
    {
      exe->result = op_result;
      return;
    }
  }

def get_partial_varname_addr(msg, vad):
    for x in range(len(msg)+1):
        partial_name = msg[:x]
        if partial_name in vad:
            return partial_name, vad[partial_name]
    return None, None

def replace_var_in_str(msg, vad):
    search = msg.split('$')
    if len(search) == 1:
        return msg
    print('before', msg)
    for item in [x.strip() for x in search if len(x.strip()) != 0]:
        var_name, var_addr = get_partial_varname_addr(item, vad)
        if var_addr is not None:
            # print(f"!!!!!!!!! {item} contains a variable")
            twobyte = var_addr.to_bytes(2, endianness)
            # print(f"${var_name}", f"|{var_addr}|", chr(twobyte[0]), chr(twobyte[1]))
            msg = msg.replace(f"${var_name}", f"{var_boundary_chr}{chr(twobyte[0])}{chr(twobyte[1])}{var_boundary_chr}")
    print('after', msg)
    return msg

void led_animation_handler(void)
{
  frame_counter++;
  for (int idx = 0; idx < NEOPIXEL_COUNT; ++idx)
  {
    int32_t current_frame = frame_counter - neo_anime[idx].animation_start;
    if(current_frame <= 0)
      continue;
    if(neo_anime[idx].animation_type == ANIMATION_NO_ANIMATION)
      continue;
    else if(neo_anime[idx].animation_type == ANIMATION_FULLY_ON)
    {
      for (int i = 0; i < THREE; ++i)
        neo_anime[idx].current_color[i] = neo_anime[idx].target_color[i];
      set_pixel_color(neo_anime[idx].index, neo_anime[idx].target_color[0], neo_anime[idx].target_color[1], neo_anime[idx].target_color[2]);
    }
    else if(neo_anime[idx].animation_type == ANIMATION_CROSS_FADE)
    {
      if(current_frame <= neo_anime[idx].animation_duration)
      {
        for (int i = 0; i < THREE; ++i)
        {
          neo_anime[idx].current_color[i] += neo_anime[idx].step[i];
          if(neo_anime[idx].current_color[i] > 255)
            neo_anime[idx].current_color[i] = 255;
          if(neo_anime[idx].current_color[i] < 0)
            neo_anime[idx].current_color[i] = 0;
        }
      }
      else
      {
         for (int i = 0; i < THREE; ++i)
          neo_anime[idx].current_color[i] = neo_anime[idx].target_color[i];
      }
      set_pixel_color(neo_anime[idx].index, neo_anime[idx].current_color[0], neo_anime[idx].current_color[1], neo_anime[idx].current_color[2]);
    }  
  }
  neopixel_show(red_buf, green_buf, blue_buf);
}


else if(this_opcode == OP_PUSHV)
  {
    uint16_t this_value = make_uint16(pgm_start[op_data], pgm_start[op_data+1]);
    op_result = stack_push(&arithmetic_stack, this_value);
    if(op_result != STACK_OP_OK)
    {
      exe->result = op_result;
      return;
    }
  }

script_exe_warning_showed = False

def run_script():
    global script_exe_warning_showed
    if len(script_textbox.get("1.0",END)) <= 2:
        return
    warning_msg = "You are about to execute this script.\n> MAKE SURE YOU TRUST IT!\n> Result might differ from real duckyPad.\n\nExecution will start after a 2-second delay."
    if 'darwin' in sys.platform:
        warning_msg = "You are about to execute this script.\n> MAKE SURE YOU TRUST IT!\n> Result might differ from real duckyPad.\n> If nothing happens, give this app Accessibility permission.\n> Check Getting Started Guide for details. \n\nExecution will start after a 2-second delay."
    if(script_exe_warning_showed is False):
        if(messagebox.askokcancel("Warning", warning_msg) == False):
            return
    # try:
    #     import autogui
    #     time.sleep(2)
    #     exe_result, exe_note = autogui.execute_file(script_textbox.get("1.0",END).replace('\r', '').split('\n'))
    #     if exe_result != 0:
    #         messagebox.showerror("Error", exe_note)
    # except Exception as e:
    #     messagebox.showerror("Error", "execution failed:\n\n" + str(e))
    script_exe_warning_showed = True

execute_button = Button(scripts_lf, text="Run script", command=run_script, state=DISABLED)
execute_button.place(x=scaled_size(160), y=scaled_size(417), width=scaled_size(80), height=BUTTON_HEIGHT)
root.update()
    execute_button.config(state=NORMAL, fg="red")


pc_app_update_label = Label(master=updates_lf)
pc_app_update_label.place(x=scaled_size(5), y=scaled_size(0))

--------------------

    # third pass, replace lines that are too long

    print("---------Second pass OK!---------\n")

    third_pass_program_listing = []
    MAX_LINE_LEN = 245
    for item in second_pass_program_listing:
        orig_line_number_starting_from_1 = item[0]
        code_content = item[1]
        first_word = code_content.split(" ")[0]

        if len(code_content) > MAX_LINE_LEN:
            if first_word.startswith(cmd_STRING) is False:
                rdict['is_success'] = False
                rdict['comments'] = "Line too long, unable to auto-split"
                rdict['error_line_number_starting_from_1'] = orig_line_number_starting_from_1
                rdict['error_line_str'] = code_content
                return rdict
            chunk_size = MAX_LINE_LEN
            chunks = [code_content[i:i+chunk_size] for i in range(0, len(code_content), chunk_size)]
            for item in chunks:
                # print(f"{orig_line_number_starting_from_1} {first_word} {item}")
                third_pass_program_listing.append((orig_line_number_starting_from_1, f"{first_word} {item}"))
        else:
            # print(f"{orig_line_number_starting_from_1} {code_content}")
            third_pass_program_listing.append((orig_line_number_starting_from_1, code_content))

    final_dict = run_once([x[1] for x in third_pass_program_listing])
    final_dict["compact_listing"] = third_pass_program_listing

    if_info_list = []
    for item in if_raw_info:    
        if_root = list(item.keys())[0]
        if_info = {
        'root':if_root,
        "else_if":item[if_root]['else_if'],
        "else":item[if_root]['else'],
        "end_if":item[if_root]['end_if'],
        }
        if_info_list.append(if_info)
    final_dict["if_info"] = if_info_list
    print("---------Preprocessing Done!---------\n")
    return final_dict