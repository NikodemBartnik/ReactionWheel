import tkinter as tk
from tkinter import ttk
import serial

# Serial port configuration
ser = serial.Serial("/dev/cu.usbserial-1410", 9600, timeout=1)


def send_pid():
    kp = kp_slider.get()
    ki = ki_slider.get()
    kd = kd_slider.get()
    ser.write(f"K {kp} {ki} {kd}\n".encode())


def toggle_motor():
    switch_status = switch_var.get()
    ser.write(f"S {switch_status}\n".encode())


root = tk.Tk()
root.title("PID Controller")

# StringVar variables for displaying slider values
kp_value = tk.StringVar()
ki_value = tk.StringVar()
kd_value = tk.StringVar()

# PID sliders
kp_label = ttk.Label(root, text="Kp:")
kp_label.grid(row=0, column=0, padx=5, pady=5)
kp_slider = ttk.Scale(
    root,
    from_=0,
    to=1,
    length=200,
    orient="horizontal",
    command=lambda value: kp_value.set(f"{float(value):.2f}"),
)
kp_slider.grid(row=0, column=1, padx=5, pady=5)
kp_value_label = ttk.Label(root, textvariable=kp_value)
kp_value_label.grid(row=0, column=2, padx=5, pady=5)

ki_label = ttk.Label(root, text="Ki:")
ki_label.grid(row=1, column=0, padx=5, pady=5)
ki_slider = ttk.Scale(
    root,
    from_=0,
    to=1,
    length=200,
    orient="horizontal",
    command=lambda value: ki_value.set(f"{float(value):.2f}"),
)
ki_slider.grid(row=1, column=1, padx=5, pady=5)
ki_value_label = ttk.Label(root, textvariable=ki_value)
ki_value_label.grid(row=1, column=2, padx=5, pady=5)

kd_label = ttk.Label(root, text="Kd:")
kd_label.grid(row=2, column=0, padx=5, pady=5)
kd_slider = ttk.Scale(
    root,
    from_=0,
    to=1,
    length=200,
    orient="horizontal",
    command=lambda value: kd_value.set(f"{float(value):.2f}"),
)
kd_slider.grid(row=2, column=1, padx=5, pady=5)
kd_value_label = ttk.Label(root, textvariable=kd_value)
kd_value_label.grid(row=2, column=2, padx=5, pady=5)

pid_button = ttk.Button(root, text="Send PID", command=send_pid)
pid_button.grid(row=3, column=0, columnspan=2, padx=5, pady=5)

# Motor switch
switch_label = ttk.Label(root, text="Motor Switch:")
switch_label.grid(row=4, column=0, padx=5, pady=5)
switch_var = tk.IntVar()
motor_switch = ttk.Checkbutton(root, variable=switch_var, command=toggle_motor)
motor_switch.grid(row=4, column=1, padx=5, pady=5)

root.mainloop()
