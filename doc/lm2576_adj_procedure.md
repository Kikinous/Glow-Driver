# LM2576-ADJ

# Given
Vout = 1.5V
Vin = 9.9V
Iload = 1.5A (Rload = 1 ohm)


#Programming output voltage
Vout = Vref ( 1+R2/R1) Vref = 1.23V

R2 = R1 (Vout/Vref -1) avec 1k < R1 < 5k
pot : R2 + R1 = 10k
R1 = 8.2k
R2 = 1.8k

# induductor selection
* E.t
E.t = (Vin-Vout)*(Vout/Vin)*(1000/52)  
E.t = (9.9-1.5)*(1.5/9.9)*(1000/52)  
E.t = 24 V/us
* figure 27
E.t = 24 & I = 1.5A => L = 47 - 68 uH
inductance choisie 64uH 5A

# output capacitor
Cout > 13,300 * Vin /(Vout * L[uH]) [uF]
but may be much larger
Cout > 13,300 * 9.9 /(1.5  * 64) [uF]
Cout > 1372 uF
J'avais 680uF, je mets 2000uF

##################################################################

LM2576 - LM2577 up converter : error

# Distronics : TO220-5 LM2596T-ADJ 
## Programming voltage
E.t = (Vin - Vout - Vsat ) . ( Vout + Vd ) / (Vin - Vsat + Vd) . (1000 / 150) [V.us]
E.t = 9.4 V.us
--> 15uH , inductor code = L34  (check table 3)
L34
L = 15
I = 3.40
Through hole or surface mount

## Output capacitor
C = 10nF tantalium or electrolitic - table 2

## Feedforward capacitor 
C = 10nF ceramic - table 2

## Catch diode
Schottky diodes : 5 A rating 

## Input capacitor 
look for RMS current to avoid heating : big capacitor qui chauffe pas

