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