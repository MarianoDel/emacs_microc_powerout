# -*- coding: utf-8 -*-
#use python3
import numpy as np
import matplotlib.pyplot as plt

samples = 256
Vmax = 1023

s_sin_rect = np.zeros(samples)
s_sin_rect_outphase = np.zeros(samples)
s_sin_full = np.zeros(samples)

s_triang = np.zeros(samples)
s_triang_outphase = np.zeros(samples)

s_square = np.zeros(samples)
s_square_outphase = np.zeros(samples)


# Sinusoidal Assembly
for i in range(np.size(s_sin_rect)):
    # rectified sinusoidal
    s_sin_rect[i] = np.sin(2*np.pi*(i+1)/samples) * Vmax
    if s_sin_rect[i] < 0:
        s_sin_rect[i] = 0
    # rectified sinusoidal outphase
    s_sin_rect_outphase[i] = np.sin(2*np.pi*(i+1)/samples + np.pi) * Vmax
    if s_sin_rect_outphase[i] < 0:
        s_sin_rect_outphase[i] = 0


# Full wave Sinusoidal Assembly
for i in range(np.size(s_sin_full)):
    # full wave rectified sinusoidal
    s_sin_full[i] = np.sin(np.pi*(i+1)/samples) * Vmax
        
        
s_sin_rect = s_sin_rect.astype(int)
s_sin_rect_outphase = s_sin_rect_outphase.astype(int)
s_sin_full = s_sin_full.astype(int)


# Triangular Assembly
samples_by_two = int(samples / 2)

for i in range(0, samples_by_two):
    s_triang[i] = (i+1) * Vmax / samples_by_two

for i in range(samples_by_two, samples):
    s_triang_outphase[i] = (i+1-samples_by_two) * Vmax / samples_by_two
    
s_triang = s_triang.astype(int)
s_triang_outphase = s_triang_outphase.astype(int)


# Square Assembly
s_square[:samples_by_two] = Vmax
s_square_outphase[samples_by_two:] = Vmax
s_square = s_square.astype(int)
s_square_outphase = s_square_outphase.astype(int)


### print on C code format ###
qtty_per_line = 10


print ('--- SINUSOIDAL RECTIFIED ---')
line = 1
print ("{",end='')
for i in range(np.size(s_sin_rect)):
    if i < ((line * qtty_per_line) - 1):
        print (str(s_sin_rect[i]) + ",",end='')
    else:
        if i == (np.size(s_sin_rect) - 1):
            print (str(s_sin_rect[i]),end='')
        else:                
            print (str(s_sin_rect[i]) + ",\n",end='')
            line += 1
        
print ("};")


print ()
print ()
print ('--- SINUSOIDAL FULL WAVE ---')
line = 1
print ("{",end='')
for i in range(np.size(s_sin_full)):
    if i < ((line * qtty_per_line) - 1):
        print (str(s_sin_full[i]) + ",",end='')
    else:
        if i == (np.size(s_sin_full) - 1):
            print (str(s_sin_full[i]),end='')
        else:                
            print (str(s_sin_full[i]) + ",\n",end='')
            line += 1
        
print ("};")


print ()
print ()
print ('--- SINUSOIDAL RECTIFIED OUTPHASE---')
line = 1
print ("{",end='')
for i in range(np.size(s_sin_rect_outphase)):
    if i < ((line * qtty_per_line) - 1):
        print (str(s_sin_rect_outphase[i]) + ",",end='')
    else:
        if i == (np.size(s_sin_rect_outphase) - 1):
            print (str(s_sin_rect_outphase[i]),end='')
        else:                
            print (str(s_sin_rect_outphase[i]) + ",\n",end='')
            line += 1
        
print ("};")


print ()
print ()
print ('--- TRIANGULAR ---')
line = 1
print ("{",end='')
for i in range(np.size(s_triang)):
    if i < ((line * qtty_per_line) - 1):
        print (str(s_triang[i]) + ",",end='')
    else:
        if i == (np.size(s_triang) - 1):
            print (str(s_triang[i]),end='')
        else:                
            print (str(s_triang[i]) + ",\n",end='')
            line += 1
        
print ("};")


print ()
print ()
print ('--- TRIANGULAR OUTPHASE---')
line = 1
print ("{",end='')
for i in range(np.size(s_triang_outphase)):
    if i < ((line * qtty_per_line) - 1):
        print (str(s_triang_outphase[i]) + ",",end='')
    else:
        if i == (np.size(s_triang_outphase) - 1):
            print (str(s_triang_outphase[i]),end='')
        else:                
            print (str(s_triang_outphase[i]) + ",\n",end='')
            line += 1
        
print ("};")


print ()
print ()
print ('--- SQUARE ---')
line = 1
print ("{",end='')
for i in range(np.size(s_square)):
    if i < ((line * qtty_per_line) - 1):
        print (str(s_square[i]) + ",",end='')
    else:
        if i == (np.size(s_square) - 1):
            print (str(s_square[i]),end='')
        else:                
            print (str(s_square[i]) + ",\n",end='')
            line += 1
        
print ("};")


print ()
print ()
print ('--- SQUARE OUTPHASE---')
line = 1
print ("{",end='')
for i in range(np.size(s_square_outphase)):
    if i < ((line * qtty_per_line) - 1):
        print (str(s_square_outphase[i]) + ",",end='')
    else:
        if i == (np.size(s_square_outphase) - 1):
            print (str(s_square_outphase[i]),end='')
        else:                
            print (str(s_square_outphase[i]) + ",\n",end='')
            line += 1
        
print ("};")

