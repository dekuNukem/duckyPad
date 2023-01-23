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