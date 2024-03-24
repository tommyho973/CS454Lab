from scipy.fftpack import fft, fftshift, fftfreq
import numpy as np
import matplotlib.pyplot as plt

# Sampling rate
fc = 10000  # Hz
# Time duration
duration = 10  # seconds

# Generate time vector from 0 to 10 seconds
t = np.linspace(0, duration, int(fc * duration))

# Generate a 1 Hz square wave with amplitude between 0 and 5
s = 5 * (np.sign(np.sin(2 * np.pi * 1 * t)) + 1) / 2

# Compute FFT of the square wave signal and shift
yf = fftshift(fft(s))
# Compute the corresponding frequencies and shift and normalize
xf = fftshift(fftfreq(n=len(t), d=1/fc))

# Normalize 
yf_normalized = np.abs(yf) / len(t)

plt.figure(figsize=(15, 6))

plt.subplot(1, 2, 1)
plt.plot(t, s)
plt.title("1 Hz Square Wave (0 to 10 seconds)")
plt.xlabel("Time (s)")
plt.ylabel("Amplitude")
plt.grid(True)

plt.subplot(1, 2, 2)
plt.stem(xf, yf_normalized, linefmt='b-', markerfmt=" ", basefmt=" ")
plt.title("FFT of the 1 Hz Square Wave")
plt.xlabel("Frequency (Hz)")
plt.ylabel("Normalized Magnitude")
plt.xlim(-5, 5)  
plt.grid(True)

plt.tight_layout()
plt.show()
