-- Set uart speed and disable echo
uart.setup(0,9600,8,0,1,0)
print("\n\r")
dofile("node_functions.lua")
